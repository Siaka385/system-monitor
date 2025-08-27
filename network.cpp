/**
 * @file network.cpp
 * @brief This file contains functions to retrieve network interface information on a Linux system.
 * It provides a function to get details about each network interface, including its name, IP address,
 * and various network statistics like received and transmitted bytes, packets, and errors.
 * The IP address information is obtained using `getifaddrs`, and the network statistics are parsed from `/proc/net/dev`.
 */

#include "header.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <cstdio>

/**
 * @brief Retrieves information about all network interfaces on the system.
 *
 * This function first uses `getifaddrs` to iterate through all network interfaces and
 * retrieve their IPv4 addresses, storing them in a map with the interface name as the key.
 *
 * It then reads the `/proc/net/dev` file to get detailed network statistics for each
 * interface, such as received and transmitted bytes, packets, and various error counts.
 * The data from `/proc/net/dev` is parsed and combined with the previously obtained
 * IP address information.
 *
 * The function handles the formatting of the interface name by removing any trailing
 * colons and assigns "N/A" if an IP address is not found for an interface.
 *
 * @return A vector of `InterfaceInfo` structs, each containing detailed information
 * about a network interface.
 */
vector<InterfaceInfo> getNetworkInfo() {
    vector<InterfaceInfo> interfaces;
    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return interfaces;
    }

    map<string, string> ip_addresses;

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) continue;

        family = ifa->ifa_addr->sa_family;

        if (family == AF_INET) {
            s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                            host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            if (s != 0) {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                continue;
            }
            ip_addresses[ifa->ifa_name] = host;
        }
    }

    freeifaddrs(ifaddr);

    ifstream file("/proc/net/dev");
    string line;
    // Skip the first two lines which are headers
    getline(file, line);
    getline(file, line);

    while (getline(file, line)) {
        InterfaceInfo info;
        char name[32];
        sscanf(line.c_str(), "%s %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld",
               name,
               &info.rx_bytes, &info.rx_packets, &info.rx_errs, &info.rx_drop, &info.rx_fifo, &info.rx_frame, &info.rx_compressed, &info.rx_multicast,
               &info.tx_bytes, &info.tx_packets, &info.tx_errs, &info.tx_drop, &info.tx_fifo, &info.tx_colls, &info.tx_carrier, &info.tx_compressed);
        info.name = string(name);
        // Remove the trailing colon from the interface name
        if (info.name.back() == ':') {
            info.name.pop_back();
        }
        // Assign the IP address from the map or "N/A" if not found
        if (ip_addresses.count(info.name)) {
            info.ip_address = ip_addresses[info.name];
        } else {
            info.ip_address = "N/A";
        }
        interfaces.push_back(info);
    }
    return interfaces;
}
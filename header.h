// To make sure you don't declare the function more than once by including the header multiple times.
#ifndef header_H
#define header_H

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <dirent.h>
#include <vector>
#include <iostream>
#include <cmath>
// lib to read from file
#include <fstream>
// for the name of the computer and the logged in user
#include <unistd.h>
#include <limits.h>
// this is for us to get the cpu information
// mostly in unix system
// not sure if it will work in windows
#include <cpuid.h>
// this is for the memory usage and other memory visualization
// for linux gotta find a way for windows
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
// for time and date
#include <ctime>
// ifconfig ip addresses
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <map>

using namespace std;

struct CPUStats {
    long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
};

CPUStats getCPUStats();

// System Statistics: Functions and structures related to overall system information
string CPUinfo();
const char *getOsName();
string getHostName();
string getUserName();
int getTotalTasks();
float getThermalInfo();
int getFanSpeed();
// Function to dynamically retrieve the number of logical CPU cores/processors
int getProcessorCount();

// student TODO : memory and processes
struct MemInfo {
    long long total;
    long long free;
    long long available;
    long long buffers;
    long long cached;
};

struct SwapInfo {
    long long total;
    long long free;
};

struct DiskInfo {
    long long total;
    long long free;
    long long used;
};

MemInfo getMemInfo();
SwapInfo getSwapInfo();
DiskInfo getDiskInfo(const char* path);
string formatBytes(long long bytes);

struct ProcessInfo {
    int pid;
    string name;
    char state;
    long long memory;
    float cpu_percentage; // Added: CPU usage in percentage
    bool is_selected = false; // Added: Selection state for process table rows, initialized to false
};

struct ProcessCPUStats { // Stores previous CPU times for a process to calculate CPU usage percentage
    long long utime;
    long long stime;
    long long total_cpu_time;
};

vector<ProcessInfo> getProcessInfo();

// Network Statistics: Functions and structures related to network information
struct InterfaceInfo {
    string name;
    string ip_address;
    long long rx_bytes;
    long long rx_packets;    // Received packets
    long long rx_errs;       // Received errors
    long long rx_drop;       // Received dropped packets
    long long rx_fifo;       // Received FIFO errors
    long long rx_frame;      // Received frame errors
    long long rx_compressed; // Received compressed packets
    long long rx_multicast;  // Received multicast packets
    long long tx_bytes;
    long long tx_packets;    // Transmitted packets
    long long tx_errs;       // Transmitted errors
    long long tx_drop;       // Transmitted dropped packets
    long long tx_fifo;       // Transmitted FIFO errors
    long long tx_colls;      // Transmitted collisions
    long long tx_carrier;    // Transmitted carrier errors
    long long tx_compressed; // Transmitted compressed packets
};

vector<InterfaceInfo> getNetworkInfo();

#endif

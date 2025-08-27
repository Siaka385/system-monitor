/**
 * @file system.cpp
 * @brief This file contains a collection of functions to retrieve various system-level information on a Linux machine.
 * It includes functions to get CPU statistics from `/proc/stat`, CPU brand information using `__cpuid`,
 * operating system name, hostname, and username. It also provides functions to count the total number of
 * running tasks (processes), get thermal information (CPU temperature), and fan speed from the `/sys` filesystem.
 * Additionally, a function is included to determine the number of logical processors.
 */

#include "header.h"
#include <fstream>
#include <string>
#include <dirent.h>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sstream>

// For `__cpuid`
#ifdef _WIN32
#include <intrin.h>
#else
#include <cpuid.h>
#endif

/**
 * @brief Retrieves system-wide CPU statistics from `/proc/stat`.
 *
 * This function reads the first line of the `/proc/stat` file, which contains
 * the aggregated CPU time spent in various states (user, nice, system, idle, etc.).
 * It parses this line and populates a `CPUStats` struct with the values.
 *
 * @return A `CPUStats` struct containing the total CPU time in different states.
 */
CPUStats getCPUStats() {
    CPUStats stats;
    ifstream file("/proc/stat");
    string line;
    getline(file, line);
    sscanf(line.c_str(), "cpu %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld",
           &stats.user, &stats.nice, &stats.system, &stats.idle, &stats.iowait,
           &stats.irq, &stats.softirq, &stats.steal, &stats.guest, &stats.guest_nice);
    return stats;
}

/**
 * @brief Retrieves the CPU brand string using the `__cpuid` instruction.
 *
 * This function uses the `__cpuid` intrinsic to query the CPU for its brand string
 * (e.g., "Intel(R) Core(TM) i7-8700K CPU @ 3.70GHz"). It queries the extended
 * CPUID functions from `0x80000002` to `0x80000004` to get the full string.
 *
 * @return A string containing the full CPU brand name.
 */
string CPUinfo()
{
    char CPUBrandString[0x40];
    unsigned int CPUInfo[4] = {0, 0, 0, 0};

    __cpuid(0x80000000, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);
    unsigned int nExIds = CPUInfo[0];

    memset(CPUBrandString, 0, sizeof(CPUBrandString));

    for (unsigned int i = 0x80000000; i <= nExIds; ++i)
    {
        __cpuid(i, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);

        if (i == 0x80000002)
            memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
        else if (i == 0x80000003)
            memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
        else if (i == 0x80000004)
            memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
    }
    string str(CPUBrandString);
    return str;
}

/**
 * @brief Determines the operating system name based on preprocessor macros.
 *
 * This function uses standard preprocessor directives to identify the current
 * operating system and returns a corresponding string literal.
 *
 * @return A const char* pointing to the name of the operating system.
 */
const char *getOsName()
{
#ifdef _WIN32
    return "Windows 32-bit";
#elif _WIN64
    return "Windows 64-bit";
#elif __APPLE__ || __MACH__
    return "Mac OSX";
#elif __linux__
    return "Linux";
#elif __FreeBSD__
    return "FreeBSD";
#elif __unix || __unix__
    return "Unix";
#else
    return "Other";
#endif
}

/**
 * @brief Retrieves the hostname of the system.
 *
 * This function uses the `gethostname` system call to get the name of the host
 * and returns it as a C++ string.
 *
 * @return A string containing the hostname.
 */
string getHostName()
{
    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    return string(hostname);
}

/**
 * @brief Retrieves the username of the current logged-in user.
 *
 * This function uses the `getlogin_r` system call to get the name of the
 * user associated with the controlling terminal and returns it as a C++ string.
 *
 * @return A string containing the username.
 */
string getUserName()
{
    char username[LOGIN_NAME_MAX];
    getlogin_r(username, LOGIN_NAME_MAX);
    return string(username);
}

/**
 * @brief Counts the total number of running tasks (processes).
 *
 * This function iterates through the `/proc` directory. Directories with names
 * that are purely numeric represent a running process's PID. It counts these
 * directories to determine the total number of tasks.
 *
 * @return An integer representing the total number of running tasks.
 */
int getTotalTasks()
{
    DIR *dir;
    struct dirent *ent;
    int count = 0;
    if ((dir = opendir("/proc")) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (ent->d_type == DT_DIR)
            {
                char *p;
                strtol(ent->d_name, &p, 10);
                if (!*p)
                {
                    count++;
                }
            }
        }
        closedir(dir);
    }
    return count;
}

/**
 * @brief Retrieves the CPU temperature from the thermal zone.
 *
 * This function reads the temperature from `/sys/class/thermal/thermal_zone0/temp`.
 * The value is typically in millidegrees Celsius, so it is divided by 1000.0f
 * to get the temperature in degrees Celsius.
 *
 * @return A float representing the CPU temperature in Celsius.
 */
float getThermalInfo() {
    ifstream file("/sys/class/thermal/thermal_zone0/temp");
    float temp;
    file >> temp;
    return temp / 1000.0f;
}

/**
 * @brief Retrieves the fan speed from the hardware monitoring sensors.
 *
 * This function reads the fan speed from `/sys/class/hwmon/hwmon0/fan1_input`.
 * This path may vary depending on the system and hardware, and this function
 * assumes a common path for fan speed information.
 *
 * @return An integer representing the fan speed in RPM (Revolutions Per Minute).
 */
int getFanSpeed() {
    ifstream file("/sys/class/hwmon/hwmon0/fan1_input");
    int speed;
    file >> speed;
    return speed;
}

/**
 * @brief Counts the number of logical processors on the system.
 *
 * This function reads the `/proc/cpuinfo` file and counts the number of lines
 * that begin with "processor". Each such line corresponds to a logical processor.
 *
 * @return An integer representing the total number of logical processors.
 */
int getProcessorCount() {
    ifstream file("/proc/cpuinfo");
    string line;
    int count = 0;
    while (getline(file, line)) {
        if (line.rfind("processor", 0) == 0) {
            count++;
        }
    }
    return count;
}
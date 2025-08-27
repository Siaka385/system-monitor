/**
 * @file system_info.cpp
 * @brief This file contains functions to retrieve system information like memory, swap, disk, CPU, and process details on Linux.
 * * It includes functions to get memory and swap usage from /proc/meminfo, disk usage from statvfs, CPU statistics from /proc/stat,
 * and process information from the /proc file system. It also provides a utility function to format byte sizes into a human-readable string.
 * The process information functions use a static map to store previous CPU statistics to calculate CPU usage percentages over time.
 */

#include "header.h"
#include <map>
#include <sstream>
#include <dirent.h>
#include <fstream>
#include <sys/statvfs.h>
#include <vector>
#include <string>

/**
 * @brief A static map to store the previous CPU stats for each process.
 * * This map is used to calculate the CPU usage percentage of a process by comparing
 * its current CPU time with the previously recorded CPU time.
 */
static map<int, ProcessCPUStats> prev_process_cpu_stats;

/**
 * @brief A static variable to store the previous total CPU time.
 * * This variable, along with the current total CPU time, is used to calculate the
 * total CPU time difference, which is essential for determining the percentage
 * of CPU time consumed by a process.
 */
static long long prev_total_cpu_time = 0;

/**
 * @brief Retrieves memory information from /proc/meminfo.
 * * This function reads the /proc/meminfo file to extract details about the
 * system's memory, including total, free, available, buffers, and cached memory.
 * * @return A MemInfo struct containing the retrieved memory statistics in kB.
 */
MemInfo getMemInfo() {
    MemInfo memInfo;
    ifstream file("/proc/meminfo");
    string line;
    while (getline(file, line)) {
        if (line.find("MemTotal:") != string::npos) {
            sscanf(line.c_str(), "MemTotal: %lld kB", &memInfo.total);
        } else if (line.find("MemFree:") != string::npos) {
            sscanf(line.c_str(), "MemFree: %lld kB", &memInfo.free);
        } else if (line.find("MemAvailable:") != string::npos) {
            sscanf(line.c_str(), "MemAvailable: %lld kB", &memInfo.available);
        } else if (line.find("Buffers:") != string::npos) {
            sscanf(line.c_str(), "Buffers: %lld kB", &memInfo.buffers);
        } else if (line.find("Cached:") != string::npos) {
            sscanf(line.c_str(), "Cached: %lld kB", &memInfo.cached);
        }
    }
    return memInfo;
}

/**
 * @brief Retrieves swap space information from /proc/meminfo.
 * * This function reads the /proc/meminfo file to get details about the system's
 * swap space, including total and free swap memory.
 * * @return A SwapInfo struct containing the retrieved swap statistics in kB.
 */
SwapInfo getSwapInfo() {
    SwapInfo swapInfo;
    ifstream file("/proc/meminfo");
    string line;
    while (getline(file, line)) {
        if (line.find("SwapTotal:") != string::npos) {
            sscanf(line.c_str(), "SwapTotal: %lld kB", &swapInfo.total);
        } else if (line.find("SwapFree:") != string::npos) {
            sscanf(line.c_str(), "SwapFree: %lld kB", &swapInfo.free);
        }
    }
    return swapInfo;
}

/**
 * @brief Retrieves disk usage information for a specified path.
 * * This function uses the statvfs system call to get file system statistics for
 * a given path, including total, free, and used space in bytes.
 * * @param path The file system path to check (e.g., "/").
 * @return A DiskInfo struct containing the retrieved disk statistics in bytes.
 */
DiskInfo getDiskInfo(const char* path) {
    struct statvfs stat;
    DiskInfo diskInfo;
    if (statvfs(path, &stat) == 0) {
        diskInfo.total = stat.f_blocks * stat.f_frsize;
        diskInfo.free = stat.f_bavail * stat.f_frsize;
        diskInfo.used = diskInfo.total - (stat.f_bfree * stat.f_frsize);
    }
    return diskInfo;
}

/**
 * @brief Formats a byte size into a human-readable string (e.g., "1.23 MB").
 * * This utility function converts a large number of bytes into a more readable
 * format using standard suffixes like KB, MB, GB, and TB.
 * * @param bytes The number of bytes to format.
 * @return A string with the formatted size and its corresponding suffix.
 */
string formatBytes(long long bytes) {
    const char* suffixes[] = {"B", "KB", "MB", "GB", "TB"};
    int i = 0;
    double dblBytes = bytes;
    while (dblBytes >= 1024 && i < 4) {
        dblBytes /= 1024;
        i++;
    }
    char buffer[16];
    sprintf(buffer, "%.2f %s", dblBytes, suffixes[i]);
    return string(buffer);
}

/**
 * @brief Retrieves a list of running processes and their statistics.
 * * This function iterates through the /proc directory to find all process IDs (PIDs).
 * For each PID, it reads /proc/[pid]/stat to get the process state, name, and CPU time,
 * and /proc/[pid]/status to get the resident memory usage (VmRSS).
 * * The CPU usage percentage is calculated by comparing the current CPU time of the
 * process with its previous CPU time, and normalizing it against the total
 * system CPU time difference. The results are stored in a vector of ProcessInfo structs.
 * * @return A vector of ProcessInfo structs, each containing details about a running process.
 */
vector<ProcessInfo> getProcessInfo() {
    vector<ProcessInfo> processes;
    DIR* dir = opendir("/proc");
    if (dir == NULL) {
        return processes;
    }

    CPUStats current_total_cpu_stats = getCPUStats();
    long long current_total_cpu_time = current_total_cpu_stats.user + current_total_cpu_stats.nice + current_total_cpu_stats.system + current_total_cpu_stats.idle + current_total_cpu_stats.iowait + current_total_cpu_stats.irq + current_total_cpu_stats.softirq + current_total_cpu_stats.steal;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            int pid = atoi(entry->d_name);
            if (pid != 0) {
                ProcessInfo p;
                p.pid = pid;

                ifstream statFile(string("/proc/") + entry->d_name + "/stat");
                if (statFile.is_open()) {
                    string line;
                    getline(statFile, line);
                    stringstream ss(line);
                    string value;
                    // Skip first 13 values to get utime (14th) and stime (15th)
                    for (int i = 0; i < 13; ++i) {
                        ss >> value;
                    }
                    long long utime, stime;
                    ss >> utime >> stime;
                    p.state = line[line.find(')') + 2]; // State is after ')' and a space
                    p.name = line.substr(line.find('(') + 1, line.find(')') - (line.find('(') + 1));

                    // Calculate CPU usage
                    if (prev_process_cpu_stats.count(pid) && prev_total_cpu_time != 0) {
                        long long process_cpu_total_diff = (utime + stime) - (prev_process_cpu_stats[pid].utime + prev_process_cpu_stats[pid].stime);
                        long long system_cpu_total_diff = current_total_cpu_time - prev_total_cpu_time;

                        if (system_cpu_total_diff > 0) {
                            p.cpu_percentage = (float)process_cpu_total_diff * 100.0f / system_cpu_total_diff * getProcessorCount();
                        } else {
                            p.cpu_percentage = 0.0f;
                        }
                    } else {
                        p.cpu_percentage = 0.0f;
                    }

                    // Store current stats for next iteration
                    prev_process_cpu_stats[pid].utime = utime;
                    prev_process_cpu_stats[pid].stime = stime;

                    statFile.close();
                }

                ifstream statusFile(string("/proc/") + entry->d_name + "/status");
                if (statusFile.is_open()) {
                    string line;
                    while (getline(statusFile, line)) {
                        if (line.rfind("VmRSS:", 0) == 0) {
                            sscanf(line.c_str(), "VmRSS: %lld kB", &p.memory);
                            break;
                        }
                    }
                    statusFile.close();
                }
                processes.push_back(p);
            }
        }
    }

    prev_total_cpu_time = current_total_cpu_time;
    closedir(dir);
    return processes;
}
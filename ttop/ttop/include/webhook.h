#ifndef WEBHOOK_H
#define WEBHOOK_H
#include "cpu.h"
#include "memory.h"
#include "thermal.h"
#include "disk.h"
#include "load.h"
#include "uptime.h"
#include "network.h"
#include "process.h"
#include "filesystem.h"
#include "gpu.h"
int webhook_send(const char *url, const char *hostname, 
                 float cpu_usage, const MemoryInfo *mem, 
                 const ThermalInfo *thermal, uint64_t disk_r, uint64_t disk_w,
                 const LoadInfo *load, const UptimeInfo *uptime,
                 const NetworkInfo *net, const FSInfo *fs, int num_fs,
                 const ProcessInfo *procs, int num_procs,
                 const GPUInfo *gpus, int num_gpus);
#endif

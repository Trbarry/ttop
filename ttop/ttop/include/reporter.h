#ifndef REPORTER_H
#define REPORTER_H
#include <stdint.h>
#include "cpu.h"
#include "memory.h"
#include "thermal.h"
#include "disk.h"
#include "process.h"
#include "filesystem.h"
#include "gpu.h"
#define TTOP_PROTO_VERSION 2
typedef struct __attribute__((packed)) {
    uint8_t  version;
    uint64_t timestamp;
    float    cpu_usage;
    uint64_t mem_total;
    uint64_t mem_avail;
    float    temp;
    uint64_t disk_read;
    uint64_t disk_write;
    uint8_t  fs_used_pct;
    struct {
        uint32_t pid;
        char     name[16];
        uint64_t rss_kb;
    } top_procs[3];
    struct {
        char     model[16];
        int8_t   usage_pct;
        uint64_t vram_used_kb;
    } gpu;
} ttop_packet_t;
int reporter_init(const char *host, int port);
void reporter_send_bin(float cpu_usage, const MemoryInfo *mem, const ThermalInfo *thermal, 
                      uint64_t disk_r, uint64_t disk_w, const FSInfo *fs, 
                      const ProcessInfo *procs, int num_procs, const GPUInfo *gpus, int num_gpus);
void reporter_close(void);
#endif

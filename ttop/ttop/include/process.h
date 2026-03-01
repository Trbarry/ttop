#ifndef PROCESS_H
#define PROCESS_H
#include <stdint.h>
typedef struct {
    uint32_t pid;
    char name[32];
    float cpu_usage;
    uint64_t mem_rss_kb;
} ProcessInfo;
int get_top_processes(ProcessInfo *procs, int max_procs);
#endif

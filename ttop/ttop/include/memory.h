#ifndef MEMORY_H
#define MEMORY_H
#include <stdint.h>
typedef struct {
    uint64_t total;
    uint64_t available;
    uint64_t free;
    uint64_t buffers;
    uint64_t cached;
    uint64_t swap_total;
    uint64_t swap_free;
    float usage_percent;
} MemoryInfo;
int get_memory_info(MemoryInfo *info);
#endif

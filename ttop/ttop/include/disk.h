#ifndef DISK_H
#define DISK_H
#include <stdint.h>
typedef struct {
    uint64_t reads;
    uint64_t writes;
} DiskStats;
int get_disk_stats(DiskStats *stats);
#endif

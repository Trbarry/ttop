#ifndef UPTIME_H
#define UPTIME_H
#include <stdint.h>
typedef struct {
    uint64_t uptime_seconds;
} UptimeInfo;
int get_uptime_info(UptimeInfo *info);
#endif

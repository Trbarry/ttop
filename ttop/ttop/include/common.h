#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <time.h>

#define TTOP_PROTO_VERSION 2

static inline void ttop_log(const char *level, const char *msg) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char timestamp[20];
    strftime(timestamp, 20, "%Y-%m-%d %H:%M:%S", tm_info);
    
    // Log to stdout/stderr (captured by journald)
    fprintf(level[0] == 'E' ? stderr : stdout, "[%s] [%s] %s\n", timestamp, level, msg);
    
    // Log to file
    FILE *f = fopen("/var/log/ttop.log", "a");
    if (f) {
        fprintf(f, "[%s] [%s] %s\n", timestamp, level, msg);
        fclose(f);
    }
}

#endif

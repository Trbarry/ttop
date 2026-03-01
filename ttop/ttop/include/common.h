#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <stdlib.h>
static inline const char* get_proc_path(const char* filename, char* buf, size_t len) {
    const char* root = getenv("TTOP_PROC_ROOT");
    snprintf(buf, len, "%s/%s", root ? root : "/proc", filename);
    return buf;
}
static inline const char* get_sys_path(const char* filename, char* buf, size_t len) {
    const char* root = getenv("TTOP_SYS_ROOT");
    snprintf(buf, len, "%s/%s", root ? root : "/sys", filename);
    return buf;
}
#endif

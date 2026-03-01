#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include <stdint.h>
typedef struct {
    char mount_point[64];
    uint64_t total_kb;
    uint64_t used_kb;
    uint8_t used_pct;
} FSInfo;
int get_fs_info(const char *path, FSInfo *info);
int get_all_fs_info(FSInfo *info, int max);
#endif

#include "filesystem.h"
#include <sys/statvfs.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int get_fs_info(const char *path, FSInfo *info) {
    struct statvfs vfs;
    if (statvfs(path, &vfs) != 0) return 0;
    strncpy(info->mount_point, path, 63);
    info->total_kb = (vfs.f_blocks * vfs.f_frsize) / 1024;
    uint64_t free_kb = (vfs.f_bavail * vfs.f_frsize) / 1024;
    info->used_kb = info->total_kb - free_kb;
    if (info->total_kb > 0) info->used_pct = (uint8_t)((info->used_kb * 100) / info->total_kb);
    else info->used_pct = 0;
    return 1;
}

int get_all_fs_info(FSInfo *info, int max) {
    FILE *f = fopen("/proc/mounts", "r");
    if (!f) return 0;
    char line[512];
    int count = 0;
    while (fgets(line, sizeof(line), f) && count < max) {
        char dev[128], mount[128], type[64];
        if (sscanf(line, "%s %s %s", dev, mount, type) == 3) {
            // Filter common physical filesystems
            if (strcmp(type, "ext4") == 0 || strcmp(type, "xfs") == 0 || 
                strcmp(type, "btrfs") == 0 || strcmp(type, "zfs") == 0) {
                if (get_fs_info(mount, &info[count])) {
                    count++;
                }
            }
        }
    }
    fclose(f);
    return count;
}

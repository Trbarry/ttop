#include "filesystem.h"
#include <sys/param.h>
#include <sys/mount.h>
#include <sys/statvfs.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int get_fs_info(const char *path, FSInfo *info) {
    struct statvfs vfs;
    if (statvfs(path, &vfs) != 0) return 0;
    strncpy(info->mount_point, path, 63);
    info->mount_point[63] = '\0';
    info->total_kb = (uint64_t)vfs.f_blocks * vfs.f_frsize / 1024;
    uint64_t free_kb = (uint64_t)vfs.f_bavail * vfs.f_frsize / 1024;
    info->used_kb = info->total_kb - free_kb;
    if (info->total_kb > 0) info->used_pct = (uint8_t)((info->used_kb * 100) / info->total_kb);
    else info->used_pct = 0;
    return 1;
}

int get_all_fs_info(FSInfo *info, int max) {
#ifdef __FreeBSD__
    struct statfs *mntbuf;
    int mntsize = getmntinfo(&mntbuf, MNT_NOWAIT);
    int count = 0;
    for (int i = 0; i < mntsize && count < max; i++) {
        // Filter physical filesystems
        if (strcmp(mntbuf[i].f_fstypename, "ufs") == 0 || 
            strcmp(mntbuf[i].f_fstypename, "zfs") == 0) {
            if (get_fs_info(mntbuf[i].f_mntonname, &info[count])) {
                count++;
            }
        }
    }
    return count;
#else
    FILE *f = fopen("/proc/mounts", "r");
    if (!f) return 0;
    char line[512];
    int count = 0;
    while (fgets(line, sizeof(line), f) && count < max) {
        char dev[128], mount[128], type[64];
        if (sscanf(line, "%127s %127s %63s", dev, mount, type) == 3) {
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
#endif
}

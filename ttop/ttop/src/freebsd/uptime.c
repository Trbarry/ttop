#include "uptime.h"
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <time.h>
int get_uptime_info(UptimeInfo *info) {
    struct timeval boot_time;
    size_t sz = sizeof(boot_time);
    if (sysctlbyname("kern.boottime", &boot_time, &sz, NULL, 0) < 0) return -1;
    info->uptime_seconds = time(NULL) - boot_time.tv_sec;
    return 0;
}

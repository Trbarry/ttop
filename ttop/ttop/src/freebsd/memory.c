#include "memory.h"
#include <sys/types.h>
#include <sys/sysctl.h>
#include <unistd.h>
int get_memory_info(MemoryInfo *info) {
    int page_size = getpagesize();
    unsigned int v_page_count, v_free_count, v_inactive_count, v_cache_count;
    size_t sz = sizeof(unsigned int);
    sysctlbyname("vm.stats.vm.v_page_count", &v_page_count, &sz, NULL, 0);
    sysctlbyname("vm.stats.vm.v_free_count", &v_free_count, &sz, NULL, 0);
    sysctlbyname("vm.stats.vm.v_inactive_count", &v_inactive_count, &sz, NULL, 0);
    sysctlbyname("vm.stats.vm.v_cache_count", &v_cache_count, &sz, NULL, 0);
    info->total = (uint64_t)v_page_count * page_size / 1024;
    info->available = (uint64_t)(v_free_count + v_inactive_count + v_cache_count) * page_size / 1024;
    if (info->total > 0) info->usage_percent = (float)(info->total - info->available) * 100 / info->total;
    return 0;
}

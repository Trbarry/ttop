#include "process.h"
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/user.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int get_top_processes(ProcessInfo *procs, int max_procs) {
    int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0};
    size_t sz;
    if (sysctl(mib, 4, NULL, &sz, NULL, 0) < 0) return 0;
    struct kinfo_proc *ki = malloc(sz);
    if (!ki) return 0;
    if (sysctl(mib, 4, ki, &sz, NULL, 0) < 0) { free(ki); return 0; }
    int n = sz / sizeof(struct kinfo_proc);
    int count = 0, ps = getpagesize();
    for (int i = 0; i < n && count < max_procs; i++) {
        procs[count].pid = ki[i].ki_pid;
        strncpy(procs[count].name, ki[i].ki_comm, 31);
        procs[count].mem_rss_kb = (uint64_t)ki[i].ki_rssize * ps / 1024;
        count++;
    }
    free(ki); return count;
}

#include "cpu.h"
#include <sys/types.h>
#include <sys/sysctl.h>
#include <stdlib.h>
int get_cpu_state(CPUState *s) {
    long cp_time[5];
    size_t sz = sizeof(cp_time);
    if (sysctlbyname("kern.cp_time", &cp_time, &sz, NULL, 0) < 0) return -1;
    s->user = cp_time[0]; s->nice = cp_time[1]; s->system = cp_time[2]; s->idle = cp_time[4];
    s->iowait = 0; s->irq = cp_time[3]; s->softirq = 0; s->steal = 0;
    return 0;
}
float calculate_cpu_usage(const CPUState *p, const CPUState *c) {
    unsigned long long p_total = p->user + p->nice + p->system + p->idle + p->irq;
    unsigned long long c_total = c->user + c->nice + c->system + c->idle + c->irq;
    unsigned long long total_diff = c_total - p_total;
    unsigned long long idle_diff = c->idle - p->idle;
    if (total_diff == 0) return 0.0f;
    return 100.0f * (total_diff - idle_diff) / total_diff;
}

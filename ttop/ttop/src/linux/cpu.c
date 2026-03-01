#include "cpu.h"
#include <stdio.h>
#include <string.h>
int get_cpu_state(CPUState *s) {
    FILE *f = fopen("/proc/stat", "r");
    if (!f) return -1;
    if (fscanf(f, "cpu %llu %llu %llu %llu %llu %llu %llu %llu", &s->user, &s->nice, &s->system, &s->idle, &s->iowait, &s->irq, &s->softirq, &s->steal) < 4) { fclose(f); return -1; }
    fclose(f); return 0;
}
float calculate_cpu_usage(const CPUState *p, const CPUState *c) {
    unsigned long long p_total = p->user + p->nice + p->system + p->idle + p->iowait + p->irq + p->softirq + p->steal;
    unsigned long long c_total = c->user + c->nice + c->system + c->idle + c->iowait + c->irq + c->softirq + c->steal;
    unsigned long long total_diff = c_total - p_total;
    unsigned long long idle_diff = (c->idle + c->iowait) - (p->idle + p->iowait);
    if (total_diff == 0) return 0.0f;
    return 100.0f * (total_diff - idle_diff) / total_diff;
}

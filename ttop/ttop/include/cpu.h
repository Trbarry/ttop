#ifndef CPU_H
#define CPU_H
typedef struct {
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
} CPUState;
int get_cpu_state(CPUState *state);
float calculate_cpu_usage(const CPUState *prev, const CPUState *curr);
#endif

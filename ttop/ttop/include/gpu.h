#ifndef GPU_H
#define GPU_H
#include <stdint.h>
typedef struct {
    char model[64];
    int usage_pct;
    uint64_t vram_total_kb;
    uint64_t vram_used_kb;
    float temp;
} GPUInfo;
int get_gpu_info(GPUInfo *gpus, int max_gpus);
#endif

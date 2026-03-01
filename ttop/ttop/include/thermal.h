#ifndef THERMAL_H
#define THERMAL_H
typedef struct {
    float temp;
} ThermalInfo;
int get_thermal_info(ThermalInfo *info);
#endif

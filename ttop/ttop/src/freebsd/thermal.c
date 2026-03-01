#include "thermal.h"
#include <sys/types.h>
#include <sys/sysctl.h>
#include <stdlib.h>
int get_thermal_info(ThermalInfo *info) {
    int temp;
    size_t sz = sizeof(temp);
    if (sysctlbyname("dev.cpu.0.temperature", &temp, &sz, NULL, 0) < 0) return -1;
    info->temp = (temp - 2732) / 10.0f;
    return 0;
}

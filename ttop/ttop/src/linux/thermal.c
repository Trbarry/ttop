#include "thermal.h"
#include <stdio.h>
#include <dirent.h>
#include <string.h>

int get_thermal_info(ThermalInfo *info) {
    info->temp = 0;
    // Try primary zone
    FILE *f = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    if (!f) f = fopen("/sys/class/thermal/thermal_zone1/temp", "r");
    
    if (f) {
        int temp;
        if (fscanf(f, "%d", &temp) == 1) info->temp = temp / 1000.0f;
        fclose(f);
        return 0;
    }
    
    // Search in hwmon as fallback
    for (int i = 0; i < 10; i++) {
        char path[128];
        snprintf(path, sizeof(path), "/sys/class/hwmon/hwmon%d/temp1_input", i);
        f = fopen(path, "r");
        if (f) {
            int temp;
            if (fscanf(f, "%d", &temp) == 1) info->temp = temp / 1000.0f;
            fclose(f);
            return 0;
        }
    }
    
    return -1;
}

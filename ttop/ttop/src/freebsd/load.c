#include "load.h"
#include <stdlib.h>
int get_load_info(LoadInfo *info) {
    double load[3];
    if (getloadavg(load, 3) != 3) return -1;
    info->load1 = load[0]; info->load5 = load[1]; info->load15 = load[2];
    return 0;
}

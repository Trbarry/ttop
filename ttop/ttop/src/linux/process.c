#define _DEFAULT_SOURCE
#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
int get_top_processes(ProcessInfo *p, int max) {
    DIR *d = opendir("/proc"); if (!d) return 0;
    struct dirent *e; int c = 0;
    while ((e = readdir(d)) && c < max) {
        if (!isdigit(e->d_name[0])) continue;
        char path[256]; snprintf(path, 256, "/proc/%s/stat", e->d_name);
        FILE *f = fopen(path, "r");
        if (f) {
            char comm[256]; unsigned long rss;
            if (fscanf(f, "%u (%[^)])", &p[c].pid, comm) == 2) {
                strncpy(p[c].name, comm, 31);
                for(int i=0; i<22; i++) fscanf(f, "%*s");
                if (fscanf(f, "%lu", &rss) == 1) { p[c].mem_rss_kb = rss * 4; p[c].cpu_usage = 0; c++; }
            }
            fclose(f);
        }
    }
    closedir(d); return c;
}

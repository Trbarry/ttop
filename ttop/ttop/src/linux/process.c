#define _DEFAULT_SOURCE
#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>

int get_top_processes(ProcessInfo *p, int max) {
    DIR *d = opendir("/proc");
    if (!d) return 0;
    struct dirent *e;
    int count = 0;
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size <= 0) page_size = 4096;

    for(int i=0; i<max; i++) p[i].mem_rss_kb = 0;

    while ((e = readdir(d))) {
        if (!isdigit(e->d_name[0])) continue;
        
        char path[256];
        snprintf(path, sizeof(path), "/proc/%s/status", e->d_name);
        FILE *f = fopen(path, "r");
        if (f) {
            char line[256];
            char name[256] = "unknown";
            uint64_t rss_kb = 0;
            int found_rss = 0;
            
            while (fgets(line, sizeof(line), f)) {
                if (strncmp(line, "Name:", 5) == 0) {
                    sscanf(line, "Name:\t%63s", name);
                } else if (strncmp(line, "VmRSS:", 6) == 0) {
                    sscanf(line, "VmRSS:\t%lu", &rss_kb);
                    found_rss = 1;
                    break;
                }
            }
            fclose(f);

            if (found_rss && rss_kb > 0) {
                // Insertion triée
                int insert_at = -1;
                for (int i = 0; i < count; i++) {
                    if (rss_kb > p[i].mem_rss_kb) {
                        insert_at = i;
                        break;
                    }
                }

                if (count < max || insert_at != -1) {
                    if (insert_at == -1) insert_at = count;
                    int move_count = (count < max ? count : max - 1) - insert_at;
                    if (move_count > 0) {
                        memmove(&p[insert_at + 1], &p[insert_at], move_count * sizeof(ProcessInfo));
                    }
                    p[insert_at].pid = atoi(e->d_name);
                    strncpy(p[insert_at].name, name, 31);
                    p[insert_at].name[31] = '\0';
                    p[insert_at].mem_rss_kb = rss_kb;
                    if (count < max) count++;
                }
            }
        }
    }
    closedir(d);
    return count;
}

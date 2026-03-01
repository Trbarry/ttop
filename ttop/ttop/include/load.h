#ifndef LOAD_H
#define LOAD_H
typedef struct {
    float load1, load5, load15;
} LoadInfo;
int get_load_info(LoadInfo *info);
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "daemon.h"
int daemonize(void) {
    pid_t pid = fork();
    if (pid < 0) return -1;
    if (pid > 0) exit(0);
    if (setsid() < 0) return -1;
    pid = fork();
    if (pid < 0) return -1;
    if (pid > 0) exit(0);
    umask(0);
    chdir("/");
    for (int x = sysconf(_SC_OPEN_MAX); x >= 0; x--) close(x);
    int fd = open("/dev/null", O_RDWR);
    dup(fd); dup(fd);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include "reporter.h"

static int sockfd = -1;
static struct sockaddr_in dest_addr;

int reporter_init(const char *host, int port) {
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) return -1;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    if (inet_aton(host, &dest_addr.sin_addr) == 0) return -1;
    return 0;
}

void reporter_send_bin(float cpu_usage, const MemoryInfo *mem, const ThermalInfo *thermal, 
                      uint64_t disk_r, uint64_t disk_w, const FSInfo *fs, 
                      const ProcessInfo *procs, int num_procs, const GPUInfo *gpus, int num_gpus) {
    if (sockfd < 0) return;
    ttop_packet_t packet;
    memset(&packet, 0, sizeof(packet));
    packet.version = TTOP_PROTO_VERSION;
    packet.timestamp = (uint64_t)time(NULL);
    packet.cpu_usage = cpu_usage;
    packet.mem_total = mem->total;
    packet.mem_avail = mem->available;
    packet.temp = thermal->temp;
    packet.disk_read = disk_r;
    packet.disk_write = disk_w;
    if (fs) packet.fs_used_pct = fs->used_pct;
    for (int i = 0; i < 3 && i < num_procs; i++) {
        packet.top_procs[i].pid = procs[i].pid;
        strncpy(packet.top_procs[i].name, procs[i].name, 15);
        packet.top_procs[i].name[15] = '\0';
        packet.top_procs[i].rss_kb = procs[i].mem_rss_kb;
    }
    if (num_gpus > 0 && gpus) {
        strncpy(packet.gpu.model, gpus[0].model, 15);
        packet.gpu.model[15] = '\0';
        packet.gpu.usage_pct = (int8_t)gpus[0].usage_pct;
        packet.gpu.vram_used_kb = gpus[0].vram_used_kb;
    } else packet.gpu.usage_pct = -1;
    sendto(sockfd, &packet, sizeof(packet), 0, (const struct sockaddr *)&dest_addr, sizeof(dest_addr));
}

void reporter_close(void) { if (sockfd >= 0) close(sockfd); }

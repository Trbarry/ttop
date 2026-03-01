#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>
#include <inttypes.h>
#include "memory.h"
#include "cpu.h"
#include "thermal.h"
#include "disk.h"
#include "load.h"
#include "uptime.h"
#include "network.h"
#include "daemon.h"
#include "reporter.h"
#include "webhook.h"
#include "process.h"
#include "filesystem.h"
#include "gpu.h"

static volatile sig_atomic_t stop = 0;
void handle_signal(int sig) { (void)sig; stop = 1; }

void save_config(const char *path, const char *webhook_url, int interval, const char *hostname) {
    FILE *f = fopen(path, "w");
    if (!f) return;
    if (webhook_url) fprintf(f, "webhook_url=%s\n", webhook_url);
    fprintf(f, "interval=%d\n", interval);
    fprintf(f, "hostname=%s\n", hostname);
    fclose(f);
    printf("Configuration sauvegardée dans %s\n", path);
}

void usage(const char *prog) {
    printf("ttop - Bare-metal lightweight system monitor\n\n");
    printf("Usage: %s [options]\n", prog);
    printf("  -d            Activer le mode démon (background)\n");
    printf("  -b            Utiliser le format binaire pour l'export UDP\n");
    printf("  -w url        URL du Webhook Discord/Slack\n");
    printf("  -i interval   Intervalle en secondes (défaut: 1)\n");
    printf("  -n hostname   Nom de l'hôte (défaut: auto)\n");
    printf("  -s            Sauvegarder les paramètres actuels dans ttop.conf\n");
    printf("  -f            Forcer l'envoi immédiat d'un webhook et quitter\n");
    printf("  -h host       Hôte UDP (défaut: 127.0.0.1)\n");
    printf("  -p port       Port UDP (défaut: 9999)\n\n");
    printf("Exemples :\n");
    printf("  1. Tester et sauvegarder un webhook :\n");
    printf("     %s -w \"https://discord.com/api/...\" -s -f\n\n", prog);
    printf("  2. Lancer en mode démon avec l'intervalle sauvegardé :\n");
    printf("     %s -d\n\n", prog);
    printf("  3. Envoyer un rapport unique avec un nom d'hôte spécifique :\n");
    printf("     %s -n \"Serveur-Prod\" -f\n", prog);
    exit(1);
}

void load_config(const char *path, char **webhook_url, int *interval, char *hostname) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        char *key = strtok(line, "=");
        char *val = strtok(NULL, "\n");
        if (key && val) {
            if (strcmp(key, "webhook_url") == 0) *webhook_url = strdup(val);
            else if (strcmp(key, "interval") == 0) *interval = atoi(val);
            else if (strcmp(key, "hostname") == 0) strncpy(hostname, val, 63);
        }
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    int is_daemon = 0, use_binary = 0, port = 9999, interval = 1;
    int save_cfg = 0, force_sync = 0;
    char *host = "127.0.0.1", *webhook_url = NULL;
    char hostname[64];
    gethostname(hostname, sizeof(hostname));

    load_config("/etc/ttop.conf", &webhook_url, &interval, hostname);
    load_config("ttop.conf", &webhook_url, &interval, hostname);

    int opt;
    while ((opt = getopt(argc, argv, "dbh:p:w:i:n:sf?")) != -1) {
        switch (opt) {
            case 'd': is_daemon = 1; break;
            case 'b': use_binary = 1; break;
            case 'h': host = optarg; break;
            case 'p': port = atoi(optarg); break;
            case 'w': webhook_url = strdup(optarg); break;
            case 'i': interval = atoi(optarg); break;
            case 'n': strncpy(hostname, optarg, sizeof(hostname)-1); break;
            case 's': save_cfg = 1; break;
            case 'f': force_sync = 1; break;
            case '?': usage(argv[0]); break;
            default: usage(argv[0]);
        }
    }
    // Handle --help or no args
    if (argc > 1 && strcmp(argv[1], "--help") == 0) usage(argv[0]);

    if (save_cfg) {
        save_config("ttop.conf", webhook_url, interval, hostname);
        if (!force_sync) return 0;
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    if (is_daemon) { if (daemonize() != 0) return 1; }
    else if (!force_sync) printf("ttop v%d démarré (intervalle: %ds, hôte: %s)\n", TTOP_PROTO_VERSION, interval, hostname);

    reporter_init(host, port);
    
    CPUState prev_cpu, curr_cpu;
    DiskStats prev_disk, curr_disk;
    NetworkInfo prev_net, curr_net;
    MemoryInfo mem; ThermalInfo thermal; LoadInfo load; UptimeInfo uptime;
    FSInfo fs[8];
    ProcessInfo procs[3]; GPUInfo gpus[1];
    
    get_cpu_state(&prev_cpu); get_disk_stats(&prev_disk); get_network_info(&prev_net);

    if (force_sync) {
        sleep(1); // Wait for deltas
        get_memory_info(&mem); get_thermal_info(&thermal); get_load_info(&load); get_uptime_info(&uptime);
        int n_fs = get_all_fs_info(fs, 8);
        int n_procs = get_top_processes(procs, 3);
        int n_gpus = get_gpu_info(gpus, 1);
        get_cpu_state(&curr_cpu); float cpu_usage = calculate_cpu_usage(&prev_cpu, &curr_cpu);
        get_disk_stats(&curr_disk);
        uint64_t disk_r = ((curr_disk.reads - prev_disk.reads) * 512) / 1024;
        uint64_t disk_w = ((curr_disk.writes - prev_disk.writes) * 512) / 1024;
        get_network_info(&curr_net);
        NetworkInfo deltas = curr_net;
        deltas.rx_kbps = 0; deltas.tx_kbps = 0;
        if (webhook_url) {
            if (webhook_send(webhook_url, hostname, cpu_usage, &mem, &thermal, disk_r, disk_w, &load, &uptime, &deltas, fs, n_fs, procs, n_procs, gpus, n_gpus) == 0)
                printf("Webhook envoyé avec succès !\n");
            else
                printf("Erreur lors de l'envoi du Webhook.\n");
        } else printf("Erreur: Aucun Webhook configuré.\n");
        return 0;
    }

    while (!stop) {
        sleep(interval); 
        get_memory_info(&mem); get_thermal_info(&thermal); get_load_info(&load); get_uptime_info(&uptime);
        int n_fs = get_all_fs_info(fs, 8);
        int n_procs = get_top_processes(procs, 3);
        int n_gpus = get_gpu_info(gpus, 1);
        
        float cpu_usage = 0;
        if (get_cpu_state(&curr_cpu) == 0) { cpu_usage = calculate_cpu_usage(&prev_cpu, &curr_cpu); prev_cpu = curr_cpu; }
        
        uint64_t disk_r = 0, disk_w = 0;
        if (get_disk_stats(&curr_disk) == 0) {
            disk_r = ((curr_disk.reads - prev_disk.reads) * 512) / 1024 / interval;
            disk_w = ((curr_disk.writes - prev_disk.writes) * 512) / 1024 / interval;
            prev_disk = curr_disk;
        }

        if (get_network_info(&curr_net) == 0) {
            NetworkInfo deltas = curr_net;
            uint64_t rx_sum = 0, tx_sum = 0;
            for(int i=0; i<curr_net.count; i++) {
                uint64_t rx = (curr_net.interfaces[i].rx_bytes > prev_net.interfaces[i].rx_bytes) ? (curr_net.interfaces[i].rx_bytes - prev_net.interfaces[i].rx_bytes) : 0;
                uint64_t tx = (curr_net.interfaces[i].tx_bytes > prev_net.interfaces[i].tx_bytes) ? (curr_net.interfaces[i].tx_bytes - prev_net.interfaces[i].tx_bytes) : 0;
                rx_sum += rx; tx_sum += tx;
            }
            deltas.rx_kbps = rx_sum / 1024 / interval;
            deltas.tx_kbps = tx_sum / 1024 / interval;
            if (webhook_url) webhook_send(webhook_url, hostname, cpu_usage, &mem, &thermal, disk_r, disk_w, &load, &uptime, &deltas, fs, n_fs, procs, n_procs, gpus, n_gpus);
            prev_net = curr_net;
        }

        if (use_binary) reporter_send_bin(cpu_usage, &mem, &thermal, disk_r, disk_w, &fs[0], procs, n_procs, gpus, n_gpus);

        if (!is_daemon) printf("[%s] CPU: %.2f%% | RAM: %.1f%% | FS: %d%% | GPU: %d%%\n", hostname, cpu_usage, mem.usage_percent, (n_fs > 0 ? fs[0].used_pct : 0), (n_gpus > 0 ? gpus[0].usage_pct : 0));
    }
    
    reporter_close();
    return 0;
}

# ttop - Bare-Metal Lightweight System Monitor

ttop est un moniteur système ultra-léger sans dépendances (C pur), conçu pour Linux (Proxmox, LXC) et FreeBSD (OPNsense).

## 🎯 Fonctionnalités Clés
- **Multi-OS** : Support natif Linux et FreeBSD.
- **Métriques Globales** : CPU, RAM, Swap, Load Average, Température.
- **Top Processus** : Les 3 processus les plus gourmands en mémoire (RSS).
- **Usage Disque** : Surveillance de l'espace disque (FS).
- **GPU** : Support NVIDIA (via nvidia-smi) et Intel/AMD.
- **Reporting** : 
  - UDP Binaire haute performance (V2).
  - Webhooks riches (Discord/Slack) avec embeds et couleurs d'alerte.
- **Démon** : Mode arrière-plan natif avec service systemd.

## 🛠️ Compilation
```bash
make
```

## 🚀 Utilisation
```bash
./ttop -i 5 -w "URL_WEBHOOK"
```
Options :
- `-d` : Mode démon.
- `-b` : Format binaire pour l'export UDP.
- `-w [url]` : URL du Webhook.
- `-i [sec]` : Intervalle.

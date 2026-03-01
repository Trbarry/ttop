# 📊 ttop - Bare-Metal Lightweight System Monitor

`ttop` est un moniteur système ultra-léger écrit en C, conçu pour Linux et FreeBSD (OPNsense). Il fournit des mesures précises et des rapports visuels via Webhook (Discord/Slack), avec une empreinte ressource minimale.

## 🎯 Fonctionnalités Clés

- **🚀 Performance** : Écrit en C "bare-metal", lecture directe de `/proc` et `/sys`.
- **📟 Visualisation Discord** : Rapports riches avec embeds, couleurs d'alerte et barres de progression ASCII.
- **🔌 Hardware complet** : Support CPU, RAM, Swap, Multi-disques dynamiques, GPU (Intel/AMD/Nvidia), Températures et Network.
- **🛡️ Hardened Security** : Permissions `600` sur la config, protection contre les injections shell et buffer overflows.
- **🤖 Automatisation** :
  - **Mode Démon** : `-d` pour un monitoring en continu.
  - **Mode Timer** : `-t hourly` pour un rapport automatique toutes les heures (via systemd).
- **⚙️ CLI Native** : Configuration et test de webhook directement depuis le terminal.

## 🛠️ Installation & Compilation

```bash
# Cloner le dépôt
git clone https://github.com/votre-repo/ttop.git
cd ttop

# Compiler
make

# Installer (nécessite sudo pour /usr/local/bin)
sudo make install
```

## 🚀 Utilisation (Exemples de l'Expert)

### 1. Configuration et test rapide
Configurez votre Webhook et envoyez un rapport de test immédiatement :
```bash
sudo ./ttop -w "https://discord.com/api/webhooks/..." -s -f
```

### 2. Activer le rapport horaire automatique (Sans démon)
La méthode la plus propre pour recevoir un statut toutes les heures sans consommer de RAM en continu :
```bash
sudo ./ttop -t hourly
```

### 3. Lancer en mode démon (Temps réel)
Pour un monitoring continu avec un envoi toutes les 5 minutes (300s) :
```bash
./ttop -d -i 300
```

## 📋 Arguments CLI

- `-w URL` : Définir l'URL du Webhook.
- `-s` : Sauvegarder les paramètres dans `ttop.conf`.
- `-f` : Forcer une synchronisation immédiate et quitter.
- `-t hourly` : Installer le timer systemd pour un rapport horaire.
- `-d` : Activer le mode démon (background).
- `-i SEC` : Intervalle en secondes (défaut: 1).
- `-n NAME` : Forcer le nom d'hôte affiché.

---

## 🛡️ Audit de Sécurité (Pentest Ready)

`ttop` a été audité pour garantir une sécurité maximale :
- **Secrets** : Fichier de config en `600` (lecture root uniquement).
- **Injections** : Protection contre les injections de commandes via un environnement shell contrôlé.
- **Mémoire** : Utilisation de `sscanf` avec limites de taille pour prévenir les buffer overflows.

---
*ttop v2 - Developed with precision.*

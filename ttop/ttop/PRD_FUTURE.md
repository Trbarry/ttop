# 🚀 ttop - Product Requirements Document (PRD) Future

## 🛠️ Vision du Projet
Faire de `ttop` le standard de monitoring "lightweight" pour les systèmes bare-metal (Routeurs, Serveurs critiques, IoT). Un outil qui allie la performance du C pur à la modernité des notifications cloud (Discord, Slack).

---

## 📅 Roadmap V3 : "The Network & Cloud Era"

### 1. Stack Réseau Native (Remplace `curl`)
*   **Objectif :** Supprimer la dépendance à `curl` et `popen()`.
*   **Action :** Implémenter une pile HTTP client minimale en C avec `openssl` (pour le HTTPS).
*   **Bénéfice :** Réduction radicale de la surface d'attaque et amélioration des performances d'envoi.

### 2. TUI (Terminal User Interface)
*   **Objectif :** Offrir un mode interactif local (type `htop`).
*   **Action :** Intégrer `ncurses` pour un tableau de bord visuel local en plus des webhooks.
*   **Bénéfice :** Monitoring temps réel sans avoir besoin de Discord.

### 3. Support OS Étendu (BSD Power)
*   **Objectif :** Parité de fonctionnalités totale entre Linux et FreeBSD.
*   **Action :** Finaliser les mesures GPU et Multi-Disques spécifiques à FreeBSD pour OPNsense.
*   **Bénéfice :** Unification du monitoring de parc hybride.

---

## 📅 Roadmap V4 : "The Intelligence Era"

### 1. Alerting Dynamique (Smart Notifications)
*   **Objectif :** Ne pas noyer l'utilisateur sous les messages inutiles.
*   **Action :** Envoyer un Webhook **uniquement** si une métrique dépasse un seuil (ex: CPU > 95% pendant 1 min).
*   **Bénéfice :** Alerting intelligent et réduction du bruit Discord.

### 2. Export Multi-Protocoles
*   **Objectif :** Intégration dans des écosystèmes plus larges.
*   **Action :** Ajouter des exporters pour **Prometheus**, **InfluxDB** et **Telegram**.
*   **Bénéfice :** Utilisation de `ttop` comme agent de collecte pour Grafana.

---

## 🛡️ Hardening & Qualité
*   **Tests Unitaires :** Mise en place d'une suite de tests complète simulant les fichiers `/proc` malveillants (Fuzzing).
*   **CI/CD :** Pipeline GitHub Actions pour la compilation multi-architectures (x86_64, ARM64).

---
*ttop : "Zero footprint, Full visibility."*

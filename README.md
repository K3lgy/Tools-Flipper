# Tools Flipper — Carnets de mission pour Flipper Zero

Ce dépôt regroupe six squelettes d'applications `.fap` (Flipper Zero, écrites en C avec uFBT) servant de **carnets de session/mission** pour la formation à l'audit de sécurité : chacune permet de saisir un identifiant de session, un périmètre autorisé, de tenir un journal d'observations horodatées, de cocher une checklist, puis d'exporter le tout en rapport Markdown via un script Python fourni (`tools/export_report.py`).

Aucun de ces projets n'embarque de logique d'acquisition réelle : les fonctions de capture matérielle (`capture_signal()`) sont des points d'intégration vides à compléter, qui se contentent aujourd'hui de journaliser une entrée générique.

## ⚠️ Avertissement — Usage pédagogique uniquement

**Ces outils sont fournis exclusivement dans un cadre de formation et d'apprentissage à la cybersécurité, sur du matériel et des périmètres pour lesquels vous disposez d'une autorisation écrite explicite.**

- Ils ne doivent en aucun cas être utilisés sur des systèmes, réseaux, badges, appareils ou personnes tiers sans consentement préalable et autorisation formelle.
- L'utilisation de ces outils en dehors d'un cadre légal et autorisé (test d'intrusion non mandaté, interception de communications, contournement de contrôle d'accès, etc.) peut constituer une infraction pénale selon la législation applicable (en France, notamment les articles 323-1 et suivants du Code pénal relatifs aux atteintes aux systèmes de traitement automatisé de données).
- **L'auteur de ce dépôt décline toute responsabilité quant à une utilisation détournée, illégale ou non autorisée de ces outils.** La responsabilité de l'usage qui en est fait incombe entièrement à l'utilisateur.
- En téléchargeant, compilant ou utilisant ces projets, vous reconnaissez en assumer seul les conséquences et vous engagez à les utiliser uniquement dans un cadre légal, éthique et autorisé.

## Projets inclus

| Dossier | Nom | Domaine | Description |
|---|---|---|---|
| `accessaudit/` | AccessAudit | RFID / NFC | Audit de badges de test (125 kHz, ISO14443…) |
| `hidseclab/` | HID Security Lab | USB HID | Validation de politiques de sécurité USB (émulation clavier/souris) sur machine de test |
| `gpiolab/` | GPIO Lab | GPIO / UART | Assistant de brochage et diagnostic GPIO |
| `rfsentinel/` | RF Sentinel | Sub-GHz (CC1101) | Diagnostic de signaux radio Sub-GHz autorisés |
| `irinspector/` | IR Inspector | Infrarouge | Inventaire/audit de télécommandes IR en environnement de test |
| `pentest_pocket/` | Pentest Pocket | Générique | Carnet de mission de pentest : notes horodatées (avec masquage automatique des valeurs sensibles préfixées `pw:`, `pass:`, `token:`, `key:`, `hash:`, `psk:`…), checklist de 12 tests |

Chaque sous-dossier contient son propre `README.md` avec le détail de ses fonctions et de sa compilation.

## Fonctions communes

- Identifiant de session/mission et périmètre autorisé déclaré dans l'app
- Point d'intégration matériel à compléter (`capture_signal()`), vide par défaut
- Journal d'observations horodatées
- Checklist dédiée à cocher/décocher
- Sauvegarde sur carte SD (`/ext/apps_data/<app>/session.txt` ou `mission.txt`)
- Export du journal en rapport Markdown via `tools/export_report.py`

## Compilation (pour chaque projet)

```bash
pip install ufbt
cd <nom_du_projet>
ufbt            # compile et produit dist/<nom_du_projet>.fap
ufbt launch     # installe et lance sur un Flipper Zero branché en USB
```

Le `.fap` compilé peut ensuite être copié dans `/ext/apps/Tools/` sur la carte SD du Flipper.

## Export des rapports

```bash
python3 tools/export_report.py session.txt -o rapport.md
```
(ou `mission.txt` pour Pentest Pocket)

## État du code

Ces applications sont des **squelettes/gabarits** : elles fournissent la structure de l'app (.fap), la navigation, la persistance sur SD et l'export, mais l'acquisition matérielle propre à chaque domaine (RFID/NFC, USB HID, GPIO/UART, Sub-GHz, IR) reste à implémenter dans `capture_signal()`, l'API exacte dépendant de la version du firmware/SDK Flipper utilisée.

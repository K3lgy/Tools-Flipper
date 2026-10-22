# HID Security Lab (hidseclab) — Flipper Zero

Tests de securite USB HID en laboratoire

> Outil de laboratoire pour la validation de politiques de securite USB HID sur une machine de test. A utiliser uniquement sur du materiel / un perimetre autorise par ecrit.

## Fonctions
- ID de session / Machine de laboratoire cible
- Lancer un scenario non destructif (point d'integration materiel a completer, voir `capture_signal()` dans `hidseclab.c`)
- Journal d'observations horodatees
- Checklist dediee
- Sauvegarde SD (`/ext/apps_data/hidseclab/session.txt`) puis export Markdown via `tools/export_report.py`

## Compilation
```bash
pip install ufbt
cd hidseclab
ufbt
ufbt launch
```

## Important
Ce squelette fournit la structure de l'app (.fap), la navigation, la persistance et l'export.
L'acquisition materielle propre a USB HID (emulation clavier/souris) (API exacte selon version de firmware/SDK) est a
brancher dans `capture_signal()` — la fonction journalise une entree generique en attendant.

## Export du rapport
```bash
python3 tools/export_report.py session.txt -o rapport.md
```

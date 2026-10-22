# GPIO Lab (gpiolab) — Flipper Zero

Assistant de brochage et diagnostic GPIO

> Outil de laboratoire pour le diagnostic de brochage GPIO/UART en laboratoire. A utiliser uniquement sur du materiel / un perimetre autorise par ecrit.

## Fonctions
- ID de session / Carte / adaptateur utilise
- Noter mesure GPIO (manuel) (point d'integration materiel a completer, voir `capture_signal()` dans `gpiolab.c`)
- Journal d'observations horodatees
- Checklist dediee
- Sauvegarde SD (`/ext/apps_data/gpiolab/session.txt`) puis export Markdown via `tools/export_report.py`

## Compilation
```bash
pip install ufbt
cd gpiolab
ufbt
ufbt launch
```

## Important
Ce squelette fournit la structure de l'app (.fap), la navigation, la persistance et l'export.
L'acquisition materielle propre a GPIO / UART (API exacte selon version de firmware/SDK) est a
brancher dans `capture_signal()` — la fonction journalise une entree generique en attendant.

## Export du rapport
```bash
python3 tools/export_report.py session.txt -o rapport.md
```

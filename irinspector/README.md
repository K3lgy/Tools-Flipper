# IR Inspector (irinspector) — Flipper Zero

Audit infrarouge de telecommandes de laboratoire

> Outil de laboratoire pour l'inventaire de telecommandes IR en environnement de test. A utiliser uniquement sur du materiel / un perimetre autorise par ecrit.

## Fonctions
- ID de session / Environnement de test autorise
- Noter protocole detecte (manuel) (point d'integration materiel a completer, voir `capture_signal()` dans `irinspector.c`)
- Journal d'observations horodatees
- Checklist dediee
- Sauvegarde SD (`/ext/apps_data/irinspector/session.txt`) puis export Markdown via `tools/export_report.py`

## Compilation
```bash
pip install ufbt
cd irinspector
ufbt
ufbt launch
```

## Important
Ce squelette fournit la structure de l'app (.fap), la navigation, la persistance et l'export.
L'acquisition materielle propre a infrarouge (recepteur/emetteur IR) (API exacte selon version de firmware/SDK) est a
brancher dans `capture_signal()` — la fonction journalise une entree generique en attendant.

## Export du rapport
```bash
python3 tools/export_report.py session.txt -o rapport.md
```

# RF Sentinel (rfsentinel) — Flipper Zero

Diagnostic radio Sub-GHz autorise

> Outil de laboratoire pour le diagnostic de signaux Sub-GHz autorises. A utiliser uniquement sur du materiel / un perimetre autorise par ecrit.

## Fonctions
- ID de session / Perimetre / bande autorisee
- Noter frequence observee (manuel) (point d'integration materiel a completer, voir `capture_signal()` dans `rfsentinel.c`)
- Journal d'observations horodatees
- Checklist dediee
- Sauvegarde SD (`/ext/apps_data/rfsentinel/session.txt`) puis export Markdown via `tools/export_report.py`

## Compilation
```bash
pip install ufbt
cd rfsentinel
ufbt
ufbt launch
```

## Important
Ce squelette fournit la structure de l'app (.fap), la navigation, la persistance et l'export.
L'acquisition materielle propre a Sub-GHz (CC1101) (API exacte selon version de firmware/SDK) est a
brancher dans `capture_signal()` — la fonction journalise une entree generique en attendant.

## Export du rapport
```bash
python3 tools/export_report.py session.txt -o rapport.md
```

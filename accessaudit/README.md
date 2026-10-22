# AccessAudit (accessaudit) — Flipper Zero

Audit RFID/NFC de badges de test

> Outil de laboratoire pour l'audit de badges RFID/NFC autorises. A utiliser uniquement sur du materiel / un perimetre autorise par ecrit.

## Fonctions
- ID de session / Perimetre / zone autorisee
- Identifier technologie (manuel) (point d'integration materiel a completer, voir `capture_signal()` dans `accessaudit.c`)
- Journal d'observations horodatees
- Checklist dediee
- Sauvegarde SD (`/ext/apps_data/accessaudit/session.txt`) puis export Markdown via `tools/export_report.py`

## Compilation
```bash
pip install ufbt
cd accessaudit
ufbt
ufbt launch
```

## Important
Ce squelette fournit la structure de l'app (.fap), la navigation, la persistance et l'export.
L'acquisition materielle propre a RFID 125kHz / NFC (ISO14443 etc.) (API exacte selon version de firmware/SDK) est a
brancher dans `capture_signal()` — la fonction journalise une entree generique en attendant.

## Export du rapport
```bash
python3 tools/export_report.py session.txt -o rapport.md
```

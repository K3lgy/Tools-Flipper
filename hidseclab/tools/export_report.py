#!/usr/bin/env python3
"""Convertit session.txt (exporte du Flipper, HID Security Lab) en rapport Markdown.

Usage : python3 export_report.py session.txt [-o rapport.md]
"""
import argparse
import datetime
import sys

CHECKLIST = [
    "Verification du controle d'acces USB (whitelist/blocklist)",
    "Scenario HID non destructif n'1 (detection de peripherique)",
    "Scenario HID non destructif n'2 (saisie benigne de test)",
    "Verification des alertes EDR/antivirus declenchees",
    "Journal des resultats",
    "Recommandations defensives consignees"
]


def parse(path):
    data = {"session": "", "target": "", "check": "", "notes": []}
    with open(path, encoding="utf-8", errors="replace") as f:
        for line in f:
            line = line.rstrip("\n")
            if line.startswith("SESSION="):
                data["session"] = line[8:]
            elif line.startswith("TARGET="):
                data["target"] = line[7:]
            elif line.startswith("CHECK="):
                data["check"] = line[6:]
            elif line.startswith("NOTE="):
                data["notes"].append(line[5:])
    return data


def build(data):
    done = sum(1 for c in data["check"] if c == "1")
    out = [
        f"# Rapport - HID Security Lab - {data['session'] or 'session non definie'}",
        "",
        f"_Genere le {datetime.datetime.now():%Y-%m-%d %H:%M}_",
        "",
        "## Machine de laboratoire cible",
        "",
        data["target"] or "_non defini_",
        "",
        f"## Checklist ({done}/{len(CHECKLIST)})",
        "",
        "| Test | Statut |",
        "|---|---|",
    ]
    for i, label in enumerate(CHECKLIST):
        ok = i < len(data["check"]) and data["check"][i] == "1"
        out.append(f"| {label} | {'OK Fait' if ok else 'A faire'} |")
    out += ["", "## Observations", ""]
    if data["notes"]:
        out += [f"- {n}" for n in data["notes"]]
    else:
        out.append("_Aucune observation._")
    out += ["", "---", "_Document de laboratoire. Verifier l'autorisation avant toute diffusion._", ""]
    return "\n".join(out)


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("input")
    ap.add_argument("-o", "--output", default="rapport_session.md")
    args = ap.parse_args()
    try:
        data = parse(args.input)
    except OSError as e:
        sys.exit(f"Erreur : {e}")
    with open(args.output, "w", encoding="utf-8") as f:
        f.write(build(data))
    print(f"Rapport ecrit : {args.output}")


if __name__ == "__main__":
    main()

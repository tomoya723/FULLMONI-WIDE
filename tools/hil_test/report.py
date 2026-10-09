"""Writes a run's results as JSON + a simple standalone HTML report."""
from __future__ import annotations

import html
import json
import os


def write(results: list[dict], out_dir: str):
    os.makedirs(out_dir, exist_ok=True)

    with open(os.path.join(out_dir, "results.json"), "w", encoding="utf-8") as f:
        json.dump(results, f, ensure_ascii=False, indent=2)

    n_pass = sum(1 for r in results if r["passed"])
    rows = []
    for r in results:
        status = "PASS" if r["passed"] else "FAIL"
        color = "#2e7d32" if r["passed"] else "#c62828"
        mismatches = "".join(f"<li>{html.escape(m)}</li>" for m in r["mismatches"])
        img_rel = os.path.basename(r["image_path"])
        rows.append(f"""
        <tr>
          <td><img src="{html.escape(img_rel)}" width="320"></td>
          <td>
            <b>{html.escape(r['id'])}</b> — <span style="color:{color}">{status}</span><br>
            <i>{html.escape(r['description'])}</i>
            <p><b>期待:</b> {html.escape(r['expect'])}</p>
            <p><b>観測:</b> {html.escape(r['observed'])}</p>
            <ul>{mismatches}</ul>
          </td>
        </tr>""")

    html_doc = f"""<!doctype html><meta charset="utf-8">
    <title>HIL test report</title>
    <style>
      body {{ font-family: sans-serif; }}
      table {{ border-collapse: collapse; width: 100%; }}
      td {{ border-bottom: 1px solid #ccc; padding: 8px; vertical-align: top; }}
    </style>
    <h1>HIL test report: {n_pass}/{len(results)} passed</h1>
    <table>{''.join(rows)}</table>
    """
    out_path = os.path.join(out_dir, "report.html")
    with open(out_path, "w", encoding="utf-8") as f:
        f.write(html_doc)
    return out_path

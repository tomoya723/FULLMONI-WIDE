#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
deploy_filter.py — ローカルの Filter (.py) を Open WebUI の DB に反映する

Open WebUI の Function の実体は DB の function テーブルの content カラムにある。
ディスク上のファイルを直しても読まれないので、これまでは管理画面で全文を貼り直す
必要があった (README §5)。出先やスマホからはそれが困難なため、DB を直接更新して
コンテナを再起動するところまでを1コマンドにする。

安全策:
  - 更新前に該当行を JSON でローカルへバックアップする
  - id が見つからなければ、存在する id の一覧を出して何もせず終了する（決め打ちしない）
  - --dry-run で差分だけ表示できる
  - 再起動後に /health を叩いて復帰を確認する
"""

from __future__ import annotations

import argparse
import base64
import datetime as dt
import json
import shutil
import subprocess
import sys
import tempfile
import time
import urllib.request
from pathlib import Path
from typing import Any, List, Optional

if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    sys.stderr.reconfigure(encoding="utf-8", errors="replace")

DEFAULT_CONTAINER = "open-webui"
DEFAULT_DB_PATH = "/app/backend/data/webui.db"


def log(*a: Any) -> None:
    print(f"[{dt.datetime.now():%H:%M:%S}]", *a, flush=True)


def run(cmd: List[str]) -> subprocess.CompletedProcess:
    return subprocess.run(cmd, capture_output=True)


def out(cp: subprocess.CompletedProcess) -> str:
    return (cp.stdout or b"").decode("utf-8", "replace").strip()


def err(cp: subprocess.CompletedProcess) -> str:
    return (cp.stderr or b"").decode("utf-8", "replace").strip()


# コンテナ内で動く本体。content は base64 で渡して引用符の問題を避ける。
_APPLY_PY = r'''# -*- coding: utf-8 -*-
import base64, json, sqlite3, sys, time

db_path, fn_id, b64_path, valves_path, mode = sys.argv[1:6]

conn = sqlite3.connect(db_path)
conn.row_factory = sqlite3.Row
cols = [r[1] for r in conn.execute("pragma table_info(function)")]

rows = conn.execute("select * from function").fetchall()
ids = [r["id"] for r in rows]
target = None
for r in rows:
    if r["id"] == fn_id:
        target = r
        break

if target is None:
    print(json.dumps({"ok": False, "reason": "not_found", "ids": ids,
                      "cols": cols}, ensure_ascii=False))
    raise SystemExit(0)

old = {k: target[k] for k in target.keys()}
for k, v in list(old.items()):
    if isinstance(v, bytes):
        old[k] = v.decode("utf-8", "replace")

with open(b64_path, "r", encoding="utf-8") as f:
    new_content = base64.b64decode(f.read()).decode("utf-8")

new_valves = None
if valves_path != "-":
    with open(valves_path, "r", encoding="utf-8") as f:
        new_valves = json.load(f)

result = {"ok": True, "ids": ids, "cols": cols,
          "old_len": len(old.get("content") or ""), "new_len": len(new_content),
          "old_valves": old.get("valves"), "backup": old}

if mode == "apply":
    sets, params = ["content = ?"], [new_content]
    if "updated_at" in cols:
        sets.append("updated_at = ?")
        params.append(int(time.time()))
    if new_valves is not None and "valves" in cols:
        cur = old.get("valves")
        merged = {}
        if isinstance(cur, str) and cur.strip():
            try:
                merged = json.loads(cur)
            except Exception:
                merged = {}
        elif isinstance(cur, dict):
            merged = dict(cur)
        merged.update(new_valves)
        sets.append("valves = ?")
        params.append(json.dumps(merged, ensure_ascii=False))
        result["new_valves"] = merged
    params.append(fn_id)
    conn.execute("update function set %s where id = ?" % ", ".join(sets), params)
    conn.commit()
    result["applied"] = True

print(json.dumps(result, ensure_ascii=False, default=str))
'''


def wait_healthy(container: str, timeout: int = 120) -> bool:
    """再起動後の復帰を待つ。/health は llama.cpp ではなく Open WebUI 側のもの。"""
    deadline = time.time() + timeout
    while time.time() < deadline:
        cp = run(["docker", "exec", container, "python", "-c",
                  "import urllib.request;"
                  "urllib.request.urlopen('http://127.0.0.1:8080/health', timeout=3);"
                  "print('ok')"])
        if cp.returncode == 0 and "ok" in out(cp):
            return True
        time.sleep(3)
    return False


def main() -> int:
    ap = argparse.ArgumentParser(
        description="ローカルの Filter (.py) を Open WebUI の DB に反映する"
    )
    ap.add_argument("--file", required=True, help="反映する .py のパス")
    ap.add_argument("--id", required=True, help="function テーブルの id")
    ap.add_argument("--container", default=DEFAULT_CONTAINER)
    ap.add_argument("--db-path", default=DEFAULT_DB_PATH)
    ap.add_argument("--valves", default="", help='上書きする Valves の JSON 文字列')
    ap.add_argument(
        "--valves-file",
        default="",
        help="上書きする Valves を書いた JSON ファイル。"
             "PowerShell からネイティブコマンドを呼ぶと JSON 文字列の二重引用符が"
             "剥がれるため、こちらを使うこと",
    )
    ap.add_argument("--backup-dir", default="", help="バックアップ先 (既定: スクリプトと同じ場所の backups/)")
    ap.add_argument("--dry-run", action="store_true", help="差分だけ表示して更新しない")
    ap.add_argument("--no-restart", action="store_true",
                    help="再起動しない。※ Open WebUI は Function モジュールをキャッシュするため、"
                         "再起動しないと変更が効かない")
    ap.add_argument("--list", action="store_true", help="function の id 一覧だけ出して終了")
    args = ap.parse_args()

    src = Path(args.file)
    if not args.list and not src.is_file():
        log("エラー: ファイルが無い:", src)
        return 2

    tmp = Path(tempfile.mkdtemp(prefix="owui_deploy_"))
    try:
        content = src.read_text(encoding="utf-8") if src.is_file() else ""
        b64 = tmp / "content.b64"
        b64.write_text(base64.b64encode(content.encode("utf-8")).decode("ascii"), encoding="utf-8")

        valves_file = "-"
        raw_valves = args.valves
        if args.valves_file:
            vf = Path(args.valves_file)
            if not vf.is_file():
                log("エラー: --valves-file が無い:", vf)
                return 2
            raw_valves = vf.read_text(encoding="utf-8")

        if raw_valves.strip():
            try:
                parsed = json.loads(raw_valves)
            except json.JSONDecodeError as e:
                log("エラー: Valves が JSON として読めない:", e)
                log("  受け取った値:", repr(raw_valves))
                return 2
            vp = tmp / "valves.json"
            vp.write_text(json.dumps(parsed, ensure_ascii=False), encoding="utf-8")
            valves_file = "/tmp/owui_valves.json"
            if run(["docker", "cp", str(vp), f"{args.container}:{valves_file}"]).returncode != 0:
                log("エラー: valves の転送に失敗")
                return 1

        apply_py = tmp / "apply.py"
        apply_py.write_text(_APPLY_PY, encoding="utf-8")

        for local, remote in ((b64, "/tmp/owui_content.b64"), (apply_py, "/tmp/owui_apply.py")):
            cp = run(["docker", "cp", str(local), f"{args.container}:{remote}"])
            if cp.returncode != 0:
                log("エラー: docker cp 失敗:", err(cp))
                return 1

        mode = "list" if (args.dry_run or args.list) else "apply"
        cp = run(["docker", "exec", args.container, "python", "/tmp/owui_apply.py",
                  args.db_path, args.id, "/tmp/owui_content.b64", valves_file, mode])
        if cp.returncode != 0:
            log("エラー: コンテナ内での実行に失敗:", err(cp))
            return 1

        try:
            res = json.loads(out(cp))
        except json.JSONDecodeError:
            log("エラー: 応答を解釈できない:", out(cp))
            return 1

        if not res.get("ok"):
            log(f"function id '{args.id}' が見つからない。存在する id:")
            for i in res.get("ids", []):
                print("   -", i)
            return 2

        log(f"対象: {args.id}")
        log(f"  content: {res['old_len']} 文字 -> {res['new_len']} 文字")
        if res.get("old_valves") is not None:
            log(f"  現在の Valves: {res['old_valves']}")
        if res.get("new_valves") is not None:
            log(f"  新しい Valves: {json.dumps(res['new_valves'], ensure_ascii=False)}")

        if args.list:
            log("function の id 一覧:")
            for i in res.get("ids", []):
                print("   -", i)
            return 0

        if args.dry_run:
            log("dry-run のため更新しなかった")
            return 0

        # バックアップ（更新後だが、backup には更新前の行が入っている）
        bdir = Path(args.backup_dir) if args.backup_dir else Path(__file__).resolve().parent / "backups"
        bdir.mkdir(parents=True, exist_ok=True)
        bfile = bdir / f"{args.id}_{dt.datetime.now():%Y-%m-%d_%H%M%S}.json"
        bfile.write_text(json.dumps(res.get("backup", {}), ensure_ascii=False, indent=2), encoding="utf-8")
        log("更新前の行をバックアップ:", bfile)
        log("DB を更新した")

        if args.no_restart:
            log("再起動していない。Open WebUI は Function をキャッシュするので、"
                "反映には docker restart が必要")
            return 0

        log("コンテナを再起動する …")
        cp = run(["docker", "restart", args.container])
        if cp.returncode != 0:
            log("エラー: 再起動に失敗:", err(cp))
            return 1
        if wait_healthy(args.container):
            log("Open WebUI 復帰を確認した")
        else:
            log("警告: 復帰確認がタイムアウトした。docker logs を確認すること")
            return 1
        return 0
    finally:
        run(["docker", "exec", args.container, "rm", "-f",
             "/tmp/owui_content.b64", "/tmp/owui_apply.py", "/tmp/owui_valves.json"])
        shutil.rmtree(tmp, ignore_errors=True)


if __name__ == "__main__":
    sys.exit(main())

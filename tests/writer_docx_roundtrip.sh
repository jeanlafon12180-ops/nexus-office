#!/usr/bin/env bash
set -euo pipefail

SOFFICE="$1"
WORK_DIR="$(mktemp -d)"
trap 'rm -rf "$WORK_DIR"' EXIT

mkdir -p "$WORK_DIR/docx" "$WORK_DIR/roundtrip" "$WORK_DIR/profile"
cat > "$WORK_DIR/source.html" <<'HTML'
<!doctype html>
<html><head><meta charset="utf-8"><title>Nexus Writer test</title></head>
<body><h1>Nexus Writer DOCX round-trip</h1><p>Unique test phrase: NEXUS_WRITER_ROUNDTRIP_OK</p></body></html>
HTML

"$SOFFICE" "-env:UserInstallation=file://$WORK_DIR/profile" --headless \
  --convert-to 'docx:Office Open XML Text' --outdir "$WORK_DIR/docx" "$WORK_DIR/source.html" >/dev/null
test -s "$WORK_DIR/docx/source.docx"

"$SOFFICE" "-env:UserInstallation=file://$WORK_DIR/profile" --headless \
  --convert-to html --outdir "$WORK_DIR/roundtrip" "$WORK_DIR/docx/source.docx" >/dev/null
test -s "$WORK_DIR/roundtrip/source.html"
grep -q "NEXUS_WRITER_ROUNDTRIP_OK" "$WORK_DIR/roundtrip/source.html"

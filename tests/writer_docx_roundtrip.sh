#!/usr/bin/env bash
set -euo pipefail

SOFFICE="$1"
WORK_DIR="$(mktemp -d)"
trap 'rm -rf "$WORK_DIR"' EXIT

mkdir -p "$WORK_DIR/docx" "$WORK_DIR/roundtrip" "$WORK_DIR/profile"
cat > "$WORK_DIR/source.html" <<'HTML'
<!doctype html>
<html>
<head><meta charset="utf-8"><title>Nexus Writer test</title></head>
<body>
  <h1>Nexus Writer DOCX round-trip</h1>
  <p><strong>Unique test phrase:</strong> NEXUS_WRITER_ROUNDTRIP_OK</p>
  <p>Texte français avec accents : é, è, à, ç, œ.</p>
  <ul><li>Premier élément</li><li>Deuxième élément</li></ul>
  <table border="1">
    <tr><th>Colonne A</th><th>Colonne B</th></tr>
    <tr><td>Cellule test</td><td>42</td></tr>
  </table>
</body>
</html>
HTML

# Export a representative rich-text document to DOCX using the same
# explicit LibreOffice filter as Nexus Writer.
"$SOFFICE" "-env:UserInstallation=file://$WORK_DIR/profile" --headless \
  --convert-to 'docx:Office Open XML Text' --outdir "$WORK_DIR/docx" "$WORK_DIR/source.html" >/dev/null
DOCX="$WORK_DIR/docx/source.docx"
test -s "$DOCX"

# Reopen the generated DOCX as HTML and verify that text and key content
# survive the import/export cycle.
"$SOFFICE" "-env:UserInstallation=file://$WORK_DIR/profile" --headless \
  --convert-to html --outdir "$WORK_DIR/roundtrip" "$DOCX" >/dev/null
ROUNDTRIP="$WORK_DIR/roundtrip/source.html"
test -s "$ROUNDTRIP"
grep -q "NEXUS_WRITER_ROUNDTRIP_OK" "$ROUNDTRIP"
grep -q "Cellule test" "$ROUNDTRIP"
grep -q "Premier élément" "$ROUNDTRIP"
grep -q "Texte français" "$ROUNDTRIP"

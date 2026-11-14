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

"$SOFFICE" "-env:UserInstallation=file://$WORK_DIR/profile" --headless \
  --convert-to 'docx:Office Open XML Text' --outdir "$WORK_DIR/docx" "$WORK_DIR/source.html" >/dev/null
DOCX="$WORK_DIR/docx/source.docx"
test -s "$DOCX"

"$SOFFICE" "-env:UserInstallation=file://$WORK_DIR/profile" --headless \
  --convert-to html --outdir "$WORK_DIR/roundtrip" "$DOCX" >/dev/null
ROUNDTRIP="$WORK_DIR/roundtrip/source.html"
test -s "$ROUNDTRIP"

# HTML output may encode accents and punctuation as entities. Parse it as
# HTML before checking the actual recovered text, rather than grepping markup.
python3 - "$ROUNDTRIP" <<'PY'
import sys
from html.parser import HTMLParser
from pathlib import Path

class TextExtractor(HTMLParser):
    def __init__(self):
        super().__init__(convert_charrefs=True)
        self.parts = []
    def handle_data(self, data):
        self.parts.append(data)

parser = TextExtractor()
parser.feed(Path(sys.argv[1]).read_text(encoding="utf-8", errors="replace"))
text = " ".join(" ".join(parser.parts).split())
required = [
    "NEXUS_WRITER_ROUNDTRIP_OK",
    "Cellule test",
    "Premier élément",
    "Texte français",
]
missing = [value for value in required if value not in text]
if missing:
    raise SystemExit("DOCX round-trip lost expected content: " + ", ".join(missing))
PY

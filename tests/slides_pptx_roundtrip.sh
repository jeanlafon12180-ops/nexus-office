#!/usr/bin/env bash
set -euo pipefail
SOFFICE="${1:?path to soffice required}"
PYTHON="${2:-python3}"
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
cat > "$WORK/slides.json" <<'JSON'
[
  {"title":"Nexus Slides PPTX Test","body":"Editable presentation content\nSecond line"},
  {"title":"Second slide","body":"Another slide"}
]
JSON
"$PYTHON" "$ROOT/tools/slides_odp.py" create "$WORK/slides.json" "$WORK/fixture.odp"
mkdir "$WORK/out" "$WORK/profile"
"$SOFFICE" "-env:UserInstallation=file://$WORK/profile" --headless --convert-to pptx --outdir "$WORK/out" "$WORK/fixture.odp" >/dev/null
test -s "$WORK/out/fixture.pptx"
mkdir "$WORK/back" "$WORK/profile2"
"$SOFFICE" "-env:UserInstallation=file://$WORK/profile2" --headless --convert-to odp --outdir "$WORK/back" "$WORK/out/fixture.pptx" >/dev/null
test -s "$WORK/back/fixture.odp"
"$PYTHON" "$ROOT/tools/slides_odp.py" extract "$WORK/back/fixture.odp" "$WORK/back/content.xml"
grep -q "Nexus Slides PPTX Test" "$WORK/back/content.xml"
grep -q "Editable presentation content" "$WORK/back/content.xml"
grep -q "Second slide" "$WORK/back/content.xml"
echo "Slides PPTX import/export round-trip passed."

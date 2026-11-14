#!/usr/bin/env bash
set -euo pipefail
SOFFICE="${1:?path to soffice required}"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
cat > "$WORK/fixture.fodp" <<'FODP'
<?xml version="1.0" encoding="UTF-8"?>
<office:document xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0"
 xmlns:draw="urn:oasis:names:tc:opendocument:xmlns:drawing:1.0"
 xmlns:text="urn:oasis:names:tc:opendocument:xmlns:text:1.0"
 office:version="1.2" office:mimetype="application/vnd.oasis.opendocument.presentation" office:class="presentation">\n <office:automatic-styles/><office:master-styles>
 <office:body><office:presentation>
  <draw:page draw:name="Slide1" draw:master-page-name="Default">
   <draw:frame><draw:text-box>
    <text:p>Nexus Slides PPTX Test</text:p>
    <text:p>Editable presentation content</text:p>
   </draw:text-box></draw:frame>
  </draw:page>
 </office:presentation></office:body>
</office:document>
FODP
mkdir "$WORK/out" "$WORK/profile"
"$SOFFICE" "-env:UserInstallation=file://$WORK/profile" --headless --convert-to pptx --outdir "$WORK/out" "$WORK/fixture.fodp" >/dev/null
test -s "$WORK/out/fixture.pptx"
mkdir "$WORK/back" "$WORK/profile2"
"$SOFFICE" "-env:UserInstallation=file://$WORK/profile2" --headless --convert-to fodp --outdir "$WORK/back" "$WORK/out/fixture.pptx" >/dev/null
test -s "$WORK/back/fixture.fodp"
grep -q "Nexus Slides PPTX Test" "$WORK/back/fixture.fodp"
grep -q "Editable presentation content" "$WORK/back/fixture.fodp"
echo "Slides PPTX import/export round-trip passed."

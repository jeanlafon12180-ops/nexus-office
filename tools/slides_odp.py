#!/usr/bin/env python3
"""Create a minimal OpenDocument Presentation package or extract its content.xml."""
import json
import sys
import zipfile
from pathlib import Path
from xml.sax.saxutils import escape

MIMETYPE = "application/vnd.oasis.opendocument.presentation"

def create(source_json: Path, output_odp: Path) -> None:
    slides = json.loads(source_json.read_text(encoding="utf-8"))
    pages = []
    for index, slide in enumerate(slides, 1):
        paragraphs = [f"<text:p>{escape(slide.get('title', ''))}</text:p>"]
        paragraphs.extend(f"<text:p>{escape(line)}</text:p>" for line in slide.get("body", "").splitlines())
        pages.append(
            f'<draw:page draw:name="Slide{index}" draw:style-name="dp1">'
            '<draw:frame presentation:class="outline" svg:width="25cm" svg:height="15cm" svg:x="1cm" svg:y="1cm">'
            '<draw:text-box>' + "".join(paragraphs) + '</draw:text-box></draw:frame></draw:page>'
        )
    content = '''<?xml version="1.0" encoding="UTF-8"?>
<office:document-content xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0"
 xmlns:style="urn:oasis:names:tc:opendocument:xmlns:style:1.0"
 xmlns:text="urn:oasis:names:tc:opendocument:xmlns:text:1.0"
 xmlns:draw="urn:oasis:names:tc:opendocument:xmlns:drawing:1.0"
 xmlns:presentation="urn:oasis:names:tc:opendocument:xmlns:presentation:1.0"
 xmlns:svg="urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0"
 office:version="1.2"><office:automatic-styles><style:style style:name="dp1" style:family="drawing-page"/></office:automatic-styles>
 <office:body><office:presentation>''' + "".join(pages) + '''</office:presentation></office:body></office:document-content>'''
    styles = '''<?xml version="1.0" encoding="UTF-8"?>
<office:document-styles xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0"
 xmlns:style="urn:oasis:names:tc:opendocument:xmlns:style:1.0"
 office:version="1.2"><office:styles/><office:automatic-styles/><office:master-styles/></office:document-styles>'''
    manifest = '''<?xml version="1.0" encoding="UTF-8"?>
<manifest:manifest xmlns:manifest="urn:oasis:names:tc:opendocument:xmlns:manifest:1.0" manifest:version="1.2">
 <manifest:file-entry manifest:full-path="/" manifest:media-type="application/vnd.oasis.opendocument.presentation"/>
 <manifest:file-entry manifest:full-path="content.xml" manifest:media-type="text/xml"/>
 <manifest:file-entry manifest:full-path="styles.xml" manifest:media-type="text/xml"/>
</manifest:manifest>'''
    with zipfile.ZipFile(output_odp, "w") as archive:
        info = zipfile.ZipInfo("mimetype")
        info.compress_type = zipfile.ZIP_STORED
        archive.writestr(info, MIMETYPE)
        archive.writestr("content.xml", content)
        archive.writestr("styles.xml", styles)
        archive.writestr("META-INF/manifest.xml", manifest)

def extract(source_odp: Path, output_xml: Path) -> None:
    with zipfile.ZipFile(source_odp) as archive:
        output_xml.write_bytes(archive.read("content.xml"))

if __name__ == "__main__":
    if len(sys.argv) != 4:
        raise SystemExit("Usage: slides_odp.py create input.json output.odp | extract input.odp output.xml")
    mode, source, destination = sys.argv[1:]
    if mode == "create":
        create(Path(source), Path(destination))
    elif mode == "extract":
        extract(Path(source), Path(destination))
    else:
        raise SystemExit("Unknown mode: " + mode)

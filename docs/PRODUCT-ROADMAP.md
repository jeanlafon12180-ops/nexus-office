# Product roadmap

## Product direction

Nexus Office is a desktop-first office suite. The primary product is an application installed and run locally on the user's computer. A future browser-based edition is a separate, later project.

## Phase 1 — Real desktop editor

- [x] Build an initial rich-text Writer prototype for HTML and plain-text files.
- [ ] Pin and validate a LibreOfficeKit version.
- [ ] Implement the LibreOfficeKit backend behind the Nexus document-engine interface.
- [ ] Build a Writer canvas with tiled rendering and keyboard/mouse input.
- [ ] Implement save and save-as workflows.
- [ ] Test document round-tripping and error handling.

The initial Writer prototype is not yet a Word-compatible editor. DOCX support remains blocked on the LibreOfficeKit backend and round-trip tests.

## Phase 2 — Desktop suite

- [ ] Add Sheets.
- [ ] Add Slides.
- [ ] Add PDF viewing and export workflows.
- [ ] Improve settings, recent files, and accessibility.

## Phase 3 — Distribution website

The official website will present the product and offer platform-specific installers. The download buttons must only be enabled when signed/release artifacts have actually been built and tested.

Planned download targets:
- Windows: installer package (for example, .exe or .msi).
- Linux: distribution packages such as .deb and/or AppImage, selected after dependency testing.
- macOS: signed application bundle distributed in a .dmg.

Each release should display its version, supported operating systems, architecture, file size, release notes, and checksum. The website must never link to placeholder or nonexistent installers.

## Phase 4 — Future online edition

A browser edition may be considered after the desktop product is stable. It will require its own architecture for document storage, authentication, collaboration, privacy, and server-side execution. It is not part of the initial desktop release.

# LibreOfficeKit integration plan

## Goal

Use LibreOfficeKit as the document engine behind the Nexus Office interface while keeping Nexus-owned UI code independent from LibreOfficeKit implementation details.

## Current status

**Preparation and adapter boundary only.** The application does not yet load, render, edit, or save office documents through LibreOfficeKit.

The document-engine interface and unavailable backend exist. They are not a real document editor.

## Confirmed technical direction

LibreOfficeKit exposes a C/C++ interface. Its tiled-editing model is the relevant path for a custom desktop UI:

- Initialize a LibreOfficeKit instance from a LibreOffice installation directory.
- Load a document through the Office API.
- Call `initializeForRendering()` after loading.
- Render document regions into pixel buffers with `paintTile()`.
- Forward keyboard and mouse input through `postKeyEvent()` and `postMouseEvent()`.
- Receive engine events through document callbacks, including tile invalidation and cursor/selection updates.
- Save through the document's `saveAs()` API.

The tiled-rendering/editing surface is marked unstable in the upstream headers and requires `LOK_USE_UNSTABLE_API` before including LibreOfficeKit headers. This creates an API-version and compatibility obligation: pin and test the LibreOffice version used for each release.

Reference: upstream LibreOfficeKit README and headers. See the upstream documentation before implementing against a particular LibreOffice release.

## Required stages

### 1. Pin a supported LibreOffice version

For Windows, Linux, and macOS separately, record:
- Exact LibreOffice release/build used
- Header and runtime locations
- Runtime libraries and data files required at launch
- How the application locates the LibreOffice installation
- Distribution and packaging requirements
- License notices and source-availability obligations

Do not assume that one platform's package layout applies to the others.

### 2. Implement the real backend

Create a `LibreOfficeKitDocumentEngine` implementation behind the Nexus-owned `DocumentEngine` interface.

Responsibilities:
- Initialize and shut down the engine safely
- Convert local file paths to valid file URLs
- Load and close a document
- Initialize tiled rendering
- Render bounded tiles into a Qt-compatible image buffer
- Forward keyboard and mouse events from the editor canvas
- Receive callbacks safely and schedule UI updates on the Qt GUI thread
- Save to a chosen output path and report errors accurately

Do not call unstable methods unless the selected LibreOffice build exposes the expected API.

### 3. Build the first Writer editing surface

Start with Writer only:
1. Open an ODT test document.
2. Display its rendered page.
3. Forward keyboard and mouse events.
4. Refresh invalidated tiles and selection/cursor overlays.
5. Save a copy and reopen it to verify the saved result.

Only after this vertical slice works should Sheets and Slides be added.

### 4. Verify compatibility and safety

Test:
- Empty, large, and multi-page documents
- Unicode text and common fonts
- Read-only and password-protected documents
- Invalid paths and unsupported formats
- Engine shutdown while a document is open
- Save failures and interrupted operations

### 5. Package the desktop application

The installer must include or reliably locate the matching LibreOffice runtime and all required dependencies. Test on clean machines without a developer environment.

A successful Qt-shell build is not evidence that the LibreOfficeKit backend or its runtime packaging works.

## Licensing checklist

Review the exact LibreOffice version, linked libraries, bundled runtime files, and all transitive dependencies. Preserve notices and satisfy applicable license terms before distributing builds. See [LICENSING.md](LICENSING.md).

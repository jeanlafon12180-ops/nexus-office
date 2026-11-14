# LibreOfficeKit integration plan

## Goal

Use LibreOfficeKit as the document engine behind the Nexus Office interface while keeping Nexus-owned UI code independent from LibreOfficeKit implementation details.

## Current status

**Preparation only.** The application does not currently load, render, edit, or save office documents through LibreOfficeKit.

The CMake option `NEXUS_ENABLE_LIBREOFFICEKIT` is OFF by default. Turning it on only checks for a header and library; it does not activate document editing.

## Required stages

### 1. Confirm platform support

For Windows, Linux, and macOS separately, record:
- Supported LibreOfficeKit release and development package
- Header and library locations
- Runtime libraries and data files required at launch
- Distribution and packaging requirements
- License notices and source-availability obligations

Do not assume that one platform's package layout applies to the others.

### 2. Verify the API

Before writing the backend, inspect the exact installed LibreOfficeKit headers and official API documentation. Confirm the supported lifecycle and callback APIs for:
- Creating and destroying a LibreOfficeKit instance
- Loading and closing a document
- Rendering document pages or tiles
- Sending editing commands
- Saving and exporting
- Receiving status, error, and document-change callbacks

### 3. Add a Nexus-owned adapter

Create a small interface in the Nexus source tree. UI code should depend on that interface, not on LibreOfficeKit headers or raw engine handles.

The adapter should report unsupported operations explicitly and must not silently claim that a document was saved or changed.

### 4. Implement and test capabilities incrementally

Recommended order:
1. Engine discovery and initialization
2. Open a test document read-only
3. Render the first page
4. Close documents and release resources safely
5. Save a copy
6. Enable editing commands
7. Verify ODT, ODS, ODP, DOCX, XLSX, and PPTX compatibility separately

### 5. Add platform CI and packaging

Build and test the backend on each target platform with the matching runtime dependencies. A successful Qt-shell build is not evidence that LibreOfficeKit integration works.

## Licensing checklist

Review the exact LibreOfficeKit version, linked libraries, bundled runtime files, and all transitive dependencies. Preserve notices and satisfy applicable license terms before distributing builds. See [LICENSING.md](LICENSING.md).

# Nexus Office

**A modern, cross-platform office suite with a custom desktop experience.**

Nexus Office aims to provide a clean, familiar productivity experience inspired by Microsoft Office, with its own visual identity and an office engine based on LibreOffice technology.

> **Project status: Early prototype (0.2.0).** The Qt desktop shell is in place. This release adds the first Nexus-owned document-engine interface and a safe placeholder backend. Real document loading, editing, and conversion are not implemented yet.

## Planned applications

| App | Purpose | Current status |
| --- | --- | --- |
| Nexus Writer | Word processing | Landing page only |
| Nexus Sheets | Spreadsheets | Landing page only |
| Nexus Slides | Presentations | Landing page only |
| Nexus PDF | PDF workflows | Landing page only |

## Target platforms

- Windows
- Linux
- macOS

## Technology

- **C++17**
- **Qt 6 Widgets**
- **CMake 3.21+**
- **LibreOfficeKit:** planned document-engine backend; not connected yet

## Build the desktop preview

Install a C++17 compiler, CMake 3.21 or newer, and Qt 6 with the Widgets module.

From the repository root:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Run the generated `nexus-office` executable from the build directory. The exact path depends on your operating system and build generator.

LibreOfficeKit is not required for this preview. The current application does not call its API. See [the integration plan](docs/LIBREOFFICEKIT-INTEGRATION.md).

## Current foundations

- Native Qt desktop window with navigation and light/dark themes
- Nexus-owned `DocumentEngine` interface, separated from the UI
- Explicit unavailable backend that refuses document operations instead of pretending they succeeded
- Automated contract test for the unavailable backend
- File picker in the UI is selection-only; no document is opened or changed

## Next milestones

1. Validate and improve the desktop shell on Windows, Linux, and macOS.
2. Verify LibreOfficeKit headers, runtime dependencies, API, and licensing on each platform.
3. Implement and test the LibreOfficeKit backend for document loading and rendering.
4. Add Writer, Sheets, and Slides workflows.
5. Add PDF viewing and export workflows.
6. Package and test platform-specific installers.

## Licensing

Nexus Office's licensing model is not finalized. LibreOffice and its components have their own licenses and notices. See [docs/LICENSING.md](docs/LICENSING.md) before copying, modifying, linking, or redistributing third-party code.

## Disclaimer

Nexus Office is an independent project and is not affiliated with or endorsed by The Document Foundation or Microsoft.

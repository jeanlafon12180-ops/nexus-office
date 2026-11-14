# Nexus Office

**A modern, cross-platform office suite with a custom desktop experience.**

Nexus Office aims to provide a clean, familiar productivity experience inspired by Microsoft Office, with its own visual identity and an office engine based on LibreOffice technology.

> **Project status: Early prototype (0.1.0).** The current code provides a Qt desktop shell with navigation, module landing pages, file selection dialogs, and light/dark themes. It does not yet edit or convert documents.

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
- **LibreOfficeKit:** planned document-engine integration; not connected yet

## Build the desktop preview

Install a C++17 compiler, CMake 3.21 or newer, and Qt 6 with the Widgets module.

From the repository root:

```bash
cmake -S . -B build
cmake --build build
```

Run the generated `nexus-office` executable from the build directory. The exact path depends on your operating system and build generator.

## Current features

- Native Qt desktop window
- Sidebar navigation between Home, Writer, Sheets, Slides, and PDF
- Module landing pages
- Light and dark themes
- File picker for selecting a document (selection only; no document is opened or changed)

## Next milestones

1. Validate and improve the desktop shell on Windows, Linux, and macOS.
2. Integrate LibreOfficeKit and verify document loading and editing.
3. Add Writer, Sheets, and Slides workflows.
4. Add PDF viewing and export workflows.
5. Package and test platform-specific installers.

## Licensing

Nexus Office's licensing model is not finalized. LibreOffice and its components have their own licenses and notices. See [docs/LICENSING.md](docs/LICENSING.md) before copying, modifying, linking, or redistributing third-party code.

## Disclaimer

Nexus Office is an independent project and is not affiliated with or endorsed by The Document Foundation or Microsoft.

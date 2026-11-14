# Nexus Office

**A modern, cross-platform office suite built around LibreOffice technology.**

Nexus Office aims to provide a clean, familiar productivity experience inspired by Microsoft Office, with its own visual identity.

> **Project status:** Early development. No production-ready editor or installer is available yet.

## Planned applications

| App | Purpose | Status |
| --- | --- | --- |
| Nexus Writer | Word processing | Planned |
| Nexus Sheets | Spreadsheets | Planned |
| Nexus Slides | Presentations | Planned |
| Nexus PDF | PDF viewing and workflows | Planned |

## Target platforms

- Windows
- Linux
- macOS

## Technology direction

- **LibreOffice / LibreOfficeKit:** document engine and interoperability research
- **C++ and Qt:** native desktop application and user interface
- **CMake:** build configuration

The integration approach is being evaluated. The first milestone is a desktop shell; document editing will be integrated only after validating the relevant LibreOfficeKit APIs and platform requirements.

## Development

Requirements for the initial desktop shell:

- CMake 3.21 or newer
- A C++17 compiler
- Qt 6 Widgets

Build:

```bash
cmake -S . -B build
cmake --build build
```

Run the generated executable from the build directory. The exact executable path depends on the platform and generator.

## Repository structure

```text
apps/             Application modules (planned)
docs/             Architecture and project documentation
src/              Initial desktop shell
branding/         Brand assets (planned)
```

## Licensing

Nexus Office's licensing model is not finalized. LibreOffice and its components have their own licenses and notices. See [docs/LICENSING.md](docs/LICENSING.md) before copying, modifying, linking, or redistributing any third-party code.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).

## Disclaimer

Nexus Office is an independent project and is not affiliated with or endorsed by The Document Foundation or Microsoft. Product names are used only to describe compatibility goals or project plans.

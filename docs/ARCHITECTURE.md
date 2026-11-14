# Architecture

## Current milestone: desktop shell (0.1.0)

The first milestone is a native Qt 6 Widgets application that establishes the Nexus Office window, navigation concept, and visual language.

The shell is not yet connected to a document engine. File selection is only a user-interface demonstration: selecting a file does not open, edit, or convert it.

## Planned layers

1. **Desktop shell** — application window, navigation, settings, theming.
2. **Document-engine adapter** — a small Nexus-owned interface that isolates the UI from LibreOfficeKit-specific calls.
3. **LibreOfficeKit backend** — document loading, rendering, editing, saving, and lifecycle management, subject to API and platform validation.
4. **Product modules** — Writer, Sheets, Slides, and PDF workflows.
5. **Packaging** — platform-specific builds and installers.
6. **Quality** — automated tests, accessibility, localization, and compatibility checks.

## Integration principles

- Keep the user interface separate from document-engine integration.
- Keep LibreOfficeKit-specific types out of general UI classes.
- Validate the supported LibreOfficeKit API surface and runtime requirements on each target platform before committing to editor features.
- Treat document loading, rendering, editing, saving, and shutdown as separate testable capabilities.
- Avoid copying third-party source until its license and required notices have been reviewed.
- Keep platform-specific code isolated behind small interfaces.
- Do not enable the optional CMake dependency switch until the matching development package is installed and verified.

## Current integration status

CMake contains an opt-in dependency check for LibreOfficeKit. It is disabled by default and does not mean the engine is integrated. The application continues to build as a standalone Qt shell.

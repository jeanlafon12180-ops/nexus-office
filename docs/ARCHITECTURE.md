# Architecture

## Current milestone: document-engine boundary (0.2.0)

The application has a native Qt 6 desktop shell. This milestone introduces a Nexus-owned C++ interface for document-engine operations and a placeholder implementation that reports the engine as unavailable.

The placeholder deliberately refuses to open, save, or close documents. It prevents the UI or future modules from claiming that a document operation succeeded before a real backend exists.

## Layers

1. **Desktop shell** — application window, navigation, settings, theming.
2. **Document-engine interface** — `nexus::documents::DocumentEngine`, independent of Qt and LibreOfficeKit.
3. **LibreOfficeKit backend** — engine lifecycle, document loading, tiled rendering, keyboard/mouse input, callbacks, and saving.
4. **Editor canvas** — Qt widget that displays rendered tiles and forwards user input to the backend.
5. **Product modules** — Writer first, then Sheets, Slides, and PDF workflows.
6. **Packaging** — platform-specific builds and installers with the matching office runtime.
7. **Quality** — automated tests, accessibility, localization, and compatibility checks.

## Integration principles

- Keep the user interface separate from document-engine integration.
- Keep LibreOfficeKit-specific types out of general UI classes and the public engine interface.
- Report unsupported operations explicitly; never report success for an operation that was not performed.
- LibreOfficeKit tiled editing uses an upstream API surface marked unstable; pin and test a specific LibreOffice release.
- Receive engine callbacks safely and marshal UI updates to the Qt GUI thread.
- Treat initialization, loading, rendering, input, saving, and shutdown as separate testable capabilities.
- Avoid copying third-party source until its license and required notices have been reviewed.
- Keep platform-specific code isolated behind small interfaces.

## Current integration status

The document-engine boundary and unavailable backend are implemented. LibreOfficeKit is not yet linked or called. The optional CMake dependency check is only preliminary and does not constitute an integration.

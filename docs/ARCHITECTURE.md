# Architecture

## Current milestone: document-engine boundary (0.2.0)

The application has a native Qt 6 desktop shell. This milestone introduces a Nexus-owned C++ interface for document-engine operations and a placeholder implementation that reports the engine as unavailable.

The placeholder deliberately refuses to open, save, or close documents. It prevents the UI or future modules from claiming that a document operation succeeded before a real backend exists.

## Layers

1. **Desktop shell** — application window, navigation, settings, theming.
2. **Document-engine interface** — `nexus::documents::DocumentEngine`, independent of Qt and LibreOfficeKit.
3. **Backend implementations** — the current unavailable backend, followed by a separately implemented LibreOfficeKit backend.
4. **Product modules** — Writer, Sheets, Slides, and PDF workflows.
5. **Packaging** — platform-specific builds and installers.
6. **Quality** — automated tests, accessibility, localization, and compatibility checks.

## Integration principles

- Keep the user interface separate from document-engine integration.
- Keep LibreOfficeKit-specific types out of general UI classes and the public engine interface.
- Report unsupported operations explicitly; never report success for an operation that was not performed.
- Validate the supported LibreOfficeKit API surface and runtime requirements on each target platform before committing to editor features.
- Treat initialization, loading, rendering, editing, saving, and shutdown as separate testable capabilities.
- Avoid copying third-party source until its license and required notices have been reviewed.
- Keep platform-specific code isolated behind small interfaces.

## Current integration status

The document-engine boundary and unavailable backend are implemented. LibreOfficeKit is not yet linked or called. The optional CMake dependency check remains OFF by default and is only a preliminary dependency check, not an integration.

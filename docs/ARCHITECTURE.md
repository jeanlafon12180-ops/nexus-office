# Architecture

## Current milestone: desktop shell

The first milestone is a native Qt 6 Widgets application that establishes the Nexus Office window, navigation concept, and visual language.

The shell is not yet connected to a document engine. Its document action cards are intentionally disabled.

## Planned layers

1. **Desktop shell** — application window, navigation, settings, theming.
2. **Document integration** — investigate LibreOfficeKit capabilities and platform support.
3. **Product modules** — Writer, Sheets, Slides, and PDF workflows.
4. **Packaging** — platform-specific builds and installers.
5. **Quality** — automated tests, accessibility, localization, and compatibility checks.

## Integration principles

- Keep the user interface separate from document-engine integration.
- Validate the supported LibreOfficeKit API surface before committing to editor features.
- Avoid copying third-party source until its license and required notices have been reviewed.
- Keep platform-specific code isolated behind small interfaces.

# ReaderView0 active highlight close icon (API 3)

## Problem

The active highlight swatch publishes a portable `Close` icon. Its foreground
and `stroke_color` were both the resolved Focus color. Re10 and Lectern0 use
`stroke_color` as the icon raster backdrop, so the identical colors collapsed
the X into a solid square.

## Shared fix

ReaderView0 continues to publish the resolved Focus color as the close glyph
foreground and now publishes the selected swatch color as its backdrop. This
keeps product-neutral icon semantics in the shared view layer and lets both
hosts rasterize a contrasting X without host-specific policy.

The draw-record test requires the `Close` identity, Focus foreground, selected
swatch backdrop, and distinct foreground/background colors.

## Dependency reconciliation

The unchanged UI0 API 91 dependency remains at commit
`cadafcacdae8e63cf0d2b505f54e2a2a228c0bec`. Its source closure is reconciled
to the already-promoted zero_foundation commit
`fa7f680f933c23d84f9b74e15887a3b8bb78d2f9`, version `0.4.3-dev`.

## Validation

- `scripts/require_ui0_dependency_current.ps1`
- `scripts/audit_architecture.ps1`
- `build/win32_build.bat`

The strict ReaderView0 suite passes with the reconciled clean dependency
closure.

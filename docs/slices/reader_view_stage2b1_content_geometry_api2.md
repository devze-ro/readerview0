# Reader View Stage 2B-1: Content Geometry API 2

Status: implemented

Date: 2026-07-17

## Outcome

Readerview0 advances from API 1 / `0.1.0-dev` to API 2 / `0.2.0-dev` and adds
the smallest shared geometry mechanism required for re10/lectern0 visual
parity. Existing chrome composition, state, capacities, actions, focus,
semantics, and responsive layout behavior are unchanged.

API 2 exact-pins UI0 API 90 at
`fda99de484d50f1b019b1edfe3489f57fae57f9a`. UI0 owns the separate six-profile
resolved theme catalog. Readerview0 does not wrap or duplicate that catalog.

## Public contract

The following constants publish the re10 reference defaults:

| Constant | Value | Meaning |
| --- | ---: | --- |
| `READER_VIEW_DEFAULT_PAGE_HORIZONTAL_INSET` | 24 | reserve on each side before a sub-max-width page |
| `READER_VIEW_DEFAULT_PAGE_MAX_WIDTH` | 660 | centered page-surface width cap |
| `READER_VIEW_DEFAULT_PAGE_MIN_WIDTH` | 160 | narrow-viewport page-width floor |
| `READER_VIEW_DEFAULT_CONTENT_INSET_X` | 52 | page-to-content horizontal inset |
| `READER_VIEW_DEFAULT_CONTENT_INSET_Y` | 68 | page-to-content vertical inset |
| `READER_VIEW_DEFAULT_CONTENT_MIN_WIDTH` | 80 | content-width floor |
| `READER_VIEW_DEFAULT_CONTENT_MIN_HEIGHT` | 48 | content-height floor |

`ReaderViewContentGeometryStyle` contains those seven scalar fields.
`reader_view_default_content_geometry_style()` returns the exact values above.

`ReaderViewContentGeometry` contains:

- `viewport_rect`: the caller-provided outer rendering viewport;
- `page_surface_rect`: the centered bounded page surface; and
- `content_rect`: the inset EPUB content/rendering rectangle.

`reader_view_resolve_content_geometry(viewport, style, out_geometry)` copies
the supplied style for one call. A null style selects the defaults. The caller
owns the output record. A missing output, empty viewport, negative inset,
non-positive width/minimum, or page minimum greater than page maximum fails
closed. Intermediate-coordinate overflow also fails closed. When output
storage exists it is zeroed before failure.

The default calculation is intentionally identical to re10's current Stage 1
reader-view adoption:

```text
available_width = viewport.w - 2 * 24
page_width = min(660, max(available_width, 160))
page_x = viewport.x + max((viewport.w - page_width) / 2, 0)
page_surface = (page_x, viewport.y, page_width, viewport.h)
content = (page_x + 52,
           viewport.y + 68,
           max(80, page_width - 104),
           max(48, viewport.h - 136))
```

## Ownership and lifetime

The operation is callback-free, allocation-free, renderer-free, document-free,
and deterministic. It stores no state, borrows no projection, and uses no
process-global mutable data. It depends only on UI0 scalar and rectangle types.

Applications continue to own EPUB rendering, document frames, images,
selection/highlight painting, theme persistence, page colors, renderer/font
bindings, native windows, accessibility adapters, and product policy.

## Exact dependency closure

- UI0 commit: `fda99de484d50f1b019b1edfe3489f57fae57f9a`;
- UI0 API: 90;
- UI0 version: `0.1.0-dev`;
- zero_foundation source closure:
  `eee57edc1b0c7af5bef7afca26f3c27a32fb6e7c`;
- zero_foundation version: `0.4.0-dev`; and
- no reader0 or direct zero_foundation package dependency.

## Deterministic validation

Strict C11 `/W4 /WX` tests cover:

- the exact 1400x780 Stage 2B-0 wide viewport result;
- the exact 1080 px docked viewport result;
- the exact 940 px narrow viewport result;
- the 24 px reserve below the 660 px cap;
- minimum page/content dimensions;
- a caller-provided alternate scalar style;
- repeat-call byte stability;
- fail-closed invalid style, empty viewport, missing output, and coordinate
  overflow; and
- all existing API 1 layout, chrome, action, focus, semantic, and diagnostic
  tests against the exact UI0 API 90 source closure.

## Deferred adoption

This slice changes neither re10 nor lectern0. Lectern0 adopts UI0 API 90 and
readerview0 API 2 during Stage 2B-2 together with draw-adapter and projection
conformance. Re10 adoption remains Stage 2B-4 after a new parallel-editor gate
and explicit reconciliation. No readerview0 remote creation or push is
authorized.

# readerview0 architecture

## Ownership

Readerview0 owns only proven-common UI0-composed EPUB reader chrome:

- responsive top-toolbar composition and overflow;
- left TOC/Find and right annotations/bookmarks panel shells;
- page gutters, progress geometry, settings and row-action popups, selection
  tools, and the bounded note-editor dialog;
- allocation-free page-surface and content geometry over a caller-provided
  central viewport and scalar style;
- deterministic layout, UI0 draw records, text bindings, and portable semantic
  records;
- caller-owned, fixed-capacity transient interaction state; and
- bounded actions for the application to execute.

The public contract is a projection/action boundary. It does not name reader0
types. Hosts translate their concrete reader0 and persistence state into
borrowed projections for one build and translate returned actions back into
application commands.

## Explicit non-ownership

- reader0 owns EPUB document, layout, typography, semantic navigation, search
  result state, selection state, history, and canonical frames;
- applications own persistence and stable record IDs, annotation records,
  bookmarks, capture, commands, decoded-image caches, platform windows,
  rendering integration, and accessibility adapters; and
- UI0 owns generic UI primitives, controls, layout, signals, drawing records,
  and theme tokens.

Readerview0 must not become a generic document-provider layer. It must not own
WIC or decoded-image policy, a database, a renderer, a native window, a global
theme, or hidden mutable reader state.

## Dependency direction

The direction is:

```text
application -> readerview0 API 2 -> UI0 API 90
```

Readerview0 source-consumes no lower implementation. The final application
compiles UI0 exactly once and compiles UI0's explicit zero_foundation base-text
source closure exactly once. Readerview0 exact-pins the compatible revisions in
`vendor/ui0_dependency`.

The source-closure revision is a consumer-build compatibility record, not a
direct package dependency. It may advance when the same UI0 API and manifest
compile unchanged against a newer compatible zero_foundation revision; such an
advance requires the full strict package test and architecture audit.

There is deliberately no reader0 or direct zero_foundation dependency.

## Lifetime and capacity

`ReaderViewProjection` pointers and strings are borrowed for one
`reader_view_build` call. `ReaderViewState` and `ReaderViewFrameStorage` are
caller-owned and have fixed public capacities. Returned frame slices point into
that storage and remain valid only until the next build using it. Action text
may also point into caller-owned state or borrowed projection text and follows
the shorter lifetime.

`ReaderViewContentGeometryStyle` is copied for one geometry call. The returned
`ReaderViewContentGeometry` is written into caller storage and contains values
only; readerview0 retains neither the style nor any rectangle.

No operation allocates, retains host records, calls a host callback, mutates a
database, or uses process-global mutable state.

## Focus and accessibility

Readerview0 owns portable focus IDs, focus order, popup/modal containment, and
semantic roles/states. `reader_view_accessibility_focus` and
`reader_view_accessibility_invoke` queue operations through the same bounded
state and action path used by pointer and keyboard input.

Applications own native accessibility objects, platform event translation,
screen-reader announcements, and action execution. API 2 does not redesign
either host's native adapter.

## Central viewport

`ReaderViewLayout.viewport_rect` is the outer application rendering contract.
`reader_view_resolve_content_geometry` derives the centered page surface and
inner content rectangle from that viewport. Its default style publishes the
re10-reference 24 px horizontal reserve, 660/160 px page widths, 52/68 px
content insets, and 80/48 px content minima. The operation accepts no document
state and emits no draw command.

The package may reserve page gutters and overlay/dock panel geometry, but it
never draws EPUB content or owns document frames, decoded images,
selection/highlight geometry, or renderer resources. Applications choose
which UI0 resolved profile colors to use and execute all page painting. A host
recomputes the layout after a frame reports a layout-affecting state change.

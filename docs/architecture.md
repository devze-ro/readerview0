# readerview0 architecture

## Ownership

Readerview0 owns only proven-common UI0-composed EPUB reader chrome:

- the fixed accepted top-toolbar composition and responsive containment;
- left TOC/Find and right annotations/bookmarks panel shells;
- page gutters, progress geometry, settings and row-action popups, selection
  tools, and the bounded note-editor dialog;
- allocation-free atomic page-surface and content geometry in the authoritative
  layout, plus a standalone caller-provided viewport/scalar-style resolver;
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
application -> readerview0 API 3 -> UI0 API 91
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
screen-reader announcements, and action execution. API 3 does not redesign
either host's native adapter.

## Central viewport

`ReaderViewLayout.viewport_rect`, `page_surface_rect`, and `content_rect` are
one authoritative application rendering contract computed from the same
transient panel/chrome state. The accepted geometry is 56 px top chrome, 38 px
footer, 420/320 px panel widths, 12 px panel inset, and 14 px panel-to-page
gaps. The default centered page style publishes the re10-reference 24 px
horizontal reserve, 660/160 px page widths, 52/68 px content insets, and 80/48
px content minima. `reader_view_resolve_content_geometry` remains as the
document-free standalone form for an explicit viewport/style; neither path
emits draw commands.

Loaded toolbar geometry is a fixed twelve-slot row aligned to the right edge:
eleven shared icon controls in the accepted order, then one host-owned trailing
slot. Open is an empty-document action only. Visible controls are icon-only,
while portable semantics keep their accessible names. `chrome_title` is an
explicit portable projection distinct from `document_title`; readerview0 does
not substitute the host document title into accepted chrome.

The package may reserve page gutters and overlay/dock panel geometry, but it
never draws EPUB content or owns document frames, decoded images,
selection/highlight geometry, or renderer resources. Applications choose
which UI0 resolved profile colors to use and execute all page painting. A host
recomputes the layout after a frame reports a layout-affecting state change.

Public progress locations and semantic ranges remain unsigned 64-bit document
values. Counts through `INT32_MAX` map one-to-one onto UI0's signed 32-bit
slider; larger counts use a deterministic, allocation-free monotonic scale
with exact endpoints and overflow-safe round-half-up arithmetic. The bounded
UI adapter therefore never truncates the range returned in a SeekLocation
action or advertised to native accessibility adapters.

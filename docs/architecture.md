# readerview0 architecture

## Ownership

Readerview0 owns only proven-common UI0-composed EPUB reader chrome:

- responsive top-toolbar composition and overflow;
- left TOC/Find and right annotations/bookmarks panel shells;
- page gutters, progress geometry, settings and row-action popups, selection
  tools, and the bounded note-editor dialog;
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
application -> readerview0 API 1 -> UI0 API 89
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

No operation allocates, retains host records, calls a host callback, mutates a
database, or uses process-global mutable state.

## Focus and accessibility

Readerview0 owns portable focus IDs, focus order, popup/modal containment, and
semantic roles/states. `reader_view_accessibility_focus` and
`reader_view_accessibility_invoke` queue operations through the same bounded
state and action path used by pointer and keyboard input.

Applications own native accessibility objects, platform event translation,
screen-reader announcements, and action execution. API 1 does not redesign
either host's native adapter.

## Central viewport

`ReaderViewLayout.viewport_rect` is the application rendering contract. The
package may reserve page gutters and overlay/dock panel geometry, but it never
draws EPUB content or owns document frames, decoded images, selection geometry,
or renderer resources. A host recomputes the layout after a frame reports a
layout-affecting state change.

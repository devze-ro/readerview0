# readerview0 architecture reservation

## Intended ownership

Readerview0 will own only reader-view state and UI0-composed chrome proven
common by re10 and lectern0. Candidate surfaces for the next audited slice are
the top reader toolbar, left TOC/Find panel shell, right annotation-panel shell,
and theme/font control presentation.

Candidate status is not ownership. Each surface must first be compared at the
data, action, lifetime, accessibility, and persistence boundaries.

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

## Dependency direction under review

The likely direction is application -> readerview0 -> UI0, with reader0 data
passed explicitly by the application. Whether the first API should directly
name reader0 frame/navigation types or consume a smaller caller-built view
input is intentionally undecided until the two-host audit. No dependency pin is
established in Slice 4B.

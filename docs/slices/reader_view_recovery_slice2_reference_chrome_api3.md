# Reader View recovery Slice 2: reference chrome API 3

> [!NOTE]
> This is a historical engineering record. See the
> [project README](../../README.md) for current information.

Date: 2026-07-17

Status: implemented and locally validated; host adoption is validated separately

## Outcome

Readerview0 advances to API 3 / `0.3.0-dev` and restores the shared foundation
needed to reproduce the accepted pre-extraction re10 Reader View. The reference
direction is frozen re10 first, extracted re10 second, and 8vo third. A
match between the two extracted hosts is not an acceptance result if it differs
from the frozen re10 reference.

This is a forward extraction recovery, not a revert. Readerview0 continues to
own only portable UI0-composed chrome and bounded interaction state. Hosts keep
their concrete EPUB engine, renderer, persistence, native window and
accessibility integration, and product policy.

## API 3 contract

`ReaderViewProjection` adds `chrome_title`. It is the explicit portable title
shown by accepted loaded chrome and is distinct from the host-owned
`document_title`. Readerview0 never substitutes the document title into the
fixed chrome title position.

`ReaderViewLayout` now returns `page_surface_rect` and `content_rect` alongside
`viewport_rect`. These rectangles are resolved atomically from one bounds,
feature, document-flag, and transient-panel snapshot. A host must paint the
page and EPUB content using those returned rectangles instead of independently
reconstructing central geometry.

`ReaderViewTextBinding` now carries one finite portable `ReaderViewTextStyle`.
The shared frame distinguishes the accepted chrome title, chrome metadata, and
menu-item text paths from the default UI0 text path. The record contains no
font object, native handle, callback, provider, or allocation; each host maps
the finite style to its existing concrete text renderer. This preserves the
accepted title scale, menu metrics, and progress-footer metrics without moving
font lifetime or native rendering into readerview0.

Focusable chrome semantics also carry a finite `ReaderViewSemanticControl`
identity for the toolbar, page gutters, and progress slider. Native adapters
use that identity when synchronizing focus; they must not infer control meaning
from localized or caller-provided accessible names. Projected menu choices keep
their caller-provided `source_key` alongside the portable control identity.

The following public constants lock the accepted chrome scalars:

| Constant | Value |
| --- | ---: |
| `READER_VIEW_REFERENCE_TOP_CHROME_HEIGHT` | 56 |
| `READER_VIEW_REFERENCE_FOOTER_HEIGHT` | 38 |
| `READER_VIEW_REFERENCE_LEFT_PANEL_WIDTH` | 420 |
| `READER_VIEW_REFERENCE_RIGHT_PANEL_WIDTH` | 320 |
| `READER_VIEW_REFERENCE_PANEL_INSET` | 12 |
| `READER_VIEW_REFERENCE_PANEL_PAGE_GAP` | 14 |

The existing standalone `reader_view_resolve_content_geometry` operation and
its 24 px page reserve, 660/160 px page widths, 52/68 px content insets, and
80/48 px content minima remain available. Layout applies the old re10
`available_x` / `available_width` calculation exactly, including the narrow
panel-clamped edge; it does not derive the page from a synthetic minimum-width
viewport.

## Accepted toolbar

Loaded chrome has twelve fixed 30 by 28 px slots, an 8 px ordinary gap, and an
18 px gap before the post-navigation group. The row is inset 20 px from the
right and 10 px from the top. Slots 0 through 10 are shared; slot 11 is reserved
for the host trailing Exit action.

The toolbar surface remains semantic-only: it groups the controls but emits no
full-width painted band. Each icon control retains the accepted standard UI0
button shell. The host trailing action must use the same shell and canonical
icon treatment when it occupies the reserved slot.

The shared visible order is:

1. Contents;
2. Find;
3. Back;
4. Forward;
5. Full screen / Exit full screen;
6. Size;
7. Spacing;
8. Font;
9. Theme;
10. Annotations; and
11. Bookmark.

Every shared toolbar control is icon-only but retains its projected accessible
name, state, value, focus, and existing bounded action path. A loaded document
never exposes Open. The empty-document Open action from recovery Slice 1
remains interactive and uses the shared Book Open icon.

At the accepted 1400 by 780 client size, the shared toolbar rectangle is
`(922,10,420,28)` and the host trailing slot is `(1350,10,30,28)`. At the
accepted 940 by 520 narrow client size they are `(462,10,420,28)` and
`(890,10,30,28)` respectively. The toolbar does not reorder or switch to a
different overflow model at the narrow reference size.

## Central geometry

For a loaded 1400 by 780 client with no panel:

- body and viewport: `(0,56,1400,686)`;
- page surface: `(370,56,660,686)`;
- content: `(422,124,556,550)`;
- progress hit/semantic rectangle: `(370,760,660,18)`;
- previous/next hot rectangles: `(4,56,366,686)` and
  `(1030,56,366,686)`; and
- previous/next visible gutter rectangles: `(165,355,44,88)` and
  `(1191,355,44,88)`.

With both wide panels open, the left and right panels are
`(12,56,420,686)` and `(1068,56,320,686)`. The page becomes
`(458,56,584,686)` and content becomes `(510,124,480,550)`, preserving the
14 px panel-to-page reserves.

For the 940 by 520 narrow reference with no panel, page and content are
`(140,56,660,426)` and `(192,124,556,290)`. A deterministic two-panel edge
test locks the old re10 clamped calculation: viewport `(434,56,172,426)`, page
`(458,56,160,426)`, and content `(510,124,80,290)`.

At the existing 580 by 780 host-reader width with both panels open, API 3 keeps
the old minimum-page rule instead of rejecting the frame: viewport
`(444,66,168,686)`, page `(468,66,160,686)`, and content
`(520,134,80,550)` for bounds `(10,10,580,780)`. The page/progress minimum may
extend past the residual narrow body after both fixed panel reserves; this is
the accepted old calculation and keeps every chrome interaction live.

Full screen and distraction-free states atomically hide shared chrome and
panels before calculating the expanded page. Bounds too small to contain the
fixed toolbar or the page/content minima fail closed with a zeroed layout.

## Paging, progress, and Font popup

Page navigation keeps the accepted large edge hit areas and 44 by 88 visible
gutter geometry. The visible chevron appears for hover, press, or
focus-visible state; the old `Prev` / `Next` text is not emitted. Pointer and
accessibility invocation continue to return the existing PreviousPage and
NextPage actions.

Progress uses the entire accepted page width and preserves its semantic slider
range, focus, keyboard movement, and SeekLocation action. Its visible thumb is
suppressed while idle and appears only for hover, press, active, or drag state.
The frozen reference does not paint a thumb or focus ring merely because the
slider has keyboard or accessibility focus; its focused semantic state and
keyboard seek behavior remain live. The metadata label is placed from the resolved track height,
ControlGap spacing token, and Body line height, then published with the muted
chrome-metadata style. The public progress location remains zero-based even
though the accepted visual slider position is one-based.

Counts through `INT32_MAX` retain the exact one-to-one visual mapping from
location `i` to slider value `i + 1`. For a larger count `c`, the bounded UI0
slider uses `[1, INT32_MAX]` and round-half-up mappings
`1 + round(i * (INT32_MAX - 1) / (c - 1))` in the forward direction and
`round((s - 1) * (c - 1) / (INT32_MAX - 1))` for a returned slider value `s`.
The implementation accumulates quotient/remainder terms without forming an
overflowing product. Both zero and `c - 1` are exactly reachable, the mapping
is monotonic, and one pointer or keyboard slider unit advances one deterministic
scaled bucket. Portable semantics and SeekLocation actions continue to carry
the full unsigned 64-bit document range. This is an API 3 contract correction,
not an API expansion.

The Font popup is anchored 6 px below the Font slot. With the accepted theme
metrics and five projected choices its outer rectangle is
`(1094,44,170,188)` at 1400 px width, and its first 144 by 32 px body row begins
at `(1112,54)`. Choices remain borrowed projections and emit the existing
bounded SelectSetting action; no menu allocation or host callback is added.

## Exact dependency closure

- readerview0 API: 3;
- readerview0 version: `0.3.0-dev`;
- UI0 commit: `cadafcacdae8e63cf0d2b505f54e2a2a228c0bec`;
- UI0 API: 91;
- UI0 version: `0.1.0-dev`;
- zero_foundation source closure:
  `eee57edc1b0c7af5bef7afca26f3c27a32fb6e7c`;
- zero_foundation version: `0.4.0-dev`; and
- no reader0 or direct zero_foundation package dependency.

UI0 API 91 supplies the portable icon identities and deterministic icon draw
records used by this slice. Readerview0 does not copy Lucide paths or own a
native icon renderer.

## Deterministic validation

Strict MSVC C11 `/W4 /WX` validation covers:

- package/API/dependency versions and the exact clean UI0 source closure;
- exact 1400 by 780 no-panel, two-panel, and full-screen layout records;
- exact 940 by 520 no-panel and two-panel edge records;
- fail-closed undersized bounds;
- fixed toolbar order, slot rectangles, icon identities, and absence of loaded
  Open or toolbar text draw commands;
- portable title identity and geometry without document-title substitution;
- stable semantic control identities independent of projected labels;
- gutter visual versus hit geometry and pointer action execution;
- page-width progress semantics and keyboard action execution, including exact
  one-to-one behavior through `INT32_MAX`, boundary counts immediately above
  it, monotonic scaled steps, and both endpoints at `UINT64_MAX`;
- exact Font popup/row geometry and SelectSetting execution;
- zero-document Open pointer/focus behavior from recovery Slice 1;
- unchanged panel, Find, note-editor, focus, accessibility, duplicate-key, and
  deterministic diagnostic paths; and
- the architecture audit forbidding allocation, callbacks/provider tables,
  reader0/platform/persistence dependencies, and process-global mutable state.

## Deferred work

Host adoption, decoded-pixel comparison, and host-specific interaction checks
are recorded in each consumer repository rather than inferred from package
tests. At this Slice 2 checkpoint the left/right panel internals were unchanged;
the subsequent
`reader_view_recovery_slice3_reference_panels_api3.md` record supersedes that
temporary deferral and locks the accepted TOC, Find, Annotations, and disabled
gutter contracts within API 3. Selection tools and note-editor details not
explicitly covered by that follow-up remain deferred. This slice adds no PDF
support and begins no Kindle-gap feature work.

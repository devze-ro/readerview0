# readerview0 architecture

## Ownership

Readerview0 owns only proven-common UI0-composed EPUB reader chrome:

- the fixed accepted top-toolbar composition and responsive containment;
- left TOC/Find and right annotations/bookmarks panel composition;
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
the shorter lifetime. Counted Annotations-filter labels are synthesized into
four fixed buffers in `ReaderViewFrameStorage`; their bindings and semantics
therefore have the same until-next-build lifetime as every other returned
frame record.

`ReaderViewFindTextMetrics` is a borrowed, values-only one-build record. A host
that advertises Find supplies bounded codepoint advances and a measured
fallback from the same system-UI face used for field painting. The private
synchronous adapter to UI0 retains neither the metric array nor executable
host behavior. Both hosts pin printable Basic Latin, protect current and
same-frame incoming/transfer/commit text, then rank placeholder, committed
query, and history/scratch scalars. At capacity, stale and lower-priority
history entries are evicted first. Thus paste and undo/redo normally have exact
first-frame prefix geometry; beyond 256 distinct scalars, listed entries remain
exact and unlisted entries use the caller-measured fallback. The boundary never
grows or allocates.

`ReaderViewNoteTextMetrics` follows the same borrowed, values-only one-build
lifetime and 256-scalar cap, and is required only while the note editor is
open. It additionally carries the concrete system-UI pixel height and layout
line height. Hosts protect the current draft, same-frame incoming/transfer
text, and localized placeholder; a missing scalar uses the caller fallback
until the refreshed next-frame table arrives. Readerview0 publishes an
explicit `ReaderViewTextStyle_NoteEditor` binding plus Body typography metadata
on every note Text draw, so hosts need not retain the build input.

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

Queued native focus/invoke IDs have one-build lifetime. A build either routes
the request to a currently published eligible identity or consumes it; a
request cannot replay after a row, popup item, panel feature, or document
projection disappears. Same-frame pointer, keyboard, and native focus changes
are patched atomically across generic controls, Sidenav/TextInput/Slider
records, gutter icon visibility, and portable semantics. Popup and modal roots
contain both focus and invocation; background Find, progress, and page-gutter
semantics cannot be entered while a transient root owns interaction.

The finite semantic-control catalog covers the toolbar, page gutters,
progress, TOC/Find tabs and rows, the Find input/clear control, Annotations
header and rows, filter choices, and annotation-row actions. Hosts synchronize
native focus and control meaning from those identities plus caller-provided
source keys, never from localized labels. Disabled gutter controls remain
focusable for portable accessibility and reference focus paint, but every
invocation path remains action-ineligible while disabled.

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
while portable semantics keep their accessible names. Contents, Find, and
Annotations use Selected as their sole open-state visual flag; the matching
semantic nodes carry Selected and Expanded, never Checked. `chrome_title` is
an explicit portable projection distinct from `document_title`; readerview0
does not substitute the host document title into accepted chrome.

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

## Reference panels

The accepted left panel has a shell-free 72 px TOC/Find rail, an inset rail
divider, an outer one-pixel divider, and a shared outlined close control. TOC
native semantic/focus bounds use 32 px rows on the frozen panel-y-plus-52
origin and 32 px stride. Their physical pointer/paint Sidenav records use the
panel-y-plus-48 origin, the same full-width 32 px height, and a 2 px gap. Depth
moves only the 12 px expander and text by 20 px per projected level; it never
narrows either row rectangle. Opening Contents focuses and reveals the current
enabled row; opening Find focuses the shared bounded input. Activating a TOC
or Find row returns one keyed action while the host remains responsible for
the concrete reader0 destination, navigation, and history mutation. A ready
zero-row TOC publishes the localized frozen `No contents` status rather than a
generic empty-surface message.

The Find input similarly separates its 308 px semantic rectangle from the
frozen 274 px pointer/paint field and adjacent outlined 24 px clear control. A
real `UI0TextInputRecord` owns the 274 px field's pointer caret placement,
selection, clipping, placeholder, and blinking-caret draw state while the
query buffer and edit history remain in `ReaderViewState`. Its measurement
comes from the caller's values-only codepoint-advance record; malformed records
fail closed when Find is advertised, and absent scalars use the
caller-measured fallback. Readerview0 owns no concrete font and never replaces
the record with a fixed byte width. The localized
`Search in book` empty-field placeholder is not the localized
`Type and press Enter` ready-status prompt. It remains visible beside the
focused empty-field caret, which uses the frozen deterministic
20 px centered field geometry and 30-visible/30-hidden
`ReaderViewBuildInput.frame_index` cycle. Its draw clip is the full 274 by 34
input field so the 20 px caret is not truncated to the 16 px text rectangle;
text and selection records keep their existing clips. A selected result retains the
88 px action row but insets its painted fill by 4 px vertically. Ready Find
status uses the frozen muted Body line. When a ready zero-row projection omits
its explicit message, shared bounded query state selects the localized
`Type and press Enter` or `No matches` fallback; no generic centered
empty-surface message is synthesized.

Find result excerpts are host-rendered. Their text bindings carry only the
projected match range; each host measures and paints the range with its actual
system-UI metrics and the resolved reader-highlight color. Readerview0 owns no
font or glyph measurement and never substitutes a fixed character width.

The accepted right panel owns only Annotations chrome: filter/export/close,
section and row composition, star/menu controls, counted filter-label
composition, and bounded actions. Metadata, excerpts, filter counts, record
keys, resolved rail colors, and persistence are host projections. `color_key`
is used only for stable identity and attachment validation; `rail_color` is a
caller-resolved value painted verbatim in light or dark profiles. A standalone
Note may carry a rail without being attached. An attached Note may visually
join only the immediately preceding Highlight with the same nonzero color key
and the same nonzero resolved rail color; malformed text and attachment
projections fail closed without unsafe comparison.

The frozen row-menu target keeps the standard 30 by 28 control shell and a
centered literal ellipsis; the 20 px star target alone is shell-free. A row
paints `UI0ColorRole_SurfaceElevated` only while hovered or active. Idle,
selected-only, and focus-only rows have no fill or border, while semantic
selection/focus and the focus ring remain published. The shell-free star
raster and Menu control are separate physical action owners from the row body.
The row-body hit rectangle ends at the Star's left edge when Star is available,
otherwise at the Menu's left edge. Its full visual and semantic rectangles
remain unchanged, and child hover/press state is reflected only into parent-row
paint. Thus Body, Star, and Menu pointer releases are mutually exclusive; even
a disabled Menu owns its right-edge region without selecting the row. Keyboard
and native-accessibility row activation continue through the full semantic
identity. The shell-free star raster preblends against `SurfaceElevated` while
unstarred and `Badge` while
starred. An enabled filter trigger uses the Focus border color while open,
active, focused, or focus-visible. Readerview0 suppresses the separate UI0
FocusRing command only for this trigger, so the frozen rounded border carries
focus once without double compositing. For the combined focused/focus-visible
state only, four post-border 1 by 1 corner records reproduce the frozen
rounded-edge coverage. The color is a widened, rounded 96/255 source-over blend
from resolved Surface to resolved Focus, yielding `#FAC6A5` for the shared
Light profile and adapting from the same tokens in every other profile.
Open/active-only, focused-only, disabled, and root-blocked triggers do not
publish those records. The Annotations title and
20 px section heading, like the visible Find-input text, each have one exact
shared text draw and binding. Hosts own translation of those records through
their concrete system-UI renderer, so glyph-raster differences are host adapter
evidence rather than a reason to alter shared geometry or duplicate records.

Ready annotation filters with no projected rows publish the localized frozen
`No annotations`, `No bookmarks`, `No highlights`, or `No notes` status at the
accepted filter-specific empty rect. These strings remain chrome labels, not
host persistence records.

Annotation row menus use the frozen trigger anchor, panel-edge flip, flat-menu
geometry, and kind policy: Bookmark has Go To/Delete Bookmark; Note has Go
To/Edit Note/Delete Note; Highlight has Go To/Add-or-Remove Star/Delete
Highlight. The inline star is independently available on all three kinds.
Every applicable path closes the transient popup and restores the menu-trigger
focus. If the owning row disappears or its action membership changes, the
popup closes before the frame publishes and clears its popup-owned interaction
state. Filter membership is reconciled the same way, and projection removal
after a normal Delete clears the vanished row's restored trigger and transient
selection. Direct row activation updates bounded shared selection before returning
navigation; popup Go To leaves that selection untouched. Edit remains
host-executed and does not enter shared selection-editor state.

Fixed-capacity snapshots retain all projected TOC, Find, and Annotations keys
and the published popup-item identities. They retire vanished focus/hot/active
and queued native requests before a new frame is published; a final
published-record pass also retires identities removed by clipping, scrolling,
or layout. A stale TOC focus rehomes to the current/selected or first enabled
row, then the Contents tab; a stale Find result rehomes to the Find input; a
vanished Annotations row is cleared. Queued native focus/invoke requests have
one-build lifetime even when projection, metric, or document validation fails.
Opening newly laid-out Contents/Find defers its exact row/input handoff by one
layout frame, while close, Escape, reset, and feature withdrawal cancel that
bounded handoff. With an open document, withdrawing a panel's owning feature
closes that panel and reports both state and layout change. Each panel uses the
full content viewport for paint, clipping, and hit testing. Frozen scrolling is
wheel-only and hidden: it reserves no track width, emits no scrollbar or thumb
record/draw, and publishes no invisible scrollbar signal target. One wheel
delta is applied and clamped per build; focus is revealed or retired against
the resulting publication. Losing a Ready scroll owner neutralizes obsolete
public thumb/drag fields while preserving the bounded offset. With a panel
open, normal-root Tab order remains Previous, Next, Progress, then the adjacent
panel controls, and reverse Tab crosses the same boundary with visible focus.

Panel visual copy and native names are distinct localized records. In
particular the short `TOC` paint announces `Contents`, while close, search,
clear, filter, row-action, export, and Annotations-close controls use the
accepted explicit native labels. Escape dismisses a transient popup first,
then TOC/Find; it does not dismiss the docked Annotations panel.

A dirty shared note draft is never discarded by Escape. Explicit Cancel is a
separate bounded action carrying selection identity/revision so applications
can clear their committed reader selection without changing navigation or
history.

Save and Delete remain host-executed persistence actions. Only after either
mutation succeeds does the host call `reader_view_close_note_editor`; the
helper closes the modal, clears its dirty marker, retires note-editor-owned
focus/hot/active and queued native requests, and restores the captured
background focus. On persistence failure the host does not call the helper,
so the editor and draft remain available for retry.

The note TextArea keeps the frozen 492 by 248 shell while its content uses
asymmetric 13 px top and 7 px bottom insets. A bounded synchronous adapter maps
pointer rows to that same origin, quantizes vertical scroll to the caller's
25 px line height, publishes at most nine complete rows, and insets caret and
selection paint by one pixel to 23 px. Note rows carry proportional
caller-supplied advances and explicit 18 px system-UI raster metadata; the
focused empty placeholder uses its separate x+8/y+7 box. The adapter restores
the copied pointer input before any other control is built and does not alter
signals outside the editor shell. After the note TextArea's ordinary draw
sequence, four ordered 1 by 1 `SurfaceElevated` commands cover only its extreme
corners. These bounded seam masks preserve every adjacent focus-border pixel,
TextArea hit/focus state, and all non-note frames.

Page gutters keep their large host-independent action rectangles separate from
the 44 by 88 painted targets. UI0's filled 18 by 32 PageCaretLeft/Right records
use a caller-owned surface preblend, and keyboard focus paints the frozen
48 by 92 boundary clipped to the reader bounds. Disabled pointer focus remains
invisible and never changes action eligibility.

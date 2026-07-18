# Reader View recovery Slice 3: reference panels API 3

Date: 2026-07-17

Status: implemented and locally validated; host adoption and decoded-pixel
acceptance are validated separately

## Outcome

This slice restores the shared TOC, Find, and Annotations panel composition and
interaction contract from the accepted pre-extraction re10 Reader View. The
direction remains frozen re10 first, extracted re10 second, and lectern0 third.
Two extracted hosts matching each other is not an acceptance result when they
both differ from the frozen reference.

Directional frozen-a6b decoded-pixel and source evidence supersedes the
earlier provisional language in this record that described a visible panel
scrollbar, an eight-pixel track reservation, or thumb dragging. The accepted
panel lists are full-width, wheel-scrollable, and visually hidden; the corrected
contract below is authoritative.

This is a forward extraction recovery, not a revert. Readerview0 owns the
portable UI0-composed panel chrome, transient focus/popup/scroll state, borrowed
one-frame projections, deterministic records, and bounded returned actions.
Hosts still own EPUB navigation and search execution, annotation persistence,
document rendering, text measurement and rasterization, native accessibility,
window integration, and product policy.

The slice completes the panel portion of the already-open API 3 recovery. It
does not advance the API number or package version again. New enum values and
record fields are added without renumbering the earlier API 3 identities.

The bounded API highlights used by this slice are:

- `ReaderViewRightProjection` appends unsigned `all_count`,
  `bookmark_count`, `highlight_count`, and `note_count` values;
- `ReaderViewRightRow` appends caller-resolved `rail_color` after every prior
  row field;
- `ReaderViewFrameStorage` appends four fixed filter-label buffers, a
  66-record reference-panel Sidenav buffer, and fixed caller-owned note-editor
  TextArea row/selection/layout storage, while retaining the earlier 64-record
  member and all subsequent offsets unchanged;
- `ReaderViewLabels` appends localized panel labels for Contents, Find, and
  the empty-field `Search in book` placeholder, the four filter-specific
  Annotations empty states, and the frozen note-editor title, field, placeholder,
  Save, and native Cancel names;
- `ReaderViewBuildInput` appends a values-only `ReaderViewFindTextMetrics`
  record containing at most 256 caller-measured codepoint advances and one
  caller-measured fallback advance;
- `ReaderViewState` appends the open annotation action/filter membership and
  fixed-capacity prior-frame row/popup identity snapshots after the earlier
  API 3 state fields, plus `pending_left_panel_focus` for the one-layout-frame
  handoff into a newly opened Contents or Find panel;
- `ReaderViewRow_AttachedToPrevious`, `ReaderViewPopup_RightFilter`, the panel
  `ReaderViewSemanticControl` identities, and
  `ReaderViewFrameError_InvalidAttachment` are append-only enum/flag values;
- `ReaderViewTextBinding` appends `match_start` and `match_size` for a validated
  borrowed Find match range;
- `ReaderViewAction_CancelNote` is appended after every earlier API 3 action
  identity; and
- `reader_view_close_note_editor` gives a host an explicit acknowledgement
  boundary after a successful Save/Delete persistence mutation.

Readerview0 composes `Label (count)` into those caller-owned buffers without
allocation. Returned text bindings and semantics retain the ordinary
until-next-build frame-storage lifetime. `color_key` remains identity and
attachment evidence only; no modulo palette or theme-dependent color guessing
is permitted.

Open Contents, Find, and Annotations toolbar controls retain Selected as their
only visual open-state flag. Their semantic records carry Selected and Expanded
but never Checked, preserving native disclosure meaning without inventing a
checked/open button treatment.

## Shared panel geometry

At the accepted 1400 by 780 client bounds, the 420 px left panel is
`(12,56,420,686)`. Its 72 px navigation rail and body reproduce these exact
records:

- TOC rail tab: `(20,68,56,34)`;
- Find rail tab: `(20,110,56,34)`;
- panel close target: `(398,66,24,24)`;
- first and second TOC native semantic/focus rows: `(104,108,308,32)` and
  `(104,140,308,32)`;
- first and second TOC physical pointer/paint rows: `(104,104,308,32)` and
  `(104,138,308,32)`;
- ready-empty Contents status: `(104,112,308,22)`;
- Find input semantic target: `(104,104,308,34)`;
- Find input pointer/paint field: `(104,104,274,34)`;
- separate Find clear target: `(382,109,24,24)`; and
- Find ready-status line and first result: `(104,146,308,18)` and
  `(104,172,308,88)`.

The left-panel rounded surface is deliberately absent. The accepted composition
uses the inset rail divider at `(83,68,1,662)` and outer divider at
`(431,56,1,686)`. Native TOC semantic/focus bounds retain the frozen
panel-y-plus-52 origin and 32 px stride. Physical pointer activation and
Sidenav paint retain the separate panel-y-plus-48 origin, 32 px height, and
2 px gap. The current first row paints body `(114,104,298,32)`, focus rail
`(104,111,3,18)`, and a 12 px expander. Depth moves only expander/text paint by
20 px per projected level; the depth-two text begins at x 182 while both its
semantic and hit bounds remain full width. The frozen full-app coordinate
`(200,154)` activates row index 1, not row index 0, and the center of either
native semantic rectangle routes to its matching physical row. The separate
52 px Win32 debug resolver remains host-owned and is not a shared
accessibility bound.

The first selected Find result retains its 88 px action row while its fill is
vertically inset to `(104,176,308,80)`. Every visible result also paints the
frozen one-pixel `BorderMuted` divider at its bottom edge; the first divider is
`(104,259,308,1)`.

The 320 px right panel is `(1068,56,320,686)`. Its accepted records are:

- filter, export, and close targets at `(1078,66,24,24)`,
  `(1112,66,24,24)`, and `(1354,66,24,24)`;
- the first sectioned annotation row at `(1078,132,300,58)`;
- its star and row-menu targets at `(1320,151,20,20)` and
  `(1340,147,30,28)`; and
- the filter popup at `(1078,96,300,136)`; and
- the ready filter-specific empty status at `(1078,144,300,24)`.

The title begins at x 1156. The row's text begins at x 1091. A row with both
metadata and primary copy uses the frozen centered 46 px stack: metadata is
`(1091,142,221,20)` and primary text is `(1091,168,221,20)` for the first
deterministic row. The star paints as a 14 by 14 icon at `(1323,154)` without a
button shell. Its raster background is the resolved Badge token, including the
frozen pale-coral starred treatment. The row menu retains its standard 30 by 28
fill/border shell and centered literal ellipsis. Annotation rows use the
elevated-surface fill while hovered or active and while their own action menu is
open; idle, selected-only, and focus-only rows have neither fill nor border.
Semantic selection/focus and the focus ring remain intact. Pointer hover over
either the star or row-menu child keeps the parent row's frozen hover fill while
the child retains activation ownership; the overlapping visual hover record
does not turn a child click into row activation.

Available filter choices use the frozen visual order All, Highlights, Notes,
and Bookmarks. With all four choices present, their 274 by 29 px rows begin at
`(1096,100)`, `(1096,132)`, `(1096,164)`, and `(1096,196)`. The accepted
labels are `All (1)`, `All Highlight Colors (0)`, `Notes (0)`, and
`Bookmarks (1)` for the deterministic fixture. The selected All rail is
`(1086,106,3,17)`. Option text uses the frozen ten-pixel horizontal padding;
the first text rect is `(1106,100,254,29)`. The filter trigger retains its
14 by 14 Select icon at `(1083,71)`. Escape is keyboard input, so it restores
the trigger with visible focus; its clipped ring is the full
`(1078,66,24,24)` trigger perimeter.

## TOC behavior

Opening Contents focuses the projected current/selected enabled row, or the
first enabled row when there is no current row. Pointer-opened focus is not
painted as keyboard focus. The focused row is scrolled into view. Activating a
row emits exactly one `ReaderViewAction_ActivateTocRow` carrying the borrowed
stable row key; readerview0 neither resolves nor executes the destination. The
panel remains open and the activated row retains portable focus.

A ready TOC with no rows emits exactly one localized `No contents` muted Body
status at `(104,112,308,22)`. It does not reuse the generic surface-status
`Nothing here` fallback.

The vertical TOC/Find rail, panel title, close control, current-row semantic
state, and trigger-focus restoration are shared records. Closing with the
button or Escape restores the toolbar trigger when one opened the panel.
Because opening a panel changes the resolved layout, its initial row/input
focus is recorded in `pending_left_panel_focus` and applied when that panel's
controls are published by the next layout frame. Switching between already
visible Contents and Find applies the handoff in the same frame. Close,
feature withdrawal, document reset, and Escape clear any deferred handoff so a
vanished panel cannot receive focus later.

## Find behavior and match paint boundary

Opening Find focuses the shared input and resets the panel scroll position.
The caller-owned bounded query and history are wired through UI0's actual
text-input context and one `UI0TextInputRecord` whose exact rect and clip are
the frozen `(104,104,274,34)` field. UI0 therefore owns pointer caret
placement, selection, horizontal clipping, and focused blinking-caret records
without a caller-supplied executable function or provider. When Find is an
advertised feature, `ReaderViewBuildInput.find_text_metrics` is required and
fails closed as bad input when its fallback, bounds, scalar identities, or
advances are invalid. Readerview0 privately and synchronously adapts those
portable values to UI0's existing text-input measurement slot and retains no
caller memory. The frozen Win32 face is additive by codepoint, so variable
width queries such as `very high tower gyp` use the same measured prefix for
draw width, caret geometry, selection, clipping, and pointer hit testing
instead of `byte_count * 8`. A missing scalar uses only the caller-measured
fallback; readerview0 never invents a fixed character width.

Each host keeps a bounded session metric set from the same concrete system-UI
face it uses to paint the field. Both hosts use the same bounded priority:
printable Basic Latin is pinned; current query plus same-frame incoming,
transfer, and commit text is protected next; then come the localized
placeholder, host committed query, and bounded Find history/scratch storage.
At the 256-entry cap, stale entries are evicted before current ones and
lower-priority history before protected text. An unlisted scalar
deterministically uses the caller-measured fallback. This explicit bounded
degradation does not alter the exact accepted Basic-Latin reference matrix.
The localized `Search in book` placeholder is
field-only, remains visible beside a focused empty-field caret, and stays
distinct from the localized ready-status prompt. Caret draws use the frozen
deterministic 30-visible/30-hidden phase derived from
`ReaderViewBuildInput.frame_index`. The
clear action still returns focus to the input and emits the same single empty
`FindChanged` action. A ready status message occupies the frozen
`(104,146,308,18)` muted Body line and is emitted exactly once. When a ready
zero-row projection has no explicit message, bounded query state selects
localized `Type and press Enter` for an empty query or `No matches` for a
nonempty query with zero total matches. Readerview0 never synthesizes a second
centered Empty record or leaks the generic `Nothing here` copy into the Find
panel.

Each result has one 88 px shared row. Its section label is muted and
right-aligned, and its full excerpt remains the primary host-rendered text.
`ReaderViewTextBinding` appends `match_start` and `match_size`, copied from the
validated `ReaderViewFindRow`. The host measures the prefix and match using its
actual frozen system-UI font metrics, paints the shared reader-highlight color,
then paints the unchanged excerpt. Readerview0 does not guess character widths,
own a font, or rasterize the highlight.

Activating a result emits exactly one `ReaderViewAction_ActivateFindRow`, keeps
Find open, moves portable focus from the input to the row without a visible
pointer focus ring, and does not emit `FindChanged` or `FindCommitted`. The host
executes the concrete search jump and navigation/history mutation.

## Annotations behavior

The shared right panel composes its title, filter/export/close controls,
section headings, 58 px rows, star toggles, and action menus. Hosts project the
host-owned metadata in `secondary` and the bookmark/highlight/note excerpt in
`primary`. Readerview0 does not manufacture location labels or persistent
record identities.

Each section reserves the frozen 26 px vertical block while its text occupies
the leading 20 px rectangle and uses the same scale-2 chrome-title typography
as the accepted reference. Each row's 30 by 28 menu target paints the centered
literal `...` inside a clipped 22 by 28 text rectangle and retains the standard
UI0 menu-trigger fill and border shell. It has no substituted icon; its
accessible name is the distinct localized `Annotation actions` label. The star
target remains shell-free. Row fill is emitted for Hovered or Active and for
the keyed owner while its action menu is open, using
`UI0ColorRole_SurfaceElevated`; selection-only and focus-only never synthesize
a row band or border.
Partially visible row, text, star, and menu records are clipped to the content
viewport's full width. A fully clipped star or menu trigger is not published as
a control or semantic focus target.

The shared frame contains exactly one bound text draw for the Annotations title,
one for each published section heading, and one for visible typed Find input.
Directional decoded-pixel evidence corrected the earlier provisional +10/+32,
16 px row-text approximation: the accepted UI0 draw records are the centered
+12/+38, 20 px metadata/primary rectangles above. Once those shared records
match, any remaining glyph-pixel mismatch belongs to the host's concrete
text-record renderer seam; readerview0 still does not own native glyph
rasterization.

The action popup is anchored four pixels below the row-menu target, flips
above a near-bottom target, and uses the frozen UI0 flat-menu metrics. At the
deterministic first Bookmark row it is `(1184,179,186,86)` with 160 by 32 body
rows; Note and Highlight use the same anchor and a 120 px three-row height.
Popup membership is exact by kind: Bookmark is Go To / Delete Bookmark, Note
is Go To / Edit Note / Delete Note, and Highlight is Go To / Add or Remove
Star / Delete Highlight. `ReaderViewRightAction_ToggleStar` independently
enables the inline star on all three row kinds and adds a popup item only for
Highlight. A row with only an inline star has a disabled menu trigger rather
than an empty popup.

The open popup snapshots its row kind and action membership. Removing its row,
changing kind, removing every action, or changing the available action set
closes the root before publication and clears popup-owned focus/hot/active and
queued accessibility state. A row that still exists remains selected. Popup
and modal controls are drawn after normal-panel rows and icons, so a flipped
menu cannot be overpainted by an underlying annotation star or row.
The filter popup likewise snapshots `available_filters`; a membership change
closes and restores the filter trigger before removed options can retain focus.
After a normal Delete action, removal of the keyed row on the next host
projection clears its restored menu-trigger focus and transient row selection
even though the popup has already closed.

Hosts also project exact filter counts and each row's resolved `rail_color`.
The package formats the four counted labels in bounded frame storage and draws
`rail_color` verbatim. It never derives a color from `color_key`. Notes-only
standalone colored notes are valid and retain the exact same projected rail in
light and dark profiles; attachment is not required merely because a rail is
present.

Selecting a filter emits one `ReaderViewAction_RightFilterChanged`, resets the
right-panel scroll to zero, closes the popup, and restores its trigger focus.
The popup uses normal UI0 root containment and initializes focus on the current
filter. Escape closes it, restores the same trigger with a visible clipped
focus ring, and does not change the filter. Activating an annotation row emits
one bounded
`ReaderViewAction_ActivateRightRow`, keeps the right panel open, and stores its
bounded transient selection in `active_right_key`. Popup Go To returns the same
host navigation action without changing that selection. Escape closes a
TOC/Find panel before considering the docked right panel and never dismisses
Annotations merely because it is the only open panel.

A ready filter with no rows emits exactly one localized muted Body status at
`(1078,144,300,24)`: `No annotations`, `No bookmarks`, `No highlights`, or
`No notes` according to the active filter. It does not reuse the generic
surface-status empty copy.

Every applicable Go To, Add/Remove Star, Edit Note, or kind-specific Delete
choice closes the popup and restores the exact menu-trigger focus before
returning its bounded action. Pointer entry focuses the first item invisibly;
keyboard entry focuses it visibly and supports bounded Up/Down activation.
`EditRightRowNote` remains a host-executed action; it does not silently enter
the selection-owned shared note-editor state.

`ReaderViewRow_AttachedToPrevious` joins a projected Note to the immediately
preceding Highlight for the accepted highlight-plus-note visual. Validation
requires a non-first Note, an immediately preceding Highlight, identical
nonzero `color_key` values, and identical nonzero resolved `rail_color` values.
Invalid text is never compared while validating the pair. Invalid attachment
projections fail deterministically with
`ReaderViewFrameError_InvalidAttachment`. This flag represents only the
host-projected visual relationship; readerview0 still owns no annotation data
model or lifetime.

Stable `ReaderViewSemanticControl` identities are appended for every left-panel
tab/close/input/row, right-panel header/row/star/menu control, filter option,
and right-row menu action. Native adapters therefore map function from finite
identities and source keys rather than localized labels.
Visual abbreviations and native names remain separate: `TOC` paints while
`Contents` is announced; the frozen panel controls announce `Close navigation`,
`Search input`, `Clear search`, `Annotation filters`, `Annotation actions`,
`Export annotations`, and `Close annotations`.

The shared note editor retains the earlier no-data-loss rule: Escape cannot
dismiss a dirty draft. The explicit Cancel button closes the editor and emits
one appended `ReaderViewAction_CancelNote` with the authoritative selection key
and revision, allowing the host to clear its committed reader selection.
Cancel revisions are normalized in deterministic action evidence just like
Save/Delete revisions. A clean draft may still close through Escape.

The frozen editing composition is an anchored 520 by 360 modal rather than a
generic centered three-button form. In the accepted 1400 by 780 right-panel
case, the modal is `(303,117,520,360)`, the real multiline UI0 TextArea is
`(317,167,492,248)`, and Delete, Close, and Save are respectively
`(317,431,74,30)`, `(675,431,62,30)`, and `(747,431,62,30)`. The title is
`Note` for an existing note and `Add Note` for a new one. The field announces
`Note text`, uses `Type a note` only as its visual placeholder, paints wrapped
rows top-aligned with the frozen fixed eight-pixel UI0 layout advance, and
retains bounded caret/selection/history behavior. Delete
is destructive and appears only for a deletable existing note, Close/Cancel is
quiet, and Save is primary and remains enabled for an unchanged matching
revision. Native edit focus order is Note text, Delete note, Cancel note, Save
note; visible edit copy remains Delete, Close, Save, while Add Note omits Delete
and paints Cancel.
Opening either composition moves visible focus into the TextArea, matching the
frozen editor rather than leaving a pointer-opened modal without a focus cue.

Save and Delete continue to return host-executed actions. A host calls
`reader_view_close_note_editor` only after the corresponding persistence
mutation succeeds. The helper closes the modal, retires note-editor-owned
focus/hot/active and queued accessibility state, clears the dirty marker, and
restores the focus captured behind the modal. A failed mutation leaves the
shared editor and draft open for retry.

Every queued accessibility focus/invoke identity has one-build lifetime.
All projected TOC, Find, and Annotations row keys, plus published popup-item
identities, are kept only in fixed caller-owned prior-frame arrays so a
projection refresh can clear or rehome stale focus/hot/active and queued
requests before publication. A final published-record reconciliation also
retires focus/hot/active when clipping, scrolling, or layout makes an identity
absent or nonfocusable in the current frame. Accessibility requests are
consumed even when a build fails closed for invalid projection, invalid Find
metrics, or stale document state, so a repaired later build cannot replay an
old request. Feature withdrawal closes its open panel when a document is open.
Popup/modal containment blocks background Find, progress, and gutter semantics,
while same-frame focus changes update generic, specialized, draw, and semantic
evidence atomically.

TOC, Find, and Annotations apply one wheel delta per build, reveal a row when
focus moves to it, and retire focus when direct scrolling removes that row from
publication. Child paint and pointer targets use and clip to the full content
viewport. No panel reserves an eight-pixel track, emits a scrollbar track/thumb
record or draw, or publishes an invisible track signal that can intercept a
row. The obsolete public active-thumb and drag-origin fields are reset when a
Ready scroll owner is closed, feature-withdrawn, or becomes
loading/unavailable/error; the bounded offset is preserved. Forward Tab retains
the frozen Previous, Next, Progress, adjacent-panel order for Contents, Find,
and Annotations, and reverse Tab leaves the panel through Progress with visible
focus.

## Page-gutter regression

UI0 API 91 supplies exact filled `PageCaretLeft` and `PageCaretRight` raster
identities. Readerview0 places their 18 by 32 records inside the frozen 44 by 88
visual targets, uses `TextMuted` at rest and `TextSecondary` while pointer-down,
and supplies the exact Surface background for caller-owned preblend. The focus
record expands to 48 by 92, uses radius 4, and clips to reader bounds.

A disabled gutter paints its caret while hovered, focused, or pressed but
remains semantically disabled and cannot emit a page action through pointer,
keyboard, or accessibility invoke. Pointer press retains invisible portable
focus even when release occurs elsewhere. This separates reference paint and
focus from action eligibility without weakening UI0 disabled-signal semantics.

## Exact dependency closure

- readerview0 API/version: 3 / `0.3.0-dev`;
- UI0 API/version: 91 / `0.1.0-dev`;
- UI0 commit: `cadafcacdae8e63cf0d2b505f54e2a2a228c0bec`;
- zero_foundation source closure:
  `eee57edc1b0c7af5bef7afca26f3c27a32fb6e7c`; and
- no reader0 or direct zero_foundation package dependency.

## Deterministic validation

Strict MSVC C11 `/W4 /WX` validation covers:

- the exact dependency guard for UI0 API 91 and its zero_foundation source
  closure;
- the allocation/callback/provider/platform/persistence architecture audit;
- exact shell-free left rail/dividers, outlined close, distinct TOC native
  semantic/focus and physical pointer/paint rows, Find semantic/pointer/paint
  input split, selected-result inset and per-row divider, annotation
  header/exact system-font-descent text stack/star composition, 18 px filter
  icon, popup,
  ten-pixel filter-option text padding, and clipped Escape focus-ring geometry;
- current-row focus, depth indentation, panel-open state, one-action TOC
  activation, native semantic-center routing, physical `(200,154)` routing to
  visual row index 1, and exact localized `No contents` empty composition;
- Find input focus, exact UI0 input-record rect/clip, localized empty
  placeholder, focused caret/selection draws, pointer caret placement,
  caller-measured variable-width prefixes, deterministic caller fallback,
  missing/duplicate/negative/oversized metric rejection,
  exact visible/hidden caret-blink boundaries,
  physical clear-button precedence/focus restoration, single ready status,
  exact prompt/zero-match fallback copy and geometry, muted
  right-aligned section metadata, exact borrowed match range, one-action result
  activation, and absence of query-change actions;
- annotation row activation, four exact counted filter labels, selected rail,
  20 px scale-2 section labels within 26 px blocks, centered literal row-menu
  ellipses without a substituted icon and with the standard 30 by 28 shell,
  shell-free stars, exact elevated-surface hover/active parent-row fill even
  over star/menu children with no idle/selected-only/focus-only fill or row
  border, all three exact
  kind-specific action matrices, anchored and bottom-flipped popup geometry,
  pointer/keyboard focus entry, native names, four exact filter-specific empty
  states, filter selection and Escape restoration, dynamic-membership closure,
  scroll reset, and bounded popup containment;
- close-before-action and trigger-focus restoration for every applicable
  Go To/Add-Remove Star/Edit Note/kind-specific Delete path, including direct
  row selection versus popup Go To;
- popup-owner removal and action-membership changes without an empty root,
  dangling focus, or incorrect normal-row-over-popup draw ordering;
- normal Delete followed by row removal without a vanished trigger/selection,
  and long-TOC full-width Sidenav paint with no scrollbar record or draw;
- dirty-note Escape preservation, explicit Cancel identity/action closure, and
  normalized opaque note-revision evidence, plus successful host Save/Delete
  acknowledgement through modal-state retirement and focus restoration;
- one-build accessibility request consumption, same-frame atomic focus
  evidence, including failed-build non-replay, deferred panel focus handoff and
  Escape cancellation, popup/modal root containment, all-projected-row and
  feature-owner reconciliation, wheel-hidden then removed TOC/Find/right-row,
  star, and menu identities, single-step wheel input, focused-row reveal,
  partial-child clipping, fully clipped star/menu withdrawal, absence of any
  scrollbar draw/record/invisible hit interception, obsolete thumb-owner
  neutralization, and open-panel Previous/Next/Progress Tab boundaries;
- verbatim standalone Notes-only rail colors under light and dark themes;
- valid and invalid highlight/note attachment identity, rail-color, and
  invalid-text projections; and
- exact left/right PageCaret geometry/background, clipped focus records, and
  disabled hover/focus/press paint with pointer/accessibility suppression and
  release-outside focus retention.

Package tests establish portable layout, draw, semantic, focus, and action
contracts. They do not establish either host's EPUB navigation execution,
persistent record mutation, native accessibility behavior, or decoded-pixel
result. Those acceptance gates belong to the re10 and lectern0 adoption slices
and the frozen-reference comparison.

## Deferred work

This slice adds no PDF support, generic document interface, callback/provider
table, hidden allocation, or process-global mutable state. It does not begin
the Kindle-gap phase. Host persistence across restart, real-book stabilization,
and feature work beyond the frozen current reader remain separately bounded
work.

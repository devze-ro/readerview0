# readerview0

`readerview0` is the reusable EPUB reader-view/chrome package in the dev0
family. Its intended consumers are re10 and 8vo.

Reader View API 3 restores the accepted pre-extraction re10 Reader View
foundation from explicit,
caller-provided projections. It owns bounded transient interaction state,
fixed reference toolbar/panel geometry, UI0 draw records, portable semantic
records, bounded action records, and renderer-free page/content geometry.
Applications execute every document or persistent mutation and retain the
central reader renderer. Extraction ownership remains intact.

API 3 resolves chrome and central geometry atomically. The accepted loaded
surface uses a 56 px top chrome, 38 px footer, 420/320 px panel widths, 12 px
panel inset, and 14 px panel-to-page gaps. `ReaderViewLayout` returns the
authoritative `viewport_rect`, `page_surface_rect`, and `content_rect` for the
same state snapshot. The page defaults preserve re10's centered contract:
24 px horizontal viewport inset, 660 px maximum and 160 px minimum page width,
52/68 px content insets, and 80/48 px minimum content dimensions. The
standalone content resolver remains available for callers needing an explicit
scalar style.

Loaded chrome uses the fixed right-aligned icon order Contents, Find, Back,
Forward, Full screen, Size, Spacing, Font, Theme, Annotations, and Bookmark,
followed by the host-owned Exit slot. It does not show Open while a document is
loaded. Icons retain explicit accessible labels. The caller supplies a
portable `chrome_title` separately from the host-owned `document_title`. An
open Contents, Find, or Annotations trigger is visually Selected only; its
portable semantic record carries Selected and Expanded, never Checked.

The left panel reproduces the accepted shell-free 72 px TOC/Find rail and inset
divider. TOC native semantic/focus rows use the frozen 32 px stride beginning
at panel y + 52, while physical Sidenav hit/paint rows begin at panel y + 48
and retain their 32 px height plus 2 px gap. It preserves
focused-current-row behavior and 88 px Find results. TOC depth indents only the
expander/text paint inside an unchanged full-width row. Ready-empty Contents
uses the localized frozen `No contents` record. The Find input
keeps its 308 px semantic target while pointer hit-testing and painting the
frozen 274 px field plus a separate outlined clear button. The field is a real
caller-backed UI0 text-input record, including pointer caret placement,
selection, clipped text, and blinking-caret draw records. Its localized
`Search in book` placeholder is distinct from the localized
`Type and press Enter` ready-status prompt and remains painted beside the
caret while the focused field is empty. The caret follows the frozen
20 px centered field geometry and frame-indexed 30-visible/30-hidden blink
cycle, and its draw command clips to the full 274 by 34 field rather than the
16 px text rectangle. Text and selection clipping remain unchanged. A host
advertising Find also
supplies a bounded values-only codepoint-advance record from the same system-UI
face it uses to paint the field. The shared input therefore uses real
variable-width prefix geometry for drawing, caret placement, selection,
clipping, and pointer hit testing; missing scalars use the caller-measured
fallback, never an internal fixed width. Find text bindings publish the
projected match range so the host can measure and paint the
highlight with its concrete system-UI renderer. Ready Find status, including
the localized empty-query and zero-match fallbacks, uses the frozen muted-body
status line.

The right panel reproduces the accepted filter/export/close header, sectioned
58 px annotation rows, 14 px shell-free star paint, standard 30 by 28 row-menu
shell, and contained filter popup. An annotation row paints the resolved
elevated-surface fill only while hovered or active; idle, selected-only, and
focus-only rows remain fill-free while retaining semantic selection/focus and
the focus ring. The full row remains its visual and semantic record, while its
physical body target ends before Star or Menu; child hover/press still carries
the frozen parent-row paint. Body, Star, and Menu pointer releases therefore
return mutually exclusive row, toggle, or popup outcomes, and a disabled Menu
cannot fall through into row selection. Keyboard and native-accessibility row
activation continue through the full semantic row. An unstarred icon preblends
against `SurfaceElevated`; a starred icon preblends against `Badge`. An
active, open, focused, or focus-visible filter trigger paints its border with
the resolved Focus color. The filter alone suppresses UI0's additional Focus
ring so visible focus is represented once by the frozen rounded border. When
the trigger is both focused and focus-visible, four ordered 1 by 1 records
restore only the rounded border's extreme corners after that border draw. Their
color is the caller's Focus token composited over Surface at the frozen 96/255
coverage (`#FAC6A5` for the shared Light profile); open/active-only,
focused-only, disabled, and blocked triggers receive no seam, and other theme
profiles use the same token-derived blend. Its
row-action popup is anchored to the exact row menu, flips
at the panel edge, and exposes the frozen Bookmark, Note, or Highlight action
matrix. Every kind retains its inline star; only Highlight carries Star in the
popup. The four filter labels are composed as bounded
`Label (caller_count)` records in caller-owned frame storage. Hosts project
each row's stable `color_key` separately from its exact resolved `rail_color`;
readerview0 paints the latter verbatim and does not infer a palette from the
identity. Every row and popup interaction returns a bounded keyed action; EPUB
navigation, search execution, annotation persistence, and native integration
remain in the applications. Ready-empty filters use the localized frozen
`No annotations`, `No bookmarks`, `No highlights`, or `No notes` record.
Direct row activation owns bounded shared selection; popup Go To navigates
without changing it. Visual abbreviations and native panel-control names are
separate localized records.

Shared records contain exactly one visible Find-input text draw, Annotations
title draw, and Annotations section-heading draw at the frozen rectangles.
Painting those bound system-UI glyphs is a host renderer responsibility, so a
missing or mismatched host raster is an adapter seam rather than absent shared
chrome.

Changing or removing an open annotation row's menu membership closes the
stale popup before publication and clears popup-owned interaction state.
Changing the filter choices does the same for the filter popup, and removing a
normally deleted row clears its restored trigger/selection on the next frame.
Explicit note-editor Cancel returns a bounded Cancel action for the host to
clear its committed selection, while Escape never discards a dirty draft.
After a host successfully persists a Save or Delete action, it calls
`reader_view_close_note_editor` to close the modal, retire note-editor-owned
focus/hot/active and queued accessibility state, clear the dirty marker, and
restore the captured background focus. A failed host mutation does not call
the helper, so the draft and editor remain available for retry.

While the note editor is open, the host also supplies a borrowed, values-only
`ReaderViewNoteTextMetrics` record: at most 256 unique Unicode advances, a
positive fallback, the concrete system-UI pixel height, and its layout line
height. The frozen field uses 18 px system-UI raster metadata in 25 px rows,
13 px top and 7 px bottom content insets, nine complete visible rows, and
23 px caret/selection rectangles. `ReaderViewTextStyle_NoteEditor` and the
explicit Body/18 px draw carrier let each host rasterize from the published
command without retaining build input. Same-frame unlisted text uses the
caller fallback; a refreshed next-frame scalar table updates wrap, hit, and
caret geometry without losing draft or focus.

After the note TextArea finishes its normal border/focus draw, four bounded
1 by 1 `SurfaceElevated` records mask only the extreme shell corners. Adjacent
focus pixels, TextArea state, and hit geometry remain unchanged, and the masks
retire with the note editor.

All projected prior-frame row keys and published popup-item identities are
retained only in fixed-capacity caller-owned state. Each build reconciles
removed, disabled, feature-owned, clipped, or scrolled-off controls before and
after publishing records; an accessibility focus/invoke request is consumed by
that build even when validation fails and never replays after repair. Opening a
newly laid-out TOC/Find panel uses one bounded deferred-focus field that close,
Escape, reset, and feature withdrawal clear. TOC, Find, and Annotations scroll
regions use the full content viewport and frozen hidden wheel scrolling. They
reserve no eight-pixel track, emit no scrollbar track/thumb record or draw,
publish no invisible scrollbar hit target, apply each wheel delta exactly once,
clamp the bounded offset, reveal keyboard/native focus, and retire focus that
scrolls out of publication. Obsolete public UI0 scroll drag fields are
neutralized without discarding the offset when a scroll owner closes or leaves
Ready state.

Previous/next gutters use UI0's exact 18 by 32 filled PageCaret identities
inside the frozen 44 by 88 visual targets. Their large semantic/hit rectangles,
disabled action eligibility, and clipped 48 by 92 focus boundary remain
separate records. With any reference panel open, forward Tab order remains
Previous, Next, Progress, then the adjacent panel records; reverse Tab crosses
the same Progress boundary with visible portable focus.

The API supports the current re10 feature set without requiring every host to
provide it: Open, page and history navigation, TOC, Find, progress seeking, the
four current reading settings (font family, font size, line spacing, and
theme), bookmark state, annotations/bookmarks, selection tools, full screen,
distraction-free reading, lookup, translation, and export.

## Build

Requirements:

- Git and PowerShell 5.1 or later
- Visual Studio 2022 Build Tools with the Desktop development with C++
  workload and a Windows SDK

Clone readerview0, UI0, and Ground0 beside one another, then check out the
exact revisions in the dependency metadata:

```powershell
git clone https://github.com/devze-ro/readerview0.git
git clone https://github.com/devze-ro/ui0.git
git clone https://github.com/devze-ro/ground0.git
$ui0Commit = Get-Content .\readerview0\vendor\ui0_dependency\COMMIT
$ground0Commit = Get-Content .\readerview0\vendor\ui0_dependency\SOURCE_CLOSURE_GROUND0_COMMIT
git -C .\ui0 checkout $ui0Commit.Trim()
git -C .\ground0 checkout $ground0Commit.Trim()
Set-Location .\readerview0
```

Build and run the strict dependency, architecture, and package tests:

```bat
build\win32_build.bat
```

The package exact-pins UI0 API 91 at commit
`2e8036d552c6f84f73e3a14e271ecf5e5c6cfe4d`. That UI0 revision supplies the
resolved theme catalog, shared portable reader icon identities, and exact
filled PageCaret raster records. The source-consumed UI0 closure requires
ground0 commit
`616dee75bdc8ac4ac20df1543c68999e46025077` (`0.4.3-dev`).

## License

Unless otherwise noted, first-party source code and documentation in this
repository are licensed under the Mozilla Public License 2.0 (`MPL-2.0`). See
[LICENSE](LICENSE).

Source-consumed dependencies and third-party materials retain their respective
licenses.

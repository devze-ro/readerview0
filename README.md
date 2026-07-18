# readerview0

`readerview0` is the reusable EPUB reader-view/chrome package in the dev0
family. Its intended consumers are re10 and lectern0.

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
portable `chrome_title` separately from the host-owned `document_title`.

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
frame-indexed 30-visible/30-hidden blink cycle. A host advertising Find also
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
58 px annotation rows, 14 px star paint, star/menu controls, and contained
filter popup. Its row-action popup is anchored to the exact row menu, flips at
the panel edge, and exposes the frozen Bookmark, Note, or Highlight action
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

All projected prior-frame row keys and published popup-item identities are
retained only in fixed-capacity caller-owned state. Each build reconciles
removed, disabled, feature-owned, clipped, or scrolled-off controls before and
after publishing records; an accessibility focus/invoke request is consumed by
that build even when validation fails and never replays after repair. Opening a
newly laid-out TOC/Find panel uses one bounded deferred-focus field that close,
Escape, reset, and feature withdrawal clear. TOC, Find, and Annotations scroll
regions clip row/child paint and pointer targets to the content viewport,
exclude the scrollbar track from activation, apply each wheel delta once, and
retire interrupted thumb-drag ownership without discarding the scroll offset.

Previous/next gutters use UI0's exact 18 by 32 filled PageCaret identities
inside the frozen 44 by 88 visual targets. Their large semantic/hit rectangles,
disabled action eligibility, and clipped 48 by 92 focus boundary remain
separate records.

The API supports the current re10 feature set without requiring every host to
provide it: Open, page and history navigation, TOC, Find, progress seeking, the
four current reading settings (font family, font size, line spacing, and
theme), bookmark state, annotations/bookmarks, selection tools, full screen,
distraction-free reading, lookup, translation, and export.

Build and run the strict tests with:

```bat
set READERVIEW0_UI0_DIR=C:\path\to\ui0
set READERVIEW0_ZERO_FOUNDATION_DIR=C:\path\to\zero_foundation-at-the-pinned-ui0-closure
build\win32_build.bat
```

The package exact-pins UI0 API 91 at commit
`cadafcacdae8e63cf0d2b505f54e2a2a228c0bec`. That UI0 revision supplies the
resolved theme catalog, shared portable reader icon identities, and exact
filled PageCaret raster records. The source-consumed UI0 closure requires
zero_foundation commit
`eee57edc1b0c7af5bef7afca26f3c27a32fb6e7c`.

This recovery slice does not authorize a push.

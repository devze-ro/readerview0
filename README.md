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
`0ad8a43b658f67715720602abe779fb0d33052e1`. That UI0 revision supplies the
resolved theme catalog and the shared portable reader icon identities. The
source-consumed UI0 closure requires zero_foundation commit
`eee57edc1b0c7af5bef7afca26f3c27a32fb6e7c`.

This recovery slice does not authorize a push.

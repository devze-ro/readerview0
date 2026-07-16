# readerview0

`readerview0` is the reusable EPUB reader-view/chrome package in the dev0
family. Its intended consumers are re10 and lectern0.

Reader View API 2 composes the proven re10 reader chrome from explicit,
caller-provided projections. It owns bounded transient interaction state,
responsive toolbar/panel geometry, UI0 draw records, portable semantic records,
bounded action records, and a renderer-free page/content geometry resolver.
Applications execute every document or persistent mutation and retain the
central reader renderer.

The API 2 geometry defaults preserve re10's current centered page contract:
24 px horizontal viewport inset, 660 px maximum and 160 px minimum page width,
52/68 px content insets, and 80/48 px minimum content dimensions. The resolver
returns caller-owned `viewport_rect`, `page_surface_rect`, and `content_rect`
records and may also consume an explicit caller-provided scalar style.

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

The package exact-pins UI0 API 90 at commit
`fda99de484d50f1b019b1edfe3489f57fae57f9a`. That UI0 revision supplies the
shared six-profile resolved theme catalog. The source-consumed UI0 closure
requires zero_foundation commit
`eee57edc1b0c7af5bef7afca26f3c27a32fb6e7c`.

No remote is configured. Do not create or push a GitHub repository without
explicit authorization.

# readerview0

`readerview0` is the reusable EPUB reader-view/chrome package in the dev0
family. Its intended consumers are re10 and lectern0.

Reader View API 1 composes the proven re10 reader chrome from explicit,
caller-provided projections. It owns bounded transient interaction state,
responsive toolbar/panel geometry, UI0 draw records, portable semantic records,
and bounded action records. Applications execute every document or persistent
mutation and retain the central reader renderer.

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

The package exact-pins UI0 API 89 at commit
`f8de965c193a6278d330193c34948bfec09e592b`. The source-consumed UI0 closure
requires zero_foundation commit
`a87938edcd16c6c09c8d423a42b6d86122d85a73`.

No remote is configured. Do not create or push a GitHub repository without
explicit authorization.

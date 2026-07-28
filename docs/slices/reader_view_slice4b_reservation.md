# Reader View Slice 4B repository reservation

Date: 2026-07-15

## Decision

The approved repository name is `readerview0`. It follows the dev0 reusable
technology naming pattern and describes presentation/view ownership without
claiming the document engine owned by reader0.

## Why reservation follows API 3

Re10 and 8vo first reconcile on one concrete EPUB semantic-navigation
contract. This prevents shared chrome from absorbing duplicate TOC fragment,
Find destination, history, or page-selection algorithms. Readerview0 can now be
designed around actions and projections rather than becoming a second reader
core.

## Slice 4B deliverable

- local repository on `main` with the approved commit identity;
- dedicated linked `codex/reader-view-slice4b-reservation` worktree;
- no remote and no GitHub repository;
- boundary documentation only; and
- no public API, source manifest, package dependency, build, or UI
  implementation.

## Next gate

The next slice must inventory the exact re10 and 8vo toolbar, TOC, Find,
annotation-panel, theme, font, and bookmark inputs/actions. It must then propose
the smallest coherent UI0-composed package API before source is moved or new
chrome is built.

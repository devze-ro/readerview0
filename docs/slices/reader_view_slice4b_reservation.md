# Reader View Slice 4B repository reservation

> [!NOTE]
> This is a historical engineering record. See the
> [project README](../../README.md) for current information.

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

## Historical Slice 4B deliverable

- The repository existed locally on `main` with the approved commit identity.
- Work continued in a dedicated linked
  `codex/reader-view-slice4b-reservation` worktree.
- No remote or GitHub repository existed at the time.
- The slice delivered boundary documentation only, with no public API, source
  manifest, package dependency, build, or UI implementation.

## Historical next gate

The next planned slice was to inventory the exact re10 and 8vo toolbar, TOC,
Find, annotation-panel, theme, font, and bookmark inputs/actions, then propose
the smallest coherent UI0-composed package API before source moved or new
chrome was built.

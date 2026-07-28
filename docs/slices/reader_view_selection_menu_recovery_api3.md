# Reader View selection-menu recovery (API 3)

Date: 2026-07-19

## Baseline and dependencies

- Readerview0 base: `6ff78cf47258ff21f79fa3473973a85066fea899`
- Recovery implementation: `4e0357f` (`Restore compact reader selection menu`)
- Readerview0 version/API remain `0.3.0-dev` / `3`.
- Exact UI0 dependency remains
  `cadafcacdae8e63cf0d2b505f54e2a2a228c0bec`, API `91`.
- Exact zero_foundation source closure remains
  `eee57edc1b0c7af5bef7afca26f3c27a32fb6e7c`.
- Frozen presentation reference:
  Re10 `a6b1555ecb39c4948c735decda02cdc5a71f452c`.

## Real-book diagnosis

The exact local fixture was:

- size: `955125` bytes
- SHA-256:
  `D5365766478A7D853821299B72432D15583F8DD10F94C2C2CF20D52E783E77F9`

Current Re10 reproduced a `420 x 176` logical selection popup with a sparse
four-column grid. The frozen pre-extraction reader used a `224`-logical-pixel
compact popup with a four-swatch strip and five stacked action rows. The
supplied screenshot
`codex-clipboard-9850fd39-e6e4-4e3a-b4ad-3eb2ac0646e1.png` is Re10: its
justified body and italic publisher heading match the Re10 host, and the live
Re10 reproduction matched its popup geometry and row ordering.

The second supplied screenshot,
`codex-clipboard-51b46916-e55a-41c5-a907-9616d0e44908.png`, is 8vo. Its
publisher-typography signature matches the live 8vo host. The menu is only
transient there because the pointer release that completes selection also
dismisses the newly opened popup; that host lifecycle defect is intentionally
outside this shared package slice.

In Re10, the shared keyboard path itself remained functional: keyboard
activation of Edit note opened the note editor, Escape dismissed it, and Right
Arrow immediately paged after focus restoration. The shared regression was the
selection-menu presentation and action grouping, not Re10's action executor.

## Ownership and changes

Readerview0 continues to own the shared Reader chrome, bounded interaction
state, semantic records, action records, and popup geometry. This slice:

- restores the frozen compact `224`-pixel menu width;
- derives height from the resolved UI0 menu style and available action rows;
- restores the four `20 x 20` swatches at the frozen offsets;
- restores stacked Add/Edit Note, Copy, Dictionary, Web lookup, and Translate
  rows using UI0 flat-row popup geometry;
- prefers below-anchor placement, falls back above, and clamps against the
  Reader viewport and page-content edges;
- uses UI0's existing `ui0_swatch_strip_build` primitive for hover, active,
  selected, focus, and action-overlay records;
- keeps color names in the semantic tree while omitting them from visual text;
- treats activation of the selected swatch as Remove Highlight, matching the
  reference action overlay, instead of adding a separate Delete grid row; and
- appends an optional caller-resolved `visual_color` to `ReaderViewChoice` so
  hosts can supply their existing theme-correct publisher highlight palette.

No callbacks, provider tables, vtables, event buses, dependency injection,
generic document interfaces, hidden allocations, or process-global mutable
state were introduced. UI0 API 91 already contained the required generic menu,
flat-row, draw, and swatch mechanisms, so no UI0 change or API bump was needed.

## Regression coverage

The direct Readerview0 regression covers:

- exact compact popup, swatch, and stacked-row geometry;
- viewport/page-edge placement above and below the selection;
- semantic color labels with visual swatch fills;
- host-resolved light and dark swatch palettes;
- themed border and visible keyboard-focus rendering;
- pointer color activation;
- accessibility activation of the selected Remove Highlight swatch;
- keyboard activation of Copy through the shared action path; and
- Escape dismissal and dismissed-selection identity.

Strict MSVC C11 `/W4 /WX` build, the complete Readerview0 test binary, the UI0
dependency guard, source-closure guard, and architecture audit pass against the
exact dependency commits above.

## Why the prior gates missed it

The 28-case frozen-reference matrix and 15-case cross-host matrix did not
perform a real text-selection workflow or render the SelectionTools popup.
Readerview0's package suite covered note-editor behavior but lacked a direct
SelectionTools geometry, swatch, activation, clamping, focus, or dismissal
regression. The frozen Re10 implementation had dedicated selection-popover
tests, but that coverage was not migrated with the shared Reader View chrome.


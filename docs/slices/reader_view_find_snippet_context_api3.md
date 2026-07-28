# Reader View Find snippet context recovery: API 3

> [!NOTE]
> This is a historical engineering record. See the
> [project README](../../README.md) for current information.

Date: 2026-07-19

Status: implemented and package-validated; exact Re10 and 8vo adoption and
rendered real-book acceptance are recorded by the host slices

## Reported failure and reference result

The exact local book fixture is 955125 bytes, SHA-256
`D5365766478A7D853821299B72432D15583F8DD10F94C2C2CF20D52E783E77F9`.
Searching for `Paran` at 1400 by 780 in the Light theme produced a first result
around Location 26 whose visible line ended at `IMPERIAL COMMAND Ganoes Stabro`.
Activating the row navigated to the correct occurrence, but the panel line did
not contain or highlight the matched term.

The promoted extracted Re10 and the frozen pre-extraction Re10 commit
`a6b1555ecb39c4948c735decda02cdc5a71f452c` reproduce the same defect. Both
semantic dumps retain the complete Reader0 result:

`IMPERIAL COMMAND Ganoes Stabro Paran, a noble-born officer in the Malazan Empire`

with exact match bytes for `Paran`. The issue is therefore an existing
one-line Reader View presentation defect, not an extraction regression and not
a Reader0 search/snippet-generation failure.

## Ownership and boundary

Reader0 continues to own search execution, match identity, source context, and
the full projected excerpt. The parent `ReaderViewSemanticNode` for the result
continues to expose that full excerpt, so accessibility and action identity do
not lose context.

Readerview0 owns the shared 88 px result-row and one-line excerpt geometry. Each
host already supplies a bounded, values-only `ReaderViewFindTextMetrics` table
measured from the actual system-UI face it uses to paint Find chrome. This slice
uses those existing advances to select a borrowed one-line text window only
when the original first fitted line would omit a valid displayable match. It:

- preserves the full binding unchanged when the first line already contains
  the complete match;
- advances only to a UTF-8-safe natural word boundary before the match;
- retains the complete match whenever its measured width fits the row;
- bounds the borrowed slice to the available one-line width;
- reserves one caller-measured fallback advance in a recovery window so small
  scalar-sum/full-string fit differences cannot rewind before the match;
- remaps `match_start` into the borrowed slice while retaining `match_size`;
  and
- rejects invalid source ranges through the existing fail-closed projection
  validation.

The binding borrows bytes from the caller's existing excerpt; no allocation or
copy is introduced. Hosts still own the concrete font, full-string/glyph
measurement, rasterization, highlight color, clipping, and native drawing.
There is no callback, provider table, vtable, event bus, dependency injection,
generic document interface, or mutable process-global state.

The public Readerview0 API remains 3 and the package version remains
`0.3.0-dev`. No record shape, enum identity, capacity, or dependency changes.

## Exact dependency closure

- Readerview0 baseline: `27e2ac64bc9db87412cf076eac313dea902792eb`;
- UI0 API/version: 91 / `0.1.0-dev`;
- exact UI0 commit: `cadafcacdae8e63cf0d2b505f54e2a2a228c0bec`;
- zero_foundation source closure:
  `eee57edc1b0c7af5bef7afca26f3c27a32fb6e7c`; and
- no Reader0, Re10, 8vo, or direct zero_foundation package dependency.

## Package regression coverage

Strict MSVC C11 `/W4 /WX` package validation covers:

- the exact long-prefix `Paran` fixture from the real-book failure;
- preservation of the complete excerpt as the parent row's semantic name;
- a natural-word visible binding beginning with `COMMAND` rather than a
  mid-codepoint or mid-word byte;
- complete `Paran` bytes and exact remapped match range inside the binding;
- caller-measured width not exceeding the shared one-line rectangle;
- unchanged behavior for the existing `A result` fixture whose match is
  already on the first line;
- UTF-8-safe borrowed-window and match boundaries; and
- existing invalid-match fail-closed behavior.

The exact UI0 dependency guard, zero_foundation source-closure guard,
allocation/indirection/platform architecture audit, and complete Readerview0
test executable pass. Rendered Re10/8vo evidence and the frozen-reference
and cross-host matrices remain host-adoption acceptance gates.

## Why earlier gates missed it

The synthetic Find fixtures placed short terms near the start of short excerpts
and asserted record identity and match byte ranges. They never supplied a valid
match beyond the first fitted line, nor required the rendered first line to
contain that match. Frozen-reference pixel parity also reproduced the same
pre-existing defect, so equality to the reference could not identify it.

The missing coverage is an exact long-prefix real-book workflow with a rendered
assertion that the active row's one visible excerpt line contains and highlights
its match, plus the package-level semantic/window/range assertions above.

## Scope exclusions

This slice does not change Reader0 snippet construction, Find navigation,
active/dormant highlight colors, page-match rendering, host persistence, native
focus routing, selection, typography, PDF support, or Kindle-style feature-gap
work.

# Reader View recovery Slice 1: zero-document interaction lifecycle

Date: 2026-07-17

Status: implemented and validated on local recovery branch
`codex/reader-view-recovery-slice1`; not promoted by this record.

## Reference-recovery context

The accepted pre-extraction re10 reader at
`a6b1555ecb39c4948c735decda02cdc5a71f452c` is the directional product
reference. Cross-host equality is not sufficient evidence: extracted re10 must
first preserve that reference, and lectern0 must then match the restored shared
contract.

This bounded change fixes one confirmed interaction regression exposed by that
audit. It does not claim that the broader appearance, interaction, or
functional reference recovery is complete.

## Defect

`reader_view_build` previously called `reader_view_state_reset_document` on
every frame whenever `ReaderViewState.document_key` was zero, even when the
incoming projection key was also zero.

Zero is the valid no-document identity used by startup and empty-reader frames.
UI0 pointer activation spans native-like press and release frames. Resetting
between those frames cleared `active_id`, so the release could not produce the
Open action. The same unconditional reset also discarded portable focus,
pending accessibility focus, and transient panel state on every no-document
frame.

## Correction

The build now resets or rejects state only when the state and projection keys
differ:

| State key | Projection key | Result |
|---|---|---|
| `0` | `0` | Same no-document identity; preserve transient state |
| `0` | nonzero | Bind the first document and reset transient state |
| nonzero | same nonzero | Preserve transient state |
| nonzero | different key, including `0` | Reject as stale until the host explicitly resets |

This preserves the existing stale-document guard. It does not silently absorb
document closure or replacement. A host continues to call
`reader_view_state_reset_document` when leaving or replacing an already-bound
document.

No callback, allocation, retained host record, native dependency, or ownership
change was introduced. Reader View remains API 2 at package version
`0.2.0-dev` with the same UI0 API 90 and zero_foundation source-closure pins.

## Regression gate

The deterministic package test now covers:

1. a no-document seed frame with an enabled Open control;
2. separate pointer-press and pointer-release builds with key zero;
3. preservation of the active control and emission of
   `ReaderViewAction_Open` on release;
4. portable Open focus publication and retention across another key-zero
   frame;
5. transient panel-state retention across a key-zero frame;
6. automatic zero-to-nonzero binding with transient-state reset;
7. fail-closed nonzero-to-zero mismatch without an explicit host reset; and
8. successful explicit nonzero-to-zero reset with cleared transient state.

Before the implementation change, the new gate failed with:

```text
FAIL: zero-document Open release emits action
FAIL: zero-document frame preserves transient panel state
```

After the change, the full package build and test pass.

## Validation

- exact UI0 dependency guard:
  `fda99de484d50f1b019b1edfe3489f57fae57f9a`, API 90;
- exact zero_foundation source closure:
  `eee57edc1b0c7af5bef7afca26f3c27a32fb6e7c`;
- readerview0 architecture audit;
- strict MSVC C11 `/W4 /WX` package build; and
- complete `readerview0_tests.exe` suite.

## Explicit non-goals

- no visual or layout redesign;
- no attempt to declare reference parity restored;
- no host persistence, document rendering, or native-window work;
- no readerview0 API or package-version increment; and
- no re10, lectern0, UI0, reader0, or zero_foundation source change.

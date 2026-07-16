# Reader View Slice 5B: API 1 implementation

Date: 2026-07-16

## Scope

This slice implements the approved smallest coherent shared-chrome boundary
needed to bring the current re10 EPUB reader feature set to lectern0. It does
not add features missing from re10; those belong to the later Kindle-gap phase
after both applications converge and parity is stable.

## Public package shape

- `code/readerview0_version.h`: package `0.1.0-dev`, API 1, UI0 API 89 guard;
- `code/readerview0.h`: public umbrella;
- `code/readerview0.c`: stable unity entry that includes the implementation
  exactly once;
- `code/reader_view/reader_view.h`: public projection, state, layout, action,
  frame-storage, semantic, and operation declarations;
- `code/reader_view/reader_view.c`: implementation; and
- `code/readerview0_sources.manifest`: final-consumer source closure.

The build is strict C11 with `/W4 /WX`. All package storage is fixed-capacity
and caller-owned.

## Exact API 1 bounds

The contract publishes bounded Find and note drafts, visible TOC/Find/right-row
projections, settings/choice lists, action records, UI0 record arrays, text
bindings, and semantic nodes. The constants in `reader_view.h` are normative;
record-cap exhaustion is reported rather than allocated around.

Exactly four reading-setting kinds exist in API 1:

1. font family;
2. font size;
3. line spacing; and
4. theme.

These mirror current re10. Font weight, margins, alignment, columns, clocks,
time remaining, orientation, and other Kindle-gap settings are intentionally
absent.

## Ownership and mutation rule

| Field or record | Owner | Lifetime |
|---|---|---|
| projection strings, rows, choices, status, IDs | application | borrowed for one build |
| document/navigation/search/selection meaning | reader0 plus application adapters | host-defined |
| bookmarks, annotations, settings, export, lookup policy | application | persistent host lifetime |
| `ReaderViewState` | application storage; readerview0 semantics | across frames/document reset |
| Find and note draft arrays | inside caller-owned state | across frames until reset/cancel |
| frame storage | application | overwritten by next build |
| draw/text/semantic/action slices | readerview0 output in frame storage | until next build |
| EPUB viewport rendering and native accessibility objects | application | host-defined |

Persistent state is never changed optimistically. Bookmark, annotation,
setting, navigation, note, fullscreen, export, and lookup requests are returned
as bounded `ReaderViewAction` records. The application executes them and
publishes the authoritative result on a later frame.

When a host executes `ReaderViewAction_EditRightRowNote`, it resolves and
publishes the authoritative selection, then calls
`reader_view_open_note_editor`. The operation copies only the bounded note
draft and revision into caller-owned transient state; it does not retain the
projection or mutate the host record.

## Capability and empty-state rule

Every feature is gated by `ReaderViewFeatureFlags`, document capability flags,
and a load-status projection. A host can supply no settings, no TOC, no Find
results, no right-panel rows, or no selection tools. Absence does not make re10
product behavior mandatory and does not synthesize records.

Unavailable, empty, loading, ready, and error states are portable shared
surface states. Error and loading details remain host-provided text.

## Responsive contract

- width at least 1180: both requested panels can dock;
- width 840 through 1179: one panel docks and a simultaneously requested
  second panel overlays according to most-recent side;
- narrower than 840: panels overlay;
- width at least 1024: full toolbar;
- width 720 through 1023: compact toolbar plus overflow; and
- narrower than 720: minimal toolbar plus overflow.

Distraction-free state hides shared chrome without changing persistent host
state. `viewport_rect` is always the host renderer's current central contract.

## Focus and modal rule

Stable semantic IDs connect pointer, keyboard, and accessibility invocation.
Popup and modal input roots use UI0 signal containment. Escape dismisses the
top transient surface, then the right panel, then the left panel. A dirty note
draft is never discarded by Escape; explicit Cancel is required. Save remains
host-authoritative and carries both the selection key and source revision.

## Validation evidence

The deterministic tests cover:

- wide and narrow responsive modes;
- an empty-capability build;
- the complete feature projection and stable semantic/layout hash;
- pointer activation of the shared Contents control;
- bounded Find draft editing and action emission;
- native-adapter accessibility invocation through the normal action path;
- native-adapter progress focus and keyboard seeking through the normal action
  path;
- arrow navigation among shared list rows plus host-requested direct note
  editing; and
- fail-closed duplicate-key validation.

The architecture guard verifies the UI0-only dependency, unity inclusion,
manifest closure, and absence of document-engine, host, allocation, platform,
persistence, callback, vtable, event-bus, and DI dependencies.

## Adoption sequence

1. Commit API 1 locally in readerview0; keep the repository local-only.
2. Source-consume the package in a fresh lectern0 worktree.
3. Project lectern0's reader0 API 3 state, implement host-owned persistence and
   actions, and close current re10 feature gaps.
4. Validate behavior, visuals, persistence, focus/accessibility, and timing.
5. Fresh-fetch and explicitly reconcile re10 before any re10 adoption.

No readerview0 GitHub repository or re10 remote dependency is authorized by
this slice.

## Reconciled re10 source closure

After lectern0 adoption, re10's parallel editor work advanced its exact
zero_foundation snapshot to
`eee57edc1b0c7af5bef7afca26f3c27a32fb6e7c` without changing UI0 API 89 or the
readerview0 manifest. API 1 was rebuilt from scratch against that revision with
the same UI0 commit, strict C11 `/W4 /WX`, deterministic tests, and architecture
audit. Only the compatibility metadata and documentation changed; the public
API, implementation, capacities, dependency direction, and ownership remain
identical to `f59c9d59e0cc128327812ad1a20edeadfd828d58`.

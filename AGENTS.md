# readerview0 repository instructions

- readerview0 is a reusable reader view/chrome package, not a document engine
  or application host.
- Keep EPUB parsing, layout, pagination, semantic navigation, search state, and
  canonical frames in reader0.
- Keep persistence, annotations, decoded-image caches, native windows,
  accessibility adapters, commands, capture, and product integration in each
  application host.
- Build concrete shared UI only after comparing the real re10 and lectern0
  consumers. Do not invent provider tables, vtables, event buses, dependency
  injection frameworks, generic document interfaces, or process-global mutable
  state.
- Use UI0 for shared controls and layout. UI0 remains a lower dependency and
  must not depend on readerview0.
- Preserve caller-owned state, frame, layout, theme-input, and storage
  lifetimes in any future API.
- Slice 4B reserves the repository and architecture boundary only. Do not add
  a public API or implementation without an approved follow-up slice.

# readerview0 repository instructions

- readerview0 is a reusable reader view/chrome package, not a document engine
  or application host.
- Keep EPUB parsing, layout, pagination, semantic navigation, search state, and
  canonical frames in reader0.
- Keep persistence, annotations, decoded-image caches, native windows,
  accessibility adapters, commands, capture, and product integration in each
  application host.
- Build concrete shared UI only after comparing the real re10 and 8vo
  consumers. Do not invent provider tables, vtables, event buses, dependency
  injection frameworks, generic document interfaces, or process-global mutable
  state.
- Use UI0 for shared controls and layout. UI0 remains a lower dependency and
  must not depend on readerview0.
- Preserve caller-owned state, frame, layout, theme-input, and storage
  lifetimes in any future API.
- Preserve Reader View API 3 compatibility unless a deliberate API revision,
  metadata update, and consumer audit are part of the same change.
- Run the strict dependency guard, architecture audit, and package tests for
  every source or public-contract change.

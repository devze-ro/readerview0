# readerview0

`readerview0` is the reserved reusable reader view/chrome repository in the
dev0 family. Its intended consumers are re10 (future lore0) and lectern0.

Reader View Slice 4B creates this local-only repository after reconciling both
applications on reader0 API 3. It deliberately contains no shared-chrome API or
implementation yet. The first implementation slice must compare the real host
surfaces and define the smallest UI0-composed toolbar/panel/theme contract that
both can consume without moving document or application policy into this
package.

No remote is configured. Do not create or push a GitHub repository without
explicit authorization.

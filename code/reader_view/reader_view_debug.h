#ifndef READERVIEW0_READER_VIEW_DEBUG_H
#define READERVIEW0_READER_VIEW_DEBUG_H

#include "reader_view.h"

/*
 * Deterministic test evidence for comparing two hosts of the same Reader View
 * contract. Opaque host keys, generated UI ids, absolute client origins, and
 * frame indices are deliberately excluded. This header is diagnostic support;
 * it is not included by the readerview0 public umbrella.
 */
typedef struct ReaderViewDebugSnapshot
{
  UI0U64 projection_hash;
  UI0U64 layout_hash;
  UI0U64 control_hash;
  UI0U64 draw_hash;
  UI0U64 semantic_hash;
  UI0U64 action_hash;
  UI0S32 control_record_count;
  UI0S32 draw_command_count;
  UI0S32 semantic_node_count;
  UI0S32 action_count;
} ReaderViewDebugSnapshot;

UI0B32 reader_view_debug_snapshot(
  const ReaderViewProjection *projection,
  const ReaderViewFrameStorage *storage,
  const ReaderViewFrame *frame,
  ReaderViewDebugSnapshot *out_snapshot);

#endif /* READERVIEW0_READER_VIEW_DEBUG_H */

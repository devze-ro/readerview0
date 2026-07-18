#include "reader_view_debug.h"

#include <string.h>

enum
{
  RVDKeyUnknown = 0x7fffffff,
  RVDKeySetting = 0x10000,
  RVDKeyToc = 0x20000,
  RVDKeyFind = 0x30000,
  RVDKeyRight = 0x40000,
  RVDKeySelection = 0x50000,
  RVDKeyHighlightColor = 0x60000,
  RVDKeyBookmark = 0x70000,
};

static UI0U64
rvd_hash_byte(UI0U64 hash, UI0U32 value)
{
  hash ^= value & 0xffu;
  hash *= 1099511628211ull;
  return hash;
}

static UI0U64
rvd_hash_u64(UI0U64 hash, UI0U64 value)
{
  UI0U32 index;
  for (index = 0; index < 8; ++index)
  {
    hash = rvd_hash_byte(hash, (UI0U32)(value >> (index * 8)));
  }
  return hash;
}

static UI0U64
rvd_hash_text(UI0U64 hash, ReaderViewText text)
{
  UI0S32 index;
  UI0S32 size = text.data && text.size > 0 ? text.size : 0;
  hash = rvd_hash_u64(hash, (UI0U64)(UI0U32)size);
  for (index = 0; index < size; ++index)
  {
    hash = rvd_hash_byte(hash, (UI0U32)(unsigned char)text.data[index]);
  }
  return hash;
}

static UI0U64
rvd_hash_rect(UI0U64 hash, UI0Rect rect, UI0S32 origin_x, UI0S32 origin_y)
{
  UI0S32 x = rect.x;
  UI0S32 y = rect.y;
  if (rect.w == 0 && rect.h == 0)
  {
    x = 0;
    y = 0;
    origin_x = 0;
    origin_y = 0;
  }
  hash = rvd_hash_u64(hash, (UI0U64)(long long)(x - origin_x));
  hash = rvd_hash_u64(hash, (UI0U64)(long long)(y - origin_y));
  hash = rvd_hash_u64(hash, (UI0U64)(long long)rect.w);
  hash = rvd_hash_u64(hash, (UI0U64)(long long)rect.h);
  return hash;
}

static UI0U64
rvd_hash_status(UI0U64 hash, ReaderViewSurfaceStatus status)
{
  hash = rvd_hash_u64(hash, (UI0U64)status.state);
  hash = rvd_hash_text(hash, status.message);
  hash = rvd_hash_text(hash, status.detail);
  return hash;
}

static UI0U64
rvd_hash_choice_control(UI0U64 hash, ReaderViewChoiceControl control)
{
  UI0S32 index;
  hash = rvd_hash_u64(hash, (UI0U64)(long long)control.count);
  hash = rvd_hash_u64(hash, (UI0U64)control.presentation);
  for (index = 0; control.items && index < control.count; ++index)
  {
    const ReaderViewChoice *choice = control.items + index;
    hash = rvd_hash_text(hash, choice->label);
    hash = rvd_hash_text(hash, choice->detail);
    hash = rvd_hash_u64(hash, choice->flags);
  }
  return hash;
}

static UI0U64
rvd_normalized_key(const ReaderViewProjection *projection, ReaderViewKey key)
{
  UI0S32 index;
  UI0S32 choice_index;
  if (key == 0) return 0;
  for (index = 0; projection->settings.items &&
                  index < projection->settings.count; ++index)
  {
    const ReaderViewSettingControl *setting = projection->settings.items + index;
    for (choice_index = 0; setting->choices.items &&
                           choice_index < setting->choices.count; ++choice_index)
      if (setting->choices.items[choice_index].key == key)
        return RVDKeySetting + (UI0U64)setting->kind * READER_VIEW_CHOICE_CAP +
               (UI0U64)choice_index + 1;
  }
  for (index = 0; projection->toc.rows && index < projection->toc.row_count; ++index)
    if (projection->toc.rows[index].key == key) return RVDKeyToc + (UI0U64)index + 1;
  for (index = 0; projection->find.rows && index < projection->find.row_count; ++index)
    if (projection->find.rows[index].key == key) return RVDKeyFind + (UI0U64)index + 1;
  for (index = 0; projection->right.rows && index < projection->right.row_count; ++index)
    if (projection->right.rows[index].key == key) return RVDKeyRight + (UI0U64)index + 1;
  if (projection->selection.selection_key == key) return RVDKeySelection + 1;
  for (index = 0; projection->selection.highlight_colors.items &&
                  index < projection->selection.highlight_colors.count; ++index)
    if (projection->selection.highlight_colors.items[index].key == key)
      return RVDKeyHighlightColor + (UI0U64)index + 1;
  if (projection->current_bookmark_key == key) return RVDKeyBookmark + 1;
  return RVDKeyUnknown;
}

static UI0U64
rvd_projection_hash(const ReaderViewProjection *projection,
                    UI0S32 origin_x, UI0S32 origin_y)
{
  UI0U64 hash = 1469598103934665603ull;
  UI0S32 index;
#define RVD_LABEL(name) hash = rvd_hash_text(hash, projection->labels.name)
  hash = rvd_hash_u64(hash, projection->features);
  hash = rvd_hash_u64(hash, projection->document_flags);
  hash = rvd_hash_status(hash, projection->content);
  hash = rvd_hash_text(hash, projection->chrome_title);
  hash = rvd_hash_text(hash, projection->document_title);
  RVD_LABEL(open); RVD_LABEL(previous_page); RVD_LABEL(next_page);
  RVD_LABEL(back); RVD_LABEL(forward); RVD_LABEL(contents); RVD_LABEL(find);
  RVD_LABEL(reading_settings); RVD_LABEL(bookmark); RVD_LABEL(remove_bookmark);
  RVD_LABEL(annotations); RVD_LABEL(fullscreen); RVD_LABEL(exit_fullscreen);
  RVD_LABEL(distraction_free); RVD_LABEL(close); RVD_LABEL(clear);
  RVD_LABEL(previous_match); RVD_LABEL(next_match); RVD_LABEL(export_rows);
  RVD_LABEL(all); RVD_LABEL(bookmarks); RVD_LABEL(highlights); RVD_LABEL(notes);
  RVD_LABEL(go_to); RVD_LABEL(star); RVD_LABEL(unstar); RVD_LABEL(edit_note);
  RVD_LABEL(save_note); RVD_LABEL(cancel); RVD_LABEL(delete_value);
  RVD_LABEL(copy); RVD_LABEL(dictionary); RVD_LABEL(web_lookup);
  RVD_LABEL(translate); RVD_LABEL(more);
  RVD_LABEL(close_navigation); RVD_LABEL(search_input);
  RVD_LABEL(clear_search); RVD_LABEL(annotation_actions);
  RVD_LABEL(annotation_filters); RVD_LABEL(export_annotations);
  RVD_LABEL(close_annotations); RVD_LABEL(delete_bookmark);
  RVD_LABEL(delete_note); RVD_LABEL(delete_highlight);
  RVD_LABEL(contents_short); RVD_LABEL(contents_panel_title);
  RVD_LABEL(find_panel_title); RVD_LABEL(filter_annotations);
  RVD_LABEL(no_contents); RVD_LABEL(find_prompt); RVD_LABEL(no_matches);
  RVD_LABEL(no_annotations); RVD_LABEL(no_bookmarks);
  RVD_LABEL(no_highlights); RVD_LABEL(no_notes);
  RVD_LABEL(find_placeholder);
#undef RVD_LABEL
  hash = rvd_hash_status(hash, projection->progress.status);
  hash = rvd_hash_u64(hash, projection->progress.location_index);
  hash = rvd_hash_u64(hash, projection->progress.location_count);
  hash = rvd_hash_u64(hash, projection->progress.page_index);
  hash = rvd_hash_u64(hash, projection->progress.page_count);
  hash = rvd_hash_text(hash, projection->progress.chapter);
  hash = rvd_hash_text(hash, projection->progress.label);
  hash = rvd_hash_u64(hash, (UI0U64)projection->progress.can_seek);
  hash = rvd_hash_status(hash, projection->settings.status);
  hash = rvd_hash_u64(hash, (UI0U64)(long long)projection->settings.count);
  for (index = 0; projection->settings.items && index < projection->settings.count; ++index)
  {
    const ReaderViewSettingControl *setting = projection->settings.items + index;
    hash = rvd_hash_u64(hash, (UI0U64)setting->kind);
    hash = rvd_hash_text(hash, setting->label);
    hash = rvd_hash_text(hash, setting->help);
    hash = rvd_hash_status(hash, setting->status);
    hash = rvd_hash_choice_control(hash, setting->choices);
  }
  hash = rvd_hash_status(hash, projection->toc.status);
  hash = rvd_hash_u64(hash, (UI0U64)(long long)projection->toc.row_count);
  hash = rvd_hash_u64(hash, projection->toc.total_count);
  for (index = 0; projection->toc.rows && index < projection->toc.row_count; ++index)
  {
    const ReaderViewTocRow *row = projection->toc.rows + index;
    hash = rvd_hash_u64(hash, row->depth);
    hash = rvd_hash_text(hash, row->label);
    hash = rvd_hash_text(hash, row->detail);
    hash = rvd_hash_u64(hash, row->flags);
  }
  hash = rvd_hash_status(hash, projection->find.status);
  hash = rvd_hash_text(hash, projection->find.committed_query);
  hash = rvd_hash_u64(hash, (UI0U64)(long long)projection->find.row_count);
  hash = rvd_hash_u64(hash, projection->find.total_count);
  hash = rvd_hash_u64(hash, (UI0U64)(long long)projection->find.active_index);
  hash = rvd_hash_u64(hash, (UI0U64)projection->find.has_more);
  hash = rvd_hash_u64(hash, (UI0U64)projection->find.can_step_previous);
  hash = rvd_hash_u64(hash, (UI0U64)projection->find.can_step_next);
  for (index = 0; projection->find.rows && index < projection->find.row_count; ++index)
  {
    const ReaderViewFindRow *row = projection->find.rows + index;
    hash = rvd_hash_text(hash, row->section);
    hash = rvd_hash_text(hash, row->excerpt);
    hash = rvd_hash_text(hash, row->detail);
    hash = rvd_hash_u64(hash, row->match_start);
    hash = rvd_hash_u64(hash, row->match_size);
    hash = rvd_hash_u64(hash, row->flags);
  }
  hash = rvd_hash_status(hash, projection->right.status);
  hash = rvd_hash_u64(hash, (UI0U64)(long long)projection->right.row_count);
  hash = rvd_hash_u64(hash, projection->right.total_count);
  hash = rvd_hash_u64(hash, (UI0U64)projection->right.has_more);
  hash = rvd_hash_u64(hash, projection->right.available_filters);
  hash = rvd_hash_u64(hash, projection->right.all_count);
  hash = rvd_hash_u64(hash, projection->right.bookmark_count);
  hash = rvd_hash_u64(hash, projection->right.highlight_count);
  hash = rvd_hash_u64(hash, projection->right.note_count);
  for (index = 0; projection->right.rows && index < projection->right.row_count; ++index)
  {
    const ReaderViewRightRow *row = projection->right.rows + index;
    hash = rvd_hash_u64(hash, (UI0U64)row->kind);
    hash = rvd_hash_text(hash, row->section);
    hash = rvd_hash_text(hash, row->primary);
    hash = rvd_hash_text(hash, row->secondary);
    hash = rvd_hash_u64(hash, rvd_normalized_key(projection, row->color_key));
    hash = rvd_hash_u64(hash, row->rail_color);
    hash = rvd_hash_u64(hash, row->flags);
    hash = rvd_hash_u64(hash, row->actions);
  }
  hash = rvd_hash_status(hash, projection->selection.status);
  hash = rvd_hash_u64(hash, projection->selection.revision != 0);
  hash = rvd_hash_text(hash, projection->selection.selected_text);
  hash = rvd_hash_text(hash, projection->selection.note_text);
  hash = rvd_hash_u64(hash, rvd_normalized_key(
    projection, projection->selection.current_color_key));
  hash = rvd_hash_u64(hash, projection->selection.flags);
  hash = rvd_hash_choice_control(hash, projection->selection.highlight_colors);
  hash = rvd_hash_rect(hash, projection->selection.anchor_rect, origin_x, origin_y);
  return hash;
}

static UI0U64
rvd_layout_hash(const ReaderViewLayout *layout)
{
  UI0U64 hash = 1469598103934665603ull;
  UI0S32 origin_x = layout->bounds.x;
  UI0S32 origin_y = layout->bounds.y;
#define RVD_SCALAR(name) hash = rvd_hash_u64(hash, (UI0U64)layout->name)
#define RVD_RECT(name) hash = rvd_hash_rect(hash, layout->name, origin_x, origin_y)
  RVD_SCALAR(mode); RVD_SCALAR(toolbar_density); RVD_SCALAR(toolbar_visible);
  RVD_SCALAR(progress_visible); RVD_SCALAR(left_panel_visible);
  RVD_SCALAR(right_panel_visible); RVD_SCALAR(left_panel_overlay);
  RVD_SCALAR(right_panel_overlay); RVD_SCALAR(previous_gutter_visible);
  RVD_SCALAR(next_gutter_visible);
  RVD_RECT(bounds); RVD_RECT(toolbar_rect); RVD_RECT(shared_toolbar_rect);
  RVD_RECT(host_toolbar_leading_rect); RVD_RECT(host_toolbar_trailing_rect);
  RVD_RECT(body_rect); RVD_RECT(left_panel_rect); RVD_RECT(right_panel_rect);
  RVD_RECT(viewport_rect); RVD_RECT(page_surface_rect); RVD_RECT(content_rect);
  RVD_RECT(previous_gutter_rect); RVD_RECT(next_gutter_rect);
  RVD_RECT(previous_gutter_visual_rect); RVD_RECT(next_gutter_visual_rect);
  RVD_RECT(progress_rect);
#undef RVD_RECT
#undef RVD_SCALAR
  return hash;
}

static UI0S32
rvd_control_count(const ReaderViewFrameStorage *storage)
{
  UI0S32 index;
  for (index = 0; index < READER_VIEW_CONTROL_CAP; ++index)
    if (storage->control_records[index].id == 0) break;
  return index;
}

static UI0U64
rvd_control_hash(const ReaderViewFrameStorage *storage, UI0S32 count,
                 UI0S32 origin_x, UI0S32 origin_y)
{
  UI0U64 hash = 1469598103934665603ull;
  UI0S32 index;
  hash = rvd_hash_u64(hash, (UI0U64)(long long)count);
  for (index = 0; index < count; ++index)
  {
    const UI0ControlRecord *record = storage->control_records + index;
    hash = rvd_hash_u64(hash, (UI0U64)record->kind);
    hash = rvd_hash_u64(hash, (UI0U64)(long long)record->box_index);
    hash = rvd_hash_u64(hash, (UI0U64)record->root);
    hash = rvd_hash_u64(hash, record->control_flags);
    hash = rvd_hash_u64(hash, record->state);
    hash = rvd_hash_u64(hash, record->signal_flags);
    hash = rvd_hash_rect(hash, record->rect, origin_x, origin_y);
    hash = rvd_hash_rect(hash, record->clip_rect, origin_x, origin_y);
    hash = rvd_hash_rect(hash, record->text_rect, origin_x, origin_y);
    hash = rvd_hash_rect(hash, record->indicator_rect, origin_x, origin_y);
    hash = rvd_hash_u64(hash, record->label_hash);
    hash = rvd_hash_u64(hash, (UI0U64)(long long)record->label_len);
    hash = rvd_hash_u64(hash, (UI0U64)record->value);
    hash = rvd_hash_u64(hash, (UI0U64)record->next_value);
  }
  return hash;
}

static UI0U64
rvd_draw_hash(const ReaderViewFrame *frame, UI0S32 origin_x, UI0S32 origin_y)
{
  UI0U64 hash = 1469598103934665603ull;
  UI0S32 index;
  hash = rvd_hash_u64(hash, (UI0U64)(long long)frame->draw_command_count);
  for (index = 0; frame->draw_commands && index < frame->draw_command_count; ++index)
  {
    const UI0DrawCommand *command = frame->draw_commands + index;
    hash = rvd_hash_u64(hash, (UI0U64)command->op);
    hash = rvd_hash_u64(hash, (UI0U64)command->source_kind);
    hash = rvd_hash_rect(hash, command->rect, origin_x, origin_y);
    hash = rvd_hash_rect(hash, command->clip_rect, origin_x, origin_y);
    hash = rvd_hash_u64(hash, command->color);
    hash = rvd_hash_u64(hash, command->stroke_color);
    hash = rvd_hash_u64(hash, (UI0U64)command->icon_kind);
    hash = rvd_hash_u64(hash, (UI0U64)(long long)command->stroke_width);
    hash = rvd_hash_u64(hash, (UI0U64)(long long)command->corner_radius);
    hash = rvd_hash_u64(hash, command->flags);
    hash = rvd_hash_u64(hash, command->label_hash);
    hash = rvd_hash_u64(hash, (UI0U64)(long long)command->label_len);
    hash = rvd_hash_u64(hash, (UI0U64)command->text_align_x);
    hash = rvd_hash_u64(hash, (UI0U64)command->text_align_y);
    hash = rvd_hash_u64(hash, (UI0U64)command->has_text_alignment);
    hash = rvd_hash_u64(hash, (UI0U64)command->typography_role);
    hash = rvd_hash_u64(hash, (UI0U64)(long long)command->typography_char_width);
    hash = rvd_hash_u64(hash, (UI0U64)(long long)command->typography_line_height);
    hash = rvd_hash_u64(hash, (UI0U64)command->has_typography_role);
  }
  return hash;
}

static UI0S32
rvd_semantic_parent_index(const ReaderViewFrame *frame, UI0ID parent_id)
{
  UI0S32 index;
  if (parent_id == 0) return -1;
  for (index = 0; index < frame->semantic_node_count; ++index)
    if (frame->semantic_nodes[index].id == parent_id) return index;
  return -2;
}

static UI0U64
rvd_semantic_hash(const ReaderViewFrame *frame, UI0S32 origin_x, UI0S32 origin_y)
{
  UI0U64 hash = 1469598103934665603ull;
  UI0S32 index;
  hash = rvd_hash_u64(hash, (UI0U64)(long long)frame->semantic_node_count);
  for (index = 0; frame->semantic_nodes && index < frame->semantic_node_count; ++index)
  {
    const ReaderViewSemanticNode *node = frame->semantic_nodes + index;
    hash = rvd_hash_u64(hash, (UI0U64)(long long)
      rvd_semantic_parent_index(frame, node->parent_id));
    hash = rvd_hash_u64(hash, (UI0U64)node->role);
    hash = rvd_hash_u64(hash, node->flags);
    hash = rvd_hash_rect(hash, node->rect, origin_x, origin_y);
    hash = rvd_hash_text(hash, node->name);
    hash = rvd_hash_text(hash, node->value);
    hash = rvd_hash_u64(hash, (UI0U64)node->control);
    hash = rvd_hash_u64(hash, node->range_value);
    hash = rvd_hash_u64(hash, node->range_min);
    hash = rvd_hash_u64(hash, node->range_max);
  }
  return hash;
}

static UI0U64
rvd_action_value(const ReaderViewAction *action)
{
  if (action->kind == ReaderViewAction_SaveNote ||
      action->kind == ReaderViewAction_DeleteNote ||
      action->kind == ReaderViewAction_CancelNote)
    return action->value != 0;
  return action->value;
}

static UI0U64
rvd_action_hash(const ReaderViewProjection *projection,
                const ReaderViewFrame *frame)
{
  UI0U64 hash = 1469598103934665603ull;
  UI0S32 index;
  hash = rvd_hash_u64(hash, (UI0U64)(long long)frame->action_count);
  for (index = 0; frame->actions && index < frame->action_count; ++index)
  {
    const ReaderViewAction *action = frame->actions + index;
    hash = rvd_hash_u64(hash, (UI0U64)action->kind);
    hash = rvd_hash_u64(hash, rvd_normalized_key(projection, action->key));
    hash = rvd_hash_u64(hash, rvd_normalized_key(projection, action->auxiliary_key));
    hash = rvd_hash_u64(hash, (UI0U64)action->setting_kind);
    hash = rvd_hash_u64(hash, (UI0U64)action->right_row_kind);
    hash = rvd_hash_u64(hash, (UI0U64)action->right_filter);
    hash = rvd_hash_u64(hash, rvd_action_value(action));
    hash = rvd_hash_text(hash, action->text);
  }
  return hash;
}

UI0B32
reader_view_debug_snapshot(const ReaderViewProjection *projection,
                           const ReaderViewFrameStorage *storage,
                           const ReaderViewFrame *frame,
                           ReaderViewDebugSnapshot *out_snapshot)
{
  UI0S32 origin_x;
  UI0S32 origin_y;
  if (!projection || !storage || !frame || !out_snapshot) return 0;
  memset(out_snapshot, 0, sizeof(*out_snapshot));
  origin_x = frame->layout.bounds.x;
  origin_y = frame->layout.bounds.y;
  out_snapshot->control_record_count = rvd_control_count(storage);
  out_snapshot->draw_command_count = frame->draw_command_count;
  out_snapshot->semantic_node_count = frame->semantic_node_count;
  out_snapshot->action_count = frame->action_count;
  out_snapshot->projection_hash = rvd_projection_hash(projection, origin_x, origin_y);
  out_snapshot->layout_hash = rvd_layout_hash(&frame->layout);
  out_snapshot->control_hash = rvd_control_hash(
    storage, out_snapshot->control_record_count, origin_x, origin_y);
  out_snapshot->draw_hash = rvd_draw_hash(frame, origin_x, origin_y);
  out_snapshot->semantic_hash = rvd_semantic_hash(frame, origin_x, origin_y);
  out_snapshot->action_hash = rvd_action_hash(projection, frame);
  return 1;
}

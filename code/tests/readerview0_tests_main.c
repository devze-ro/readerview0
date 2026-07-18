#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "readerview0.h"
#include "reader_view/reader_view_debug.h"

static int failures;

static void
check(int condition, const char *name)
{
  if (!condition)
  {
    fprintf(stderr, "FAIL: %s\n", name);
    failures += 1;
  }
}

static int
rect_equal(UI0Rect a, UI0Rect b)
{
  return a.x == b.x && a.y == b.y && a.w == b.w && a.h == b.h;
}

static ReaderViewSurfaceStatus
ready_status(void)
{
  ReaderViewSurfaceStatus result;
  memset(&result, 0, sizeof(result));
  result.state = ReaderViewLoad_Ready;
  return result;
}

static ReaderViewProjection
minimal_projection(void)
{
  ReaderViewProjection result;
  memset(&result, 0, sizeof(result));
  result.document_key = 1;
  result.content = ready_status();
  result.progress.status = ready_status();
  result.settings.status = ready_status();
  result.toc.status = ready_status();
  result.find.status = ready_status();
  result.find.active_index = -1;
  result.right.status = ready_status();
  result.selection.status = ready_status();
  return result;
}

static int
text_equal(ReaderViewText text, const char *literal)
{
  size_t size = strlen(literal);
  return text.size == (UI0S32)size &&
         (size == 0 || memcmp(text.data, literal, size) == 0);
}

static const ReaderViewSemanticNode *
find_semantic(const ReaderViewFrame *frame, const char *name)
{
  UI0S32 index;
  for (index = 0; index < frame->semantic_node_count; ++index)
    if (text_equal(frame->semantic_nodes[index].name, name))
      return frame->semantic_nodes + index;
  return 0;
}

static const ReaderViewSemanticNode *
find_semantic_role(const ReaderViewFrame *frame, const char *name,
                   ReaderViewSemanticRole role)
{
  UI0S32 index;
  for (index = 0; index < frame->semantic_node_count; ++index)
    if (frame->semantic_nodes[index].role == role &&
        text_equal(frame->semantic_nodes[index].name, name))
      return frame->semantic_nodes + index;
  return 0;
}

static const ReaderViewAction *
find_action(const ReaderViewFrame *frame, ReaderViewActionKind kind)
{
  UI0S32 index;
  for (index = 0; index < frame->action_count; ++index)
    if (frame->actions[index].kind == kind)
      return frame->actions + index;
  return 0;
}

static UI0S32
test_find_codepoint_advance(UI0U32 codepoint)
{
  if (codepoint == (UI0U32)'v') return 5;
  if (codepoint == (UI0U32)'w') return 11;
  if (codepoint == (UI0U32)'y') return 6;
  if (codepoint == (UI0U32)'g') return 7;
  return 8;
}

static ReaderViewFindTextMetrics
test_find_text_metrics(ReaderViewCodepointAdvance *advances)
{
  ReaderViewFindTextMetrics result;
  UI0S32 index;
  for (index = 1; index < 128; ++index)
  {
    advances[index - 1].codepoint = (UI0U32)index;
    advances[index - 1].advance =
      test_find_codepoint_advance((UI0U32)index);
  }
  result.advances = advances;
  result.advance_count = 127;
  result.fallback_advance = 8;
  return result;
}

static UI0S32
test_find_text_width(const char *text)
{
  UI0S32 result = 0;
  while (text && *text)
  {
    result += test_find_codepoint_advance((unsigned char)*text);
    text += 1;
  }
  return result;
}

static UI0S32
test_find_text_range_width(ReaderViewText text)
{
  UI0S32 result = 0;
  UI0S32 index;
  for (index = 0; index < text.size; ++index)
    result += test_find_codepoint_advance((unsigned char)text.data[index]);
  return result;
}

static UI0S32
test_note_codepoint_advance(UI0U32 codepoint)
{
  if (codepoint == (UI0U32)' ') return 5;
  if (codepoint == (UI0U32)'i') return 4;
  if (codepoint == (UI0U32)'t') return 7;
  if (codepoint == (UI0U32)'a') return 6;
  return 10;
}

static ReaderViewNoteTextMetrics
test_note_text_metrics(ReaderViewCodepointAdvance *advances)
{
  ReaderViewNoteTextMetrics result;
  UI0S32 index;
  for (index = 1; index < 128; ++index)
  {
    advances[index - 1].codepoint = (UI0U32)index;
    advances[index - 1].advance =
      test_note_codepoint_advance((UI0U32)index);
  }
  result.advances = advances;
  result.advance_count = 127;
  result.fallback_advance = 10;
  result.pixel_height = 18;
  result.line_height = 25;
  return result;
}

static const ReaderViewSemanticNode *
find_semantic_control_source(const ReaderViewFrame *frame,
                             ReaderViewSemanticControl control,
                             ReaderViewKey source_key)
{
  UI0S32 index;
  for (index = 0; index < frame->semantic_node_count; ++index)
  {
    const ReaderViewSemanticNode *node = frame->semantic_nodes + index;
    if (node->control == control && node->source_key == source_key)
      return node;
  }
  return 0;
}

static const ReaderViewSemanticNode *
find_semantic_id(const ReaderViewFrame *frame, UI0ID id)
{
  UI0S32 index;
  for (index = 0; index < frame->semantic_node_count; ++index)
    if (frame->semantic_nodes[index].id == id)
      return frame->semantic_nodes + index;
  return 0;
}

static const UI0SidenavRecord *
find_reference_sidenav_record(const ReaderViewFrameStorage *storage,
                              UI0ID id)
{
  UI0S32 index;
  for (index = 0; index < READER_VIEW_REFERENCE_SIDENAV_RECORD_CAP; ++index)
    if (storage->reference_sidenav_records[index].id == id)
      return storage->reference_sidenav_records + index;
  return 0;
}

static UI0S32
count_draw_op_for_source(const ReaderViewFrame *frame,
                         UI0DrawOpKind op,
                         UI0ID source_id)
{
  UI0S32 index;
  UI0S32 result = 0;
  for (index = 0; index < frame->draw_command_count; ++index)
    if (frame->draw_commands[index].op == op &&
        frame->draw_commands[index].source_id == source_id)
    {
      result += 1;
    }
  return result;
}

static UI0S32
count_draw_op(const ReaderViewFrame *frame, UI0DrawOpKind op)
{
  UI0S32 index;
  UI0S32 result = 0;
  for (index = 0; index < frame->draw_command_count; ++index)
    if (frame->draw_commands[index].op == op) result += 1;
  return result;
}

static const UI0DrawCommand *
find_icon_for_source(const ReaderViewFrame *frame, UI0ID source_id)
{
  UI0S32 index;
  for (index = 0; index < frame->draw_command_count; ++index)
    if (frame->draw_commands[index].op == UI0DrawOp_Icon &&
        frame->draw_commands[index].source_id == source_id)
    {
      return frame->draw_commands + index;
    }
  return 0;
}

static const UI0DrawCommand *
find_draw_for_source(const ReaderViewFrame *frame,
                     UI0DrawOpKind op,
                     UI0ID source_id)
{
  UI0S32 index;
  for (index = 0; index < frame->draw_command_count; ++index)
    if (frame->draw_commands[index].op == op &&
        frame->draw_commands[index].source_id == source_id)
      return frame->draw_commands + index;
  return 0;
}

static UI0S32
find_draw_index_for_source(const ReaderViewFrame *frame,
                           UI0DrawOpKind op,
                           UI0ID source_id)
{
  UI0S32 index;
  for (index = 0; index < frame->draw_command_count; ++index)
    if (frame->draw_commands[index].op == op &&
        frame->draw_commands[index].source_id == source_id)
      return index;
  return -1;
}

static UI0S32
collect_one_pixel_draws_for_source(const ReaderViewFrame *frame,
                                   UI0DrawOpKind op,
                                   UI0ID source_id,
                                   const UI0DrawCommand **commands,
                                   UI0S32 *indices,
                                   UI0S32 capacity)
{
  UI0S32 index;
  UI0S32 count = 0;
  for (index = 0; index < frame->draw_command_count; ++index)
  {
    const UI0DrawCommand *command = frame->draw_commands + index;
    if (command->op != op || command->source_id != source_id ||
        command->rect.w != 1 || command->rect.h != 1)
      continue;
    if (count < capacity)
    {
      if (commands) commands[count] = command;
      if (indices) indices[count] = index;
    }
    count += 1;
  }
  return count;
}

static const UI0DrawCommand *
find_draw_for_rect(const ReaderViewFrame *frame,
                   UI0DrawOpKind op,
                   UI0Rect rect)
{
  UI0S32 index;
  for (index = 0; index < frame->draw_command_count; ++index)
    if (frame->draw_commands[index].op == op &&
        rect_equal(frame->draw_commands[index].rect, rect))
      return frame->draw_commands + index;
  return 0;
}

static const UI0ControlRecord *
find_control_for_source(const ReaderViewFrameStorage *storage, UI0ID source_id)
{
  UI0S32 index;
  for (index = 0; index < READER_VIEW_CONTROL_CAP; ++index)
    if (storage->control_records[index].id == source_id)
      return storage->control_records + index;
  return 0;
}

static const UI0SignalRecord *
find_signal_for_source(const ReaderViewFrameStorage *storage, UI0ID source_id)
{
  UI0S32 index;
  for (index = 0; index < READER_VIEW_SIGNAL_CAP; ++index)
    if (storage->signal_records[index].id == source_id)
      return storage->signal_records + index;
  return 0;
}

static int
scroll_records_empty(const ReaderViewFrameStorage *storage)
{
  UI0S32 index;
  for (index = 0; index < READER_VIEW_SCROLL_CAP; ++index)
  {
    if (storage->scroll_records[index].id != 0) return 0;
  }
  return 1;
}

static const ReaderViewTextBinding *
find_text_binding(const ReaderViewFrame *frame, UI0ID source_id)
{
  UI0S32 index;
  for (index = 0; index < frame->text_binding_count; ++index)
    if (frame->text_bindings[index].source_id == source_id)
      return frame->text_bindings + index;
  return 0;
}

static UI0S32
count_semantic(const ReaderViewFrame *frame, const char *name)
{
  UI0S32 index;
  UI0S32 result = 0;
  for (index = 0; index < frame->semantic_node_count; ++index)
    if (text_equal(frame->semantic_nodes[index].name, name)) result += 1;
  return result;
}

static UI0S32
count_semantic_role(const ReaderViewFrame *frame, ReaderViewSemanticRole role)
{
  UI0S32 index;
  UI0S32 result = 0;
  for (index = 0; index < frame->semantic_node_count; ++index)
    if (frame->semantic_nodes[index].role == role) result += 1;
  return result;
}

static void
check_icon_control(const ReaderViewFrame *frame,
                   const char *name,
                   UI0Rect rect,
                   UI0IconKind icon_kind,
                   const char *test_name)
{
  const ReaderViewSemanticNode *semantic = find_semantic(frame, name);
  const UI0DrawCommand *icon = semantic ?
    find_icon_for_source(frame, semantic->id) : 0;
  check(semantic != 0 && rect_equal(semantic->rect, rect) &&
        icon != 0 && icon->icon_kind == icon_kind &&
        count_draw_op_for_source(frame, UI0DrawOp_Text,
                                 semantic ? semantic->id : 0) == 0,
        test_name);
}

static void
check_loaded_toolbar_icon_order(const ReaderViewFrame *frame)
{
  static const UI0IconKind expected[11] = {
    UI0IconKind_List,
    UI0IconKind_Search,
    UI0IconKind_ArrowLeft,
    UI0IconKind_ArrowRight,
    UI0IconKind_Expand,
    UI0IconKind_TextSize,
    UI0IconKind_LineSpacing,
    UI0IconKind_CaseSensitive,
    UI0IconKind_SunMoon,
    UI0IconKind_Notebook,
    UI0IconKind_Bookmark,
  };
  UI0S32 draw_index;
  UI0S32 icon_index = 0;
  UI0B32 matches = 1;
  for (draw_index = 0; draw_index < frame->draw_command_count; ++draw_index)
  {
    const UI0DrawCommand *command = frame->draw_commands + draw_index;
    if (command->op != UI0DrawOp_Icon) continue;
    if (icon_index >= 11 || command->icon_kind != expected[icon_index])
      matches = 0;
    icon_index += 1;
  }
  check(matches && icon_index == 11,
        "loaded toolbar has exact fixed icon order and count");
}

static UI0U64
hash_bytes(UI0U64 hash, const void *data, UI0U32 size)
{
  const unsigned char *bytes = (const unsigned char *)data;
  UI0U32 index;
  UI0U64 result = hash ? hash : 1469598103934665603ull;
  for (index = 0; index < size; ++index)
  {
    result ^= bytes[index];
    result *= 1099511628211ull;
  }
  return result;
}

static UI0U64
frame_contract_hash(const ReaderViewFrame *frame)
{
  UI0U64 hash = 1469598103934665603ull;
  UI0S32 index;
  hash = hash_bytes(hash, &frame->layout, (UI0U32)sizeof(frame->layout));
  for (index = 0; index < frame->semantic_node_count; ++index)
  {
    const ReaderViewSemanticNode *node = frame->semantic_nodes + index;
    hash = hash_bytes(hash, &node->id, (UI0U32)sizeof(node->id));
    hash = hash_bytes(hash, &node->parent_id, (UI0U32)sizeof(node->parent_id));
    hash = hash_bytes(hash, &node->role, (UI0U32)sizeof(node->role));
    hash = hash_bytes(hash, &node->flags, (UI0U32)sizeof(node->flags));
    hash = hash_bytes(hash, &node->rect, (UI0U32)sizeof(node->rect));
    hash = hash_bytes(hash, node->name.data, (UI0U32)node->name.size);
    hash = hash_bytes(hash, node->value.data, (UI0U32)node->value.size);
    hash = hash_bytes(hash, &node->source_key,
                      (UI0U32)sizeof(node->source_key));
  }
  return hash;
}

static ReaderViewProjection
full_projection(ReaderViewSettingControl *settings,
                ReaderViewChoice *setting_choices,
                ReaderViewTocRow *toc_rows,
                ReaderViewFindRow *find_rows,
                ReaderViewRightRow *right_rows)
{
  ReaderViewProjection result = minimal_projection();
  ReaderViewChoice *font_choices = setting_choices;
  memset(settings, 0, READER_VIEW_SETTING_CAP * sizeof(*settings));
  memset(setting_choices, 0, 8 * sizeof(*setting_choices));
  memset(toc_rows, 0, 2 * sizeof(*toc_rows));
  memset(find_rows, 0, sizeof(*find_rows));
  memset(right_rows, 0, sizeof(*right_rows));
  result.features = ReaderViewFeature_Open |
                    ReaderViewFeature_Paging |
                    ReaderViewFeature_History |
                    ReaderViewFeature_Contents |
                    ReaderViewFeature_Find |
                    ReaderViewFeature_Progress |
                    ReaderViewFeature_ReadingSettings |
                    ReaderViewFeature_Bookmark |
                    ReaderViewFeature_Annotations |
                    ReaderViewFeature_SelectionTools |
                    ReaderViewFeature_Fullscreen |
                    ReaderViewFeature_DistractionFree |
                    ReaderViewFeature_Lookup |
                    ReaderViewFeature_Export;
  result.document_flags = ReaderViewDocument_Open |
                          ReaderViewDocument_CanOpen |
                          ReaderViewDocument_CanGoPreviousPage |
                          ReaderViewDocument_CanGoNextPage |
                          ReaderViewDocument_CanGoBack |
                          ReaderViewDocument_CanGoForward |
                          ReaderViewDocument_CanToggleFullscreen |
                          ReaderViewDocument_CanToggleDistraction;
  result.chrome_title.data = "EPUB Reader";
  result.chrome_title.size = 11;
  result.document_title.data = "Host document title";
  result.document_title.size = 19;
  result.labels.annotations.data = "Annotations";
  result.labels.annotations.size = 11;
  result.progress.location_count = 100;
  result.progress.location_index = 24;
  result.progress.page_count = 10;
  result.progress.page_index = 2;
  result.progress.can_seek = 1;
  result.progress.chapter.data = "Chapter 1";
  result.progress.chapter.size = 9;
  result.progress.label.data = "3 of 10";
  result.progress.label.size = 7;

  {
    static const char *font_labels[5] = {
      "Serif", "Sans", "Georgia", "Atkinson", "System"
    };
    UI0S32 index;
    for (index = 0; index < 5; ++index)
    {
      font_choices[index].key = 10 + (ReaderViewKey)index;
      font_choices[index].label.data = font_labels[index];
      font_choices[index].label.size = (UI0S32)strlen(font_labels[index]);
      font_choices[index].flags = ReaderViewChoice_Enabled |
        (index == 0 ? ReaderViewChoice_Selected : 0);
    }
  }
  setting_choices[5].key = 15;
  setting_choices[5].label.data = "Default";
  setting_choices[5].label.size = 7;
  setting_choices[5].flags = ReaderViewChoice_Enabled |
                             ReaderViewChoice_Selected;
  setting_choices[6].key = 16;
  setting_choices[6].label.data = "Comfortable";
  setting_choices[6].label.size = 11;
  setting_choices[6].flags = ReaderViewChoice_Enabled |
                             ReaderViewChoice_Selected;
  setting_choices[7].key = 17;
  setting_choices[7].label.data = "Light";
  setting_choices[7].label.size = 5;
  setting_choices[7].flags = ReaderViewChoice_Enabled |
                             ReaderViewChoice_Selected;

  settings[0].kind = ReaderViewSetting_Theme;
  settings[0].label.data = "Theme";
  settings[0].label.size = 5;
  settings[0].status = ready_status();
  settings[0].choices.items = setting_choices + 7;
  settings[0].choices.count = 1;
  settings[0].choices.presentation = ReaderViewChoicePresentation_Segments;
  settings[1].kind = ReaderViewSetting_FontFamily;
  settings[1].label.data = "Font";
  settings[1].label.size = 4;
  settings[1].status = ready_status();
  settings[1].choices.items = font_choices;
  settings[1].choices.count = 5;
  settings[1].choices.presentation = ReaderViewChoicePresentation_Menu;
  settings[2].kind = ReaderViewSetting_LineSpacing;
  settings[2].label.data = "Spacing";
  settings[2].label.size = 7;
  settings[2].status = ready_status();
  settings[2].choices.items = setting_choices + 6;
  settings[2].choices.count = 1;
  settings[2].choices.presentation = ReaderViewChoicePresentation_Stepper;
  settings[3].kind = ReaderViewSetting_FontSize;
  settings[3].label.data = "Size";
  settings[3].label.size = 4;
  settings[3].status = ready_status();
  settings[3].choices.items = setting_choices + 5;
  settings[3].choices.count = 1;
  settings[3].choices.presentation = ReaderViewChoicePresentation_Stepper;
  result.settings.items = settings;
  result.settings.count = READER_VIEW_SETTING_CAP;

  toc_rows[0].key = 20;
  toc_rows[0].label.data = "One";
  toc_rows[0].label.size = 3;
  toc_rows[0].flags = ReaderViewRow_Enabled | ReaderViewRow_Current;
  toc_rows[1].key = 21;
  toc_rows[1].label.data = "Two";
  toc_rows[1].label.size = 3;
  toc_rows[1].flags = ReaderViewRow_Enabled;
  result.toc.rows = toc_rows;
  result.toc.row_count = 2;
  result.toc.total_count = 2;

  find_rows[0].key = 30;
  find_rows[0].section.data = "One";
  find_rows[0].section.size = 3;
  find_rows[0].excerpt.data = "A result";
  find_rows[0].excerpt.size = 8;
  find_rows[0].match_start = 2;
  find_rows[0].match_size = 6;
  find_rows[0].flags = ReaderViewRow_Enabled;
  result.find.rows = find_rows;
  result.find.row_count = 1;
  result.find.total_count = 1;
  result.find.can_step_next = 1;

  right_rows[0].key = 40;
  right_rows[0].kind = ReaderViewRightRow_Bookmark;
  right_rows[0].section.data = "One";
  right_rows[0].section.size = 3;
  right_rows[0].primary.data = "Saved place";
  right_rows[0].primary.size = 11;
  right_rows[0].flags = ReaderViewRow_Enabled;
  right_rows[0].actions = ReaderViewRightAction_Activate |
                          ReaderViewRightAction_ToggleStar |
                          ReaderViewRightAction_Delete;
  result.right.rows = right_rows;
  result.right.row_count = 1;
  result.right.total_count = 1;
  result.right.available_filters = ReaderViewRightFilterFlag_All |
                                   ReaderViewRightFilterFlag_Bookmarks;
  return result;
}

static void
test_zero_document_interaction(const UI0ResolvedTheme *theme)
{
  static ReaderViewState state;
  static ReaderViewFrameStorage storage;
  ReaderViewLayoutInput layout_input;
  ReaderViewLayout layout;
  ReaderViewProjection projection = minimal_projection();
  ReaderViewInput frame_input;
  ReaderViewBuildInput build_input;
  ReaderViewFrame frame;
  const ReaderViewSemanticNode *open;
  UI0ID open_id = 0;
  UI0Rect open_rect = ui0_rect(0, 0, 0, 0);

  projection.document_key = 0;
  projection.features = ReaderViewFeature_Open;
  projection.document_flags = ReaderViewDocument_CanOpen;
  projection.content.state = ReaderViewLoad_Empty;

  reader_view_state_init(&state);
  memset(&layout_input, 0, sizeof(layout_input));
  layout_input.bounds = ui0_rect(0, 0, 1280, 800);
  layout_input.features = projection.features;
  layout_input.document_flags = projection.document_flags;
  check(reader_view_resolve_layout(&state, &layout_input, &layout),
        "zero-document layout resolves");

  memset(&frame_input, 0, sizeof(frame_input));
  memset(&build_input, 0, sizeof(build_input));
  build_input.frame_index = 1;
  build_input.state = &state;
  build_input.layout = &layout;
  build_input.projection = &projection;
  build_input.input = &frame_input;
  build_input.theme = theme;
  check(build_input.find_text_metrics.advances == 0 &&
        build_input.find_text_metrics.advance_count == 0 &&
        build_input.find_text_metrics.fallback_advance == 0,
        "Open-only frames require no Find metric record");
  check(reader_view_build(&build_input, &storage, &frame),
        "zero-document seed build");
  open = find_semantic(&frame, "Open");
  check(open != 0, "zero-document Open semantic present");
  if (open)
  {
    open_id = open->id;
    open_rect = open->rect;
    frame_input.ui = ui0_input_pointer(open_rect.x + open_rect.w / 2,
                                       open_rect.y + open_rect.h / 2,
                                       1, 1, 0);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "zero-document Open press build");
    check(state.active_id == open_id,
          "zero-document Open press remains active");

    frame_input.ui = ui0_input_pointer(open_rect.x + open_rect.w / 2,
                                       open_rect.y + open_rect.h / 2,
                                       0, 0, 1);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "zero-document Open release build");
    check(find_action(&frame, ReaderViewAction_Open) != 0,
          "zero-document Open release emits action");

    check(reader_view_accessibility_focus(&state, open_id),
          "zero-document Open focus queued");
    memset(&frame_input, 0, sizeof(frame_input));
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "zero-document Open focus build");
    open = find_semantic(&frame, "Open");
    check(open && (open->flags & ReaderViewSemantic_Focused) != 0,
          "zero-document Open focus is published");
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "zero-document Open retained-focus build");
    open = find_semantic(&frame, "Open");
    check(open && (open->flags & ReaderViewSemantic_Focused) != 0 &&
          state.focus_id == open_id,
          "zero-document frame preserves focus state");

    state.hot_id = open_id;
    state.active_id = open_id;
    state.pending_accessibility_focus_id = open_id;
    state.pending_accessibility_invoke_id = open_id;
    projection.features = 0;
    layout_input.features = projection.features;
    check(reader_view_resolve_layout(&state, &layout_input, &layout),
          "zero-document Open-withdrawal layout resolves");
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame) &&
          state.focus_id == 0 && !state.focus_visible &&
          state.hot_id == 0 && state.active_id == 0 &&
          state.pending_accessibility_focus_id == 0 &&
          state.pending_accessibility_invoke_id == 0 &&
          find_semantic_control_source(
            &frame, ReaderViewSemanticControl_Open, 0) == 0 &&
          frame.action_count == 0,
          "withdrawing Open clears every retained interaction identity");
    projection.features = ReaderViewFeature_Open;
    layout_input.features = projection.features;
    check(reader_view_resolve_layout(&state, &layout_input, &layout),
          "zero-document restored-Open layout resolves");
  }

  state.left_panel = ReaderViewLeftPanel_Find;
  state.most_recent_panel = ReaderViewPanel_Left;
  memset(&frame_input, 0, sizeof(frame_input));
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "zero-document retained-state build");
  check(state.left_panel == ReaderViewLeftPanel_Find &&
        state.most_recent_panel == ReaderViewPanel_Left,
        "zero-document frame preserves transient panel state");

  projection.document_key = 77;
  projection.document_flags = ReaderViewDocument_Open |
                              ReaderViewDocument_CanOpen;
  projection.content = ready_status();
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "zero-to-document transition build");
  check(state.document_key == 77 &&
        state.left_panel == ReaderViewLeftPanel_None &&
        state.most_recent_panel == ReaderViewPanel_None &&
        state.active_id == 0 && state.focus_id == 0,
        "zero-to-document transition resets transient state");

  projection.document_key = 0;
  projection.document_flags = ReaderViewDocument_CanOpen;
  projection.content.state = ReaderViewLoad_Empty;
  build_input.frame_index += 1;
  check(!reader_view_build(&build_input, &storage, &frame) &&
        (frame.error_flags & ReaderViewFrameError_StaleDocumentState) != 0,
        "document-to-zero transition requires explicit reset");
  check(state.document_key == 77,
        "stale document transition does not mutate state");

  reader_view_state_reset_document(&state, 0);
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "explicit document-to-zero reset build");
  check(state.document_key == 0 &&
        state.left_panel == ReaderViewLeftPanel_None &&
        state.active_id == 0 && state.focus_id == 0,
        "explicit document-to-zero reset clears transient state");
}

static void
test_progress_u64_scaling(const UI0ResolvedTheme *theme)
{
  typedef struct ProgressScaleCase
  {
    UI0U64 location_count;
    UI0U64 location_index;
    UI0S32 slider_max;
    UI0S32 slider_value;
  } ProgressScaleCase;
  static const ProgressScaleCase cases[] = {
    { 100ull, 24ull, 100, 25 },
    { 2147483646ull, 2147483645ull, 2147483646, 2147483646 },
    { 2147483647ull, 2147483646ull, INT32_MAX, INT32_MAX },
    { 2147483648ull, 0ull, INT32_MAX, 1 },
    { 2147483648ull, 1ull, INT32_MAX, 2 },
    { 2147483648ull, 2147483647ull, INT32_MAX, INT32_MAX },
    { 2147483649ull, 2147483648ull, INT32_MAX, INT32_MAX },
    { UINT64_MAX, 8589934600ull, INT32_MAX, 2 },
    { UINT64_MAX, 9223372036854775807ull, INT32_MAX, 1073741824 },
    { UINT64_MAX, UINT64_MAX - 1ull, INT32_MAX, INT32_MAX },
  };
  static ReaderViewState state;
  static ReaderViewFrameStorage storage;
  ReaderViewLayoutInput layout_input;
  ReaderViewLayout layout;
  ReaderViewProjection projection = minimal_projection();
  ReaderViewInput frame_input;
  ReaderViewBuildInput build_input;
  ReaderViewFrame frame;
  const ReaderViewSemanticNode *node;
  const ReaderViewAction *action;
  UI0S32 index;

  projection.document_key = 701;
  projection.features = ReaderViewFeature_Progress;
  projection.document_flags = ReaderViewDocument_Open;
  projection.progress.can_seek = 1;
  projection.progress.page_count = 1;
  projection.progress.page_index = 0;
  projection.progress.chapter.data = "Chapter";
  projection.progress.chapter.size = 7;
  projection.progress.label.data = "Progress range";
  projection.progress.label.size = 14;

  reader_view_state_init(&state);
  memset(&layout_input, 0, sizeof(layout_input));
  layout_input.bounds = ui0_rect(0, 0, 1400, 780);
  layout_input.features = projection.features;
  layout_input.document_flags = projection.document_flags;
  check(reader_view_resolve_layout(&state, &layout_input, &layout),
        "U64 progress layout resolves");

  memset(&frame_input, 0, sizeof(frame_input));
  memset(&build_input, 0, sizeof(build_input));
  build_input.frame_index = 1;
  build_input.state = &state;
  build_input.layout = &layout;
  build_input.projection = &projection;
  build_input.input = &frame_input;
  build_input.theme = theme;

  for (index = 0; index < (UI0S32)(sizeof(cases) / sizeof(cases[0])); ++index)
  {
    const UI0SliderRecord *slider = storage.slider_records;
    projection.progress.location_count = cases[index].location_count;
    projection.progress.location_index = cases[index].location_index;
    memset(&frame_input, 0, sizeof(frame_input));
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "U64 progress scale case builds");
    node = find_semantic_role(&frame, "Progress range",
                              ReaderViewSemantic_Slider);
    check(node != 0 && slider->id == node->id &&
          slider->min_value == 1 &&
          slider->max_value == cases[index].slider_max &&
          slider->value == cases[index].slider_value &&
          slider->next_value == cases[index].slider_value &&
          slider->step == 1,
          "U64 progress scale case has exact bounded slider record");
    check(node != 0 &&
          node->range_value == cases[index].location_index &&
          node->range_min == 0 &&
          node->range_max == cases[index].location_count - 1ull,
          "U64 progress scale case preserves full semantic range");
  }

  projection.progress.location_count = UINT64_MAX;
  projection.progress.location_index = 0;
  memset(&frame_input, 0, sizeof(frame_input));
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "U64 progress action seed builds");
  node = find_semantic_role(&frame, "Progress range",
                            ReaderViewSemantic_Slider);
  check(node != 0 && reader_view_accessibility_focus(&state, node->id),
        "U64 progress focus queues");
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "U64 progress focus builds");

  frame_input.move_horizontal_delta = 1;
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "U64 progress first scaled step builds");
  action = find_action(&frame, ReaderViewAction_SeekLocation);
  check(action != 0 && action->value == 8589934600ull,
        "U64 progress first scaled step is exact");

  frame_input.move_horizontal_delta = INT32_MAX - 1;
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "U64 progress upper endpoint step builds");
  action = find_action(&frame, ReaderViewAction_SeekLocation);
  check(action != 0 && action->value == UINT64_MAX - 1ull,
        "U64 progress upper endpoint is exactly reachable");

  projection.progress.location_index = UINT64_MAX - 1ull;
  frame_input.move_horizontal_delta = -(INT32_MAX - 1);
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "U64 progress lower endpoint step builds");
  action = find_action(&frame, ReaderViewAction_SeekLocation);
  check(action != 0 && action->value == 0,
        "U64 progress lower endpoint is exactly reachable");

  projection.progress.location_index = 9223372036854775807ull;
  frame_input.move_horizontal_delta = 1;
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "U64 progress midpoint next step builds");
  action = find_action(&frame, ReaderViewAction_SeekLocation);
  check(action != 0 && action->value == 9223372045444710407ull,
        "U64 progress midpoint next step uses deterministic rounding");

  frame_input.move_horizontal_delta = -1;
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "U64 progress midpoint previous step builds");
  action = find_action(&frame, ReaderViewAction_SeekLocation);
  check(action != 0 && action->value == 9223372028264841207ull,
        "U64 progress midpoint previous step uses deterministic rounding");

  projection.progress.location_count = 2147483648ull;
  projection.progress.location_index = 0;
  frame_input.move_horizontal_delta = 1;
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "INT32 boundary progress step builds");
  action = find_action(&frame, ReaderViewAction_SeekLocation);
  check(action != 0 && action->value == 1,
        "INT32 boundary progress keeps one-location first step");
}

static void
test_reference_panels_and_disabled_gutter(const UI0ResolvedTheme *theme)
{
  static ReaderViewState state;
  static ReaderViewFrameStorage storage;
  ReaderViewLayoutInput layout_input;
  ReaderViewLayout layout;
  ReaderViewSettingControl settings[READER_VIEW_SETTING_CAP];
  ReaderViewChoice setting_choices[8];
  ReaderViewTocRow toc_rows[2];
  ReaderViewFindRow find_rows[1];
  ReaderViewRightRow right_rows[2];
  ReaderViewCodepointAdvance find_advances[127];
  ReaderViewProjection projection = full_projection(
    settings, setting_choices, toc_rows, find_rows, right_rows);
  ReaderViewInput frame_input;
  ReaderViewBuildInput build_input;
  ReaderViewFrame frame;
  const ReaderViewSemanticNode *node;
  const ReaderViewSemanticNode *row;
  const ReaderViewSemanticNode *filter;
  const ReaderViewSemanticNode *option;
  const ReaderViewSemanticNode *clear_control;
  const ReaderViewTextBinding *binding;
  const UI0DrawCommand *command;
  const UI0DrawCommand *fill;
  const UI0DrawCommand *border;
  const UI0DrawCommand *icon;
  const UI0DrawCommand *indicator;
  const UI0DrawCommand *caret;
  const UI0ControlRecord *control;
  const UI0SignalRecord *row_signal;
  const UI0SignalRecord *star_signal;
  const UI0SignalRecord *menu_signal;
  const UI0TextInputRecord *input_record;
  const ReaderViewAction *action;
  UI0Rect rect;
  UI0ID previous_id;
  UI0ID toc_id;
  UI0ID find_input_id;
  UI0ID find_id;
  UI0ID right_id;
  UI0ID right_close_id;
  UI0ID right_menu_id;
  UI0S32 index;

  projection.find.status.message.data = "1 match";
  projection.find.status.message.size = 7;
  projection.right.available_filters = ReaderViewRightFilterFlag_All |
                                       ReaderViewRightFilterFlag_Bookmarks |
                                       ReaderViewRightFilterFlag_Highlights |
                                       ReaderViewRightFilterFlag_Notes;
  projection.right.all_count = 1;
  projection.right.bookmark_count = 1;
  projection.right.highlight_count = 0;
  projection.right.note_count = 0;
  projection.labels.highlights.data = "All Highlight Colors";
  projection.labels.highlights.size = 20;
  right_rows[0].secondary.data = "Bookmark - re10 loc 1";
  right_rows[0].secondary.size =
    (UI0S32)strlen(right_rows[0].secondary.data);
  toc_rows[1].depth = 2;

  reader_view_state_reset_document(&state, projection.document_key);
  memset(&layout_input, 0, sizeof(layout_input));
  layout_input.bounds = ui0_rect(0, 0, 1400, 780);
  layout_input.features = projection.features;
  layout_input.document_flags = projection.document_flags;
  layout_input.host_toolbar_trailing_width = 38;
  check(reader_view_resolve_layout(&state, &layout_input, &layout),
        "panel recovery base layout resolves");
  memset(&frame_input, 0, sizeof(frame_input));
  memset(&build_input, 0, sizeof(build_input));
  build_input.frame_index = 1000;
  build_input.state = &state;
  build_input.layout = &layout;
  build_input.projection = &projection;
  build_input.input = &frame_input;
  build_input.theme = theme;
  build_input.find_text_metrics = test_find_text_metrics(find_advances);
  {
    ReaderViewFindTextMetrics valid_metrics = build_input.find_text_metrics;
    memset(&build_input.find_text_metrics, 0,
           sizeof(build_input.find_text_metrics));
    check(!reader_view_build(&build_input, &storage, &frame) &&
          (frame.error_flags & ReaderViewFrameError_BadInput) != 0,
          "Find feature fails closed without caller text metrics");
    build_input.find_text_metrics = valid_metrics;

    find_advances[1].codepoint = find_advances[0].codepoint;
    check(!reader_view_build(&build_input, &storage, &frame) &&
          (frame.error_flags & ReaderViewFrameError_BadInput) != 0,
          "Find metrics reject duplicate codepoints");
    find_advances[1].codepoint = 2;

    find_advances[0].advance = -1;
    check(!reader_view_build(&build_input, &storage, &frame) &&
          (frame.error_flags & ReaderViewFrameError_BadInput) != 0,
          "Find metrics reject negative advances");
    find_advances[0].advance = test_find_codepoint_advance(1);

    find_advances[0].advance = 0x100001;
    check(!reader_view_build(&build_input, &storage, &frame) &&
          (frame.error_flags & ReaderViewFrameError_BadInput) != 0,
          "Find metrics reject oversized advances");
    find_advances[0].advance = test_find_codepoint_advance(1);
  }
  check(reader_view_build(&build_input, &storage, &frame),
        "panel recovery base frame builds");

  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_Contents, 0);
  check(node != 0, "panel recovery Contents toolbar control exists");
  if (node)
  {
    rect = node->rect;
    frame_input.ui = ui0_input_pointer(rect.x + rect.w / 2,
                                       rect.y + rect.h / 2,
                                       1, 1, 0);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "panel recovery Contents press builds");
    frame_input.ui = ui0_input_pointer(rect.x + rect.w / 2,
                                       rect.y + rect.h / 2,
                                       0, 0, 1);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "panel recovery Contents release builds");
  }
  check(state.left_panel == ReaderViewLeftPanel_Contents,
        "Contents toolbar opens accepted left panel");
  check(reader_view_resolve_layout(&state, &layout_input, &layout),
        "Contents accepted layout resolves");
  memset(&frame_input, 0, sizeof(frame_input));
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Contents accepted frame builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_Contents, 0);
  control = node ? find_control_for_source(&storage, node->id) : 0;
  fill = node ? find_draw_for_source(&frame, UI0DrawOp_ControlFill,
                                     node->id) : 0;
  check(node != 0 && control != 0 && fill != 0 &&
        (node->flags & (ReaderViewSemantic_Selected |
                        ReaderViewSemantic_Expanded)) ==
          (ReaderViewSemantic_Selected | ReaderViewSemantic_Expanded) &&
        (node->flags & ReaderViewSemantic_Checked) == 0 &&
        (control->state & UI0ControlState_Selected) != 0 &&
        (control->state & (UI0ControlState_Checked | UI0ControlState_On |
                           UI0ControlState_Open)) == 0 &&
        (fill->flags & UI0DrawFlag_Selected) != 0 &&
        (fill->flags & (UI0DrawFlag_Checked | UI0DrawFlag_On |
                        UI0DrawFlag_Open)) == 0,
        "open Contents keeps semantic selected/expanded state while its "
        "frozen UI0 visual state is selected-only");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_LeftPanelClose, 0);
  check(node != 0 && rect_equal(node->rect, ui0_rect(398, 66, 24, 24)) &&
        text_equal(node->name, "Close navigation"),
        "Contents close uses the frozen target and native name");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_LeftContentsTab, 0);
  binding = node ? find_text_binding(&frame, node->id) : 0;
  check(node != 0 && rect_equal(node->rect, ui0_rect(20, 68, 56, 34)) &&
        text_equal(node->name, "Contents") && binding != 0 &&
        text_equal(binding->text, "TOC"),
        "Contents rail keeps the frozen native name and short visual copy");
  fill = node ? find_draw_for_source(&frame, UI0DrawOp_ControlFill,
                                     node->id) : 0;
  indicator = node ? find_draw_for_source(&frame, UI0DrawOp_IndicatorFill,
                                          node->id) : 0;
  command = node ? find_draw_for_source(&frame, UI0DrawOp_Text,
                                        node->id) : 0;
  check(fill != 0 && rect_equal(fill->rect, ui0_rect(27, 68, 49, 34)) &&
        indicator != 0 &&
        rect_equal(indicator->rect, ui0_rect(20, 75, 3, 20)) &&
        indicator->color == theme->colors[UI0ColorRole_Focus] &&
        command != 0 && command->rect.x == 36,
        "Contents rail paints the frozen Sidenav body, rail, and text inset");
  node = find_semantic_role(&frame, "Table of Contents",
                            ReaderViewSemantic_Panel);
  check(node != 0 &&
        count_draw_op_for_source(&frame, UI0DrawOp_ControlFill,
                                 node ? node->id : 0) == 0 &&
        count_draw_op_for_source(&frame, UI0DrawOp_ControlBorder,
                                 node ? node->id : 0) == 0,
        "Contents omits the post-extraction rounded left-panel shell");
  node = find_semantic_role(&frame, "Table of Contents",
                            ReaderViewSemantic_Group);
  command = node ? find_draw_for_source(&frame, UI0DrawOp_Text,
                                        node->id) : 0;
  check(command != 0 &&
        rect_equal(command->rect, ui0_rect(104, 68, 284, 24)) &&
        command->has_typography_role &&
        command->typography_role == UI0TypographyRole_SectionTitle,
        "Contents title uses the frozen placement and section-title token");
  command = find_draw_for_rect(&frame, UI0DrawOp_ControlFill,
                               ui0_rect(83, 68, 1, 662));
  check(command != 0 &&
        command->color == theme->colors[UI0ColorRole_BorderMuted] &&
        command->stroke_color == command->color,
        "Contents rail divider is the frozen inset one-pixel rule");
  command = find_draw_for_rect(&frame, UI0DrawOp_ControlFill,
                               ui0_rect(431, 56, 1, 686));
  check(command != 0 &&
        command->color == theme->colors[UI0ColorRole_BorderMuted] &&
        command->stroke_color == command->color,
        "Contents keeps the frozen outer one-pixel divider");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_LeftPanelClose, 0);
  fill = node ? find_draw_for_source(&frame, UI0DrawOp_ControlFill,
                                     node->id) : 0;
  border = node ? find_draw_for_source(&frame, UI0DrawOp_ControlBorder,
                                       node->id) : 0;
  icon = node ? find_icon_for_source(&frame, node->id) : 0;
  check(fill != 0 && border != 0 && icon != 0 &&
        icon->stroke_color == fill->color,
        "Contents close restores the outlined shell and exact icon background");
  row = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_TocRow, 20);
  check(row != 0 && rect_equal(row->rect, ui0_rect(104, 108, 308, 32)) &&
        (row->flags & (ReaderViewSemantic_Focused |
                       ReaderViewSemantic_Current)) ==
          (ReaderViewSemantic_Focused | ReaderViewSemantic_Current) &&
        state.focus_id == row->id && !state.focus_visible,
        "Contents current row auto-focuses with frozen native geometry");
  fill = row ? find_draw_for_source(&frame, UI0DrawOp_ControlFill,
                                    row->id) : 0;
  indicator = row ? find_draw_for_source(&frame, UI0DrawOp_IndicatorFill,
                                         row->id) : 0;
  command = row ? find_draw_for_source(&frame, UI0DrawOp_Text,
                                       row->id) : 0;
  icon = row ? find_icon_for_source(&frame, row->id) : 0;
  check(fill != 0 &&
        rect_equal(fill->rect, ui0_rect(114, 104, 298, 32)) &&
        indicator != 0 &&
        rect_equal(indicator->rect, ui0_rect(104, 111, 3, 18)) &&
        indicator->color == theme->colors[UI0ColorRole_Focus] &&
        command != 0 && command->rect.x == 142 &&
        icon != 0 && icon->icon_kind == UI0IconKind_ChevronDown &&
        rect_equal(icon->rect, ui0_rect(124, 114, 12, 12)),
        "Contents current entry paints the frozen compact row composition");
  row = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_TocRow, 21);
  check(row != 0 && rect_equal(row->rect, ui0_rect(104, 140, 308, 32)),
        "Contents depth preserves the full-width native semantic row");
  command = row ? find_draw_for_source(&frame, UI0DrawOp_Text,
                                       row->id) : 0;
  check(command != 0 && command->rect.x == 182 && command->rect.y == 138 &&
        command->rect.h == 32,
        "Contents depth indents only frozen expander/text visual geometry");
  frame_input.ui = ui0_input_pointer(200, 154, 1, 1, 0);
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Contents frozen second-visual-row coordinate press builds");
  frame_input.ui = ui0_input_pointer(200, 154, 0, 0, 1);
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Contents frozen second-visual-row coordinate release builds");
  action = find_action(&frame, ReaderViewAction_ActivateTocRow);
  check(action != 0 && action->key == 21 && frame.action_count == 1 &&
        state.active_toc_key == 21,
        "Contents full-app (200,154) activates visual row 1, never row 0");
  row = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_TocRow, 21);
  if (row)
  {
    rect = row->rect;
    frame_input.ui = ui0_input_pointer(rect.x + rect.w / 2,
                                       rect.y + rect.h / 2,
                                       1, 1, 0);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "Contents second native semantic-center press builds");
    frame_input.ui = ui0_input_pointer(rect.x + rect.w / 2,
                                       rect.y + rect.h / 2,
                                       0, 0, 1);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "Contents second native semantic-center release builds");
  }
  action = find_action(&frame, ReaderViewAction_ActivateTocRow);
  check(action != 0 && action->key == 21 && frame.action_count == 1,
        "Contents second native semantic center hits its matching visual row");
  row = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_TocRow, 20);
  toc_id = row ? row->id : 0;
  if (row)
  {
    rect = row->rect;
    frame_input.ui = ui0_input_pointer(rect.x + rect.w / 2,
                                       rect.y + rect.h / 2,
                                       1, 1, 0);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "Contents current row press builds");
    frame_input.ui = ui0_input_pointer(rect.x + rect.w / 2,
                                       rect.y + rect.h / 2,
                                       0, 0, 1);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "Contents current row release builds");
  }
  action = find_action(&frame, ReaderViewAction_ActivateTocRow);
  check(action != 0 && action->key == 20 && frame.action_count == 1 &&
        state.left_panel == ReaderViewLeftPanel_Contents &&
        state.active_toc_key == 20 && state.focus_id == toc_id &&
        !state.focus_visible,
        "Contents native semantic center routes to its matching physical row "
        "and keeps panel/focus state");

  {
    ReaderViewTocRow long_toc_rows[32];
    const UI0ControlRecord *row_control;
    const UI0DrawCommand *row_text;
    memset(long_toc_rows, 0, sizeof(long_toc_rows));
    for (index = 0; index < 32; ++index)
    {
      long_toc_rows[index].key = (ReaderViewKey)(1000 + index);
      long_toc_rows[index].label.data = "Long chapter";
      long_toc_rows[index].label.size = 12;
      long_toc_rows[index].flags = ReaderViewRow_Enabled;
    }
    state.active_toc_key = 0;
    state.focus_id = 0;
    state.focus_visible = 0;
    projection.toc.rows = long_toc_rows;
    projection.toc.row_count = 32;
    projection.toc.total_count = 32;
    memset(&frame_input, 0, sizeof(frame_input));
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "long Contents scroll-layer frame builds");
    row = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_TocRow, 1000);
    row_control = row ? find_control_for_source(&storage, row->id) : 0;
    row_text = row ? find_draw_for_source(
      &frame, UI0DrawOp_Text, row->id) : 0;
    check(row_control != 0 && row_text != 0 &&
          row_control->clip_rect.x == row_control->rect.x &&
          row_control->clip_rect.w == row_control->rect.w &&
          row_text->clip_rect.x == row_control->clip_rect.x &&
          row_text->clip_rect.w == row_control->clip_rect.w &&
          scroll_records_empty(&storage) &&
          count_draw_op(&frame, UI0DrawOp_ScrollTrack) == 0 &&
          count_draw_op(&frame, UI0DrawOp_ScrollThumb) == 0,
          "long Contents keeps full-width rows and the frozen hidden-scroll "
          "surface without a track or thumb");
  }

  projection.toc.rows = 0;
  projection.toc.row_count = 0;
  projection.toc.total_count = 0;
  memset(&frame_input, 0, sizeof(frame_input));
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Contents ready-empty frame builds");
  node = find_semantic_role(&frame, "No contents",
                            ReaderViewSemantic_Status);
  command = node ? find_draw_for_source(&frame, UI0DrawOp_Text,
                                        node->id) : 0;
  check(node != 0 && command != 0 &&
        rect_equal(node->rect, ui0_rect(104, 112, 308, 22)) &&
        rect_equal(command->rect, ui0_rect(104, 112, 308, 22)) &&
        rect_equal(command->clip_rect, ui0_rect(104, 112, 308, 22)) &&
        command->color == theme->colors[UI0ColorRole_TextMuted] &&
        command->has_text_alignment &&
        command->text_align_x == UI0TextAlignX_Start &&
        command->text_align_y == UI0TextAlignY_Center &&
        command->has_typography_role &&
        command->typography_role == UI0TypographyRole_Body &&
        command->typography_char_width == 8 &&
        command->typography_line_height == 16 &&
        count_semantic(&frame, "Nothing here") == 0,
        "Contents ready-empty copy uses the exact frozen rect and muted "
        "body style without the generic fallback");
  projection.labels.no_contents.data = "Localized no contents";
  projection.labels.no_contents.size = 21;
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame) &&
        find_semantic_role(&frame, "Localized no contents",
                           ReaderViewSemantic_Status) != 0,
        "Contents ready-empty copy remains caller-localizable");
  projection.labels.no_contents.data = 0;
  projection.labels.no_contents.size = 0;
  projection.toc.rows = toc_rows;
  projection.toc.row_count = 2;
  projection.toc.total_count = 2;

  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_LeftFindTab, 0);
  check(node != 0, "Find rail tab exists");
  if (node)
  {
    rect = node->rect;
    frame_input.ui = ui0_input_pointer(rect.x + rect.w / 2,
                                       rect.y + rect.h / 2,
                                       1, 1, 0);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "Find rail press builds");
    frame_input.ui = ui0_input_pointer(rect.x + rect.w / 2,
                                       rect.y + rect.h / 2,
                                       0, 0, 1);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "Find rail release builds");
  }
  memset(&frame_input, 0, sizeof(frame_input));
  build_input.frame_index = 1050;
  check(reader_view_build(&build_input, &storage, &frame),
        "Find accepted frame builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_Find, 0);
  control = node ? find_control_for_source(&storage, node->id) : 0;
  fill = node ? find_draw_for_source(&frame, UI0DrawOp_ControlFill,
                                     node->id) : 0;
  check(node != 0 && control != 0 && fill != 0 &&
        (node->flags & (ReaderViewSemantic_Selected |
                        ReaderViewSemantic_Expanded)) ==
          (ReaderViewSemantic_Selected | ReaderViewSemantic_Expanded) &&
        (node->flags & ReaderViewSemantic_Checked) == 0 &&
        (control->state & UI0ControlState_Selected) != 0 &&
        (control->state & (UI0ControlState_Checked | UI0ControlState_On |
                           UI0ControlState_Open)) == 0 &&
        (fill->flags & UI0DrawFlag_Selected) != 0 &&
        (fill->flags & (UI0DrawFlag_Checked | UI0DrawFlag_On |
                        UI0DrawFlag_Open)) == 0,
        "open Find keeps semantic selected/expanded state while its frozen "
        "UI0 visual state is selected-only");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_FindInput, 0);
  find_input_id = node ? node->id : 0;
  input_record = storage.text_input_records;
  caret = node ? find_draw_for_source(&frame, UI0DrawOp_TextCaret,
                                       node->id) : 0;
  check(node != 0 && rect_equal(node->rect, ui0_rect(104, 104, 308, 34)) &&
        text_equal(node->name, "Search input") && node->value.size == 0 &&
        state.focus_id == node->id && !state.focus_visible,
        "Find opens with accepted input geometry, stable AT name/current "
        "value, and invisible input focus");
  check(input_record->id == find_input_id &&
        input_record->box_index == 0 &&
        rect_equal(input_record->rect, ui0_rect(104, 104, 274, 34)) &&
        rect_equal(input_record->clip_rect,
                   ui0_rect(104, 104, 274, 34)) &&
        rect_equal(input_record->text_rect,
                   ui0_rect(112, 113, 258, 16)) &&
        (input_record->state & UI0TextInputState_Focused) != 0 &&
        (input_record->state &
         UI0TextInputState_PlaceholderVisible) != 0 &&
        count_draw_op_for_source(&frame, UI0DrawOp_ControlFill,
                                 find_input_id) == 1 &&
        count_draw_op_for_source(&frame, UI0DrawOp_ControlBorder,
                                 find_input_id) == 1 &&
        count_draw_op_for_source(&frame, UI0DrawOp_TextCaret,
                                 find_input_id) == 1 &&
        caret != 0 &&
        rect_equal(caret->rect, ui0_rect(112, 111, 1, 20)) &&
        rect_equal(caret->clip_rect, ui0_rect(104, 104, 274, 34)),
        "Find uses one real focused UI0 text-input record at the exact "
        "274px frozen field and gives its 20px caret the full-field clip");
  binding = node ? find_text_binding(&frame, node->id) : 0;
  command = node ? find_draw_for_source(&frame, UI0DrawOp_Text,
                                        node->id) : 0;
  check(binding != 0 && text_equal(binding->text, "Search in book") &&
        command != 0 &&
        command->label_hash == input_record->placeholder_hash &&
        rect_equal(command->rect, ui0_rect(112, 104, 112, 34)) &&
        rect_equal(input_record->caret_rect,
                   ui0_rect(112, 111, 1, 20)),
        "Find focused empty input paints the frozen Search in book text and "
        "caret together");
  build_input.frame_index = 1051;
  check(reader_view_build(&build_input, &storage, &frame),
        "Find first hidden caret frame builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_FindInput, 0);
  input_record = storage.text_input_records;
  check(node != 0 &&
        (input_record->state & UI0TextInputState_Focused) != 0 &&
        (input_record->state &
         UI0TextInputState_PlaceholderVisible) != 0 &&
        count_draw_op_for_source(&frame, UI0DrawOp_Text,
                                 node->id) == 1 &&
        count_draw_op_for_source(&frame, UI0DrawOp_TextCaret,
                                 node->id) == 0,
        "Find caret switches off exactly between visible frame 1050 and "
        "hidden frame 1051 without hiding the placeholder");

  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_LeftPanelClose, 0);
  check(node != 0 && reader_view_accessibility_focus(&state, node->id),
        "Find can move focus away for placeholder coverage");
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Find unfocused placeholder frame builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_FindInput, 0);
  input_record = storage.text_input_records;
  binding = node ? find_text_binding(&frame, node->id) : 0;
  command = node ? find_draw_for_source(&frame, UI0DrawOp_Text,
                                        node->id) : 0;
  check(node != 0 && input_record->id == node->id &&
        (input_record->state &
         UI0TextInputState_PlaceholderVisible) != 0 &&
        (input_record->state & UI0TextInputState_Focused) == 0 &&
        binding != 0 && text_equal(binding->text, "Search in book") &&
        command != 0 &&
        command->label_hash == input_record->placeholder_hash &&
        rect_equal(command->rect, ui0_rect(112, 104, 112, 34)) &&
        rect_equal(command->clip_rect, ui0_rect(112, 104, 258, 34)) &&
        count_draw_op_for_source(&frame, UI0DrawOp_TextCaret,
                                 node->id) == 0,
        "Find empty unfocused field draws the distinct frozen Search in "
        "book placeholder from the UI0 record");
  projection.labels.find_placeholder.data = "Localized search";
  projection.labels.find_placeholder.size = 16;
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Find localized placeholder frame builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_FindInput, 0);
  binding = node ? find_text_binding(&frame, node->id) : 0;
  check(binding != 0 && text_equal(binding->text, "Localized search"),
        "Find placeholder remains caller-localizable independently of "
        "ready status");
  projection.labels.find_placeholder.data = 0;
  projection.labels.find_placeholder.size = 0;
  check(node != 0 && reader_view_accessibility_focus(&state, node->id),
        "Find input accepts bounded accessibility refocus");
  build_input.frame_index = 1080;
  check(reader_view_build(&build_input, &storage, &frame),
        "Find input refocus frame builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_FindInput, 0);
  input_record = storage.text_input_records;
  check(node != 0 &&
        (input_record->state & UI0TextInputState_Focused) != 0 &&
        (input_record->state &
         UI0TextInputState_PlaceholderVisible) != 0 &&
        count_draw_op_for_source(&frame, UI0DrawOp_Text,
                                 node->id) == 1 &&
        count_draw_op_for_source(&frame, UI0DrawOp_TextCaret,
                                 node->id) == 0,
        "Find frame 1080 keeps focused placeholder visible while the exact "
        "30-frame caret blink is hidden");
  memcpy(state.find_query, "alpha", 5);
  state.find_query[5] = 0;
  state.find_query_length = 5;
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Find current-query semantic frame builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_FindInput, 0);
  binding = node ? find_text_binding(&frame, node->id) : 0;
  check(node != 0 && text_equal(node->name, "Search input") &&
        text_equal(node->value, "alpha") && binding != 0 &&
        text_equal(binding->text, "alpha"),
        "Find SearchBox keeps a stable name while current query is both "
        "semantic value and visual binding");
  fill = node ? find_draw_for_source(&frame, UI0DrawOp_ControlFill,
                                     node->id) : 0;
  border = node ? find_draw_for_source(&frame, UI0DrawOp_ControlBorder,
                                       node->id) : 0;
  check(fill != 0 && border != 0 &&
        rect_equal(fill->rect, ui0_rect(104, 104, 274, 34)) &&
        rect_equal(border->rect, ui0_rect(104, 104, 274, 34)),
        "Find preserves the 308px semantic input while painting the frozen "
        "274px field");
  input_record = storage.text_input_records;
  command = node ? find_draw_for_source(&frame, UI0DrawOp_Text,
                                        node->id) : 0;
  check(input_record->id == find_input_id &&
        input_record->text_len == 5 && input_record->caret == 0 &&
        input_record->selection_start == 0 &&
        input_record->selection_end == 0 &&
        command != 0 &&
        count_draw_op_for_source(&frame, UI0DrawOp_Text,
                                 find_input_id) == 1 &&
        rect_equal(command->rect, ui0_rect(112, 104, 40, 34)) &&
        rect_equal(command->clip_rect, ui0_rect(112, 104, 258, 34)),
        "Find current query has exactly one text draw from the bounded UI0 "
        "input record");
  memset(&frame_input, 0, sizeof(frame_input));
  frame_input.find_text.select_all = 1;
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Find focused selection frame builds");
  input_record = storage.text_input_records;
  check((input_record->state & UI0TextInputState_Focused) != 0 &&
        (input_record->state & UI0TextInputState_HasSelection) != 0 &&
        input_record->caret == 5 &&
        input_record->selection_start == 0 &&
        input_record->selection_end == 5 &&
        rect_equal(input_record->selection_rect,
                   ui0_rect(112, 113, 40, 16)) &&
        rect_equal(input_record->caret_rect,
                   ui0_rect(152, 111, 1, 20)) &&
        count_draw_op_for_source(&frame, UI0DrawOp_TextSelection,
                                 find_input_id) == 1 &&
        count_draw_op_for_source(&frame, UI0DrawOp_TextCaret,
                                 find_input_id) == 1 &&
        find_action(&frame, ReaderViewAction_FindChanged) == 0,
        "Find focused UI0 record publishes exact selection and blinking "
        "caret geometry without a false edit action");
  frame_input.find_text.select_all = 0;
  frame_input.ui = ui0_input_pointer(129, 121, 1, 1, 0);
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Find pointer caret press frame builds");
  input_record = storage.text_input_records;
  check(state.find_input.caret == 2 &&
        state.find_input.selection_anchor == 2 &&
        input_record->caret == 2 &&
        input_record->selection_start == 2 &&
        input_record->selection_end == 2 &&
        (input_record->state & UI0TextInputState_Pressed) != 0 &&
        (input_record->state & UI0TextInputState_HasSelection) == 0 &&
        rect_equal(input_record->caret_rect,
                   ui0_rect(128, 111, 1, 20)),
        "Find pointer hit uses UI0 measurement to place and collapse the "
        "caret inside the 274px field");
  frame_input.ui = ui0_input_pointer(129, 121, 0, 0, 1);
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Find pointer caret release frame builds");
  memset(&frame_input, 0, sizeof(frame_input));
  {
    const char *metric_query = "very high tower gyp";
    UI0S32 metric_len = (UI0S32)strlen(metric_query);
    UI0S32 metric_width = test_find_text_width(metric_query);
    memcpy(state.find_query, metric_query, (size_t)metric_len + 1);
    state.find_query_length = metric_len;
    state.find_input.caret = metric_len;
    state.find_input.selection_anchor = metric_len;
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "Find variable-width system-metric frame builds");
    node = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_FindInput, 0);
    input_record = storage.text_input_records;
    command = node ? find_draw_for_source(&frame, UI0DrawOp_Text,
                                           node->id) : 0;
    check(metric_width != metric_len * 8 &&
          input_record->full_text_width == metric_width &&
          input_record->text_draw_rect.w == metric_width &&
          input_record->caret == metric_len &&
          rect_equal(input_record->caret_rect,
                     ui0_rect(input_record->text_rect.x + metric_width,
                              input_record->rect.y + 7, 1, 20)) &&
          command != 0 && command->rect.w == metric_width,
          "Find caret and draw geometry use caller-supplied variable-width "
          "system-UI advances instead of len times eight");
  }
  {
    const char missing_metric_query[] = "\xc3\xa9";
    memcpy(state.find_query, missing_metric_query,
           sizeof(missing_metric_query));
    state.find_query_length = 2;
    state.find_input.caret = 2;
    state.find_input.selection_anchor = 2;
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "Find missing-codepoint fallback frame builds");
    input_record = storage.text_input_records;
    check(input_record->full_text_width ==
            build_input.find_text_metrics.fallback_advance &&
          rect_equal(input_record->caret_rect,
                     ui0_rect(input_record->text_rect.x +
                                build_input.find_text_metrics.fallback_advance,
                              input_record->rect.y + 7, 1, 20)),
          "Find missing scalar deterministically uses the caller-measured "
          "fallback advance");
  }
  node = find_semantic_role(&frame, "Search", ReaderViewSemantic_Group);
  command = node ? find_draw_for_source(&frame, UI0DrawOp_Text,
                                        node->id) : 0;
  check(command != 0 &&
        count_draw_op_for_source(&frame, UI0DrawOp_Text,
                                 node ? node->id : 0) == 1 &&
        rect_equal(command->rect, ui0_rect(104, 68, 284, 24)) &&
        command->has_typography_role &&
        command->typography_role == UI0TypographyRole_SectionTitle,
        "Find title uses the frozen left-panel title composition");
  clear_control = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_FindClear, 0);
  fill = clear_control ?
    find_draw_for_source(&frame, UI0DrawOp_ControlFill,
                         clear_control->id) : 0;
  border = clear_control ?
    find_draw_for_source(&frame, UI0DrawOp_ControlBorder,
                         clear_control->id) : 0;
  icon = clear_control ? find_icon_for_source(&frame, clear_control->id) : 0;
  check(clear_control != 0 &&
        rect_equal(clear_control->rect, ui0_rect(382, 109, 24, 24)) &&
        text_equal(clear_control->name, "Clear search") &&
        fill != 0 && border != 0 && icon != 0 &&
        icon->stroke_color == fill->color,
        "Find clear restores the separate frozen outlined 24px button");
  if (clear_control)
  {
    rect = clear_control->rect;
    frame_input.ui = ui0_input_pointer(rect.x + rect.w / 2,
                                       rect.y + rect.h / 2,
                                       1, 1, 0);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "Find physical clear press builds");
    frame_input.ui = ui0_input_pointer(rect.x + rect.w / 2,
                                       rect.y + rect.h / 2,
                                       0, 0, 1);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "Find physical clear release builds");
  }
  action = find_action(&frame, ReaderViewAction_FindChanged);
  input_record = storage.text_input_records;
  binding = find_input_id ? find_text_binding(&frame, find_input_id) : 0;
  check(state.find_query_length == 0 && action != 0 &&
        frame.action_count == 1 && state.focus_id == find_input_id &&
        !state.focus_visible &&
        (input_record->state & UI0TextInputState_Focused) != 0 &&
        (input_record->state &
         UI0TextInputState_PlaceholderVisible) != 0 &&
        binding != 0 && text_equal(binding->text, "Search in book") &&
        count_draw_op_for_source(&frame, UI0DrawOp_Text,
                                 find_input_id) == 1 &&
        count_draw_op_for_source(&frame, UI0DrawOp_TextCaret,
                                 find_input_id) == 1,
        "Find clear physical target wins its non-overlapping hit row and "
        "restores frozen focused Search in book plus caret");
  memcpy(state.find_query, "alpha", 5);
  state.find_query[5] = 0;
  state.find_query_length = 5;
  memset(&frame_input, 0, sizeof(frame_input));
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Find query restores for accessibility clear coverage");
  clear_control = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_FindClear, 0);
  check(clear_control != 0 &&
        reader_view_accessibility_focus(&state, clear_control->id) &&
        reader_view_accessibility_invoke(&state, clear_control->id),
        "Find clear accepts bounded accessibility focus/invoke");
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Find accessibility clear frame builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_FindInput, 0);
  input_record = storage.text_input_records;
  check(state.find_query_length == 0 && node != 0 &&
        state.focus_id == node->id && state.focus_visible &&
        node->value.size == 0 &&
        (input_record->state & UI0TextInputState_Focused) != 0 &&
        (input_record->state &
         UI0TextInputState_PlaceholderVisible) != 0 &&
        count_draw_op_for_source(&frame, UI0DrawOp_Text,
                                 node->id) == 1 &&
        count_draw_op_for_source(&frame, UI0DrawOp_TextCaret,
                                 node->id) == 1,
        "Find accessibility clear restores visible input focus and current "
        "empty placeholder/caret state");
  row = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_FindRow, 30);
  check(row != 0 && rect_equal(row->rect, ui0_rect(104, 172, 308, 88)),
        "Find result uses frozen 88px row geometry");
  fill = find_draw_for_rect(&frame, UI0DrawOp_ControlFill,
                            ui0_rect(104, 259, 308, 1));
  check(fill != 0 &&
        fill->color == theme->colors[UI0ColorRole_BorderMuted] &&
        fill->stroke_color == theme->colors[UI0ColorRole_BorderMuted] &&
        rect_equal(fill->clip_rect, ui0_rect(104, 172, 308, 560)),
        "Find result restores the frozen one-pixel muted divider");
  projection.find.active_index = 0;
  memset(&frame_input, 0, sizeof(frame_input));
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Find selected-result visual frame builds");
  row = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_FindRow, 30);
  fill = row ? find_draw_for_source(&frame, UI0DrawOp_ControlFill,
                                    row->id) : 0;
  check(fill != 0 &&
        rect_equal(fill->rect, ui0_rect(104, 176, 308, 80)),
        "Find selected result paints the frozen vertically inset fill");
  projection.find.active_index = -1;
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Find selected-result visual state restores");
  row = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_FindRow, 30);
  node = find_semantic_role(&frame, "1 match", ReaderViewSemantic_Status);
  command = node ? find_draw_for_source(&frame, UI0DrawOp_Text,
                                        node->id) : 0;
  check(count_semantic(&frame, "1 match") == 1 &&
        node != 0 && command != 0 &&
        rect_equal(node->rect, ui0_rect(104, 146, 308, 18)) &&
        rect_equal(command->rect, ui0_rect(104, 146, 308, 18)) &&
        rect_equal(command->clip_rect, ui0_rect(104, 146, 308, 18)) &&
        command->color == theme->colors[UI0ColorRole_TextMuted] &&
        command->has_text_alignment &&
        command->text_align_x == UI0TextAlignX_Start &&
        command->text_align_y == UI0TextAlignY_Center &&
        command->has_typography_role &&
        command->typography_role == UI0TypographyRole_Body &&
        command->typography_char_width == 8 &&
        command->typography_line_height == 16,
        "Find ready status is emitted once at the frozen muted-body rect");
  binding = 0;
  command = 0;
  for (index = 0; index < frame.semantic_node_count; ++index)
  {
    const ReaderViewSemanticNode *candidate = frame.semantic_nodes + index;
    if (row && candidate->parent_id == row->id &&
        candidate->role == ReaderViewSemantic_Group &&
        text_equal(candidate->name, "One"))
    {
      command = find_draw_for_source(&frame, UI0DrawOp_Text, candidate->id);
      check(command != 0 && command->has_text_alignment &&
            command->text_align_x == UI0TextAlignX_End &&
            command->color == theme->colors[UI0ColorRole_TextMuted],
            "Find section is muted and right-aligned exactly");
    }
    if (row && candidate->parent_id == row->id &&
        candidate->role == ReaderViewSemantic_Group &&
        text_equal(candidate->name, "A result"))
      binding = find_text_binding(&frame, candidate->id);
  }
  check(binding != 0 && binding->match_start == 2 &&
        binding->match_size == 6,
        "Find excerpt publishes the exact host-measured match range");
  find_id = row ? row->id : 0;
  if (row)
  {
    rect = row->rect;
    frame_input.ui = ui0_input_pointer(rect.x + rect.w / 2,
                                       rect.y + rect.h / 2,
                                       1, 1, 0);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "Find result press builds");
    frame_input.ui = ui0_input_pointer(rect.x + rect.w / 2,
                                       rect.y + rect.h / 2,
                                       0, 0, 1);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "Find result release builds");
  }
  action = find_action(&frame, ReaderViewAction_ActivateFindRow);
  check(action != 0 && action->key == 30 && frame.action_count == 1 &&
        state.left_panel == ReaderViewLeftPanel_Find &&
        state.active_find_key == 30 && state.focus_id == find_id &&
        !state.focus_visible &&
        find_action(&frame, ReaderViewAction_FindChanged) == 0 &&
        find_action(&frame, ReaderViewAction_FindCommitted) == 0,
        "Find result invokes once without rebuilding query or closing panel");

  projection.find.rows = 0;
  projection.find.row_count = 0;
  projection.find.total_count = 0;
  projection.find.status.message.data = 0;
  projection.find.status.message.size = 0;
  memcpy(state.find_query, "alpha", 5);
  state.find_query[5] = 0;
  state.find_query_length = 5;
  memset(&frame_input, 0, sizeof(frame_input));
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Find ready zero-result frame builds");
  node = find_semantic_role(&frame, "No matches",
                            ReaderViewSemantic_Status);
  command = node ? find_draw_for_source(&frame, UI0DrawOp_Text,
                                        node->id) : 0;
  check(node != 0 && command != 0 &&
        rect_equal(node->rect, ui0_rect(104, 146, 308, 18)) &&
        rect_equal(command->rect, ui0_rect(104, 146, 308, 18)) &&
        rect_equal(command->clip_rect, ui0_rect(104, 146, 308, 18)) &&
        command->color == theme->colors[UI0ColorRole_TextMuted] &&
        command->has_text_alignment &&
        command->text_align_x == UI0TextAlignX_Start &&
        command->text_align_y == UI0TextAlignY_Center &&
        command->has_typography_role &&
        command->typography_role == UI0TypographyRole_Body &&
        command->typography_char_width == 8 &&
        command->typography_line_height == 16 &&
        count_semantic(&frame, "Nothing here") == 0,
        "Find ready zero-result copy uses exact frozen copy/geometry without "
        "a generic fallback");
  state.find_query[0] = 0;
  state.find_query_length = 0;
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Find ready empty-query frame builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_FindInput, 0);
  binding = node ? find_text_binding(&frame, node->id) : 0;
  check(find_semantic_role(&frame, "Type and press Enter",
                           ReaderViewSemantic_Status) != 0 &&
        node != 0 && binding != 0 &&
        text_equal(binding->text, "Search in book") &&
        !text_equal(binding->text, "Type and press Enter"),
        "Find ready empty-query prompt preserves the frozen copy");
  projection.labels.no_matches.data = "Localized no matches";
  projection.labels.no_matches.size = 20;
  memcpy(state.find_query, "alpha", 5);
  state.find_query[5] = 0;
  state.find_query_length = 5;
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame) &&
        find_semantic_role(&frame, "Localized no matches",
                           ReaderViewSemantic_Status) != 0,
        "Find ready zero-result copy remains caller-localizable");
  projection.labels.no_matches.data = 0;
  projection.labels.no_matches.size = 0;
  projection.find.rows = find_rows;
  projection.find.row_count = 1;
  projection.find.total_count = 1;
  projection.find.status.message.data = "1 match";
  projection.find.status.message.size = 7;

  projection.find.status.state = ReaderViewLoad_Loading;
  projection.find.status.message.data = "Loading search";
  projection.find.status.message.size = 14;
  memset(&frame_input, 0, sizeof(frame_input));
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame) &&
        count_semantic(&frame, "Loading search") == 1,
        "Find non-ready status is emitted exactly once");
  projection.find.status.state = ReaderViewLoad_Ready;
  projection.find.status.message.data = "1 match";
  projection.find.status.message.size = 7;

  state.left_panel = ReaderViewLeftPanel_None;
  state.right_panel_open = 1;
  check(reader_view_resolve_layout(&state, &layout_input, &layout),
        "Annotations accepted layout resolves");
  memset(&frame_input, 0, sizeof(frame_input));
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Annotations accepted frame builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_Annotations, 0);
  control = node ? find_control_for_source(&storage, node->id) : 0;
  fill = node ? find_draw_for_source(&frame, UI0DrawOp_ControlFill,
                                     node->id) : 0;
  check(node != 0 && control != 0 && fill != 0 &&
        (node->flags & (ReaderViewSemantic_Selected |
                        ReaderViewSemantic_Expanded)) ==
          (ReaderViewSemantic_Selected | ReaderViewSemantic_Expanded) &&
        (node->flags & ReaderViewSemantic_Checked) == 0 &&
        (control->state & UI0ControlState_Selected) != 0 &&
        (control->state & (UI0ControlState_Checked | UI0ControlState_On |
                           UI0ControlState_Open)) == 0 &&
        (fill->flags & UI0DrawFlag_Selected) != 0 &&
        (fill->flags & (UI0DrawFlag_Checked | UI0DrawFlag_On |
                        UI0DrawFlag_Open)) == 0,
        "open Annotations keeps semantic selected/expanded state while its "
        "frozen UI0 visual state is selected-only");
  filter = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightFilter,
    ReaderViewRightFilter_All);
  check(filter != 0 && rect_equal(filter->rect, ui0_rect(1078, 66, 24, 24)) &&
        text_equal(filter->name, "Filter annotations"),
        "Annotations filter trigger uses the frozen target and native name");
  fill = filter ? find_draw_for_source(&frame, UI0DrawOp_ControlFill,
                                       filter->id) : 0;
  border = filter ? find_draw_for_source(&frame, UI0DrawOp_ControlBorder,
                                         filter->id) : 0;
  icon = filter ? find_icon_for_source(&frame, filter->id) : 0;
  check(fill != 0 && border != 0 && icon != 0 &&
        icon->stroke_color == fill->color &&
        rect_equal(icon->rect, ui0_rect(1083, 71, 14, 14)),
        "Annotations filter restores its frozen outlined 14px Select icon "
        "shell");
  check(filter != 0 &&
        count_draw_op_for_source(&frame, UI0DrawOp_ControlFill,
                                 filter->id) == 1 &&
        collect_one_pixel_draws_for_source(
          &frame, UI0DrawOp_ControlFill, filter->id, 0, 0, 0) == 0,
        "unfocused closed filter trigger has no focus corner seam");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightExport, 0);
  check(node != 0 && rect_equal(node->rect, ui0_rect(1112, 66, 24, 24)) &&
        text_equal(node->name, "Export annotations"),
        "Annotations export uses the frozen target and native name");
  fill = node ? find_draw_for_source(&frame, UI0DrawOp_ControlFill,
                                     node->id) : 0;
  border = node ? find_draw_for_source(&frame, UI0DrawOp_ControlBorder,
                                       node->id) : 0;
  icon = node ? find_icon_for_source(&frame, node->id) : 0;
  check(fill != 0 && border != 0 && icon != 0 &&
        icon->stroke_color == fill->color,
        "Annotations export restores its frozen outlined icon shell");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightPanelClose, 0);
  check(node != 0 && rect_equal(node->rect, ui0_rect(1354, 66, 24, 24)) &&
        text_equal(node->name, "Close annotations"),
        "Annotations close uses the frozen target and native name");
  right_close_id = node ? node->id : 0;
  fill = node ? find_draw_for_source(&frame, UI0DrawOp_ControlFill,
                                     node->id) : 0;
  border = node ? find_draw_for_source(&frame, UI0DrawOp_ControlBorder,
                                       node->id) : 0;
  icon = node ? find_icon_for_source(&frame, node->id) : 0;
  check(fill != 0 && border != 0 && icon != 0 &&
        icon->stroke_color == fill->color,
        "Annotations close restores its frozen outlined icon shell");
  node = find_semantic_role(&frame, "Annotations", ReaderViewSemantic_Group);
  command = node ? find_draw_for_source(&frame, UI0DrawOp_Text,
                                        node->id) : 0;
  check(command != 0 &&
        count_draw_op_for_source(&frame, UI0DrawOp_Text,
                                 node ? node->id : 0) == 1 &&
        rect_equal(command->rect, ui0_rect(1156, 64, 186, 28)) &&
        command->has_typography_role &&
        command->typography_role == UI0TypographyRole_SectionTitle,
        "Annotations title restores the frozen header placement");
  node = find_semantic_role(&frame, "One", ReaderViewSemantic_Group);
  command = node ? find_draw_for_source(&frame, UI0DrawOp_Text,
                                        node->id) : 0;
  binding = node ? find_text_binding(&frame, node->id) : 0;
  check(command != 0 &&
        count_draw_op_for_source(&frame, UI0DrawOp_Text,
                                 node ? node->id : 0) == 1 &&
        rect_equal(command->rect, ui0_rect(1078, 106, 300, 20)) &&
        binding != 0 &&
        binding->style == ReaderViewTextStyle_ChromeTitle,
        "Annotations section heading restores the frozen twenty-pixel label "
        "inside its twenty-six-pixel block");
  row = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightRow, 40);
  check(row != 0 && rect_equal(row->rect, ui0_rect(1078, 132, 300, 58)),
        "Annotations row uses frozen section and row geometry");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightRowStar, 40);
  check(node != 0 && rect_equal(node->rect, ui0_rect(1320, 151, 20, 20)),
        "Annotations star uses frozen 20px target");
  icon = node ? find_icon_for_source(&frame, node->id) : 0;
  check(icon != 0 && icon->icon_kind == UI0IconKind_Star &&
        rect_equal(icon->rect, ui0_rect(1323, 154, 14, 14)) &&
        icon->color == theme->colors[UI0ColorRole_TextMuted] &&
        icon->stroke_color ==
          theme->colors[UI0ColorRole_SurfaceElevated] &&
        count_draw_op_for_source(&frame, UI0DrawOp_ControlFill,
                                 node ? node->id : 0) == 0 &&
        count_draw_op_for_source(&frame, UI0DrawOp_ControlBorder,
                                 node ? node->id : 0) == 0,
        "Annotations unstarred icon uses the frozen 14px raster target "
        "without a button shell");
  right_rows[0].flags |= ReaderViewRow_Starred;
  memset(&frame_input, 0, sizeof(frame_input));
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Annotations starred raster frame builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightRowStar, 40);
  icon = node ? find_icon_for_source(&frame, node->id) : 0;
  check(icon && icon->color == theme->colors[UI0ColorRole_Focus] &&
        icon->stroke_color == theme->colors[UI0ColorRole_Badge] &&
        rect_equal(icon->rect, ui0_rect(1323, 154, 14, 14)),
        "Annotations starred icon preblends against the frozen badge fill");
  right_rows[0].flags &= ~ReaderViewRow_Starred;
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Annotations unstarred raster state restores");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightRowMenu, 40);
  check(node != 0 && rect_equal(node->rect, ui0_rect(1340, 147, 30, 28)) &&
        text_equal(node->name, "Annotation actions"),
        "Annotations row menu uses the frozen target and native name");
  fill = node ? find_draw_for_source(&frame, UI0DrawOp_ControlFill,
                                     node->id) : 0;
  border = node ? find_draw_for_source(&frame, UI0DrawOp_ControlBorder,
                                       node->id) : 0;
  command = node ? find_draw_for_source(&frame, UI0DrawOp_Text,
                                        node->id) : 0;
  binding = node ? find_text_binding(&frame, node->id) : 0;
  check(fill != 0 && border != 0 && command != 0 &&
        rect_equal(fill->rect, ui0_rect(1340, 147, 30, 28)) &&
        rect_equal(border->rect, ui0_rect(1340, 147, 30, 28)) &&
        rect_equal(command->rect, ui0_rect(1344, 147, 22, 28)) &&
        rect_equal(command->clip_rect, ui0_rect(1340, 147, 30, 28)) &&
        command->has_text_alignment &&
        command->text_align_x == UI0TextAlignX_Center &&
        binding != 0 && text_equal(binding->text, "...") &&
        find_icon_for_source(&frame, node ? node->id : 0) == 0 &&
        count_draw_op_for_source(&frame, UI0DrawOp_ControlFill,
                                 node ? node->id : 0) == 1 &&
        count_draw_op_for_source(&frame, UI0DrawOp_ControlBorder,
                                 node ? node->id : 0) == 1,
        "Annotations row menu restores the frozen centered literal ellipsis "
        "inside the standard clipped 30px menu-trigger shell");
  row = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightRow, 40);
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightRowStar, 40);
  row_signal = row ? find_signal_for_source(&storage, row->id) : 0;
  star_signal = node ? find_signal_for_source(&storage, node->id) : 0;
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightRowMenu, 40);
  menu_signal = node ? find_signal_for_source(&storage, node->id) : 0;
  check(row != 0 && row_signal != 0 && star_signal != 0 &&
        menu_signal != 0 &&
        rect_equal(row->rect, ui0_rect(1078, 132, 300, 58)) &&
        rect_equal(row_signal->rect, ui0_rect(1083, 132, 295, 58)) &&
        rect_equal(row_signal->hit_rect, ui0_rect(1083, 132, 237, 58)) &&
        rect_equal(star_signal->hit_rect, ui0_rect(1320, 151, 20, 20)) &&
        rect_equal(menu_signal->hit_rect, ui0_rect(1340, 147, 30, 28)),
        "Annotations keeps full visual/semantic row geometry while its "
        "physical body ends before the Star and Menu child targets");
  node = find_semantic_role(&frame, "Bookmark - re10 loc 1",
                            ReaderViewSemantic_Group);
  command = node ? find_draw_for_source(&frame, UI0DrawOp_Text,
                                        node->id) : 0;
  check(command != 0 &&
        rect_equal(command->rect, ui0_rect(1091, 142, 221, 20)) &&
        command->color == theme->colors[UI0ColorRole_TextSecondary] &&
        command->has_typography_role &&
        command->typography_role == UI0TypographyRole_Metadata,
        "Annotations secondary record uses the frozen padded metadata row");
  node = find_semantic_role(&frame, "Saved place",
                            ReaderViewSemantic_Group);
  command = node ? find_draw_for_source(&frame, UI0DrawOp_Text,
                                        node->id) : 0;
  check(command != 0 &&
        rect_equal(command->rect, ui0_rect(1091, 168, 221, 20)),
        "Annotations primary record restores the frozen centered 20px text "
        "stack");
  memset(&frame_input, 0, sizeof(frame_input));
  frame_input.ui = ui0_input_pointer(1330, 161, 0, 0, 0);
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Annotations star-child hover frame builds");
  row = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightRow, 40);
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightRowStar, 40);
  control = node ? find_control_for_source(&storage, node->id) : 0;
  fill = row ? find_draw_for_source(&frame, UI0DrawOp_ControlFill,
                                    row->id) : 0;
  check(row != 0 && node != 0 && control != 0 && fill != 0 &&
        (fill->flags & UI0DrawFlag_Hovered) != 0 &&
        (control->state & UI0ControlState_Hovered) != 0 &&
        (node->flags & ReaderViewSemantic_Focusable) != 0,
        "Annotations row keeps its frozen parent hover fill over the star "
        "while the star remains the bounded child target");
  frame_input.ui = ui0_input_pointer(1330, 161, 1, 1, 0);
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Annotations overlapping star press frame builds");
  frame_input.ui = ui0_input_pointer(1330, 161, 0, 0, 1);
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Annotations overlapping star release frame builds");
  action = find_action(&frame, ReaderViewAction_ToggleRightRowStar);
  check(action != 0 && action->key == 40 && frame.action_count == 1 &&
        find_action(&frame, ReaderViewAction_ActivateRightRow) == 0 &&
        state.active_right_key == 0 &&
        state.popup == ReaderViewPopup_None,
        "Annotations overlapping parent hover preserves star-only pointer "
        "activation");
  memset(&frame_input, 0, sizeof(frame_input));
  frame_input.ui = ui0_input_pointer(1355, 161, 0, 0, 0);
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Annotations menu-child hover frame builds");
  row = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightRow, 40);
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightRowMenu, 40);
  control = node ? find_control_for_source(&storage, node->id) : 0;
  fill = row ? find_draw_for_source(&frame, UI0DrawOp_ControlFill,
                                    row->id) : 0;
  check(row != 0 && node != 0 && control != 0 && fill != 0 &&
        (fill->flags & UI0DrawFlag_Hovered) != 0 &&
        (control->state & UI0ControlState_Hovered) != 0 &&
        (node->flags & ReaderViewSemantic_Focusable) != 0,
        "Annotations row keeps its frozen parent hover fill over the menu "
        "while the menu remains the bounded child target");
  frame_input.ui = ui0_input_pointer(1355, 161, 1, 1, 0);
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Annotations menu-only pointer press frame builds");
  frame_input.ui = ui0_input_pointer(1355, 161, 0, 0, 1);
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Annotations menu-only pointer release frame builds");
  check(frame.action_count == 0 &&
        find_action(&frame, ReaderViewAction_ActivateRightRow) == 0 &&
        find_action(&frame, ReaderViewAction_ToggleRightRowStar) == 0 &&
        state.active_right_key == 0 &&
        state.popup == ReaderViewPopup_RightRowActions &&
        state.right_menu_key == 40,
        "Annotations Menu pointer opens only its popup without a row "
        "activation or host navigation/history action");
  state.popup = ReaderViewPopup_None;
  state.restore_focus_id = 0;
  state.right_menu_key = 0;
  state.focus_id = 0;
  state.focus_visible = 0;
  state.hot_id = 0;
  state.active_id = 0;
  memset(&frame_input, 0, sizeof(frame_input));
  right_rows[0].flags |= ReaderViewRow_Selected | ReaderViewRow_Starred;
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Annotations selected-starred visual frame builds");
  row = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightRow, 40);
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightRowStar, 40);
  icon = node ? find_icon_for_source(&frame, node->id) : 0;
  check(row != 0 &&
        (row->flags & ReaderViewSemantic_Selected) != 0 &&
        count_draw_op_for_source(&frame, UI0DrawOp_ControlFill,
                                 row->id) == 0 &&
        icon != 0 && icon->color == theme->colors[UI0ColorRole_Focus],
        "Annotations selected rows stay shell-free while starred color is "
        "the exact focus token");
  right_rows[0].flags = ReaderViewRow_Enabled;
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Annotations selected-starred visual state restores");
  row = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightRow, 40);
  right_id = row ? row->id : 0;
  check(row != 0 && reader_view_accessibility_focus(&state, row->id),
        "Annotations row accepts focus-only visual coverage");
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Annotations focus-only visual frame builds");
  row = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightRow, 40);
  check(row != 0 &&
        (row->flags & ReaderViewSemantic_Focused) != 0 &&
        (row->flags & ReaderViewSemantic_Selected) == 0 &&
        count_draw_op_for_source(&frame, UI0DrawOp_ControlFill,
                                 row->id) == 0 &&
        count_draw_op_for_source(&frame, UI0DrawOp_FocusRing,
                                 row->id) == 1,
        "Annotations focus-only row preserves semantic/focus-ring evidence "
        "without inventing a row fill");
  if (row)
  {
    rect = row->rect;
    frame_input.ui = ui0_input_pointer(rect.x + 100,
                                       rect.y + rect.h / 2,
                                       0, 0, 0);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "Annotations row hover builds");
    row = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_RightRow, 40);
    fill = row ? find_draw_for_source(&frame, UI0DrawOp_ControlFill,
                                      row->id) : 0;
    check(fill != 0 &&
          fill->color == theme->colors[UI0ColorRole_SurfaceElevated] &&
          fill->stroke_color ==
            theme->colors[UI0ColorRole_SurfaceElevated] &&
          count_draw_op_for_source(&frame, UI0DrawOp_ControlFill,
                                   row ? row->id : 0) == 1 &&
          count_draw_op_for_source(&frame, UI0DrawOp_ControlBorder,
                                   row ? row->id : 0) == 0,
          "Annotations hover uses the frozen elevated-surface fill without "
          "a row border");
    frame_input.ui = ui0_input_pointer(rect.x + 100,
                                       rect.y + rect.h / 2,
                                       1, 1, 0);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "Annotations row press builds");
    row = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_RightRow, 40);
    fill = row ? find_draw_for_source(&frame, UI0DrawOp_ControlFill,
                                      row->id) : 0;
    check(fill != 0 &&
          fill->color == theme->colors[UI0ColorRole_SurfaceElevated] &&
          fill->stroke_color ==
            theme->colors[UI0ColorRole_SurfaceElevated] &&
          count_draw_op_for_source(&frame, UI0DrawOp_ControlBorder,
                                   row ? row->id : 0) == 0,
          "Annotations active row retains the frozen elevated-surface fill "
          "without a row border");
    frame_input.ui = ui0_input_pointer(rect.x + 100,
                                       rect.y + rect.h / 2,
                                       0, 0, 1);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "Annotations row release builds");
  }
  action = find_action(&frame, ReaderViewAction_ActivateRightRow);
  check(action != 0 && action->key == 40 && frame.action_count == 1 &&
        find_action(&frame, ReaderViewAction_ToggleRightRowStar) == 0 &&
        state.popup == ReaderViewPopup_None &&
        state.right_panel_open && state.active_right_key == 40 &&
        state.focus_id == right_id && !state.focus_visible,
        "Annotations row invokes exactly once and keeps panel/focus state");
  memset(&frame_input, 0, sizeof(frame_input));
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Annotations direct-row selection follow-up frame builds");
  row = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightRow, 40);
  check(row != 0 &&
        (row->flags & ReaderViewSemantic_Selected) != 0,
        "direct annotation activation owns bounded selected chrome state");
  check(row != 0 &&
        reader_view_accessibility_focus(&state, row ? row->id : 0) &&
        reader_view_accessibility_invoke(&state, row ? row->id : 0),
        "Annotations full semantic row remains accessibility-invokable after "
        "the physical child-target carve");
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Annotations accessibility row activation frame builds");
  action = find_action(&frame, ReaderViewAction_ActivateRightRow);
  check(action != 0 && action->key == 40 && frame.action_count == 1 &&
        state.focus_id == right_id && state.focus_visible,
        "Annotations accessibility row activation remains exact and "
        "independent of physical hit geometry");

  filter = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightFilter,
    ReaderViewRightFilter_All);
  if (filter)
  {
    rect = filter->rect;
    frame_input.ui = ui0_input_pointer(rect.x + rect.w / 2,
                                       rect.y + rect.h / 2,
                                       1, 1, 0);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "Annotations filter press builds");
    frame_input.ui = ui0_input_pointer(rect.x + rect.w / 2,
                                       rect.y + rect.h / 2,
                                       0, 0, 1);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "Annotations filter release builds");
  }
  memset(&frame_input, 0, sizeof(frame_input));
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Annotations filter popup builds");
  node = find_semantic_role(&frame, "Annotation filters",
                            ReaderViewSemantic_Menu);
  check(node != 0 && rect_equal(node->rect, ui0_rect(1078, 96, 300, 136)),
        "Annotations filter popup uses the frozen containment rectangle and "
        "native name");
  option = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightFilterOption,
    ReaderViewRightFilter_All);
  check(option != 0 && rect_equal(option->rect,
                                  ui0_rect(1096, 100, 274, 29)) &&
        text_equal(option->name, "All (1)") &&
        (option->flags & ReaderViewSemantic_Focused) != 0 &&
        state.focus_id == option->id && !state.focus_visible &&
        state.restore_focus_id == filter->id,
        "Filter popup initializes selected option focus invisibly");
  binding = option ? find_text_binding(&frame, option->id) : 0;
  indicator = option ?
    find_draw_for_source(&frame, UI0DrawOp_IndicatorFill,
                         option->id) : 0;
  command = option ? find_draw_for_source(&frame, UI0DrawOp_Text,
                                           option->id) : 0;
  check(binding != 0 && text_equal(binding->text, "All (1)") &&
        indicator != 0 &&
        rect_equal(indicator->rect, ui0_rect(1086, 106, 3, 17)) &&
        indicator->color == theme->colors[UI0ColorRole_Focus] &&
        command != 0 &&
        rect_equal(command->rect, ui0_rect(1106, 100, 254, 29)) &&
        rect_equal(command->clip_rect, ui0_rect(1096, 100, 274, 29)),
        "Filter All publishes the exact counted label, selected rail, and "
        "frozen ten-pixel text padding");
  option = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightFilterOption,
    ReaderViewRightFilter_Highlights);
  check(option != 0 && rect_equal(option->rect,
                                  ui0_rect(1096, 132, 274, 29)) &&
        text_equal(option->name, "All Highlight Colors (0)") &&
        find_text_binding(&frame, option ? option->id : 0) != 0 &&
        text_equal(find_text_binding(&frame, option ? option->id : 0)->text,
                   "All Highlight Colors (0)"),
        "Filter Highlights option uses frozen ordering");
  option = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightFilterOption,
    ReaderViewRightFilter_Notes);
  check(option != 0 && rect_equal(option->rect,
                                  ui0_rect(1096, 164, 274, 29)) &&
        text_equal(option->name, "Notes (0)") &&
        find_text_binding(&frame, option ? option->id : 0) != 0 &&
        text_equal(find_text_binding(&frame, option ? option->id : 0)->text,
                   "Notes (0)"),
        "Filter Notes option uses frozen ordering");
  option = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightFilterOption,
    ReaderViewRightFilter_Bookmarks);
  check(option != 0 && rect_equal(option->rect,
                                  ui0_rect(1096, 196, 274, 29)) &&
        text_equal(option->name, "Bookmarks (1)") &&
        find_text_binding(&frame, option ? option->id : 0) != 0 &&
        text_equal(find_text_binding(&frame, option ? option->id : 0)->text,
                   "Bookmarks (1)"),
        "Filter Bookmarks option uses frozen ordering");

  check(reader_view_accessibility_focus(&state, right_close_id) &&
        reader_view_accessibility_invoke(&state, right_close_id),
        "underlying Annotations close queues while filter popup is active");
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "filter popup accessibility-containment frame builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightPanelClose, 0);
  option = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightFilterOption,
    ReaderViewRightFilter_All);
  check(state.right_panel_open && state.popup == ReaderViewPopup_RightFilter &&
        frame.action_count == 0 && node != 0 &&
        (node->flags & (ReaderViewSemantic_Enabled |
                        ReaderViewSemantic_Focusable |
                        ReaderViewSemantic_Focused)) == 0 &&
        option != 0 && (option->flags & ReaderViewSemantic_Focused) != 0 &&
        state.focus_id == option->id,
        "popup consumes blocked AT focus/invoke and keeps focus contained");

  {
    UI0ThemeProfile light_profile =
      ui0_theme_profile_for_kind(UI0ThemeProfile_Light);
    UI0ThemeProfile dark_profile =
      ui0_theme_profile_for_kind(UI0ThemeProfile_Dark);
    const UI0DrawCommand *seams[4] = { 0, 0, 0, 0 };
    UI0S32 seam_indices[4] = { -1, -1, -1, -1 };
    UI0S32 border_index;
    UI0S32 seam_count;
    UI0S32 seam_index;
    static const UI0Rect expected_corners[4] = {
      { 1078, 66, 1, 1 }, { 1101, 66, 1, 1 },
      { 1078, 89, 1, 1 }, { 1101, 89, 1, 1 },
    };

    build_input.theme = &light_profile.resolved;
    frame_input.escape_pressed = 1;
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "Filter popup Escape builds");
    filter = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_RightFilter,
      ReaderViewRightFilter_All);
    command = filter ? find_draw_for_source(&frame, UI0DrawOp_FocusRing,
                                             filter->id) : 0;
    border = filter ? find_draw_for_source(&frame, UI0DrawOp_ControlBorder,
                                            filter->id) : 0;
    control = filter ? find_control_for_source(&storage, filter->id) : 0;
    border_index = filter ? find_draw_index_for_source(
      &frame, UI0DrawOp_ControlBorder, filter->id) : -1;
    seam_count = filter ? collect_one_pixel_draws_for_source(
      &frame, UI0DrawOp_ControlFill, filter->id,
      seams, seam_indices, 4) : 0;
    check(state.popup == ReaderViewPopup_None &&
          state.restore_focus_id == 0 && filter &&
          state.focus_id == filter->id && state.focus_visible &&
          (filter->flags & ReaderViewSemantic_Focused) != 0 && command == 0 &&
          border &&
          border->color ==
            light_profile.resolved.colors[UI0ColorRole_Focus] &&
          (border->flags &
           (UI0DrawFlag_Focused | UI0DrawFlag_FocusVisible)) ==
            (UI0DrawFlag_Focused | UI0DrawFlag_FocusVisible) &&
          (border->flags & (UI0DrawFlag_Open | UI0DrawFlag_Active)) == 0 &&
          rect_equal(border->rect, ui0_rect(1078, 66, 24, 24)) &&
          rect_equal(border->clip_rect, ui0_rect(1078, 66, 24, 24)) &&
          count_draw_op_for_source(&frame, UI0DrawOp_FocusRing,
                                   filter->id) == 0,
          "Filter Escape restores the frozen focused trigger border and "
          "suppresses its separate double-composited Focus ring");
    check(light_profile.resolved.colors[UI0ColorRole_Surface] ==
            0xFFFFFDF9u &&
          light_profile.resolved.colors[UI0ColorRole_Focus] ==
            0xFFF26A1Bu &&
          seam_count == 4 &&
          count_draw_op_for_source(&frame, UI0DrawOp_ControlFill,
                                   filter ? filter->id : 0) == 5,
          "focused filter adds exactly four frozen light-profile corner "
          "seams over its ordinary fill");
    for (seam_index = 0; seam_index < 4; ++seam_index)
    {
      check(seams[seam_index] != 0 && control != 0 &&
            rect_equal(seams[seam_index]->rect,
                       expected_corners[seam_index]) &&
            rect_equal(seams[seam_index]->clip_rect,
                       ui0_rect(1078, 66, 24, 24)) &&
            seams[seam_index]->source_id == filter->id &&
            seams[seam_index]->source_kind == UI0ControlKind_SelectTrigger &&
            seams[seam_index]->source_index == control->box_index &&
            seams[seam_index]->color == 0xFFFAC6A5u &&
            seams[seam_index]->stroke_color == 0xFFFAC6A5u &&
            seams[seam_index]->flags == UI0DrawFlag_RadiusExplicit &&
            seams[seam_index]->corner_radius == 0 &&
            seam_indices[seam_index] > border_index &&
            (seam_index == 0 ||
             seam_indices[seam_index] > seam_indices[seam_index - 1]),
            "focused filter corner seam preserves exact source, clip, "
            "light color, and TL/TR/BL/BR post-border order");
    }

    memset(&frame_input, 0, sizeof(frame_input));
    build_input.theme = &dark_profile.resolved;
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "focused filter dark-profile adaptation builds");
    filter = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_RightFilter,
      ReaderViewRightFilter_All);
    for (seam_index = 0; seam_index < 4; ++seam_index)
      seams[seam_index] = 0;
    seam_count = filter ? collect_one_pixel_draws_for_source(
      &frame, UI0DrawOp_ControlFill, filter->id, seams, 0, 4) : 0;
    check(dark_profile.resolved.colors[UI0ColorRole_Surface] ==
            0xFF181716u &&
          dark_profile.resolved.colors[UI0ColorRole_Focus] ==
            0xFFFF7A2Fu &&
          seam_count == 4 && seams[0] && seams[1] && seams[2] && seams[3] &&
          seams[0]->color == 0xFF6F3C1Fu &&
          seams[1]->color == 0xFF6F3C1Fu &&
          seams[2]->color == 0xFF6F3C1Fu &&
          seams[3]->color == 0xFF6F3C1Fu &&
          frame.action_count == 0 && state.popup == ReaderViewPopup_None &&
          filter && state.focus_id == filter->id && state.focus_visible,
          "filter corner coverage adapts every channel to the dark Surface "
          "and Focus tokens without changing interaction");
    build_input.theme = theme;
  }

  state.left_panel = ReaderViewLeftPanel_Contents;
  state.right_panel_open = 1;
  check(reader_view_resolve_layout(&state, &layout_input, &layout),
        "simultaneous panel Escape layout resolves");
  memset(&frame_input, 0, sizeof(frame_input));
  frame_input.escape_pressed = 1;
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame) &&
        state.left_panel == ReaderViewLeftPanel_None &&
        state.right_panel_open,
        "Escape closes TOC or Find before the docked Annotations panel");
  check(reader_view_resolve_layout(&state, &layout_input, &layout),
        "right-only Escape layout resolves");
  memset(&frame_input, 0, sizeof(frame_input));
  frame_input.escape_pressed = 1;
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame) &&
        state.left_panel == ReaderViewLeftPanel_None &&
        state.right_panel_open && state.popup == ReaderViewPopup_None,
        "Escape leaves the frozen docked Annotations panel open");

  memset(&frame_input, 0, sizeof(frame_input));
  filter = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightFilter,
    ReaderViewRightFilter_All);
  if (filter)
  {
    rect = filter->rect;
    frame_input.ui = ui0_input_pointer(rect.x + rect.w / 2,
                                       rect.y + rect.h / 2,
                                       1, 1, 0);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "Filter reopen press builds");
    filter = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_RightFilter,
      ReaderViewRightFilter_All);
    border = filter ? find_draw_for_source(
      &frame, UI0DrawOp_ControlBorder, filter->id) : 0;
    check(filter != 0 && border != 0 &&
          (border->flags & UI0DrawFlag_Active) != 0 &&
          (border->flags & UI0DrawFlag_Open) == 0 &&
          border->color == theme->colors[UI0ColorRole_Focus] &&
          count_draw_op_for_source(&frame, UI0DrawOp_ControlFill,
                                   filter->id) == 1 &&
          collect_one_pixel_draws_for_source(
            &frame, UI0DrawOp_ControlFill, filter->id, 0, 0, 0) == 0,
          "active filter trigger retains its exact Focus border without an "
          "open-state substitution or focus corner seam");
    frame_input.ui = ui0_input_pointer(rect.x + rect.w / 2,
                                       rect.y + rect.h / 2,
                                       0, 0, 1);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "Filter reopen release builds");
  }
  memset(&frame_input, 0, sizeof(frame_input));
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Filter reopened popup builds");
  filter = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightFilter,
    ReaderViewRightFilter_All);
  border = filter ? find_draw_for_source(
    &frame, UI0DrawOp_ControlBorder, filter->id) : 0;
  check(filter != 0 && border != 0 &&
        (border->flags & UI0DrawFlag_Open) != 0 &&
        border->color == theme->colors[UI0ColorRole_Focus] &&
        count_draw_op_for_source(&frame, UI0DrawOp_ControlFill,
                                 filter->id) == 1 &&
        collect_one_pixel_draws_for_source(
          &frame, UI0DrawOp_ControlFill, filter->id, 0, 0, 0) == 0,
        "open filter trigger retains its exact Focus border while the popup "
        "owns interaction without a focus corner seam");
  option = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightFilterOption,
    ReaderViewRightFilter_Bookmarks);
  if (option)
  {
    rect = option->rect;
    frame_input.ui = ui0_input_pointer(rect.x + rect.w / 2,
                                       rect.y + rect.h / 2,
                                       1, 1, 0);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "Bookmarks filter option press builds");
    frame_input.ui = ui0_input_pointer(rect.x + rect.w / 2,
                                       rect.y + rect.h / 2,
                                       0, 0, 1);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "Bookmarks filter option release builds");
  }
  action = find_action(&frame, ReaderViewAction_RightFilterChanged);
  filter = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightFilter,
    ReaderViewRightFilter_All);
  border = filter ? find_draw_for_source(&frame, UI0DrawOp_ControlBorder,
                                          filter->id) : 0;
  check(action != 0 &&
        action->right_filter == ReaderViewRightFilter_Bookmarks &&
        state.right_filter == ReaderViewRightFilter_Bookmarks &&
        state.right_scroll_y == 0 && state.popup == ReaderViewPopup_None &&
        filter && state.focus_id == filter->id && border &&
        (border->flags & UI0DrawFlag_Open) != 0 &&
        border->color == theme->colors[UI0ColorRole_Focus] &&
        count_draw_op_for_source(&frame, UI0DrawOp_ControlFill,
                                 filter->id) == 1 &&
        collect_one_pixel_draws_for_source(
          &frame, UI0DrawOp_ControlFill, filter->id, 0, 0, 0) == 0,
        "Bookmarks filter selection emits one bounded action and preserves "
        "the closing frame's exact open-trigger draw state without a focus "
        "corner seam");
  memset(&frame_input, 0, sizeof(frame_input));
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Bookmarks filter settled trigger frame builds");
  filter = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightFilter,
    ReaderViewRightFilter_Bookmarks);
  border = filter ? find_draw_for_source(&frame, UI0DrawOp_ControlBorder,
                                          filter->id) : 0;
  command = filter ? find_draw_for_source(&frame, UI0DrawOp_FocusRing,
                                           filter->id) : 0;
  check(filter && state.focus_id == filter->id &&
        state.right_filter == ReaderViewRightFilter_Bookmarks && border &&
        (border->flags & UI0DrawFlag_Focused) != 0 &&
        (border->flags &
         (UI0DrawFlag_Open | UI0DrawFlag_Active |
          UI0DrawFlag_FocusVisible)) == 0 &&
        border->color == theme->colors[UI0ColorRole_Focus] && command == 0 &&
        count_draw_op_for_source(&frame, UI0DrawOp_ControlFill,
                                 filter->id) == 1 &&
        collect_one_pixel_draws_for_source(
          &frame, UI0DrawOp_ControlFill, filter->id, 0, 0, 0) == 0,
        "settled Bookmarks trigger retains its exact filter/focus state with "
        "the frozen focused border and no Focus-ring or corner-seam command");

  projection.right.available_filters = ReaderViewRightFilterFlag_All;
  state.popup = ReaderViewPopup_RightFilter;
  state.right_filter_menu_flags = ReaderViewRightFilterFlag_All;
  state.restore_focus_id = filter ? filter->id : 0;
  state.focus_id = state.restore_focus_id;
  state.focus_visible = 1;
  memset(&frame_input, 0, sizeof(frame_input));
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "narrowed filter popup builds with a stale prior selection");
  option = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightFilterOption,
    ReaderViewRightFilter_All);
  check(option != 0 &&
        (option->flags & ReaderViewSemantic_Focused) != 0 &&
        state.focus_id == option->id && state.focus_visible &&
        state.right_filter == ReaderViewRightFilter_Bookmarks &&
        frame.action_count == 0,
        "filter popup contains focus on first available choice without "
        "silently changing host-visible selection");
  {
    UI0ID expected_filter_focus = state.restore_focus_id;
    projection.right.available_filters = 0;
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame) &&
          state.popup == ReaderViewPopup_None &&
          state.right_filter_menu_flags == 0 &&
          state.restore_focus_id == 0 &&
          state.focus_id == expected_filter_focus && state.focus_visible &&
          find_semantic_role(&frame, "Annotation filters",
                             ReaderViewSemantic_Menu) == 0 &&
          (frame.change_flags & ReaderViewFrameChange_StateChanged) != 0 &&
          (frame.change_flags & ReaderViewFrameChange_FocusChanged) != 0,
          "removing available filters closes the stale popup and restores "
          "its trigger without a dangling option focus");
  }
  projection.right.available_filters = ReaderViewRightFilterFlag_All |
                                       ReaderViewRightFilterFlag_Bookmarks |
                                       ReaderViewRightFilterFlag_Highlights |
                                       ReaderViewRightFilterFlag_Notes;
  state.popup = ReaderViewPopup_None;
  state.restore_focus_id = 0;

  memset(right_rows, 0, sizeof(right_rows));
  right_rows[0].key = 70;
  right_rows[0].kind = ReaderViewRightRow_Note;
  right_rows[0].primary.data = "Colored note";
  right_rows[0].primary.size = 12;
  right_rows[0].color_key = 7000;
  right_rows[0].rail_color = 0xff2468acu;
  right_rows[0].flags = ReaderViewRow_Enabled;
  projection.right.rows = right_rows;
  projection.right.row_count = 1;
  projection.right.total_count = 1;
  projection.right.available_filters = ReaderViewRightFilterFlag_Notes;
  projection.right.all_count = 1;
  projection.right.bookmark_count = 0;
  projection.right.highlight_count = 0;
  projection.right.note_count = 1;
  state.right_filter = ReaderViewRightFilter_Notes;
  memset(&frame_input, 0, sizeof(frame_input));
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Notes-only colored-note projection builds without attachment");
  command = find_draw_for_rect(&frame, UI0DrawOp_ControlFill,
                               ui0_rect(1078, 108, 4, 56));
  check((right_rows[0].flags & ReaderViewRow_AttachedToPrevious) == 0 &&
        command != 0 && command->color == 0xff2468acu &&
        command->stroke_color == 0xff2468acu,
        "Notes-only standalone note retains the exact caller rail color in "
        "the light profile");
  {
    UI0TokenSet dark_tokens = ui0_default_tokens(UI0ThemeKind_Dark);
    UI0ResolvedTheme dark_theme = ui0_resolve_tokens(&dark_tokens);
    build_input.theme = &dark_theme;
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "Notes-only colored-note dark-profile frame builds");
    command = find_draw_for_rect(&frame, UI0DrawOp_ControlFill,
                                 ui0_rect(1078, 108, 4, 56));
    check(command != 0 && command->color == 0xff2468acu &&
          command->stroke_color == 0xff2468acu,
          "Notes-only standalone note retains the exact caller rail color "
          "in the dark profile");
    build_input.theme = theme;
  }

  {
    typedef struct RightMenuExpectation
    {
      ReaderViewRightRowKind kind;
      ReaderViewRightActionFlags flags;
      UI0S32 count;
      ReaderViewSemanticControl controls[3];
      ReaderViewActionKind actions[3];
      const char *labels[3];
    } RightMenuExpectation;
    static const RightMenuExpectation variants[3] = {
      {
        ReaderViewRightRow_Bookmark,
        ReaderViewRightAction_Activate |
          ReaderViewRightAction_ToggleStar |
          ReaderViewRightAction_Delete,
        2,
        {
          ReaderViewSemanticControl_RightActionGoTo,
          ReaderViewSemanticControl_RightActionDelete,
          ReaderViewSemanticControl_None,
        },
        {
          ReaderViewAction_ActivateRightRow,
          ReaderViewAction_DeleteRightRow,
          ReaderViewAction_None,
        },
        {"Go to", "Delete bookmark", ""},
      },
      {
        ReaderViewRightRow_Note,
        ReaderViewRightAction_Activate |
          ReaderViewRightAction_ToggleStar |
          ReaderViewRightAction_EditNote |
          ReaderViewRightAction_Delete,
        3,
        {
          ReaderViewSemanticControl_RightActionGoTo,
          ReaderViewSemanticControl_RightActionEditNote,
          ReaderViewSemanticControl_RightActionDelete,
        },
        {
          ReaderViewAction_ActivateRightRow,
          ReaderViewAction_EditRightRowNote,
          ReaderViewAction_DeleteRightRow,
        },
        {"Go to", "Edit note", "Delete note"},
      },
      {
        ReaderViewRightRow_Highlight,
        ReaderViewRightAction_Activate |
          ReaderViewRightAction_ToggleStar |
          ReaderViewRightAction_Delete,
        3,
        {
          ReaderViewSemanticControl_RightActionGoTo,
          ReaderViewSemanticControl_RightActionToggleStar,
          ReaderViewSemanticControl_RightActionDelete,
        },
        {
          ReaderViewAction_ActivateRightRow,
          ReaderViewAction_ToggleRightRowStar,
          ReaderViewAction_DeleteRightRow,
        },
        {"Go to", "Add star", "Delete highlight"},
      },
    };
    UI0S32 variant_index;
    for (variant_index = 0; variant_index < 3; ++variant_index)
    {
      const RightMenuExpectation *variant = variants + variant_index;
      right_rows[0].kind = variant->kind;
      right_rows[0].actions = variant->flags;
      right_rows[0].section.data = "One";
      right_rows[0].section.size = 3;
      state.popup = ReaderViewPopup_None;
      state.restore_focus_id = 0;
      state.focus_id = 0;
      state.focus_visible = 0;
      memset(&frame_input, 0, sizeof(frame_input));
      build_input.frame_index += 1;
      check(reader_view_build(&build_input, &storage, &frame),
            "kind-specific annotation action seed frame builds");
      node = find_semantic_control_source(
        &frame, ReaderViewSemanticControl_RightRowStar, 70);
      check(node != 0,
            "Bookmark, Note, and Highlight retain the frozen inline star");
      node = find_semantic_control_source(
        &frame, ReaderViewSemanticControl_RightRowMenu, 70);
      right_menu_id = node ? node->id : 0;
      check(right_menu_id != 0 && node != 0 &&
            text_equal(node->name, "Annotation actions"),
            "kind-specific annotation row exposes its named menu trigger");

      state.popup = ReaderViewPopup_RightRowActions;
      state.right_menu_key = 70;
      state.right_menu_kind = variant->kind;
      state.right_menu_actions = variant->flags;
      state.restore_focus_id = right_menu_id;
      state.focus_id = right_menu_id;
      state.focus_visible = 1;
      memset(&frame_input, 0, sizeof(frame_input));
      build_input.frame_index += 1;
      check(reader_view_build(&build_input, &storage, &frame),
            "kind-specific annotation row action popup builds");
      node = find_semantic_role(&frame, "Annotation actions",
                                ReaderViewSemantic_Menu);
      check(node != 0 &&
            rect_equal(node->rect,
                       ui0_rect(1184, 179, 186,
                                variant->count == 2 ? 86 : 120)) &&
            count_semantic_role(&frame, ReaderViewSemantic_MenuItem) ==
              variant->count,
            "annotation action popup restores frozen anchored geometry and "
            "kind-specific row count");
      row = find_semantic_control_source(
        &frame, ReaderViewSemanticControl_RightRow, 70);
      fill = row ? find_draw_for_source(&frame, UI0DrawOp_ControlFill,
                                        row->id) : 0;
      check(row && fill &&
            fill->color == theme->colors[UI0ColorRole_SurfaceElevated] &&
            count_draw_op_for_source(&frame, UI0DrawOp_ControlFill,
                                     row->id) == 1,
            "open annotation action menu retains the frozen elevated owner "
            "row fill");
      for (index = 0; index < variant->count; ++index)
      {
        option = find_semantic_control_source(
          &frame, variant->controls[index], 70);
        check(option != 0 &&
              rect_equal(option->rect,
                         ui0_rect(1202, 189 + index * 34, 160, 32)) &&
              text_equal(option->name, variant->labels[index]),
              "annotation action uses exact frozen body geometry and copy");
      }
      check((variant->kind == ReaderViewRightRow_Highlight ||
             find_semantic_control_source(
               &frame,
               ReaderViewSemanticControl_RightActionToggleStar, 70) == 0) &&
            (variant->kind == ReaderViewRightRow_Note ||
             find_semantic_control_source(
               &frame,
               ReaderViewSemanticControl_RightActionEditNote, 70) == 0),
            "annotation popup suppresses kind-inapplicable Star and Edit "
            "items without removing the inline star");

      for (index = 0; index < variant->count; ++index)
      {
        state.popup = ReaderViewPopup_RightRowActions;
        state.right_menu_key = 70;
        state.right_menu_kind = variant->kind;
        state.right_menu_actions = variant->flags;
        state.restore_focus_id = right_menu_id;
        state.focus_id = right_menu_id;
        state.focus_visible = 1;
        memset(&frame_input, 0, sizeof(frame_input));
        build_input.frame_index += 1;
        check(reader_view_build(&build_input, &storage, &frame),
              "annotation row action invocation seed builds");
        option = find_semantic_control_source(
          &frame, variant->controls[index], 70);
        check(option != 0 &&
              reader_view_accessibility_focus(&state,
                                              option ? option->id : 0) &&
              reader_view_accessibility_invoke(&state,
                                               option ? option->id : 0),
              "annotation row action accepts contained accessibility invoke");
        build_input.frame_index += 1;
        check(reader_view_build(&build_input, &storage, &frame),
              "annotation row action invocation frame builds");
        action = find_action(&frame, variant->actions[index]);
        check(action != 0 && action->key == 70 && frame.action_count == 1 &&
              state.popup == ReaderViewPopup_None &&
              state.restore_focus_id == 0 &&
              state.focus_id == right_menu_id && state.focus_visible &&
              state.active_right_key == 0,
              "annotation action closes before return and restores the exact "
              "row-menu focus while clearing the vanished prior-row selection");
      }
    }

    right_rows[0].kind = ReaderViewRightRow_Highlight;
    right_rows[0].actions = variants[2].flags;
    state.active_right_key = 70;
    state.popup = ReaderViewPopup_RightRowActions;
    state.right_menu_key = 70;
    state.right_menu_kind = ReaderViewRightRow_Highlight;
    state.right_menu_actions = right_rows[0].actions;
    state.restore_focus_id = right_menu_id;
    state.focus_id = right_menu_id;
    state.focus_visible = 1;
    memset(&frame_input, 0, sizeof(frame_input));
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "annotation popup changing-actions seed frame builds");
    option = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_RightActionToggleStar, 70);
    state.focus_id = option ? option->id : 0;
    state.focus_visible = 1;
    right_rows[0].actions = ReaderViewRightAction_Activate |
                            ReaderViewRightAction_Delete;
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame) &&
          state.popup == ReaderViewPopup_None &&
          state.active_right_key == 70 && state.focus_id == 0 &&
          !state.focus_visible &&
          (frame.change_flags & ReaderViewFrameChange_StateChanged) != 0 &&
          (frame.change_flags & ReaderViewFrameChange_FocusChanged) != 0 &&
          find_semantic_role(&frame, "Annotation actions",
                             ReaderViewSemantic_Menu) == 0,
          "changing annotation actions closes the stale popup without "
          "clearing the selected row or leaving stale focus");

    right_rows[0].actions = variants[2].flags;
    state.popup = ReaderViewPopup_RightRowActions;
    state.right_menu_key = 70;
    state.right_menu_kind = ReaderViewRightRow_Highlight;
    state.right_menu_actions = right_rows[0].actions;
    state.restore_focus_id = right_menu_id;
    state.focus_id = right_menu_id;
    state.focus_visible = 0;
    memset(&frame_input, 0, sizeof(frame_input));
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "pointer-style annotation popup entry builds");
    option = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_RightActionGoTo, 70);
    check(option != 0 &&
          (option->flags & ReaderViewSemantic_Focused) != 0 &&
          state.focus_id == option->id && !state.focus_visible,
          "pointer-opened annotation popup enters its first item invisibly");

    state.popup = ReaderViewPopup_None;
    state.restore_focus_id = 0;
    state.focus_id = right_menu_id;
    state.focus_visible = 1;
    memset(&frame_input, 0, sizeof(frame_input));
    frame_input.ui = ui0_input_keyboard(1, 0, 0);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame) &&
          state.popup == ReaderViewPopup_RightRowActions,
          "keyboard activation opens the annotation action popup");
    memset(&frame_input, 0, sizeof(frame_input));
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "keyboard-opened annotation popup entry frame builds");
    option = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_RightActionGoTo, 70);
    check(option != 0 && state.focus_id == option->id &&
          state.focus_visible,
          "keyboard-opened annotation popup enters its first item visibly");
    frame_input.move_vertical_delta = 1;
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "annotation popup arrow-navigation frame builds");
    option = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_RightActionToggleStar, 70);
    check(option != 0 && state.focus_id == option->id &&
          state.focus_visible,
          "annotation popup Down moves to the next kind-specific item");
    memset(&frame_input, 0, sizeof(frame_input));
    frame_input.ui = ui0_input_keyboard(1, 0, 0);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "annotation popup keyboard activation frame builds");
    action = find_action(&frame, ReaderViewAction_ToggleRightRowStar);
    check(action != 0 && action->key == 70 && frame.action_count == 1 &&
          state.popup == ReaderViewPopup_None &&
          state.focus_id == right_menu_id && state.focus_visible,
          "keyboard annotation action executes once and restores its trigger");

    for (variant_index = 0; variant_index < 2; ++variant_index)
    {
      right_rows[0].kind = variants[variant_index].kind;
      right_rows[0].actions = ReaderViewRightAction_ToggleStar;
      state.popup = ReaderViewPopup_None;
      state.restore_focus_id = 0;
      memset(&frame_input, 0, sizeof(frame_input));
      build_input.frame_index += 1;
      check(reader_view_build(&build_input, &storage, &frame),
            "inline-star-only annotation row frame builds");
      node = find_semantic_control_source(
        &frame, ReaderViewSemanticControl_RightRowStar, 70);
      option = find_semantic_control_source(
        &frame, ReaderViewSemanticControl_RightRowMenu, 70);
      check(node != 0 &&
            (node->flags & ReaderViewSemantic_Enabled) != 0 &&
            option != 0 &&
            (option->flags & ReaderViewSemantic_Enabled) == 0 &&
            reader_view_accessibility_invoke(
              &state, option ? option->id : 0),
            "Bookmark and Note retain inline star while an empty action "
            "popup trigger is disabled");
      build_input.frame_index += 1;
      check(reader_view_build(&build_input, &storage, &frame) &&
            state.popup == ReaderViewPopup_None && frame.action_count == 0,
            "disabled empty annotation menu cannot open through AT invoke");
    }

    right_rows[0].kind = ReaderViewRightRow_Bookmark;
    right_rows[0].actions = ReaderViewRightAction_None;
    state.active_right_key = 0;
    state.popup = ReaderViewPopup_None;
    state.restore_focus_id = 0;
    state.focus_id = 0;
    state.focus_visible = 0;
    memset(&frame_input, 0, sizeof(frame_input));
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "no-star disabled-menu annotation row frame builds");
    row = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_RightRow, 70);
    option = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_RightRowMenu, 70);
    row_signal = row ? find_signal_for_source(&storage, row->id) : 0;
    menu_signal = option ? find_signal_for_source(&storage, option->id) : 0;
    check(row != 0 && option != 0 && row_signal != 0 && menu_signal != 0 &&
          find_semantic_control_source(
            &frame, ReaderViewSemanticControl_RightRowStar, 70) == 0 &&
          (option->flags & ReaderViewSemantic_Enabled) == 0 &&
          row_signal->hit_rect.x + row_signal->hit_rect.w ==
            menu_signal->hit_rect.x,
          "Annotations row without Star ends its physical body at the "
          "disabled Menu target while retaining the child record");
    if (menu_signal)
      frame_input.ui = ui0_input_pointer(
        menu_signal->hit_rect.x + menu_signal->hit_rect.w / 2,
        menu_signal->hit_rect.y + menu_signal->hit_rect.h / 2,
        1, 1, 0);
    build_input.frame_index += 1;
    check(menu_signal != 0 &&
          reader_view_build(&build_input, &storage, &frame),
          "disabled annotation Menu pointer press frame builds");
    if (menu_signal)
      frame_input.ui = ui0_input_pointer(
        menu_signal->hit_rect.x + menu_signal->hit_rect.w / 2,
        menu_signal->hit_rect.y + menu_signal->hit_rect.h / 2,
        0, 0, 1);
    build_input.frame_index += 1;
    check(menu_signal != 0 &&
          reader_view_build(&build_input, &storage, &frame) &&
          frame.action_count == 0 && state.active_right_key == 0 &&
          state.popup == ReaderViewPopup_None,
          "disabled annotation Menu owns its right-edge pointer region "
          "without falling through to row selection");

    right_rows[0].kind = ReaderViewRightRow_Highlight;
    right_rows[0].actions = variants[2].flags;
    state.active_right_key = 70;
    state.popup = ReaderViewPopup_RightRowActions;
    state.right_menu_key = 70;
    state.right_menu_kind = ReaderViewRightRow_Highlight;
    state.right_menu_actions = right_rows[0].actions;
    state.restore_focus_id = right_menu_id;
    state.focus_id = right_menu_id;
    state.focus_visible = 1;
    memset(&frame_input, 0, sizeof(frame_input));
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "annotation Delete workflow popup seed builds");
    option = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_RightActionDelete, 70);
    check(option != 0 && reader_view_accessibility_invoke(
            &state, option ? option->id : 0),
          "annotation Delete workflow queues the popup action");
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "annotation Delete workflow action frame builds");
    action = find_action(&frame, ReaderViewAction_DeleteRightRow);
    check(action != 0 && action->key == 70 &&
          state.popup == ReaderViewPopup_None &&
          state.focus_id == right_menu_id && state.right_menu_key == 70,
          "annotation Delete returns once and restores the owning trigger");
    projection.right.rows = 0;
    projection.right.row_count = 0;
    projection.right.total_count = 0;
    projection.right.all_count = 0;
    projection.right.highlight_count = 0;
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame) &&
          state.right_menu_key == 0 && state.active_right_key == 0 &&
          state.focus_id == 0 && !state.focus_visible &&
          (frame.change_flags & ReaderViewFrameChange_StateChanged) != 0 &&
          (frame.change_flags & ReaderViewFrameChange_FocusChanged) != 0,
          "removing a normally deleted annotation clears its restored trigger "
          "and transient selection on the next projection");
    projection.right.rows = right_rows;
    projection.right.row_count = 1;
    projection.right.total_count = 1;
    projection.right.all_count = 1;
    projection.right.highlight_count = 1;

    state.popup = ReaderViewPopup_RightRowActions;
    state.right_menu_key = 70;
    state.right_menu_kind = ReaderViewRightRow_Highlight;
    state.right_menu_actions = right_rows[0].actions;
    state.restore_focus_id = right_menu_id;
    state.focus_id = right_menu_id;
    state.focus_visible = 1;
    memset(&frame_input, 0, sizeof(frame_input));
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "annotation popup stale-row seed frame builds");
    option = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_RightActionGoTo, 70);
    state.focus_id = option ? option->id : 0;
    state.focus_visible = 1;
    state.hot_id = option ? option->id : 0;
    state.active_id = option ? option->id : 0;
    state.pending_accessibility_focus_id = option ? option->id : 0;
    state.pending_accessibility_invoke_id = option ? option->id : 0;
    projection.right.rows = 0;
    projection.right.row_count = 0;
    projection.right.total_count = 0;
    projection.right.all_count = 0;
    projection.right.highlight_count = 0;
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame) &&
          state.popup == ReaderViewPopup_None &&
          state.restore_focus_id == 0 && state.right_menu_key == 0 &&
          state.focus_id == 0 && !state.focus_visible &&
          state.hot_id == 0 && state.active_id == 0 &&
          state.pending_accessibility_focus_id == 0 &&
          state.pending_accessibility_invoke_id == 0 &&
          (frame.change_flags & ReaderViewFrameChange_StateChanged) != 0 &&
          (frame.change_flags & ReaderViewFrameChange_FocusChanged) != 0 &&
          find_semantic_role(&frame, "Annotation actions",
                             ReaderViewSemantic_Menu) == 0,
          "removing the popup owner closes stale annotation state without an "
          "empty menu root or dangling focus");
    projection.right.rows = right_rows;
    projection.right.row_count = 1;
    projection.right.total_count = 1;
    projection.right.all_count = 1;
    projection.right.highlight_count = 1;
  }

  {
    ReaderViewRightRow bottom_rows[20];
    UI0ID top_menu_id;
    UI0ID top_option_id;
    UI0ID bottom_menu_id;
    UI0ID bottom_option_id;
    memset(bottom_rows, 0, sizeof(bottom_rows));
    for (index = 0; index < 20; ++index)
    {
      bottom_rows[index].key = (ReaderViewKey)(800 + index);
      bottom_rows[index].kind = ReaderViewRightRow_Bookmark;
      bottom_rows[index].section.data = "One";
      bottom_rows[index].section.size = 3;
      bottom_rows[index].primary.data = "Saved place";
      bottom_rows[index].primary.size = 11;
      bottom_rows[index].flags = ReaderViewRow_Enabled;
      bottom_rows[index].actions = ReaderViewRightAction_Activate |
        ReaderViewRightAction_ToggleStar | ReaderViewRightAction_Delete;
    }
    projection.right.rows = bottom_rows;
    projection.right.row_count = 20;
    projection.right.total_count = 20;
    projection.right.all_count = 20;
    projection.right.bookmark_count = 20;
    projection.right.highlight_count = 0;
    projection.right.note_count = 0;
    projection.right.available_filters = ReaderViewRightFilterFlag_All |
      ReaderViewRightFilterFlag_Bookmarks;
    state.right_filter = ReaderViewRightFilter_All;
    state.popup = ReaderViewPopup_None;
    state.right_scroll_y = 0;
    memset(&frame_input, 0, sizeof(frame_input));
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "near-bottom annotation menu seed frame builds");
    node = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_RightRowMenu, 808);
    right_menu_id = node ? node->id : 0;
    state.popup = ReaderViewPopup_RightRowActions;
    state.right_menu_key = 808;
    state.right_menu_kind = ReaderViewRightRow_Bookmark;
    state.right_menu_actions = right_rows[0].actions;
    state.restore_focus_id = right_menu_id;
    state.focus_id = right_menu_id;
    state.focus_visible = 0;
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "near-bottom annotation action popup builds");
    node = find_semantic_role(&frame, "Annotation actions",
                              ReaderViewSemantic_Menu);
    option = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_RightActionGoTo, 808);
    check(node != 0 && rect_equal(node->rect,
                                  ui0_rect(1184, 585, 186, 86)) &&
          option != 0 && rect_equal(option->rect,
                                    ui0_rect(1202, 595, 160, 32)),
          "annotation action popup flips above a near-bottom trigger and "
          "retains frozen flat-menu geometry");
    row = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_RightRowStar, 807);
    check(row != 0 && node != 0 &&
          find_draw_index_for_source(&frame, UI0DrawOp_Icon,
                                     row ? row->id : 0) >= 0 &&
          find_draw_index_for_source(&frame, UI0DrawOp_ControlFill,
                                     node ? node->id : 0) >
            find_draw_index_for_source(&frame, UI0DrawOp_Icon,
                                       row ? row->id : 0),
          "overlapping normal-row star paint precedes the popup surface in "
          "root z-order");

    state.popup = ReaderViewPopup_None;
    state.restore_focus_id = 0;
    state.right_menu_key = 0;
    state.right_menu_actions = ReaderViewRightAction_None;
    state.focus_id = 0;
    state.focus_visible = 0;
    state.right_scroll_y = 0;
    memset(&frame_input, 0, sizeof(frame_input));
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "top-clipped annotation owner seed frame builds");
    node = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_RightRowMenu, 800);
    top_menu_id = node ? node->id : 0;
    state.popup = ReaderViewPopup_RightRowActions;
    state.right_menu_key = 800;
    state.right_menu_kind = ReaderViewRightRow_Bookmark;
    state.right_menu_actions = bottom_rows[0].actions;
    state.restore_focus_id = top_menu_id;
    state.focus_id = top_menu_id;
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "top annotation owner popup seed builds");
    option = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_RightActionGoTo, 800);
    top_option_id = option ? option->id : 0;
    state.focus_id = top_option_id;
    state.focus_visible = 1;
    state.right_scroll_y = 80;
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame) &&
          state.popup == ReaderViewPopup_None &&
          state.right_menu_key == 0 && state.restore_focus_id == 0 &&
          state.focus_id == 0 && !state.focus_visible &&
          find_semantic_control_source(
            &frame, ReaderViewSemanticControl_RightRowStar, 800) == 0 &&
          find_semantic_control_source(
            &frame, ReaderViewSemanticControl_RightRowMenu, 800) == 0 &&
          find_semantic_role(&frame, "Annotation actions",
                             ReaderViewSemantic_Menu) == 0,
          "a top-partial row cannot retain a popup whose menu trigger is "
          "fully clipped");

    state.popup = ReaderViewPopup_None;
    state.restore_focus_id = 0;
    state.right_menu_key = 0;
    state.right_menu_actions = ReaderViewRightAction_None;
    state.focus_id = 0;
    state.focus_visible = 0;
    state.right_scroll_y = 200;
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "bottom-clipped annotation owner seed frame builds");
    node = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_RightRowMenu, 810);
    bottom_menu_id = node ? node->id : 0;
    state.popup = ReaderViewPopup_RightRowActions;
    state.right_menu_key = 810;
    state.right_menu_kind = ReaderViewRightRow_Bookmark;
    state.right_menu_actions = bottom_rows[10].actions;
    state.restore_focus_id = bottom_menu_id;
    state.focus_id = bottom_menu_id;
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "bottom annotation owner popup seed builds");
    option = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_RightActionGoTo, 810);
    bottom_option_id = option ? option->id : 0;
    state.focus_id = bottom_option_id;
    state.focus_visible = 1;
    state.right_scroll_y = 70;
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame) &&
          state.popup == ReaderViewPopup_None &&
          state.right_menu_key == 0 && state.restore_focus_id == 0 &&
          state.focus_id == 0 && !state.focus_visible &&
          find_semantic_control_source(
            &frame, ReaderViewSemanticControl_RightRowStar, 810) == 0 &&
          find_semantic_control_source(
            &frame, ReaderViewSemanticControl_RightRowMenu, 810) == 0 &&
          find_semantic_role(&frame, "Annotation actions",
                             ReaderViewSemantic_Menu) == 0,
          "a bottom-partial row cannot retain a popup whose menu trigger is "
          "fully clipped");
  }

  projection.right.rows = right_rows;
  projection.right.row_count = 1;
  projection.right.total_count = 1;
  projection.right.all_count = 1;
  projection.right.bookmark_count = 0;
  projection.right.highlight_count = 1;
  projection.right.note_count = 0;
  projection.right.available_filters = ReaderViewRightFilterFlag_All |
                                       ReaderViewRightFilterFlag_Bookmarks |
                                       ReaderViewRightFilterFlag_Highlights |
                                       ReaderViewRightFilterFlag_Notes;
  state.popup = ReaderViewPopup_None;
  state.restore_focus_id = 0;

  {
    static const ReaderViewRightFilter filters[4] = {
      ReaderViewRightFilter_All,
      ReaderViewRightFilter_Bookmarks,
      ReaderViewRightFilter_Highlights,
      ReaderViewRightFilter_Notes,
    };
    static const char *messages[4] = {
      "No annotations",
      "No bookmarks",
      "No highlights",
      "No notes",
    };
    projection.right.rows = 0;
    projection.right.row_count = 0;
    projection.right.total_count = 0;
    projection.right.all_count = 0;
    projection.right.bookmark_count = 0;
    projection.right.highlight_count = 0;
    projection.right.note_count = 0;
    projection.right.available_filters = ReaderViewRightFilterFlag_All |
                                         ReaderViewRightFilterFlag_Bookmarks |
                                         ReaderViewRightFilterFlag_Highlights |
                                         ReaderViewRightFilterFlag_Notes;
    state.popup = ReaderViewPopup_None;
    state.restore_focus_id = 0;
    memset(&frame_input, 0, sizeof(frame_input));
    for (index = 0; index < 4; ++index)
    {
      state.right_filter = filters[index];
      build_input.frame_index += 1;
      check(reader_view_build(&build_input, &storage, &frame),
            "filter-specific Annotations empty frame builds");
      node = find_semantic_role(&frame, messages[index],
                                ReaderViewSemantic_Status);
      command = node ? find_draw_for_source(&frame, UI0DrawOp_Text,
                                            node->id) : 0;
      check(node != 0 && command != 0 &&
            rect_equal(node->rect, ui0_rect(1078, 144, 300, 24)) &&
            rect_equal(command->rect, ui0_rect(1078, 144, 300, 24)) &&
            rect_equal(command->clip_rect, ui0_rect(1078, 144, 300, 24)) &&
            command->color == theme->colors[UI0ColorRole_TextMuted] &&
            command->has_text_alignment &&
            command->text_align_x == UI0TextAlignX_Start &&
            command->text_align_y == UI0TextAlignY_Center &&
            command->has_typography_role &&
            command->typography_role == UI0TypographyRole_Body &&
            command->typography_char_width == 8 &&
            command->typography_line_height == 16 &&
            count_semantic(&frame, "Nothing here") == 0,
            "All/Bookmarks/Highlights/Notes empty copy uses the exact "
            "frozen position and muted body style");
    }
    projection.labels.no_notes.data = "Localized no notes";
    projection.labels.no_notes.size = 18;
    state.right_filter = ReaderViewRightFilter_Notes;
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame) &&
          find_semantic_role(&frame, "Localized no notes",
                             ReaderViewSemantic_Status) != 0,
          "Annotations empty copy remains an explicit caller-localized label");
    projection.labels.no_notes.data = 0;
    projection.labels.no_notes.size = 0;
  }

  memset(right_rows, 0, sizeof(right_rows));
  right_rows[0].key = 70;
  right_rows[0].kind = ReaderViewRightRow_Highlight;
  right_rows[0].primary.data = "Highlight";
  right_rows[0].primary.size = 9;
  right_rows[0].color_key = 5000;
  right_rows[0].rail_color = 0xff2468acu;
  right_rows[0].flags = ReaderViewRow_Enabled;
  right_rows[1].key = 71;
  right_rows[1].kind = ReaderViewRightRow_Note;
  right_rows[1].primary.data = "Note";
  right_rows[1].primary.size = 4;
  right_rows[1].color_key = 5000;
  right_rows[1].rail_color = 0xff2468acu;
  right_rows[1].flags = ReaderViewRow_Enabled |
                        ReaderViewRow_AttachedToPrevious;
  projection.right.rows = right_rows;
  projection.right.row_count = 2;
  projection.right.total_count = 2;
  projection.right.available_filters = ReaderViewRightFilterFlag_All |
                                       ReaderViewRightFilterFlag_Bookmarks |
                                       ReaderViewRightFilterFlag_Highlights |
                                       ReaderViewRightFilterFlag_Notes;
  state.right_filter = ReaderViewRightFilter_All;
  memset(&frame_input, 0, sizeof(frame_input));
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "valid attached annotation projection builds");
  right_rows[1].rail_color = 0;
  build_input.frame_index += 1;
  check(!reader_view_build(&build_input, &storage, &frame) &&
        (frame.error_flags & ReaderViewFrameError_InvalidAttachment) != 0,
        "attached note requires a nonzero resolved rail color");
  right_rows[1].rail_color = 0xff13579bu;
  build_input.frame_index += 1;
  check(!reader_view_build(&build_input, &storage, &frame) &&
        (frame.error_flags & ReaderViewFrameError_InvalidAttachment) != 0,
        "attached highlight and note require one continuous resolved rail");
  right_rows[1].rail_color = 0xff2468acu;
  right_rows[0].color_key = 0;
  build_input.frame_index += 1;
  check(!reader_view_build(&build_input, &storage, &frame) &&
        (frame.error_flags & ReaderViewFrameError_InvalidAttachment) != 0,
        "invalid attached annotation projection fails deterministically");
  right_rows[0].color_key = 5000;
  right_rows[0].section.data = 0;
  right_rows[0].section.size = 5;
  right_rows[1].section.data = 0;
  right_rows[1].section.size = 5;
  build_input.frame_index += 1;
  check(!reader_view_build(&build_input, &storage, &frame) &&
        (frame.error_flags & ReaderViewFrameError_InvalidText) != 0 &&
        (frame.error_flags & ReaderViewFrameError_InvalidAttachment) != 0,
        "invalid attached section text fails closed without comparing null "
        "storage");
  right_rows[0].section.data = "Chapter One";
  right_rows[0].section.size = 11;
  right_rows[1].section.data = "Chapter Two";
  right_rows[1].section.size = 11;
  build_input.frame_index += 1;
  check(!reader_view_build(&build_input, &storage, &frame) &&
        (frame.error_flags & ReaderViewFrameError_InvalidAttachment) != 0,
        "attached note cannot insert a different section between its "
        "highlight pair");
  right_rows[0].section.data = 0;
  right_rows[0].section.size = 0;
  right_rows[1].section.data = 0;
  right_rows[1].section.size = 0;
  right_rows[1].flags = ReaderViewRow_Enabled | (1u << 30);
  build_input.frame_index += 1;
  check(!reader_view_build(&build_input, &storage, &frame) &&
        (frame.error_flags & ReaderViewFrameError_BadInput) != 0,
        "unknown projected row bits fail deterministically");
  right_rows[1].flags = ReaderViewRow_Enabled |
                        ReaderViewRow_AttachedToPrevious;
  right_rows[1].actions = 1u << 30;
  build_input.frame_index += 1;
  check(!reader_view_build(&build_input, &storage, &frame) &&
        (frame.error_flags & ReaderViewFrameError_BadInput) != 0,
        "unknown projected right-action bits fail deterministically");
  right_rows[1].actions = ReaderViewRightAction_None;
  right_rows[0].actions = ReaderViewRightAction_EditNote;
  build_input.frame_index += 1;
  check(!reader_view_build(&build_input, &storage, &frame) &&
        (frame.error_flags & ReaderViewFrameError_BadInput) != 0,
        "non-Note rows cannot project the Note-only popup action");
  right_rows[0].actions = ReaderViewRightAction_None;

  projection = full_projection(settings, setting_choices, toc_rows,
                               find_rows, right_rows);
  projection.document_flags &= ~ReaderViewDocument_CanGoPreviousPage;
  reader_view_state_reset_document(&state, projection.document_key);
  layout_input.features = projection.features;
  layout_input.document_flags = projection.document_flags;
  check(reader_view_resolve_layout(&state, &layout_input, &layout),
        "disabled Previous layout resolves");
  memset(&frame_input, 0, sizeof(frame_input));
  build_input.projection = &projection;
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "disabled Previous base frame builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_PreviousPage, 0);
  check(node != 0 &&
        (node->flags & ReaderViewSemantic_Enabled) == 0 &&
        (node->flags & ReaderViewSemantic_Focusable) != 0,
        "disabled Previous remains non-enabled but focusable");
  previous_id = node ? node->id : 0;
  if (node)
    check(reader_view_accessibility_focus(&state, node->id),
          "disabled Previous focus queues");
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "disabled Previous focused frame builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_PreviousPage, 0);
  icon = find_icon_for_source(&frame, previous_id);
  command = find_draw_for_source(&frame, UI0DrawOp_FocusRing,
                                 previous_id);
  check(node != 0 && (node->flags & ReaderViewSemantic_Focused) != 0 &&
        icon != 0 && icon->icon_kind == UI0IconKind_PageCaretLeft &&
        rect_equal(icon->rect,
                   ui0_rect(layout.previous_gutter_visual_rect.x + 13,
                            layout.previous_gutter_visual_rect.y + 28,
                            18, 32)) &&
        icon->color == theme->colors[UI0ColorRole_TextMuted] &&
        icon->stroke_color == theme->colors[UI0ColorRole_Surface] &&
        count_draw_op_for_source(&frame, UI0DrawOp_FocusRing,
                                 previous_id) == 1 &&
        command != 0 &&
        rect_equal(command->rect,
                   ui0_rect(layout.previous_gutter_visual_rect.x - 2,
                            layout.previous_gutter_visual_rect.y - 2,
                            48, 92)) &&
        rect_equal(command->clip_rect, layout.bounds) &&
        command->corner_radius == 4 && command->stroke_width >= 1 &&
        find_action(&frame, ReaderViewAction_PreviousPage) == 0,
        "disabled focused Previous paints the exact caret/focus record "
        "without invoking");
  check(reader_view_accessibility_invoke(&state, previous_id),
        "disabled Previous invoke request queues");
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame) &&
        find_action(&frame, ReaderViewAction_PreviousPage) == 0,
        "disabled Previous accessibility invoke stays non-invokable");
  state.focus_id = 0;
  state.focus_visible = 0;
  frame_input.ui = ui0_input_pointer(
    layout.previous_gutter_rect.x + layout.previous_gutter_rect.w / 2,
    layout.previous_gutter_rect.y + layout.previous_gutter_rect.h / 2,
    0, 0, 0);
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame) &&
        find_icon_for_source(&frame, previous_id) != 0 &&
        find_action(&frame, ReaderViewAction_PreviousPage) == 0,
        "disabled hovered Previous paints caret without invoking");
  frame_input.ui = ui0_input_pointer(
    layout.previous_gutter_rect.x + layout.previous_gutter_rect.w / 2,
    layout.previous_gutter_rect.y + layout.previous_gutter_rect.h / 2,
    1, 1, 0);
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame) &&
        state.focus_id == previous_id && !state.focus_visible &&
        find_icon_for_source(&frame, previous_id) != 0 &&
        find_icon_for_source(&frame, previous_id)->color ==
          theme->colors[UI0ColorRole_TextSecondary] &&
        find_action(&frame, ReaderViewAction_PreviousPage) == 0,
        "disabled Previous pointer press keeps invisible focus and uses the "
        "active caret color without invoking");
  frame_input.ui = ui0_input_pointer(layout.page_surface_rect.x + 20,
                                     layout.page_surface_rect.y + 20,
                                     0, 0, 1);
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame) &&
        state.focus_id == previous_id && !state.focus_visible &&
        find_action(&frame, ReaderViewAction_PreviousPage) == 0,
        "disabled Previous retains invisible focus when release occurs "
        "outside the gutter");
  frame_input.ui = ui0_input_pointer(
    layout.next_gutter_rect.x + layout.next_gutter_rect.w / 2,
    layout.next_gutter_rect.y + layout.next_gutter_rect.h / 2,
    0, 0, 0);
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "Next gutter exact-caret hover frame builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_NextPage, 0);
  icon = node ? find_icon_for_source(&frame, node->id) : 0;
  check(icon != 0 && icon->icon_kind == UI0IconKind_PageCaretRight &&
        rect_equal(icon->rect,
                   ui0_rect(layout.next_gutter_visual_rect.x + 13,
                            layout.next_gutter_visual_rect.y + 28,
                            18, 32)) &&
        icon->color == theme->colors[UI0ColorRole_TextMuted] &&
        icon->stroke_color == theme->colors[UI0ColorRole_Surface],
        "Next gutter paints the exact mirrored PageCaret record");
}

static UI0B32
lifecycle_build(ReaderViewState *state,
                ReaderViewLayoutInput *layout_input,
                ReaderViewLayout *layout,
                ReaderViewProjection *projection,
                ReaderViewInput *input,
                const UI0ResolvedTheme *theme,
                ReaderViewCodepointAdvance *advances,
                ReaderViewFrameStorage *storage,
                ReaderViewFrame *frame,
                UI0U64 frame_index)
{
  ReaderViewBuildInput build_input;
  ReaderViewCodepointAdvance note_advances[127];
  layout_input->features = projection->features;
  layout_input->document_flags = projection->document_flags;
  if (!reader_view_resolve_layout(state, layout_input, layout)) return 0;
  memset(&build_input, 0, sizeof(build_input));
  build_input.frame_index = frame_index;
  build_input.state = state;
  build_input.layout = layout;
  build_input.projection = projection;
  build_input.input = input;
  build_input.theme = theme;
  build_input.find_text_metrics = test_find_text_metrics(advances);
  build_input.note_text_metrics = test_note_text_metrics(note_advances);
  return reader_view_build(&build_input, storage, frame);
}

static void
test_focus_root_and_refresh_lifecycle(const UI0ResolvedTheme *theme)
{
  static ReaderViewState state;
  static ReaderViewFrameStorage storage;
  ReaderViewSettingControl settings[READER_VIEW_SETTING_CAP];
  ReaderViewChoice choices[8];
  ReaderViewTocRow toc_rows[2];
  ReaderViewFindRow find_rows[1];
  ReaderViewRightRow right_rows[1];
  ReaderViewCodepointAdvance advances[127];
  ReaderViewProjection projection = full_projection(
    settings, choices, toc_rows, find_rows, right_rows);
  ReaderViewLayoutInput layout_input;
  ReaderViewLayout layout;
  ReaderViewInput input;
  ReaderViewFrame frame;
  const ReaderViewSemanticNode *node;
  const UI0ControlRecord *control;
  const UI0SidenavRecord *sidenav;
  UI0ID popup_focus;
  UI0ID progress_id;
  UI0U64 frame_index = 1;

  memset(&layout_input, 0, sizeof(layout_input));
  layout_input.bounds = ui0_rect(0, 0, 1400, 800);
  memset(&input, 0, sizeof(input));
  reader_view_state_reset_document(&state, projection.document_key);
  state.left_panel = ReaderViewLeftPanel_Contents;
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "refresh lifecycle Contents seed builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_TocRow, 20);
  check(node && reader_view_accessibility_focus(&state, node->id),
        "refresh lifecycle queues Contents row focus");
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "refresh lifecycle Contents focus builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_TocRow, 20);
  control = node ? find_control_for_source(&storage, node->id) : 0;
  sidenav = node ? find_reference_sidenav_record(&storage, node->id) : 0;
  check(node && (node->flags & ReaderViewSemantic_Focused) != 0 &&
        control && (control->state & UI0ControlState_Focused) != 0 &&
        sidenav && (sidenav->state & UI0SidenavState_Focused) != 0,
        "AT focus updates Contents semantic, control, and Sidenav records in one frame");
  input.move_vertical_delta = 1;
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "refresh lifecycle Contents arrow builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_TocRow, 21);
  control = node ? find_control_for_source(&storage, node->id) : 0;
  sidenav = node ? find_reference_sidenav_record(&storage, node->id) : 0;
  check(node && (node->flags & ReaderViewSemantic_Focused) != 0 &&
        control && (control->state & UI0ControlState_Focused) != 0 &&
        sidenav && (sidenav->state & UI0SidenavState_Focused) != 0,
        "arrow navigation repaints Contents focus in the same frame");
  memset(&input, 0, sizeof(input));
  state.active_toc_key = 21;
  projection.toc.status.state = ReaderViewLoad_Loading;
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "retained-row Loading Contents build");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_LeftContentsTab, 0);
  check(node && (node->flags & ReaderViewSemantic_Focused) != 0 &&
        state.active_toc_key == 0 && storage.scroll_records[0].id == 0 &&
        (frame.change_flags & ReaderViewFrameChange_StateChanged) != 0 &&
        (frame.change_flags & ReaderViewFrameChange_FocusChanged) != 0,
        "Loading Contents retires rows, rehomes focus, and publishes no stale scroll root");

  projection.toc.status = ready_status();
  state.left_panel = ReaderViewLeftPanel_Find;
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "refresh lifecycle Find seed builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_FindRow, 30);
  check(node && reader_view_accessibility_focus(&state, node->id) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "refresh lifecycle Find row focus builds");
  state.active_find_key = 30;
  projection.find.status.state = ReaderViewLoad_Empty;
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "retained-row empty Find build");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_FindInput, 0);
  check(node && (node->flags & ReaderViewSemantic_Focused) != 0 &&
        state.active_find_key == 0 &&
        find_semantic_control_source(
          &frame, ReaderViewSemanticControl_FindRow, 30) == 0,
        "empty Find retires row state and rehomes focus to the input");

  projection.find.status = ready_status();
  state.left_panel = ReaderViewLeftPanel_None;
  state.right_panel_open = 1;
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "refresh lifecycle Annotations seed builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightRow, 40);
  check(node && reader_view_accessibility_focus(&state, node->id) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "refresh lifecycle Annotations row focus builds");
  state.active_right_key = 40;
  projection.right.status.state = ReaderViewLoad_Loading;
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "retained-row Loading Annotations build");
  check(state.focus_id == 0 && state.active_right_key == 0 &&
        find_semantic_control_source(
          &frame, ReaderViewSemanticControl_RightRow, 40) == 0,
        "Loading Annotations retires vanished row interaction state");

  projection.right.status = ready_status();
  state.left_panel = ReaderViewLeftPanel_Find;
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "AT-only popup seed builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightFilter, 0);
  check(node && reader_view_accessibility_invoke(&state, node->id) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "AT-only filter trigger opens");
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "AT-only filter popup contained-focus build");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightFilterOption,
    (ReaderViewKey)ReaderViewRightFilter_All);
  popup_focus = node ? node->id : 0;
  check(node && (node->flags & ReaderViewSemantic_Focused) != 0 &&
        state.focus_id == popup_focus && state.focus_visible,
        "AT-only filter invocation enters a visible contained option");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_FindInput, 0);
  check(node && reader_view_accessibility_focus(&state, node->id) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "blocked Find accessibility request is consumed");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_FindInput, 0);
  check(state.focus_id == popup_focus &&
        state.pending_accessibility_focus_id == 0 && node &&
        (node->flags & (ReaderViewSemantic_Focusable |
                        ReaderViewSemantic_Focused)) == 0,
        "popup blocks Find input focus and semantics without deferred replay");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_Progress, 0);
  progress_id = node ? node->id : 0;
  check(node && reader_view_accessibility_focus(&state, progress_id) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "blocked progress accessibility request is consumed");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_Progress, 0);
  check(state.focus_id == popup_focus && node &&
        (node->flags & (ReaderViewSemantic_Focusable |
                        ReaderViewSemantic_Focused)) == 0,
        "popup blocks progress focus and background slider semantics");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_PreviousPage, 0);
  check(node && (node->flags & (ReaderViewSemantic_Focusable |
                                ReaderViewSemantic_Focused)) == 0,
        "popup blocks side-gutter focus semantics");
  input.escape_pressed = 1;
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "filter Escape close build");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightFilter, 0);
  check(state.popup == ReaderViewPopup_None && node &&
        (node->flags & (ReaderViewSemantic_Enabled |
                        ReaderViewSemantic_Focusable |
                        ReaderViewSemantic_Focused)) ==
          (ReaderViewSemantic_Enabled | ReaderViewSemantic_Focusable |
           ReaderViewSemantic_Focused) &&
        (frame.change_flags & ReaderViewFrameChange_FocusChanged) != 0,
        "popup Escape restores a same-frame focusable trigger");

  memset(&input, 0, sizeof(input));
  reader_view_state_reset_document(&state, projection.document_key);
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "setting AT-only seed builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_FontFamily, 0);
  check(node && reader_view_accessibility_invoke(&state, node->id) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "setting AT-only popup opens with contained focus");
  node = find_semantic_role(&frame, "Serif", ReaderViewSemantic_MenuItem);
  check(node && (node->flags & ReaderViewSemantic_Focused) != 0 &&
        reader_view_accessibility_invoke(&state, node->id) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "setting choice invocation closes through shared restoration");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_FontFamily, 0);
  check(state.popup == ReaderViewPopup_None && state.restore_focus_id == 0 &&
        node && (node->flags & ReaderViewSemantic_Focused) != 0,
        "setting choice restores its exact trigger in the close frame");
}

static void
test_open_panel_focus_boundaries(const UI0ResolvedTheme *theme)
{
  typedef struct PanelFocusCase
  {
    ReaderViewLeftPanelMode left_panel;
    UI0B32 right_panel_open;
    ReaderViewSemanticControl panel_entry;
  } PanelFocusCase;
  static const PanelFocusCase cases[] = {
    { ReaderViewLeftPanel_Contents, 0,
      ReaderViewSemanticControl_LeftContentsTab },
    { ReaderViewLeftPanel_Find, 0,
      ReaderViewSemanticControl_LeftContentsTab },
    { ReaderViewLeftPanel_None, 1,
      ReaderViewSemanticControl_RightFilter },
  };
  static ReaderViewState state;
  static ReaderViewFrameStorage storage;
  ReaderViewSettingControl settings[READER_VIEW_SETTING_CAP];
  ReaderViewChoice choices[8];
  ReaderViewTocRow toc_rows[2];
  ReaderViewFindRow find_rows[1];
  ReaderViewRightRow right_rows[1];
  ReaderViewCodepointAdvance advances[127];
  ReaderViewProjection projection = full_projection(
    settings, choices, toc_rows, find_rows, right_rows);
  ReaderViewLayoutInput layout_input;
  ReaderViewLayout layout;
  ReaderViewInput input;
  ReaderViewFrame frame;
  UI0U64 frame_index = 800;
  UI0S32 index;

  memset(&layout_input, 0, sizeof(layout_input));
  layout_input.bounds = ui0_rect(0, 0, 1400, 800);
  for (index = 0;
       index < (UI0S32)(sizeof(cases) / sizeof(cases[0]));
       ++index)
  {
    const ReaderViewSemanticNode *node;
    UI0ID previous_id;
    UI0ID next_id;
    UI0ID progress_id;
    UI0ID panel_entry_id;

    memset(&input, 0, sizeof(input));
    reader_view_state_reset_document(&state, projection.document_key);
    state.left_panel = cases[index].left_panel;
    state.right_panel_open = cases[index].right_panel_open;
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++),
          "open-panel focus boundary seed builds");
    node = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_PreviousPage, 0);
    previous_id = node ? node->id : 0;
    node = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_NextPage, 0);
    next_id = node ? node->id : 0;
    node = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_Progress, 0);
    progress_id = node ? node->id : 0;
    node = find_semantic_control_source(
      &frame, cases[index].panel_entry, 0);
    panel_entry_id = node ? node->id : 0;
    check(previous_id != 0 && next_id != 0 && progress_id != 0 &&
          panel_entry_id != 0,
          "open-panel focus boundary publishes gutters, progress, and panel entry");

    check(reader_view_accessibility_focus(&state, previous_id) &&
          lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++),
          "open-panel Previous focus builds");
    check(state.focus_id == previous_id && state.focus_visible,
          "open-panel Previous starts the gutter boundary visibly focused");

    input.ui = ui0_input_keyboard(0, 1, 0);
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++),
          "open-panel Tab to Next builds");
    check(state.focus_id == next_id && state.focus_visible,
          "open-panel Tab retains Previous, Next ordering");

    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++),
          "open-panel Tab to Progress builds");
    check(state.focus_id == progress_id && state.focus_visible,
          "open-panel Tab retains Next, Progress ordering");

    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++),
          "open-panel Tab into panel builds");
    node = find_semantic_control_source(
      &frame, cases[index].panel_entry, 0);
    check(node && state.focus_id == panel_entry_id && state.focus_visible &&
          (node->flags & ReaderViewSemantic_Focused) != 0,
          "open-panel Tab enters the adjacent panel record with visible focus");

    input.ui = ui0_input_keyboard(0, 0, 1);
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++),
          "open-panel reverse Tab to Progress builds");
    node = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_Progress, 0);
    check(node && state.focus_id == progress_id && state.focus_visible &&
          (node->flags & ReaderViewSemantic_Focused) != 0,
          "open-panel reverse Tab leaves the panel for Progress visibly focused");
  }
}

static void
test_frozen_note_editor_composition(const UI0ResolvedTheme *theme)
{
  static ReaderViewState state;
  static ReaderViewFrameStorage storage;
  ReaderViewSettingControl settings[READER_VIEW_SETTING_CAP];
  ReaderViewChoice choices[8];
  ReaderViewTocRow toc_rows[2];
  ReaderViewFindRow find_rows[1];
  ReaderViewRightRow right_rows[1];
  ReaderViewCodepointAdvance advances[127];
  ReaderViewProjection projection = full_projection(
    settings, choices, toc_rows, find_rows, right_rows);
  ReaderViewLayoutInput layout_input;
  ReaderViewLayout layout;
  ReaderViewInput input;
  ReaderViewFrame frame;
  const ReaderViewSemanticNode *dialog;
  const ReaderViewSemanticNode *editor;
  const ReaderViewSemanticNode *delete_note;
  const ReaderViewSemanticNode *cancel_note;
  const ReaderViewSemanticNode *save_note;
  const ReaderViewTextBinding *binding;
  const UI0ControlRecord *control;
  const UI0TextAreaRecord *text_area;
  const UI0TextAreaRowRecord *text_row;
  const UI0DrawCommand *row_draw;
  const UI0DrawCommand *caret_draw;
  const UI0DrawCommand *corner_draws;
  UI0Rect corner_rects[4];
  UI0ID editor_id;
  UI0B32 corner_masks_exact;
  UI0S32 index;
  UI0U64 frame_index = 901;

  memset(&layout_input, 0, sizeof(layout_input));
  layout_input.bounds = ui0_rect(0, 0, 1400, 780);
  memset(&layout, 0, sizeof(layout));
  memset(&input, 0, sizeof(input));
  reader_view_state_reset_document(&state, projection.document_key);
  state.right_panel_open = 1;
  memset(&projection.selection, 0, sizeof(projection.selection));
  projection.selection.status = ready_status();
  projection.selection.selection_key = 5050;
  projection.selection.revision = 12;
  projection.selection.flags = ReaderViewSelection_Active |
    ReaderViewSelection_CanEditNote |
    ReaderViewSelection_CanDeleteNote;
  projection.selection.note_text.data = "Attached parity note";
  projection.selection.note_text.size = 20;

  check(reader_view_open_note_editor(&state, &projection.selection) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "frozen anchored note editor builds");
  check(rect_equal(layout.content_rect, ui0_rect(255, 124, 556, 550)),
        "right-panel reader content keeps the frozen note anchor geometry");

  dialog = find_semantic_role(&frame, "Note", ReaderViewSemantic_Dialog);
  editor = find_semantic_role(&frame, "Note text",
                              ReaderViewSemantic_TextArea);
  editor_id = editor ? editor->id : 0;
  delete_note = find_semantic(&frame, "Delete note");
  cancel_note = find_semantic(&frame, "Cancel note");
  save_note = find_semantic(&frame, "Save note");
  check(dialog && rect_equal(dialog->rect, ui0_rect(303, 117, 520, 360)) &&
        editor && rect_equal(editor->rect, ui0_rect(317, 167, 492, 248)) &&
        text_equal(editor->value, "Attached parity note") &&
        (editor->flags & (ReaderViewSemantic_Enabled |
                          ReaderViewSemantic_Focusable |
                          ReaderViewSemantic_Focused)) ==
          (ReaderViewSemantic_Enabled | ReaderViewSemantic_Focusable |
           ReaderViewSemantic_Focused),
        "note modal and real multiline text area restore frozen anchored "
        "geometry and native value semantics");

  text_area = storage.text_area_records;
  text_row = text_area->row_count > 0 ?
    storage.note_text_area_row_records + text_area->row_start : 0;
  row_draw = text_row ?
    find_draw_for_rect(&frame, UI0DrawOp_Text, text_row->rect) : 0;
  caret_draw = editor ? find_draw_for_source(
    &frame, UI0DrawOp_TextCaret, editor->id) : 0;
  binding = row_draw ? find_text_binding(&frame, row_draw->source_id) : 0;
  check(text_area->id == (editor ? editor->id : 0) &&
        rect_equal(text_area->rect, ui0_rect(317, 167, 492, 248)) &&
        rect_equal(text_area->text_rect, ui0_rect(325, 180, 476, 228)) &&
        (text_area->state & (UI0TextAreaState_Focused |
                             UI0TextAreaState_FocusVisible)) ==
          (UI0TextAreaState_Focused | UI0TextAreaState_FocusVisible) &&
        text_area->row_count == 1 && text_row &&
        rect_equal(text_row->rect, ui0_rect(325, 180, 164, 25)) &&
        row_draw && binding && text_equal(binding->text,
                                          "Attached parity note") &&
        binding->style == ReaderViewTextStyle_NoteEditor &&
        row_draw->has_typography_role &&
        row_draw->typography_role == UI0TypographyRole_Body &&
        row_draw->typography_char_width == 10 &&
        row_draw->typography_line_height == 18 &&
        caret_draw &&
        rect_equal(caret_draw->rect, ui0_rect(489, 181, 1, 23)) &&
        rect_equal(caret_draw->clip_rect,
                   ui0_rect(325, 180, 476, 228)) &&
        caret_draw->color == theme->colors[UI0ColorRole_Focus] &&
        count_draw_op_for_source(&frame, UI0DrawOp_ControlFill,
                                 editor ? editor->id : 0) == 5,
        "note body restores proportional 18px system-UI text, 25px rows, "
        "and the frozen 23px caret through explicit caller metadata without "
        "the Find-only full-field clip patch");
  corner_rects[0] = ui0_rect(317, 167, 1, 1);
  corner_rects[1] = ui0_rect(808, 167, 1, 1);
  corner_rects[2] = ui0_rect(317, 414, 1, 1);
  corner_rects[3] = ui0_rect(808, 414, 1, 1);
  corner_draws = frame.draw_command_count >= 4 ?
    frame.draw_commands + frame.draw_command_count - 4 : 0;
  corner_masks_exact = corner_draws != 0;
  for (index = 0; index < 4 && corner_masks_exact; ++index)
  {
    const UI0DrawCommand *corner = corner_draws + index;
    corner_masks_exact =
      corner->op == UI0DrawOp_ControlFill &&
      corner->source_id == editor_id &&
      corner->source_kind == UI0ControlKind_TextArea &&
      rect_equal(corner->rect, corner_rects[index]) &&
      rect_equal(corner->clip_rect, text_area->clip_rect) &&
      corner->color == theme->colors[UI0ColorRole_SurfaceElevated] &&
      corner->stroke_color == corner->color &&
      (corner->flags & UI0DrawFlag_RadiusExplicit) != 0 &&
      corner->corner_radius == 0;
  }
  check(corner_masks_exact,
        "note TextArea appends exactly four ordered 1px SurfaceElevated "
        "corner masks after every border/focus/text draw");

  check(delete_note &&
        rect_equal(delete_note->rect, ui0_rect(317, 431, 74, 30)) &&
        cancel_note &&
        rect_equal(cancel_note->rect, ui0_rect(675, 431, 62, 30)) &&
        save_note && rect_equal(save_note->rect,
                                ui0_rect(747, 431, 62, 30)),
        "note Delete, Close, and Save targets restore frozen composition");
  control = delete_note ? find_control_for_source(&storage,
                                                   delete_note->id) : 0;
  binding = delete_note ? find_text_binding(&frame, delete_note->id) : 0;
  check(control && (control->control_flags & UI0Control_Destructive) != 0 &&
        binding && text_equal(binding->text, "Delete"),
        "note Delete keeps its destructive native name and frozen copy");
  control = cancel_note ? find_control_for_source(&storage,
                                                   cancel_note->id) : 0;
  binding = cancel_note ? find_text_binding(&frame, cancel_note->id) : 0;
  check(control && (control->control_flags & UI0Control_Quiet) != 0 &&
        binding && text_equal(binding->text, "Close"),
        "editing note Cancel action keeps the frozen quiet Close copy");
  control = save_note ? find_control_for_source(&storage, save_note->id) : 0;
  binding = save_note ? find_text_binding(&frame, save_note->id) : 0;
  check(control && (control->control_flags & UI0Control_Primary) != 0 &&
        (control->control_flags & UI0Control_Disabled) == 0 &&
        binding && text_equal(binding->text, "Save"),
        "unchanged matching note keeps the frozen enabled primary Save");

  input.ui = ui0_input_keyboard(0, 1, 0);
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        delete_note && state.focus_id == delete_note->id &&
        (storage.text_area_records[0].state &
         (UI0TextAreaState_Focused | UI0TextAreaState_FocusVisible)) == 0,
        "note modal Tab order moves from text to Delete");
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        cancel_note && state.focus_id == cancel_note->id,
        "note modal Tab order moves from Delete to Cancel");
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        save_note && state.focus_id == save_note->id,
        "note modal Tab order moves from Cancel to Save");
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        editor && state.focus_id == editor->id &&
        (storage.text_area_records[0].state &
         (UI0TextAreaState_Focused | UI0TextAreaState_FocusVisible)) ==
          (UI0TextAreaState_Focused | UI0TextAreaState_FocusVisible),
        "note modal focus wraps from Save to the real text area");

  check(reader_view_close_note_editor(&state) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "frozen editing note closes before Add Note coverage");
  check(find_semantic_role(&frame, "Note text",
                           ReaderViewSemantic_TextArea) == 0 &&
        count_draw_op_for_source(&frame, UI0DrawOp_ControlFill,
                                 editor_id) == 0,
        "note corner seam masks retire with the NoteEditor TextArea and do "
        "not leak into the ordinary reader frame");
  memset(&input, 0, sizeof(input));
  projection.selection.selection_key = 5051;
  projection.selection.revision = 13;
  projection.selection.flags = ReaderViewSelection_Active |
    ReaderViewSelection_CanAddNote;
  projection.selection.note_text.data = 0;
  projection.selection.note_text.size = 0;
  check(reader_view_open_note_editor(&state, &projection.selection) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "frozen Add Note composition builds");
  dialog = find_semantic_role(&frame, "Add Note", ReaderViewSemantic_Dialog);
  editor = find_semantic_role(&frame, "Note text",
                              ReaderViewSemantic_TextArea);
  cancel_note = find_semantic(&frame, "Cancel note");
  binding = cancel_note ? find_text_binding(&frame, cancel_note->id) : 0;
  check(dialog && rect_equal(dialog->rect, ui0_rect(303, 117, 520, 360)) &&
        find_semantic(&frame, "Delete note") == 0 &&
        cancel_note && binding && text_equal(binding->text, "Cancel"),
        "Add Note omits Delete and uses the frozen Cancel copy");
  text_area = storage.text_area_records;
  row_draw = editor ? find_draw_for_source(&frame, UI0DrawOp_Text,
                                            editor->id) : 0;
  binding = editor ? find_text_binding(&frame, editor->id) : 0;
  check(editor && text_area->id == editor->id &&
        (text_area->state & UI0TextAreaState_PlaceholderVisible) != 0 &&
        rect_equal(text_area->placeholder_rect,
                   ui0_rect(325, 174, 476, 234)) &&
        row_draw && rect_equal(row_draw->rect,
                               text_area->placeholder_rect) &&
        row_draw->has_typography_role &&
        row_draw->typography_role == UI0TypographyRole_Body &&
        row_draw->typography_char_width == 10 &&
        row_draw->typography_line_height == 18 &&
        binding && binding->style == ReaderViewTextStyle_NoteEditor &&
        text_equal(binding->text, "Type a note"),
        "focused empty note preserves the frozen placeholder box and "
        "explicit NoteEditor typography carrier");
}

static void
test_note_metric_contract(const UI0ResolvedTheme *theme)
{
  static ReaderViewState state;
  static ReaderViewFrameStorage storage;
  ReaderViewSettingControl settings[READER_VIEW_SETTING_CAP];
  ReaderViewChoice choices[8];
  ReaderViewTocRow toc_rows[2];
  ReaderViewFindRow find_rows[1];
  ReaderViewRightRow right_rows[1];
  ReaderViewCodepointAdvance find_advances[127];
  ReaderViewCodepointAdvance note_advances[127];
  ReaderViewCodepointAdvance over_cap[READER_VIEW_NOTE_CODEPOINT_ADVANCE_CAP + 1];
  ReaderViewProjection projection = full_projection(
    settings, choices, toc_rows, find_rows, right_rows);
  ReaderViewLayoutInput layout_input;
  ReaderViewLayout layout;
  ReaderViewInput input;
  ReaderViewBuildInput build_input;
  ReaderViewFrame frame;
  ReaderViewNoteTextMetrics valid_metrics;
  const UI0TextAreaRecord *record;
  const UI0DrawCommand *caret;
  const ReaderViewSemanticNode *editor;
  UI0S32 index;

  memset(&layout_input, 0, sizeof(layout_input));
  layout_input.bounds = ui0_rect(0, 0, 1400, 780);
  layout_input.features = projection.features;
  layout_input.document_flags = projection.document_flags;
  memset(&input, 0, sizeof(input));
  reader_view_state_reset_document(&state, projection.document_key);
  check(reader_view_resolve_layout(&state, &layout_input, &layout),
        "note metric contract layout resolves");
  memset(&build_input, 0, sizeof(build_input));
  build_input.frame_index = 1021;
  build_input.state = &state;
  build_input.layout = &layout;
  build_input.projection = &projection;
  build_input.input = &input;
  build_input.theme = theme;
  build_input.find_text_metrics = test_find_text_metrics(find_advances);
  check(reader_view_build(&build_input, &storage, &frame),
        "closed note editor requires no note metric carrier");

  projection.selection.status = ready_status();
  projection.selection.selection_key = 7070;
  projection.selection.revision = 31;
  projection.selection.flags = ReaderViewSelection_Active |
    ReaderViewSelection_CanEditNote;
  projection.selection.note_text.data = "Metric note";
  projection.selection.note_text.size = 11;
  check(reader_view_open_note_editor(&state, &projection.selection),
        "note metric validation editor opens");
  build_input.frame_index += 1;
  check(!reader_view_build(&build_input, &storage, &frame) &&
        (frame.error_flags & ReaderViewFrameError_BadInput) != 0,
        "open note editor fails closed without caller note metrics");

  valid_metrics = test_note_text_metrics(note_advances);
  build_input.note_text_metrics = valid_metrics;
  check(reader_view_build(&build_input, &storage, &frame),
        "valid bounded note metrics build");
  note_advances[1].codepoint = note_advances[0].codepoint;
  check(!reader_view_build(&build_input, &storage, &frame) &&
        (frame.error_flags & ReaderViewFrameError_BadInput) != 0,
        "note metrics reject duplicate Unicode scalars");
  note_advances[1].codepoint = 2;
  note_advances[0].advance = -1;
  check(!reader_view_build(&build_input, &storage, &frame) &&
        (frame.error_flags & ReaderViewFrameError_BadInput) != 0,
        "note metrics reject negative advances");
  note_advances[0].advance = test_note_codepoint_advance(1);
  note_advances[0].codepoint = 0xd800u;
  check(!reader_view_build(&build_input, &storage, &frame) &&
        (frame.error_flags & ReaderViewFrameError_BadInput) != 0,
        "note metrics reject surrogate scalar values");
  note_advances[0].codepoint = 1;

  for (index = 0;
       index < READER_VIEW_NOTE_CODEPOINT_ADVANCE_CAP + 1;
       ++index)
  {
    over_cap[index].codepoint = (UI0U32)index + 1u;
    over_cap[index].advance = 10;
  }
  build_input.note_text_metrics.advances = over_cap;
  build_input.note_text_metrics.advance_count =
    READER_VIEW_NOTE_CODEPOINT_ADVANCE_CAP + 1;
  check(!reader_view_build(&build_input, &storage, &frame) &&
        (frame.error_flags & ReaderViewFrameError_BadInput) != 0,
        "note metric table rejects entries beyond its 256-scalar cap");
  build_input.note_text_metrics = valid_metrics;
  build_input.note_text_metrics.fallback_advance = 0;
  check(!reader_view_build(&build_input, &storage, &frame) &&
        (frame.error_flags & ReaderViewFrameError_BadInput) != 0,
        "note metrics require a positive deterministic fallback");
  build_input.note_text_metrics = valid_metrics;
  build_input.note_text_metrics.pixel_height = 0;
  check(!reader_view_build(&build_input, &storage, &frame) &&
        (frame.error_flags & ReaderViewFrameError_BadInput) != 0,
        "note metrics require a positive raster pixel height");
  build_input.note_text_metrics = valid_metrics;
  build_input.note_text_metrics.line_height = 17;
  check(!reader_view_build(&build_input, &storage, &frame) &&
        (frame.error_flags & ReaderViewFrameError_BadInput) != 0,
        "note metrics reject a line height below pixel height");

  check(reader_view_close_note_editor(&state),
        "note metric validation editor closes");
  projection.selection.selection_key = 7071;
  projection.selection.revision = 32;
  projection.selection.flags = ReaderViewSelection_Active |
    ReaderViewSelection_CanAddNote;
  projection.selection.note_text.data = 0;
  projection.selection.note_text.size = 0;
  check(reader_view_open_note_editor(&state, &projection.selection),
        "fallback-refresh note editor opens");
  memset(&build_input.note_text_metrics, 0,
         sizeof(build_input.note_text_metrics));
  build_input.note_text_metrics.fallback_advance = 10;
  build_input.note_text_metrics.pixel_height = 18;
  build_input.note_text_metrics.line_height = 25;
  input.note_text.text = "\xc3\xa9";
  input.note_text.text_len = 2;
  build_input.frame_index = 1021;
  check(reader_view_build(&build_input, &storage, &frame),
        "same-frame unlisted note scalar uses fallback metrics");
  record = storage.text_area_records;
  editor = find_semantic_role(&frame, "Note text",
                              ReaderViewSemantic_TextArea);
  caret = editor ? find_draw_for_source(&frame, UI0DrawOp_TextCaret,
                                         editor->id) : 0;
  check(state.note_draft_length == 2 &&
        memcmp(state.note_draft, "\xc3\xa9", 2) == 0 && caret &&
        caret->rect.x == record->text_rect.x + 10,
        "unlisted incoming scalar keeps draft/focus and deterministic "
        "fallback caret geometry");
  memset(&input, 0, sizeof(input));
  note_advances[0].codepoint = 0xe9u;
  note_advances[0].advance = 14;
  build_input.note_text_metrics.advances = note_advances;
  build_input.note_text_metrics.advance_count = 1;
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "next-frame refreshed scalar metric builds");
  record = storage.text_area_records;
  editor = find_semantic_role(&frame, "Note text",
                              ReaderViewSemantic_TextArea);
  caret = editor ? find_draw_for_source(&frame, UI0DrawOp_TextCaret,
                                         editor->id) : 0;
  check(state.note_draft_length == 2 && editor &&
        state.focus_id == editor->id &&
        caret && caret->rect.x == record->text_rect.x + 14,
        "next-frame metric refresh updates exact caret without losing "
        "draft or focus");
}

static void
test_note_reference_rows_and_hits(const UI0ResolvedTheme *theme)
{
  static const char note_text[] =
    "aaaaaaaaaa\naaaaaaaaaa\naaaaaaaaaa\naaaaaaaaaa\naaaaaaaaaa\n"
    "aaaaaaaaaa\naaaaaaaaaa\naaaaaaaaaa\naaaaaaaaaa\naaaaaaaaaa";
  static ReaderViewState state;
  static ReaderViewFrameStorage storage;
  ReaderViewSettingControl settings[READER_VIEW_SETTING_CAP];
  ReaderViewChoice choices[8];
  ReaderViewTocRow toc_rows[2];
  ReaderViewFindRow find_rows[1];
  ReaderViewRightRow right_rows[1];
  ReaderViewCodepointAdvance advances[127];
  ReaderViewProjection projection = full_projection(
    settings, choices, toc_rows, find_rows, right_rows);
  ReaderViewLayoutInput layout_input;
  ReaderViewLayout layout;
  ReaderViewInput input;
  ReaderViewFrame frame;
  UI0SignalRecord outside_signals[READER_VIEW_SIGNAL_CAP];
  const UI0TextAreaRecord *record;
  const UI0TextAreaRowRecord *first_row;
  const UI0TextAreaRowRecord *last_row;
  const UI0TextAreaSelectionRecord *first_selection;
  const UI0TextAreaSelectionRecord *last_selection;
  const UI0DrawCommand *caret;
  const ReaderViewSemanticNode *editor;
  UI0U64 frame_index = 961;
  UI0S32 text_x;

  memset(&layout_input, 0, sizeof(layout_input));
  layout_input.bounds = ui0_rect(0, 0, 1400, 780);
  memset(&input, 0, sizeof(input));
  reader_view_state_reset_document(&state, projection.document_key);
  projection.selection.status = ready_status();
  projection.selection.selection_key = 6060;
  projection.selection.revision = 21;
  projection.selection.flags = ReaderViewSelection_Active |
    ReaderViewSelection_CanEditNote;
  projection.selection.note_text.data = note_text;
  projection.selection.note_text.size = (UI0S32)strlen(note_text);
  check(reader_view_open_note_editor(&state, &projection.selection) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "ten-row note reference frame builds");
  record = storage.text_area_records;
  first_row = storage.note_text_area_row_records + record->row_start;
  last_row = first_row + record->row_count - 1;
  text_x = record->text_rect.x;
  editor = find_semantic_role(&frame, "Note text",
                              ReaderViewSemantic_TextArea);
  caret = editor ? find_draw_for_source(&frame, UI0DrawOp_TextCaret,
                                         editor->id) : 0;
  check(record->row_count == 9 && record->visible_row_count == 9 &&
        record->scroll_y == 25 && state.note_input.scroll_y == 25 &&
        first_row->byte_start == 11 && last_row->byte_start == 99 &&
        rect_equal(first_row->rect, ui0_rect(text_x, 180, 60, 25)) &&
        rect_equal(last_row->rect, ui0_rect(text_x, 380, 60, 25)) &&
        caret && rect_equal(caret->rect,
                            ui0_rect(text_x + 60, 381, 1, 23)),
        "note adapter publishes exactly nine complete rows and the last-row "
        "caret after line-quantized scrolling");

  state.note_input.caret = 0;
  state.note_input.selection_anchor = 0;
  state.note_input.scroll_y = 0;
  state.note_input.manual_scroll_anchor_valid = 0;
  input.ui = ui0_input_pointer(text_x + 1, 181, 1, 1, 0);
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "note first visible row pointer press builds");
  record = storage.text_area_records;
  first_row = storage.note_text_area_row_records + record->row_start;
  last_row = first_row + record->row_count - 1;
  editor = find_semantic_role(&frame, "Note text",
                              ReaderViewSemantic_TextArea);
  caret = editor ? find_draw_for_source(&frame, UI0DrawOp_TextCaret,
                                         editor->id) : 0;
  check(record->row_count == 9 && record->scroll_y == 0 &&
        first_row->byte_start == 0 && last_row->byte_start == 88 &&
        state.note_input.caret == 0 && caret &&
        rect_equal(caret->rect, ui0_rect(text_x, 181, 1, 23)) &&
        record->text_rect.y - record->rect.y == 13 &&
        record->rect.y + record->rect.h -
          (record->text_rect.y + record->text_rect.h) == 7,
        "first-row hit and caret preserve the frozen asymmetric +13/+7 "
        "content clipping");
  input.ui = ui0_input_pointer(text_x + 1, 181, 0, 0, 1);
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "note first-row pointer release builds");

  input.ui = ui0_input_pointer(text_x + 1, 381, 1, 1, 0);
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "note last visible row pointer press builds");
  record = storage.text_area_records;
  editor = find_semantic_role(&frame, "Note text",
                              ReaderViewSemantic_TextArea);
  caret = editor ? find_draw_for_source(&frame, UI0DrawOp_TextCaret,
                                         editor->id) : 0;
  check(state.note_input.caret == 88 && record->scroll_y == 0 && caret &&
        rect_equal(caret->rect, ui0_rect(text_x, 381, 1, 23)),
        "last visible row hit maps to the ninth complete row and caret");
  input.ui = ui0_input_pointer(text_x + 1, 381, 0, 0, 1);
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "note last-row pointer release builds");

  memset(&input, 0, sizeof(input));
  state.note_input.scroll_y = 0;
  state.note_input.manual_scroll_anchor_valid = 0;
  input.note_text.select_all = 1;
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "note nine-row selection frame builds");
  record = storage.text_area_records;
  first_selection = storage.note_text_area_selection_records +
    record->selection_start_record;
  last_selection = first_selection + record->selection_count - 1;
  check(record->selection_count == 9 && record->scroll_y == 25 &&
        rect_equal(first_selection->rect,
                   ui0_rect(text_x, 181, 60, 23)) &&
        rect_equal(last_selection->rect,
                   ui0_rect(text_x, 381, 60, 23)),
        "note selection adapter emits nine inset 23px selection records "
        "without partial first or tenth rows");

  memset(&input, 0, sizeof(input));
  input.ui = ui0_input_pointer(100, 700, 0, 0, 0);
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "outside-note signal baseline builds");
  memcpy(outside_signals, storage.signal_records,
         sizeof(outside_signals));
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        input.ui.pointer_y == 700 &&
        memcmp(outside_signals, storage.signal_records,
               sizeof(outside_signals)) == 0,
        "temporary note pointer mapping restores exactly and leaves every "
        "signal outside the editor shell bit-identical");
}

static void
test_modal_feature_and_scroll_lifecycle(const UI0ResolvedTheme *theme)
{
  static ReaderViewState state;
  static ReaderViewFrameStorage storage;
  ReaderViewSettingControl settings[READER_VIEW_SETTING_CAP];
  ReaderViewChoice choices[8];
  ReaderViewTocRow toc_seed[2];
  ReaderViewFindRow find_seed[1];
  ReaderViewRightRow right_seed[1];
  ReaderViewCodepointAdvance advances[127];
  ReaderViewProjection projection = full_projection(
    settings, choices, toc_seed, find_seed, right_seed);
  ReaderViewFeatureFlags all_features = projection.features;
  ReaderViewLayoutInput layout_input;
  ReaderViewLayout layout;
  ReaderViewInput input;
  ReaderViewFrame frame;
  const ReaderViewAction *action;
  const ReaderViewSemanticNode *node;
  UI0ID restore_focus;
  UI0U64 frame_index = 1000;
  char note_text[101];

  memset(&layout_input, 0, sizeof(layout_input));
  layout_input.bounds = ui0_rect(0, 0, 1400, 800);
  memset(&input, 0, sizeof(input));
  reader_view_state_reset_document(&state, projection.document_key);
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "modal lifecycle seed builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_Progress, 0);
  restore_focus = node ? node->id : 0;
  check(node && reader_view_accessibility_focus(&state, restore_focus) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "modal lifecycle progress focus builds");
  memset(&projection.selection, 0, sizeof(projection.selection));
  projection.selection.status.state = ReaderViewLoad_Loading;
  projection.selection.selection_key = 50;
  projection.selection.flags = ReaderViewSelection_Active |
                               ReaderViewSelection_CanEditNote;
  check(!reader_view_open_note_editor(&state, &projection.selection),
        "public note editor rejects a non-ready selection");
  memset(note_text, 'n', sizeof(note_text) - 1);
  note_text[sizeof(note_text) - 1] = 0;
  projection.selection.status = ready_status();
  projection.selection.revision = 7;
  projection.selection.note_text.data = note_text;
  projection.selection.note_text.size = 100;
  projection.selection.flags |= ReaderViewSelection_CanDeleteNote;
  check(reader_view_open_note_editor(&state, &projection.selection) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "public note editor enters the modal root");
  node = find_semantic_role(&frame, "Note text", ReaderViewSemantic_TextArea);
  check(node && (node->flags & ReaderViewSemantic_Focused) != 0 &&
        state.focus_id == node->id,
        "note modal moves focus into its text area");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_Progress, 0);
  check(node && (node->flags & (ReaderViewSemantic_Focusable |
                                ReaderViewSemantic_Focused)) == 0,
        "note modal suppresses blocked progress semantics");
  input.note_text.move_vertical_delta = -1;
  input.move_vertical_delta = -1;
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        state.note_input.caret == 53,
        "note vertical movement follows the real wrapped row once when host "
        "deltas mirror");
  memset(&input, 0, sizeof(input));
  node = find_semantic(&frame, "Cancel note");
  check(node && reader_view_accessibility_focus(&state, node->id) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "note Cancel focus builds");
  input.note_text.text = "X";
  input.note_text.text_len = 1;
  input.note_text.undo_pressed = 1;
  input.note_text.delete_pressed = 1;
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        reader_view_note_draft(&state).size == 100,
        "button-focused note edit commands cannot mutate the draft");
  memset(&input, 0, sizeof(input));
  projection.selection.revision = 8;
  check(!lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                         theme, advances, &storage, &frame, frame_index++) &&
        (frame.error_flags & ReaderViewFrameError_StaleNoteRevision) != 0,
        "stale note revision build");
  node = find_semantic(&frame, "Delete note");
  check(node && (node->flags & ReaderViewSemantic_Enabled) == 0 &&
        reader_view_accessibility_invoke(&state, node->id) &&
        !lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                         theme, advances, &storage, &frame, frame_index++) &&
        find_action(&frame, ReaderViewAction_DeleteNote) == 0,
        "stale note revision disables Delete and consumes its AT invoke");

  projection.selection.revision = 7;
  memset(&input, 0, sizeof(input));
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "matching note revision restores an eligible modal");
  node = find_semantic_role(&frame, "Note text",
                            ReaderViewSemantic_TextArea);
  check(node && reader_view_accessibility_focus(&state, node->id) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "matching note text area focus builds");
  input.note_text.text = "X";
  input.note_text.text_len = 1;
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        state.note_dirty && reader_view_note_draft(&state).size == 101,
        "matching note edit creates a retryable dirty draft");
  memset(&input, 0, sizeof(input));
  node = find_semantic(&frame, "Save note");
  check(node && reader_view_accessibility_invoke(&state, node->id) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "matching dirty note Save invocation builds");
  action = find_action(&frame, ReaderViewAction_SaveNote);
  check(action && action->key == projection.selection.selection_key &&
        action->value == projection.selection.revision &&
        action->text.size == 101 &&
        state.popup == ReaderViewPopup_NoteEditor && state.note_dirty &&
        reader_view_note_draft(&state).size == 101,
        "Save emits once while the unacknowledged modal and draft remain");
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        find_action(&frame, ReaderViewAction_SaveNote) == 0 &&
        state.popup == ReaderViewPopup_NoteEditor && state.note_dirty &&
        reader_view_note_draft(&state).size == 101,
        "failed host persistence leaves the note modal available for retry");
  check(reader_view_close_note_editor(&state) &&
        state.popup == ReaderViewPopup_None &&
        state.restore_focus_id == 0 && state.focus_id == restore_focus &&
        !state.note_dirty && state.hot_id == 0 && state.active_id == 0 &&
        state.pending_accessibility_focus_id == 0 &&
        state.pending_accessibility_invoke_id == 0,
        "successful host Save acknowledgement restores focus and retires "
        "modal state");

  memset(&input, 0, sizeof(input));
  check(reader_view_open_note_editor(&state, &projection.selection) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "matching note editor reopens for Delete acknowledgement lifecycle");
  node = find_semantic_role(&frame, "Note text", ReaderViewSemantic_TextArea);
  check(node && reader_view_accessibility_focus(&state, node->id) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "Delete lifecycle note text area focus builds");
  input.note_text.text = "Y";
  input.note_text.text_len = 1;
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        state.note_dirty && reader_view_note_draft(&state).size == 101,
        "Delete lifecycle creates a retryable dirty draft");
  memset(&input, 0, sizeof(input));
  node = find_semantic(&frame, "Delete note");
  check(node && reader_view_accessibility_invoke(&state, node->id) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "matching dirty note Delete invocation builds");
  action = find_action(&frame, ReaderViewAction_DeleteNote);
  check(action && action->key == projection.selection.selection_key &&
        action->value == projection.selection.revision &&
        state.popup == ReaderViewPopup_NoteEditor && state.note_dirty &&
        reader_view_note_draft(&state).size == 101,
        "Delete emits once while the unacknowledged modal and draft remain");
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        find_action(&frame, ReaderViewAction_DeleteNote) == 0 &&
        state.popup == ReaderViewPopup_NoteEditor && state.note_dirty &&
        reader_view_note_draft(&state).size == 101,
        "failed host Delete persistence leaves the note modal available for "
        "retry");
  check(reader_view_close_note_editor(&state) &&
        state.popup == ReaderViewPopup_None &&
        state.restore_focus_id == 0 && state.focus_id == restore_focus &&
        !state.note_dirty && state.hot_id == 0 && state.active_id == 0 &&
        state.pending_accessibility_focus_id == 0 &&
        state.pending_accessibility_invoke_id == 0,
        "successful host Delete acknowledgement restores focus and retires "
        "modal state");

  {
    ReaderViewBuildInput bad_build;
    ReaderViewKey second_toc_key;
    ReaderViewKey document_key;
    projection = full_projection(settings, choices, toc_seed,
                                 find_seed, right_seed);
    all_features = projection.features;
    reader_view_state_reset_document(&state, projection.document_key);
    memset(&input, 0, sizeof(input));
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++),
          "failed-build AT projection seed builds");
    node = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_Contents, 0);
    second_toc_key = toc_seed[1].key;
    check(node && reader_view_accessibility_invoke(&state, node->id),
          "projection-invalid build queues an AT invoke");
    toc_seed[1].key = toc_seed[0].key;
    check(!lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                           theme, advances, &storage, &frame, frame_index++) &&
          (frame.change_flags &
           ReaderViewFrameChange_ProjectionInvalid) != 0 &&
          state.pending_accessibility_invoke_id == 0,
          "projection-invalid build consumes its one-shot AT invoke");
    toc_seed[1].key = second_toc_key;
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++) &&
          state.left_panel == ReaderViewLeftPanel_None &&
          frame.action_count == 0,
          "repaired projection cannot replay a failed-build AT invoke");

    node = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_Find, 0);
    check(node && reader_view_accessibility_invoke(&state, node->id),
          "bad-metrics build queues an AT invoke");
    memset(&bad_build, 0, sizeof(bad_build));
    bad_build.frame_index = frame_index++;
    bad_build.state = &state;
    bad_build.layout = &layout;
    bad_build.projection = &projection;
    bad_build.input = &input;
    bad_build.theme = theme;
    bad_build.find_text_metrics = test_find_text_metrics(advances);
    bad_build.find_text_metrics.fallback_advance = 0;
    check(!reader_view_build(&bad_build, &storage, &frame) &&
          (frame.error_flags & ReaderViewFrameError_BadInput) != 0 &&
          state.pending_accessibility_invoke_id == 0,
          "bad Find metrics consume their one-shot AT invoke");
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++) &&
          state.left_panel == ReaderViewLeftPanel_None &&
          frame.action_count == 0,
          "repaired Find metrics cannot replay a failed-build AT invoke");

    node = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_Bookmark, 0);
    check(node && reader_view_accessibility_invoke(&state, node->id),
          "stale-document build queues an AT invoke");
    document_key = projection.document_key;
    projection.document_key = document_key + 1;
    check(!lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                           theme, advances, &storage, &frame, frame_index++) &&
          (frame.error_flags &
           ReaderViewFrameError_StaleDocumentState) != 0 &&
          state.pending_accessibility_invoke_id == 0,
          "stale-document build consumes its one-shot AT invoke");
    projection.document_key = document_key;
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++) &&
          find_action(&frame, ReaderViewAction_ToggleBookmark) == 0,
          "repaired document identity cannot replay a failed-build invoke");
  }

  reader_view_state_reset_document(&state, projection.document_key);
  state.left_panel = ReaderViewLeftPanel_Find;
  projection.selection.status = ready_status();
  projection.selection.flags = 0;
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "feature-withdrawal Find seed builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_Find, 0);
  check(node && reader_view_accessibility_invoke(&state, node->id),
        "disappearing Find control accepts a one-build AT request");
  projection.features &= ~ReaderViewFeature_Find;
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        state.left_panel == ReaderViewLeftPanel_None &&
        state.pending_accessibility_invoke_id == 0 &&
        (frame.change_flags & ReaderViewFrameChange_LayoutChanged) != 0,
        "Find feature withdrawal closes its panel and consumes the request");
  projection.features |= ReaderViewFeature_Find;
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        state.left_panel == ReaderViewLeftPanel_None,
        "restored Find feature does not replay a stale AT invoke");
  state.right_panel_open = 1;
  projection.features &= ~ReaderViewFeature_Annotations;
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        !state.right_panel_open,
        "Annotations feature withdrawal closes its host-owned panel state");

  {
    static const struct
    {
      ReaderViewFeatureFlags feature;
      ReaderViewSemanticControl control;
      const char *name;
    } cases[] = {
      { ReaderViewFeature_Contents,
        ReaderViewSemanticControl_Contents,
        "direct-focused Contents trigger withdrawal" },
      { ReaderViewFeature_Find,
        ReaderViewSemanticControl_Find,
        "direct-focused Find trigger withdrawal" },
      { ReaderViewFeature_History,
        ReaderViewSemanticControl_HistoryBack,
        "direct-focused History trigger withdrawal" },
      { ReaderViewFeature_Fullscreen,
        ReaderViewSemanticControl_Fullscreen,
        "direct-focused Fullscreen trigger withdrawal" },
      { ReaderViewFeature_ReadingSettings,
        ReaderViewSemanticControl_FontFamily,
        "direct-focused setting trigger withdrawal" },
      { ReaderViewFeature_Annotations,
        ReaderViewSemanticControl_Annotations,
        "direct-focused Annotations trigger withdrawal" },
      { ReaderViewFeature_Bookmark,
        ReaderViewSemanticControl_Bookmark,
        "direct-focused Bookmark trigger withdrawal" },
      { ReaderViewFeature_Paging,
        ReaderViewSemanticControl_PreviousPage,
        "direct-focused Previous gutter withdrawal" },
      { ReaderViewFeature_Progress,
        ReaderViewSemanticControl_Progress,
        "direct-focused progress withdrawal" },
    };
    UI0S32 case_index;
    for (case_index = 0;
         case_index < (UI0S32)(sizeof(cases) / sizeof(cases[0]));
         ++case_index)
    {
      projection.features = all_features;
      reader_view_state_reset_document(&state, projection.document_key);
      memset(&input, 0, sizeof(input));
      check(lifecycle_build(&state, &layout_input, &layout, &projection,
                            &input, theme, advances, &storage, &frame,
                            frame_index++),
            cases[case_index].name);
      node = find_semantic_control_source(
        &frame, cases[case_index].control, 0);
      check(node && reader_view_accessibility_focus(&state, node->id) &&
            lifecycle_build(&state, &layout_input, &layout, &projection,
                            &input, theme, advances, &storage, &frame,
                            frame_index++),
            cases[case_index].name);
      node = find_semantic_control_source(
        &frame, cases[case_index].control, 0);
      if (node)
      {
        state.hot_id = node->id;
        state.active_id = node->id;
        state.pending_accessibility_focus_id = node->id;
        state.pending_accessibility_invoke_id = node->id;
      }
      projection.features &= ~cases[case_index].feature;
      check(lifecycle_build(&state, &layout_input, &layout, &projection,
                            &input, theme, advances, &storage, &frame,
                            frame_index++) &&
            state.focus_id == 0 && !state.focus_visible &&
            state.hot_id == 0 && state.active_id == 0 &&
            state.pending_accessibility_focus_id == 0 &&
            state.pending_accessibility_invoke_id == 0 &&
            find_semantic_control_source(
              &frame, cases[case_index].control, 0) == 0 &&
            frame.action_count == 0,
            cases[case_index].name);
    }

    projection.features = all_features & ~ReaderViewFeature_Fullscreen;
    reader_view_state_reset_document(&state, projection.document_key);
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++),
          "direct-focused distraction trigger seed builds");
    node = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_DistractionFree, 0);
    check(node && reader_view_accessibility_focus(&state, node->id) &&
          lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++),
          "direct-focused distraction trigger focus builds");
    projection.features &= ~ReaderViewFeature_DistractionFree;
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++) &&
          state.focus_id == 0 && !state.focus_visible,
          "direct-focused distraction trigger withdrawal clears focus");

    projection.features = all_features;
    reader_view_state_reset_document(&state, projection.document_key);
    state.right_panel_open = 1;
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++),
          "direct-focused Export trigger seed builds");
    node = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_RightExport, 0);
    check(node && reader_view_accessibility_focus(&state, node->id) &&
          lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++),
          "direct-focused Export trigger focus builds");
    projection.features &= ~ReaderViewFeature_Export;
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++) &&
          state.focus_id == 0 && !state.focus_visible,
          "direct-focused Export trigger withdrawal clears focus");
  }

  projection.features = all_features;
  reader_view_state_reset_document(&state, projection.document_key);
  memset(&input, 0, sizeof(input));
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "natural Contents open seed builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_Contents, 0);
  check(node && reader_view_accessibility_invoke(&state, node->id) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        state.left_panel == ReaderViewLeftPanel_Contents &&
        state.left_panel_restore_focus_id != 0 && state.focus_id != 0,
        "natural Contents open captures trigger and focuses its row");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_TocRow, toc_seed[0].key);
  check(node && (node->flags & ReaderViewSemantic_Focused) != 0 &&
        state.pending_left_panel_focus == ReaderViewLeftPanel_None,
        "next-layout Contents frame hands focus from trigger to TOC row");
  projection.features &= ~ReaderViewFeature_Contents;
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        state.left_panel == ReaderViewLeftPanel_None &&
        state.left_panel_restore_focus_id == 0 && state.focus_id == 0 &&
        !state.focus_visible,
        "Contents withdrawal cannot restore focus to its vanished trigger");

  projection.features = all_features;
  reader_view_state_reset_document(&state, projection.document_key);
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "natural Find open seed builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_Find, 0);
  check(node && reader_view_accessibility_invoke(&state, node->id) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        state.left_panel == ReaderViewLeftPanel_Find &&
        state.left_panel_restore_focus_id != 0 && state.focus_id != 0,
        "natural Find open captures trigger and focuses its input");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_FindInput, 0);
  check(node && (node->flags & ReaderViewSemantic_Focused) != 0 &&
        state.pending_left_panel_focus == ReaderViewLeftPanel_None,
        "next-layout Find frame hands focus from trigger to its input");
  projection.features &= ~ReaderViewFeature_Find;
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        state.left_panel == ReaderViewLeftPanel_None &&
        state.left_panel_restore_focus_id == 0 && state.focus_id == 0 &&
        !state.focus_visible,
        "Find withdrawal cannot restore focus to its vanished trigger");

  projection.features = all_features;
  reader_view_state_reset_document(&state, projection.document_key);
  memset(&input, 0, sizeof(input));
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "pending Contents Escape seed builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_Contents, 0);
  check(node && reader_view_accessibility_invoke(&state, node->id) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        state.pending_left_panel_focus == ReaderViewLeftPanel_Contents,
        "new Contents layout retains a bounded deferred-focus handoff");
  input.escape_pressed = 1;
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        state.left_panel == ReaderViewLeftPanel_None &&
        state.pending_left_panel_focus == ReaderViewLeftPanel_None,
        "Escape clears a not-yet-published left-panel focus handoff");
  memset(&input, 0, sizeof(input));

  projection.features = all_features;
  reader_view_state_reset_document(&state, projection.document_key);
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "natural Annotations open seed builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_Annotations, 0);
  check(node && reader_view_accessibility_invoke(&state, node->id) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        state.right_panel_open && state.right_panel_restore_focus_id != 0,
        "natural Annotations open captures its trigger");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightRow, right_seed[0].key);
  check(node && reader_view_accessibility_focus(&state, node->id) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "natural Annotations row focus builds");
  projection.features &= ~ReaderViewFeature_Annotations;
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        !state.right_panel_open &&
        state.right_panel_restore_focus_id == 0 && state.focus_id == 0 &&
        !state.focus_visible,
        "Annotations withdrawal cannot restore focus to its vanished trigger");

  projection.features = all_features;
  reader_view_state_reset_document(&state, projection.document_key);
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "natural setting popup seed builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_FontFamily, 0);
  check(node && reader_view_accessibility_invoke(&state, node->id) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        state.popup == ReaderViewPopup_SettingMenu && state.focus_id != 0,
        "natural setting popup contains focus");
  projection.features &= ~ReaderViewFeature_ReadingSettings;
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        state.popup == ReaderViewPopup_None &&
        state.restore_focus_id == 0 && state.focus_id == 0 &&
        !state.focus_visible,
        "setting owner withdrawal cannot restore a vanished trigger");

  projection.features = all_features;
  reader_view_state_reset_document(&state, projection.document_key);
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "natural filter popup Annotations seed builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_Annotations, 0);
  check(node && reader_view_accessibility_invoke(&state, node->id) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++),
        "natural filter popup opens Annotations");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_RightFilter, 0);
  check(node && reader_view_accessibility_invoke(&state, node->id) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        state.popup == ReaderViewPopup_RightFilter && state.focus_id != 0,
        "natural Annotations filter popup contains focus");
  projection.features &= ~ReaderViewFeature_Annotations;
  check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                        theme, advances, &storage, &frame, frame_index++) &&
        !state.right_panel_open && state.popup == ReaderViewPopup_None &&
        state.restore_focus_id == 0 && state.focus_id == 0 &&
        !state.focus_visible,
        "filter owner withdrawal cannot restore vanished panel/toolbar "
        "triggers");

  {
    static ReaderViewTocRow long_toc[32];
    const UI0ControlRecord *row_control;
    UI0S32 index;
    UI0S32 retained_scroll;
    for (index = 0; index < 32; ++index)
    {
      memset(long_toc + index, 0, sizeof(long_toc[index]));
      long_toc[index].key = 1000 + (ReaderViewKey)index;
      long_toc[index].label.data = "Row";
      long_toc[index].label.size = 3;
      long_toc[index].flags = ReaderViewRow_Enabled;
    }
    projection.features |= ReaderViewFeature_Annotations;
    projection.toc.rows = long_toc;
    projection.toc.row_count = 32;
    projection.toc.total_count = 32;
    projection.toc.status = ready_status();
    reader_view_state_reset_document(&state, projection.document_key);
    state.left_panel = ReaderViewLeftPanel_Contents;
    memset(&input, 0, sizeof(input));
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++),
          "single-delta Contents scroll seed builds");
    check(scroll_records_empty(&storage) &&
          count_draw_op(&frame, UI0DrawOp_ScrollTrack) == 0 &&
          count_draw_op(&frame, UI0DrawOp_ScrollThumb) == 0,
          "Contents publishes no visible or interactive scrollbar records");
    input.ui.pointer_x = layout.left_panel_rect.x + 100;
    input.ui.pointer_y = layout.left_panel_rect.y + 180;
    input.ui.wheel_delta_y = 17;
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++) &&
          state.toc_scroll_y == 17 &&
          (frame.change_flags & ReaderViewFrameChange_StateChanged) != 0,
          "Contents wheel delta applies exactly once and reports state change");
    node = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_TocRow, 1000);
    row_control = node ? find_control_for_source(&storage, node->id) : 0;
    check(row_control != 0 &&
          row_control->clip_rect.x == row_control->rect.x &&
          row_control->clip_rect.w == row_control->rect.w,
          "scrolled Contents keeps its partial row clipped at full width");
    if (row_control)
      input.ui = ui0_input_pointer(
        row_control->clip_rect.x + row_control->clip_rect.w - 2,
        row_control->clip_rect.y + row_control->clip_rect.h / 2,
        1, 1, 0);
    check(row_control && lifecycle_build(
            &state, &layout_input, &layout, &projection, &input,
            theme, advances, &storage, &frame, frame_index++),
          "Contents full-width right-edge press builds without a track owner");
    if (row_control)
      input.ui = ui0_input_pointer(
        row_control->clip_rect.x + row_control->clip_rect.w - 2,
        row_control->clip_rect.y + row_control->clip_rect.h / 2,
        0, 0, 1);
    check(row_control && lifecycle_build(
            &state, &layout_input, &layout, &projection, &input,
            theme, advances, &storage, &frame, frame_index++) &&
          find_action(&frame, ReaderViewAction_ActivateTocRow) != 0 &&
          find_action(&frame, ReaderViewAction_ActivateTocRow)->key == 1000,
          "Contents old track stripe belongs to the full-width row hit target");

    memset(&input, 0, sizeof(input));
    state.toc_scroll_y = 0;
    input.ui = ui0_input_pointer_wheel(
      layout.left_panel_rect.x + 100,
      layout.left_panel_rect.y + 180, 0, 0, 0, 0, INT32_MAX);
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++) &&
          state.toc_scroll_y == 438,
          "Contents hidden wheel scrolling clamps safely at its exact maximum");
    input.ui.wheel_delta_y = INT32_MIN;
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++) &&
          state.toc_scroll_y == 0,
          "Contents hidden wheel scrolling clamps safely back to zero");

    state.popup = ReaderViewPopup_SettingMenu;
    state.active_setting_kind = ReaderViewSetting_FontFamily;
    retained_scroll = state.toc_scroll_y;
    input.ui = ui0_input_pointer_wheel(
      layout.left_panel_rect.x + 100,
      layout.left_panel_rect.y + 180, 0, 0, 0, 0, 40);
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++) &&
          state.popup == ReaderViewPopup_SettingMenu &&
          state.toc_scroll_y == retained_scroll,
          "popup root blocks hidden Contents wheel scrolling underneath it");
    state.popup = ReaderViewPopup_None;
    state.active_setting_kind = ReaderViewSetting_FontFamily;
    state.restore_focus_id = 0;

    memset(&input, 0, sizeof(input));
    state.toc_scroll_y = 0;
    state.focus_id = 0;
    state.focus_visible = 0;
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++),
          "wheel-hidden Contents focus seed builds");
    node = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_TocRow, 1000);
    check(node && reader_view_accessibility_focus(&state, node->id) &&
          lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++),
          "wheel-hidden Contents row focus builds");
    input.ui = ui0_input_pointer_wheel(
      layout.left_panel_rect.x + 100,
      layout.left_panel_rect.y + 180,
      0, 0, 0, 0, 1000);
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++) &&
          state.focus_id == 0 && !state.focus_visible,
          "large Contents wheel retires focus when its row leaves publication");
    projection.toc.rows = long_toc + 1;
    projection.toc.row_count = 31;
    projection.toc.total_count = 31;
    memset(&input, 0, sizeof(input));
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++) &&
          state.focus_id == 0 &&
          find_semantic_control_source(
            &frame, ReaderViewSemanticControl_TocRow, 1000) == 0,
          "removing a wheel-hidden Contents row cannot leave stale focus");
    projection.toc.rows = long_toc;
    projection.toc.row_count = 32;
    projection.toc.total_count = 32;

    state.toc_scroll.active_thumb_id = 91;
    state.toc_scroll.drag_start_pointer_y = 120;
    state.toc_scroll.drag_start_scroll_y = 30;
    retained_scroll = state.toc_scroll_y;
    memset(&input, 0, sizeof(input));
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++) &&
          state.toc_scroll.active_thumb_id == 0 &&
          state.toc_scroll.drag_start_pointer_y == 0 &&
          state.toc_scroll.drag_start_scroll_y == 0 &&
          state.toc_scroll_y == retained_scroll &&
          (frame.change_flags & ReaderViewFrameChange_StateChanged) != 0,
          "ready Contents retires obsolete thumb state without losing offset");
    state.toc_scroll.active_thumb_id = 92;
    state.toc_scroll.drag_start_pointer_y = 121;
    state.toc_scroll.drag_start_scroll_y = 31;
    projection.toc.status.state = ReaderViewLoad_Loading;
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++) &&
          state.toc_scroll.active_thumb_id == 0 &&
          state.toc_scroll.drag_start_pointer_y == 0 &&
          state.toc_scroll.drag_start_scroll_y == 0 &&
          state.toc_scroll_y == retained_scroll,
          "Loading Contents retires obsolete interaction state but keeps scroll");
    projection.toc.status = ready_status();
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++) &&
          state.toc_scroll.active_thumb_id == 0 &&
          state.toc_scroll_y == retained_scroll &&
          scroll_records_empty(&storage),
          "ready Contents cannot revive retired thumb or track ownership");
  }

  {
    static ReaderViewFindRow long_find[16];
    const UI0ControlRecord *row_control;
    const UI0ControlRecord *section_control;
    const UI0ControlRecord *excerpt_control;
    const UI0DrawCommand *section_draw;
    const UI0DrawCommand *excerpt_draw;
    UI0S32 index;
    UI0S32 retained_scroll;
    for (index = 0; index < 16; ++index)
    {
      memset(long_find + index, 0, sizeof(long_find[index]));
      long_find[index].key = 2000 + (ReaderViewKey)index;
      long_find[index].section.data = index == 0 ? "F0" : "F";
      long_find[index].section.size = index == 0 ? 2 : 1;
      long_find[index].excerpt.data =
        index == 0 ? "Excerpt zero" : "Excerpt";
      long_find[index].excerpt.size = index == 0 ? 12 : 7;
      long_find[index].flags = ReaderViewRow_Enabled;
    }
    projection.features = all_features;
    projection.find.rows = long_find;
    projection.find.row_count = 16;
    projection.find.total_count = 16;
    projection.find.active_index = -1;
    projection.find.status = ready_status();
    reader_view_state_reset_document(&state, projection.document_key);
    state.left_panel = ReaderViewLeftPanel_Find;
    state.find_scroll_y = 0;
    memset(&input, 0, sizeof(input));
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++),
          "long Find scroll seed frame builds");
    check(scroll_records_empty(&storage) &&
          count_draw_op(&frame, UI0DrawOp_ScrollTrack) == 0 &&
          count_draw_op(&frame, UI0DrawOp_ScrollThumb) == 0,
          "Find publishes no visible or interactive scrollbar records");
    input.ui = ui0_input_pointer_wheel(
      layout.left_panel_rect.x + 160,
      layout.left_panel_rect.y + 180,
      0, 0, 0, 0, 17);
    check(lifecycle_build(
            &state, &layout_input, &layout, &projection, &input,
            theme, advances, &storage, &frame, frame_index++) &&
          state.find_scroll_y == 17 &&
          (frame.change_flags & ReaderViewFrameChange_StateChanged) != 0,
          "Find wheel delta applies exactly once and reports state change");
    node = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_FindRow, 2000);
    row_control = node ? find_control_for_source(&storage, node->id) : 0;
    node = find_semantic_role(&frame, "F0", ReaderViewSemantic_Group);
    section_control = node ? find_control_for_source(&storage, node->id) : 0;
    section_draw = node ? find_draw_for_source(
      &frame, UI0DrawOp_Text, node->id) : 0;
    node = find_semantic_role(&frame, "Excerpt zero",
                              ReaderViewSemantic_Group);
    excerpt_control = node ? find_control_for_source(&storage, node->id) : 0;
    excerpt_draw = node ? find_draw_for_source(
      &frame, UI0DrawOp_Text, node->id) : 0;
    check(row_control && section_control && excerpt_control &&
          section_draw && excerpt_draw &&
          row_control->clip_rect.h > 0 &&
          row_control->clip_rect.h < row_control->rect.h &&
          section_control->clip_rect.h > 0 &&
          section_control->clip_rect.h < section_control->rect.h &&
          row_control->clip_rect.x == row_control->rect.x &&
          row_control->clip_rect.w == row_control->rect.w &&
          section_control->clip_rect.x == section_control->rect.x &&
          section_control->clip_rect.w == section_control->rect.w &&
          excerpt_control->clip_rect.x == excerpt_control->rect.x &&
          excerpt_control->clip_rect.w == excerpt_control->rect.w &&
          rect_equal(section_draw->clip_rect, section_control->clip_rect) &&
          rect_equal(excerpt_draw->clip_rect, excerpt_control->clip_rect),
          "partial Find row and child paint/control clips retain the full "
          "frozen width");
    if (row_control)
      input.ui = ui0_input_pointer(
        row_control->clip_rect.x + row_control->clip_rect.w - 2,
        row_control->clip_rect.y + row_control->clip_rect.h / 2,
        1, 1, 0);
    check(row_control && lifecycle_build(
            &state, &layout_input, &layout, &projection, &input,
            theme, advances, &storage, &frame, frame_index++) &&
          find_action(&frame, ReaderViewAction_ActivateFindRow) == 0 &&
          find_action(&frame, ReaderViewAction_FindChanged) == 0 &&
          find_action(&frame, ReaderViewAction_FindCommitted) == 0,
          "Find full-width right-edge press has no hidden track owner");
    if (row_control)
      input.ui = ui0_input_pointer(
        row_control->clip_rect.x + row_control->clip_rect.w - 2,
        row_control->clip_rect.y + row_control->clip_rect.h / 2,
        0, 0, 1);
    check(row_control && lifecycle_build(
            &state, &layout_input, &layout, &projection, &input,
            theme, advances, &storage, &frame, frame_index++) &&
          find_action(&frame, ReaderViewAction_ActivateFindRow) != 0 &&
          find_action(&frame, ReaderViewAction_ActivateFindRow)->key == 2000 &&
          find_action(&frame, ReaderViewAction_FindChanged) == 0 &&
          find_action(&frame, ReaderViewAction_FindCommitted) == 0,
          "Find old track stripe belongs to the full-width result hit target");

    memset(&input, 0, sizeof(input));
    state.find_scroll_y = 0;
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++),
          "wheel-hidden Find focus seed builds");
    node = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_FindRow, 2000);
    check(node && reader_view_accessibility_focus(&state, node->id) &&
          lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++),
          "wheel-hidden Find row focus builds");
    input.ui = ui0_input_pointer_wheel(
      layout.left_panel_rect.x + 160,
      layout.left_panel_rect.y + 180,
      0, 0, 0, 0, 1000);
    check(lifecycle_build(
            &state, &layout_input, &layout, &projection, &input,
            theme, advances, &storage, &frame, frame_index++) &&
          state.focus_id == 0 && !state.focus_visible,
          "large Find wheel retires focus when its row leaves publication");
    projection.find.rows = long_find + 1;
    projection.find.row_count = 15;
    projection.find.total_count = 15;
    memset(&input, 0, sizeof(input));
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++) &&
          state.focus_id == 0 &&
          find_semantic_control_source(
            &frame, ReaderViewSemanticControl_FindRow, 2000) == 0,
          "removing a wheel-hidden Find row cannot leave stale focus");
    projection.find.rows = long_find;
    projection.find.row_count = 16;
    projection.find.total_count = 16;

    state.find_scroll.active_thumb_id = 101;
    state.find_scroll.drag_start_pointer_y = 130;
    state.find_scroll.drag_start_scroll_y = 40;
    retained_scroll = state.find_scroll_y;
    memset(&input, 0, sizeof(input));
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++) &&
          state.find_scroll.active_thumb_id == 0 &&
          state.find_scroll.drag_start_pointer_y == 0 &&
          state.find_scroll.drag_start_scroll_y == 0 &&
          state.find_scroll_y == retained_scroll &&
          (frame.change_flags & ReaderViewFrameChange_StateChanged) != 0,
          "ready Find retires obsolete thumb state without losing offset");
    state.find_scroll.active_thumb_id = 102;
    state.find_scroll.drag_start_pointer_y = 131;
    state.find_scroll.drag_start_scroll_y = 41;
    projection.find.status.state = ReaderViewLoad_Loading;
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++) &&
          state.find_scroll.active_thumb_id == 0 &&
          state.find_scroll.drag_start_pointer_y == 0 &&
          state.find_scroll.drag_start_scroll_y == 0 &&
          state.find_scroll_y == retained_scroll,
          "Loading Find retires obsolete interaction state but keeps scroll");
    projection.find.status = ready_status();
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++) &&
          state.find_scroll.active_thumb_id == 0 &&
          state.find_scroll_y == retained_scroll &&
          scroll_records_empty(&storage),
          "ready Find cannot revive retired thumb or track ownership");

    memset(&input, 0, sizeof(input));
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++),
          "Find immediate-close obsolete-owner seed builds");
    state.find_scroll.active_thumb_id = 103;
    state.find_scroll.drag_start_pointer_y = 132;
    state.find_scroll.drag_start_scroll_y = 42;
    node = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_LeftPanelClose, 0);
    check(node && reader_view_accessibility_invoke(&state, node->id),
          "Find close queues after obsolete scroll ownership is retired");
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++) &&
          state.left_panel == ReaderViewLeftPanel_None &&
          state.find_scroll.active_thumb_id == 0 &&
          state.find_scroll.drag_start_pointer_y == 0 &&
          state.find_scroll.drag_start_scroll_y == 0,
          "same-build Find close keeps obsolete thumb ownership retired");
  }

  {
    static ReaderViewRightRow long_right[20];
    static const ReaderViewSemanticControl focused_controls[] = {
      ReaderViewSemanticControl_RightRow,
      ReaderViewSemanticControl_RightRowStar,
      ReaderViewSemanticControl_RightRowMenu,
    };
    const UI0ControlRecord *row_control;
    const UI0ControlRecord *secondary_control;
    const UI0ControlRecord *primary_control;
    const UI0ControlRecord *star_control;
    const UI0ControlRecord *menu_control;
    const UI0DrawCommand *secondary_draw;
    const UI0DrawCommand *primary_draw;
    const UI0DrawCommand *star_draw;
    const UI0DrawCommand *menu_draw;
    UI0S32 index;
    UI0S32 retained_scroll;
    for (index = 0; index < 20; ++index)
    {
      memset(long_right + index, 0, sizeof(long_right[index]));
      long_right[index].key = 3000 + (ReaderViewKey)index;
      long_right[index].kind = ReaderViewRightRow_Highlight;
      long_right[index].section.data = "R";
      long_right[index].section.size = 1;
      long_right[index].secondary.data =
        index == 0 ? "Location zero" : "Location";
      long_right[index].secondary.size = index == 0 ? 13 : 8;
      long_right[index].primary.data =
        index == 0 ? "Entry zero" : "Entry";
      long_right[index].primary.size = index == 0 ? 10 : 5;
      long_right[index].flags = ReaderViewRow_Enabled;
      long_right[index].actions = ReaderViewRightAction_Activate |
                                  ReaderViewRightAction_ToggleStar |
                                  ReaderViewRightAction_Delete;
    }
    projection.features = all_features;
    projection.right.rows = long_right;
    projection.right.row_count = 20;
    projection.right.total_count = 20;
    projection.right.all_count = 20;
    projection.right.highlight_count = 20;
    projection.right.bookmark_count = 0;
    projection.right.note_count = 0;
    projection.right.available_filters = ReaderViewRightFilterFlag_All |
      ReaderViewRightFilterFlag_Highlights;
    projection.right.status = ready_status();
    reader_view_state_reset_document(&state, projection.document_key);
    state.right_panel_open = 1;
    state.right_scroll_y = 0;
    memset(&input, 0, sizeof(input));
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++),
          "long Annotations scroll seed frame builds");
    check(scroll_records_empty(&storage) &&
          count_draw_op(&frame, UI0DrawOp_ScrollTrack) == 0 &&
          count_draw_op(&frame, UI0DrawOp_ScrollThumb) == 0,
          "Annotations publishes no visible or interactive scrollbar records");
    input.ui = ui0_input_pointer_wheel(
      layout.right_panel_rect.x + 100,
      layout.right_panel_rect.y + 180,
      0, 0, 0, 0, 40);
    check(lifecycle_build(
            &state, &layout_input, &layout, &projection, &input,
            theme, advances, &storage, &frame, frame_index++) &&
          state.right_scroll_y == 40 &&
          (frame.change_flags & ReaderViewFrameChange_StateChanged) != 0,
          "Annotations wheel delta applies exactly once and reports state "
          "change");
    node = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_RightRow, 3000);
    row_control = node ? find_control_for_source(&storage, node->id) : 0;
    node = find_semantic_role(&frame, "Location zero",
                              ReaderViewSemantic_Group);
    secondary_control = node ? find_control_for_source(&storage, node->id) : 0;
    secondary_draw = node ? find_draw_for_source(
      &frame, UI0DrawOp_Text, node->id) : 0;
    node = find_semantic_role(&frame, "Entry zero",
                              ReaderViewSemantic_Group);
    primary_control = node ? find_control_for_source(&storage, node->id) : 0;
    primary_draw = node ? find_draw_for_source(
      &frame, UI0DrawOp_Text, node->id) : 0;
    node = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_RightRowStar, 3000);
    star_control = node ? find_control_for_source(&storage, node->id) : 0;
    star_draw = node ? find_draw_for_source(
      &frame, UI0DrawOp_Icon, node->id) : 0;
    node = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_RightRowMenu, 3000);
    menu_control = node ? find_control_for_source(&storage, node->id) : 0;
    menu_draw = node ? find_draw_for_source(
      &frame, UI0DrawOp_Text, node->id) : 0;
    check(row_control && secondary_control && primary_control &&
          star_control && menu_control && secondary_draw && primary_draw &&
          star_draw && menu_draw &&
          row_control->clip_rect.h > 0 &&
          row_control->clip_rect.h < row_control->rect.h &&
          secondary_control->clip_rect.h > 0 &&
          secondary_control->clip_rect.h < secondary_control->rect.h &&
          row_control->clip_rect.x == layout.right_panel_rect.x + 10 &&
          row_control->clip_rect.w == layout.right_panel_rect.w - 20 &&
          secondary_control->clip_rect.x == secondary_control->rect.x &&
          secondary_control->clip_rect.w == secondary_control->rect.w &&
          primary_control->clip_rect.x == primary_control->rect.x &&
          primary_control->clip_rect.w == primary_control->rect.w &&
          star_control->clip_rect.x == star_control->rect.x &&
          star_control->clip_rect.w == star_control->rect.w &&
          menu_control->clip_rect.x == menu_control->rect.x &&
          menu_control->clip_rect.w == menu_control->rect.w &&
          rect_equal(secondary_draw->clip_rect,
                     secondary_control->clip_rect) &&
          rect_equal(primary_draw->clip_rect, primary_control->clip_rect) &&
          rect_equal(star_draw->clip_rect, star_control->clip_rect) &&
          rect_equal(menu_draw->clip_rect, menu_control->clip_rect),
          "partial Annotations row/children/star/menu clips retain the full "
          "frozen width");
    if (row_control && star_control)
      input.ui = ui0_input_pointer(
        star_control->rect.x - 2,
        row_control->clip_rect.y + row_control->clip_rect.h / 2,
        1, 1, 0);
    check(row_control && star_control && lifecycle_build(
            &state, &layout_input, &layout, &projection, &input,
            theme, advances, &storage, &frame, frame_index++) &&
          find_action(&frame, ReaderViewAction_ActivateRightRow) == 0 &&
          find_action(&frame, ReaderViewAction_ToggleRightRowStar) == 0 &&
          state.popup == ReaderViewPopup_None,
          "Annotations full-width main-row press has no hidden track owner");
    if (row_control && star_control)
      input.ui = ui0_input_pointer(
        star_control->rect.x - 2,
        row_control->clip_rect.y + row_control->clip_rect.h / 2,
        0, 0, 1);
    check(row_control && star_control && lifecycle_build(
            &state, &layout_input, &layout, &projection, &input,
            theme, advances, &storage, &frame, frame_index++) &&
          find_action(&frame, ReaderViewAction_ActivateRightRow) != 0 &&
          find_action(&frame, ReaderViewAction_ActivateRightRow)->key == 3000 &&
          find_action(&frame, ReaderViewAction_ToggleRightRowStar) == 0 &&
          state.popup == ReaderViewPopup_None,
          "Annotations full-width main-row release activates without a track "
          "signal intercepting it");

    for (index = 0;
         index < (UI0S32)(sizeof(focused_controls) /
                          sizeof(focused_controls[0]));
         ++index)
    {
      memset(&input, 0, sizeof(input));
      state.right_scroll_y = 0;
      check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                            theme, advances, &storage, &frame, frame_index++),
            "wheel-hidden Annotations focus seed builds");
      node = find_semantic_control_source(
        &frame, focused_controls[index], 3000);
      check(node && reader_view_accessibility_focus(&state, node->id) &&
            lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                            theme, advances, &storage, &frame, frame_index++),
            "wheel-hidden Annotations row control focus builds");
      input.ui = ui0_input_pointer_wheel(
        layout.right_panel_rect.x + 100,
        layout.right_panel_rect.y + 180,
        0, 0, 0, 0, 1000);
      check(lifecycle_build(
              &state, &layout_input, &layout, &projection, &input,
              theme, advances, &storage, &frame, frame_index++) &&
            state.focus_id == 0 && !state.focus_visible,
            "large Annotations wheel retires row/star/menu focus when its "
            "owner leaves publication");
      projection.right.rows = long_right + 1;
      projection.right.row_count = 19;
      projection.right.total_count = 19;
      projection.right.all_count = 19;
      projection.right.highlight_count = 19;
      memset(&input, 0, sizeof(input));
      check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                            theme, advances, &storage, &frame, frame_index++) &&
            state.focus_id == 0 &&
            find_semantic_control_source(
              &frame, ReaderViewSemanticControl_RightRow, 3000) == 0 &&
            find_semantic_control_source(
              &frame, ReaderViewSemanticControl_RightRowStar, 3000) == 0 &&
            find_semantic_control_source(
              &frame, ReaderViewSemanticControl_RightRowMenu, 3000) == 0,
            "removing a wheel-hidden Annotations row retires row/star/menu "
            "identity");
      projection.right.rows = long_right;
      projection.right.row_count = 20;
      projection.right.total_count = 20;
      projection.right.all_count = 20;
      projection.right.highlight_count = 20;
    }

    state.right_scroll.active_thumb_id = 111;
    state.right_scroll.drag_start_pointer_y = 140;
    state.right_scroll.drag_start_scroll_y = 50;
    retained_scroll = state.right_scroll_y;
    memset(&input, 0, sizeof(input));
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++) &&
          state.right_scroll.active_thumb_id == 0 &&
          state.right_scroll.drag_start_pointer_y == 0 &&
          state.right_scroll.drag_start_scroll_y == 0 &&
          state.right_scroll_y == retained_scroll &&
          (frame.change_flags & ReaderViewFrameChange_StateChanged) != 0,
          "ready Annotations retires obsolete thumb state without losing "
          "offset");
    state.right_scroll.active_thumb_id = 112;
    state.right_scroll.drag_start_pointer_y = 141;
    state.right_scroll.drag_start_scroll_y = 51;
    projection.right.status.state = ReaderViewLoad_Loading;
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++) &&
          state.right_scroll.active_thumb_id == 0 &&
          state.right_scroll.drag_start_pointer_y == 0 &&
          state.right_scroll.drag_start_scroll_y == 0 &&
          state.right_scroll_y == retained_scroll,
          "Loading Annotations retires obsolete interaction state but keeps "
          "scroll");
    projection.right.status = ready_status();
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++) &&
          state.right_scroll.active_thumb_id == 0 &&
          state.right_scroll_y == retained_scroll &&
          scroll_records_empty(&storage),
          "ready Annotations cannot revive retired thumb or track ownership");

    memset(&input, 0, sizeof(input));
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++),
          "Annotations immediate-close hidden-scroll seed builds");
    state.right_scroll.active_thumb_id = 113;
    state.right_scroll.drag_start_pointer_y = 142;
    state.right_scroll.drag_start_scroll_y = 52;
    node = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_RightPanelClose, 0);
    check(node && reader_view_accessibility_invoke(&state, node->id),
          "Annotations close queues with obsolete scroll state present");
    check(lifecycle_build(&state, &layout_input, &layout, &projection, &input,
                          theme, advances, &storage, &frame, frame_index++) &&
          !state.right_panel_open &&
          state.right_scroll.active_thumb_id == 0 &&
          state.right_scroll.drag_start_pointer_y == 0 &&
          state.right_scroll.drag_start_scroll_y == 0,
          "same-build Annotations close retires obsolete thumb ownership");
  }
}

static void
test_find_excerpt_visible_match_window(const UI0ResolvedTheme *theme)
{
  static const char long_excerpt[] =
    "IMPERIAL COMMAND Ganoes Stabro Paran, a noble-born officer in the "
    "Malazan Empire";
  static const char utf8_excerpt[] =
    "A very long caf\xc3\xa9 preface before Paran tail context";
  static ReaderViewState state;
  static ReaderViewFrameStorage storage;
  ReaderViewSettingControl settings[READER_VIEW_SETTING_CAP];
  ReaderViewChoice choices[8];
  ReaderViewTocRow toc_rows[2];
  ReaderViewFindRow find_rows[1];
  ReaderViewRightRow right_rows[1];
  ReaderViewCodepointAdvance advances[127];
  ReaderViewProjection projection = full_projection(
    settings, choices, toc_rows, find_rows, right_rows);
  ReaderViewLayoutInput layout_input;
  ReaderViewLayout layout;
  ReaderViewInput input;
  ReaderViewBuildInput build_input;
  ReaderViewFrame frame;
  const ReaderViewSemanticNode *row;
  const ReaderViewTextBinding *binding = 0;
  const UI0DrawCommand *draw = 0;
  const char *match;
  UI0S32 index;

  match = strstr(long_excerpt, "Paran");
  check(match != 0, "long Find fixture contains its match");
  find_rows[0].excerpt.data = long_excerpt;
  find_rows[0].excerpt.size = (UI0S32)strlen(long_excerpt);
  find_rows[0].match_start = match ? (UI0U32)(match - long_excerpt) : 0;
  find_rows[0].match_size = 5;
  projection.find.active_index = 0;
  projection.find.status.message.data = "1 match";
  projection.find.status.message.size = 7;

  reader_view_state_reset_document(&state, projection.document_key);
  state.left_panel = ReaderViewLeftPanel_Find;
  memset(&layout_input, 0, sizeof(layout_input));
  layout_input.bounds = ui0_rect(0, 0, 1400, 780);
  layout_input.features = projection.features;
  layout_input.document_flags = projection.document_flags;
  layout_input.host_toolbar_trailing_width = 38;
  check(reader_view_resolve_layout(&state, &layout_input, &layout),
        "long Find excerpt layout resolves");
  memset(&input, 0, sizeof(input));
  memset(&build_input, 0, sizeof(build_input));
  build_input.frame_index = 7000;
  build_input.state = &state;
  build_input.layout = &layout;
  build_input.projection = &projection;
  build_input.input = &input;
  build_input.theme = theme;
  build_input.find_text_metrics = test_find_text_metrics(advances);
  check(reader_view_build(&build_input, &storage, &frame),
        "long Find excerpt frame builds");

  row = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_FindRow, find_rows[0].key);
  for (index = 0; row && index < frame.semantic_node_count; ++index)
  {
    const ReaderViewSemanticNode *candidate = frame.semantic_nodes + index;
    const ReaderViewTextBinding *candidate_binding;
    if (candidate->parent_id != row->id) continue;
    candidate_binding = find_text_binding(&frame, candidate->id);
    if (candidate_binding && candidate_binding->match_size > 0)
    {
      binding = candidate_binding;
      draw = find_draw_for_source(&frame, UI0DrawOp_Text, candidate->id);
      break;
    }
  }
  check(row != 0 && text_equal(row->name, long_excerpt),
        "Find row retains the complete Reader0 excerpt as its semantic name");
  check(binding != 0 && binding->text.data > long_excerpt &&
        binding->text.size < (UI0S32)strlen(long_excerpt) &&
        binding->text.size >= 7 &&
        memcmp(binding->text.data, "COMMAND", 7) == 0,
        "long-prefix Find result publishes a bounded natural-word window");
  check(binding != 0 && binding->match_size == 5 &&
        binding->match_start + binding->match_size <=
          (UI0U32)binding->text.size &&
        memcmp(binding->text.data + binding->match_start, "Paran", 5) == 0,
        "visible Find excerpt remaps and retains the complete match bytes");
  check(binding != 0 && draw != 0 &&
        test_find_text_range_width(binding->text) <=
          draw->rect.w - build_input.find_text_metrics.fallback_advance,
        "visible Find excerpt fits with one caller-measured advance reserved "
        "against host full-string fit differences");

  match = strstr(utf8_excerpt, "Paran");
  find_rows[0].excerpt.data = utf8_excerpt;
  find_rows[0].excerpt.size = (UI0S32)strlen(utf8_excerpt);
  find_rows[0].match_start = match ? (UI0U32)(match - utf8_excerpt) : 0;
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "UTF-8 long-prefix Find excerpt frame builds");
  row = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_FindRow, find_rows[0].key);
  binding = 0;
  for (index = 0; row && index < frame.semantic_node_count; ++index)
  {
    const ReaderViewSemanticNode *candidate = frame.semantic_nodes + index;
    const ReaderViewTextBinding *candidate_binding;
    if (candidate->parent_id != row->id) continue;
    candidate_binding = find_text_binding(&frame, candidate->id);
    if (candidate_binding && candidate_binding->match_size > 0)
    {
      binding = candidate_binding;
      break;
    }
  }
  check(binding != 0 &&
        (((unsigned char)binding->text.data[0] & 0xc0u) != 0x80u) &&
        binding->match_start + binding->match_size <=
          (UI0U32)binding->text.size &&
        memcmp(binding->text.data + binding->match_start, "Paran", 5) == 0,
        "Find excerpt window preserves UTF-8 and match boundaries");

  find_rows[0].match_start = (UI0U32)find_rows[0].excerpt.size - 1;
  find_rows[0].match_size = 2;
  build_input.frame_index += 1;
  check(!reader_view_build(&build_input, &storage, &frame) &&
        (frame.error_flags & ReaderViewFrameError_InvalidMatch) != 0,
        "Find excerpt window retains strict invalid-range rejection");
}

int
main(void)
{
  ReaderViewState state;
  ReaderViewLayoutInput layout_input;
  ReaderViewLayout layout;
  ReaderViewProjection projection = minimal_projection();
  ReaderViewInput frame_input;
  ReaderViewBuildInput build_input;
  ReaderViewFrameStorage storage;
  ReaderViewFrame frame;
  UI0TokenSet tokens = ui0_default_tokens(UI0ThemeKind_Light);
  UI0ResolvedTheme theme = ui0_resolve_tokens(&tokens);
  ReaderViewSettingControl settings[READER_VIEW_SETTING_CAP];
  ReaderViewChoice setting_choices[8];
  ReaderViewTocRow toc_rows[2];
  ReaderViewFindRow find_rows[1];
  ReaderViewRightRow right_rows[1];
  ReaderViewCodepointAdvance find_advances[127];
  ReaderViewCodepointAdvance note_advances[127];
  const ReaderViewSemanticNode *node;
  const ReaderViewAction *action;
  UI0U64 first_hash;
  ReaderViewDebugSnapshot debug_first;
  ReaderViewDebugSnapshot debug_repeat;
  ReaderViewDebugSnapshot debug_shifted;
  ReaderViewDebugSnapshot debug_changed;
  ReaderViewContentGeometryStyle geometry_style;
  ReaderViewContentGeometryStyle compact_style;
  ReaderViewContentGeometry geometry;
  ReaderViewContentGeometry geometry_repeat;

  check(READERVIEW0_API_VERSION == 3,
        "public API version");
  check(READERVIEW0_VERSION_MAJOR == 0 &&
        READERVIEW0_VERSION_MINOR == 3 &&
        READERVIEW0_VERSION_PATCH == 0 &&
        strcmp(READERVIEW0_VERSION_STRING, "0.3.0-dev") == 0,
        "public package version");
  check(READERVIEW0_UI0_REQUIRED_API_VERSION == 91 &&
        UI0_API_VERSION == 91,
        "exact UI0 API version");
  check(READER_VIEW_REFERENCE_TOP_CHROME_HEIGHT == 56 &&
        READER_VIEW_REFERENCE_FOOTER_HEIGHT == 38 &&
        READER_VIEW_REFERENCE_LEFT_PANEL_WIDTH == 420 &&
        READER_VIEW_REFERENCE_RIGHT_PANEL_WIDTH == 320 &&
        READER_VIEW_REFERENCE_PANEL_INSET == 12 &&
        READER_VIEW_REFERENCE_PANEL_PAGE_GAP == 14,
        "re10 reference chrome geometry constants");

  test_zero_document_interaction(&theme);
  test_progress_u64_scaling(&theme);
  test_reference_panels_and_disabled_gutter(&theme);
  test_focus_root_and_refresh_lifecycle(&theme);
  test_open_panel_focus_boundaries(&theme);
  test_frozen_note_editor_composition(&theme);
  test_note_metric_contract(&theme);
  test_note_reference_rows_and_hits(&theme);
  test_modal_feature_and_scroll_lifecycle(&theme);
  test_find_excerpt_visible_match_window(&theme);

  geometry_style = reader_view_default_content_geometry_style();
  memset(&geometry, 0, sizeof(geometry));
  memset(&geometry_repeat, 0, sizeof(geometry_repeat));
  check(geometry_style.page_horizontal_inset == 24 &&
        geometry_style.page_max_width == 660 &&
        geometry_style.page_min_width == 160 &&
        geometry_style.content_inset_x == 52 &&
        geometry_style.content_inset_y == 68 &&
        geometry_style.content_min_width == 80 &&
        geometry_style.content_min_height == 48,
        "default content geometry style matches re10 reference scalars");
  check(reader_view_resolve_content_geometry(
          ui0_rect(0, 48, 1400, 694), 0, &geometry),
        "wide reference content geometry resolves");
  check(rect_equal(geometry.viewport_rect,
                   ui0_rect(0, 48, 1400, 694)) &&
        rect_equal(geometry.page_surface_rect,
                   ui0_rect(370, 48, 660, 694)) &&
        rect_equal(geometry.content_rect,
                   ui0_rect(422, 116, 556, 558)),
        "wide reference content geometry is exact");
  check(reader_view_resolve_content_geometry(
          ui0_rect(320, 48, 1080, 694), &geometry_style, &geometry),
        "docked reference content geometry resolves");
  check(rect_equal(geometry.page_surface_rect,
                   ui0_rect(530, 48, 660, 694)) &&
        rect_equal(geometry.content_rect,
                   ui0_rect(582, 116, 556, 558)),
        "docked reference geometry centers inside shared viewport");
  check(reader_view_resolve_content_geometry(
          ui0_rect(0, 48, 940, 434), &geometry_style, &geometry),
        "narrow reference content geometry resolves");
  check(rect_equal(geometry.page_surface_rect,
                   ui0_rect(140, 48, 660, 434)) &&
        rect_equal(geometry.content_rect,
                   ui0_rect(192, 116, 556, 298)),
        "narrow reference content geometry is exact");
  check(reader_view_resolve_content_geometry(
          ui0_rect(0, 48, 600, 434), &geometry_style, &geometry),
        "inset-width content geometry resolves");
  check(rect_equal(geometry.page_surface_rect,
                   ui0_rect(24, 48, 552, 434)) &&
        rect_equal(geometry.content_rect,
                   ui0_rect(76, 116, 448, 298)),
        "page horizontal reserve is exact below max width");
  check(reader_view_resolve_content_geometry(
          ui0_rect(10, 20, 180, 100), &geometry_style, &geometry),
        "minimum content geometry resolves");
  check(rect_equal(geometry.page_surface_rect,
                   ui0_rect(20, 20, 160, 100)) &&
        rect_equal(geometry.content_rect,
                   ui0_rect(72, 88, 80, 48)),
        "minimum page and content bounds are exact");

  compact_style = geometry_style;
  compact_style.page_horizontal_inset = 10;
  compact_style.page_max_width = 500;
  compact_style.page_min_width = 120;
  compact_style.content_inset_x = 20;
  compact_style.content_inset_y = 24;
  compact_style.content_min_width = 60;
  compact_style.content_min_height = 40;
  check(reader_view_resolve_content_geometry(
          ui0_rect(5, 7, 800, 500), &compact_style, &geometry),
        "explicit content geometry style resolves");
  check(rect_equal(geometry.page_surface_rect,
                   ui0_rect(155, 7, 500, 500)) &&
        rect_equal(geometry.content_rect,
                   ui0_rect(175, 31, 460, 452)),
        "explicit content geometry style is honored");
  check(reader_view_resolve_content_geometry(
          ui0_rect(5, 7, 800, 500), &compact_style, &geometry_repeat) &&
        memcmp(&geometry, &geometry_repeat, sizeof(geometry)) == 0,
        "content geometry is deterministic");

  compact_style.page_min_width = compact_style.page_max_width + 1;
  geometry.viewport_rect = ui0_rect(1, 2, 3, 4);
  check(!reader_view_resolve_content_geometry(
          ui0_rect(5, 7, 800, 500), &compact_style, &geometry) &&
        rect_equal(geometry.viewport_rect, ui0_rect(0, 0, 0, 0)) &&
        rect_equal(geometry.page_surface_rect, ui0_rect(0, 0, 0, 0)) &&
        rect_equal(geometry.content_rect, ui0_rect(0, 0, 0, 0)),
        "invalid content geometry style fails closed");
  check(!reader_view_resolve_content_geometry(
          ui0_rect(0, 0, 0, 100), 0, &geometry) &&
        !reader_view_resolve_content_geometry(
          ui0_rect(0, 0, 100, 100), 0, 0),
        "invalid content geometry arguments fail closed");
  check(!reader_view_resolve_content_geometry(
          ui0_rect(INT32_MAX, INT32_MAX, 100, 100), 0, &geometry) &&
        rect_equal(geometry.viewport_rect, ui0_rect(0, 0, 0, 0)),
        "overflowing content geometry fails closed");

  reader_view_state_init(&state);
  memset(&layout_input, 0, sizeof(layout_input));
  layout_input.bounds = ui0_rect(0, 0, 1280, 800);
  check(reader_view_resolve_layout(&state, &layout_input, &layout),
        "wide layout resolves");
  check(layout.mode == ReaderViewLayout_WideDocked,
        "wide layout mode");
  check(layout.toolbar_density == ReaderViewToolbar_Compact,
        "fixed compact toolbar density");

  memset(&frame_input, 0, sizeof(frame_input));
  memset(&build_input, 0, sizeof(build_input));
  build_input.frame_index = 1;
  build_input.state = &state;
  build_input.layout = &layout;
  build_input.projection = &projection;
  build_input.input = &frame_input;
  build_input.theme = &theme;
  build_input.find_text_metrics = test_find_text_metrics(find_advances);
  build_input.note_text_metrics = test_note_text_metrics(note_advances);
  check(reader_view_build(&build_input, &storage, &frame),
        "minimal build");
  check(frame.error_flags == ReaderViewFrameError_None,
        "minimal build errors");

  layout_input.bounds = ui0_rect(0, 0, 600, 700);
  check(reader_view_resolve_layout(&state, &layout_input, &layout),
        "narrow layout resolves");
  check(layout.mode == ReaderViewLayout_Overlay,
        "narrow overlay mode");
  check(layout.toolbar_density == ReaderViewToolbar_Compact,
        "narrow fixed compact density");

  projection = full_projection(settings, setting_choices, toc_rows,
                               find_rows, right_rows);
  reader_view_state_reset_document(&state, projection.document_key);
  memset(&layout_input, 0, sizeof(layout_input));
  layout_input.bounds = ui0_rect(0, 0, 1400, 780);
  layout_input.features = projection.features;
  layout_input.document_flags = projection.document_flags;
  layout_input.host_toolbar_trailing_width = 38;
  check(reader_view_resolve_layout(&state, &layout_input, &layout),
        "full layout resolves");
  check(layout.mode == ReaderViewLayout_WideDocked &&
        layout.toolbar_density == ReaderViewToolbar_Compact &&
        layout.toolbar_visible && layout.progress_visible &&
        !layout.left_panel_visible && !layout.right_panel_visible &&
        rect_equal(layout.toolbar_rect, ui0_rect(0, 0, 1400, 56)) &&
        rect_equal(layout.shared_toolbar_rect,
                   ui0_rect(922, 10, 420, 28)) &&
        rect_equal(layout.host_toolbar_trailing_rect,
                   ui0_rect(1350, 10, 30, 28)) &&
        rect_equal(layout.body_rect, ui0_rect(0, 56, 1400, 686)) &&
        rect_equal(layout.viewport_rect, ui0_rect(0, 56, 1400, 686)) &&
        rect_equal(layout.page_surface_rect,
                   ui0_rect(370, 56, 660, 686)) &&
        rect_equal(layout.content_rect, ui0_rect(422, 124, 556, 550)) &&
        rect_equal(layout.progress_rect, ui0_rect(370, 760, 660, 18)),
        "atomic reference chrome and content geometry is exact");
  check(rect_equal(layout.previous_gutter_rect,
                   ui0_rect(4, 56, 366, 686)) &&
        rect_equal(layout.next_gutter_rect,
                   ui0_rect(1030, 56, 366, 686)) &&
        rect_equal(layout.previous_gutter_visual_rect,
                   ui0_rect(165, 355, 44, 88)) &&
        rect_equal(layout.next_gutter_visual_rect,
                   ui0_rect(1191, 355, 44, 88)),
        "reference paging hot and visual geometry is exact");

  state.left_panel = ReaderViewLeftPanel_Contents;
  state.right_panel_open = 1;
  check(reader_view_resolve_layout(&state, &layout_input, &layout),
        "two-panel reference layout resolves");
  check(rect_equal(layout.left_panel_rect,
                   ui0_rect(12, 56, 420, 686)) &&
        rect_equal(layout.right_panel_rect,
                   ui0_rect(1068, 56, 320, 686)) &&
        rect_equal(layout.viewport_rect, ui0_rect(434, 56, 632, 686)) &&
        rect_equal(layout.page_surface_rect,
                   ui0_rect(458, 56, 584, 686)) &&
        rect_equal(layout.content_rect, ui0_rect(510, 124, 480, 550)) &&
        rect_equal(layout.progress_rect, ui0_rect(458, 760, 584, 18)),
        "two-panel reference page reservation is exact");
  check(rect_equal(layout.previous_gutter_rect,
                   ui0_rect(440, 56, 18, 686)) &&
        rect_equal(layout.next_gutter_rect,
                   ui0_rect(1042, 56, 18, 686)) &&
        rect_equal(layout.previous_gutter_visual_rect,
                   ui0_rect(440, 355, 18, 88)) &&
        rect_equal(layout.next_gutter_visual_rect,
                   ui0_rect(1042, 355, 18, 88)),
        "two-panel gutter reservation is exact");

  layout_input.document_flags |= ReaderViewDocument_Fullscreen;
  check(reader_view_resolve_layout(&state, &layout_input, &layout),
        "fullscreen reference layout resolves");
  check(!layout.toolbar_visible && !layout.progress_visible &&
        !layout.left_panel_visible && !layout.right_panel_visible &&
        rect_equal(layout.body_rect, ui0_rect(0, 0, 1400, 780)) &&
        rect_equal(layout.viewport_rect, ui0_rect(0, 0, 1400, 780)) &&
        rect_equal(layout.page_surface_rect,
                   ui0_rect(370, 0, 660, 780)) &&
        rect_equal(layout.content_rect, ui0_rect(422, 68, 556, 644)),
        "fullscreen atomically hides chrome and expands page geometry");

  state.left_panel = ReaderViewLeftPanel_None;
  state.right_panel_open = 0;
  layout_input.document_flags = projection.document_flags;
  layout_input.bounds = ui0_rect(0, 0, 940, 520);
  check(reader_view_resolve_layout(&state, &layout_input, &layout),
        "narrow reference layout resolves exactly");
  check(layout.mode == ReaderViewLayout_SingleDocked &&
        layout.toolbar_density == ReaderViewToolbar_Compact &&
        rect_equal(layout.toolbar_rect, ui0_rect(0, 0, 940, 56)) &&
        rect_equal(layout.shared_toolbar_rect,
                   ui0_rect(462, 10, 420, 28)) &&
        rect_equal(layout.host_toolbar_trailing_rect,
                   ui0_rect(890, 10, 30, 28)) &&
        rect_equal(layout.body_rect, ui0_rect(0, 56, 940, 426)) &&
        rect_equal(layout.viewport_rect, ui0_rect(0, 56, 940, 426)) &&
        rect_equal(layout.page_surface_rect,
                   ui0_rect(140, 56, 660, 426)) &&
        rect_equal(layout.content_rect, ui0_rect(192, 124, 556, 290)) &&
        rect_equal(layout.progress_rect, ui0_rect(140, 500, 660, 18)),
        "narrow reference toolbar, page, content, and progress are exact");
  check(rect_equal(layout.previous_gutter_rect,
                   ui0_rect(4, 56, 136, 426)) &&
        rect_equal(layout.next_gutter_rect,
                   ui0_rect(800, 56, 136, 426)) &&
        rect_equal(layout.previous_gutter_visual_rect,
                   ui0_rect(50, 225, 44, 88)) &&
        rect_equal(layout.next_gutter_visual_rect,
                   ui0_rect(846, 225, 44, 88)),
        "narrow reference gutter geometry is exact");

  state.left_panel = ReaderViewLeftPanel_Contents;
  state.right_panel_open = 1;
  check(reader_view_resolve_layout(&state, &layout_input, &layout),
        "narrow two-panel reference edge resolves");
  check(rect_equal(layout.left_panel_rect,
                   ui0_rect(12, 56, 313, 426)) &&
        rect_equal(layout.right_panel_rect,
                   ui0_rect(693, 56, 235, 426)) &&
        rect_equal(layout.viewport_rect, ui0_rect(434, 56, 172, 426)) &&
        rect_equal(layout.page_surface_rect,
                   ui0_rect(458, 56, 160, 426)) &&
        rect_equal(layout.content_rect, ui0_rect(510, 124, 80, 290)) &&
        rect_equal(layout.progress_rect, ui0_rect(458, 500, 160, 18)),
        "narrow panel edge preserves exact old available-rect page math");
  check(rect_equal(layout.previous_gutter_rect,
                   ui0_rect(333, 56, 125, 426)) &&
        rect_equal(layout.next_gutter_rect,
                   ui0_rect(618, 56, 67, 426)) &&
        rect_equal(layout.previous_gutter_visual_rect,
                   ui0_rect(373, 225, 44, 88)) &&
        rect_equal(layout.next_gutter_visual_rect,
                   ui0_rect(629, 225, 44, 88)),
        "narrow panel edge preserves exact gutter math");
  state.left_panel = ReaderViewLeftPanel_None;
  state.right_panel_open = 0;

  layout_input.bounds = ui0_rect(10, 10, 580, 780);
  state.left_panel = ReaderViewLeftPanel_Find;
  state.right_panel_open = 1;
  check(reader_view_resolve_layout(&state, &layout_input, &layout),
        "minimum-toolbar two-panel host layout resolves");
  check(rect_equal(layout.left_panel_rect,
                   ui0_rect(22, 66, 300, 686)) &&
        rect_equal(layout.right_panel_rect,
                   ui0_rect(428, 66, 150, 686)) &&
        rect_equal(layout.viewport_rect, ui0_rect(444, 66, 168, 686)) &&
        rect_equal(layout.page_surface_rect,
                   ui0_rect(468, 66, 160, 686)) &&
        rect_equal(layout.content_rect, ui0_rect(520, 134, 80, 550)) &&
        rect_equal(layout.progress_rect, ui0_rect(468, 770, 160, 18)),
        "minimum-toolbar panels preserve the accepted minimum page math");
  check(rect_equal(layout.previous_gutter_rect,
                   ui0_rect(330, 66, 138, 686)) &&
        rect_equal(layout.next_gutter_rect,
                   ui0_rect(628, 66, 0, 686)),
        "minimum-toolbar panels preserve bounded old gutter behavior");
  state.left_panel = ReaderViewLeftPanel_None;
  state.right_panel_open = 0;

  layout_input.bounds = ui0_rect(11, 13, 497, 520);
  layout.bounds = ui0_rect(1, 2, 3, 4);
  check(!reader_view_resolve_layout(&state, &layout_input, &layout) &&
        rect_equal(layout.bounds, ui0_rect(0, 0, 0, 0)),
        "undersized fixed-toolbar layout fails closed");
  layout_input.bounds = ui0_rect(11, 13, 940, 277);
  layout.bounds = ui0_rect(1, 2, 3, 4);
  check(!reader_view_resolve_layout(&state, &layout_input, &layout) &&
        rect_equal(layout.bounds, ui0_rect(0, 0, 0, 0)),
        "undersized content-height layout fails closed");

  layout_input.bounds = ui0_rect(0, 0, 1400, 780);
  check(reader_view_resolve_layout(&state, &layout_input, &layout),
        "full reference layout restored");
  memset(&frame_input, 0, sizeof(frame_input));
  build_input.frame_index = 10;
  build_input.layout = &layout;
  build_input.projection = &projection;
  check(reader_view_build(&build_input, &storage, &frame), "full build");
  check(frame.draw_command_count > 0, "full draw records");
  check(frame.semantic_node_count > 10, "full semantic records");
  check(count_semantic(&frame, "Open") == 0,
        "loaded toolbar never exposes Open");
  check_icon_control(&frame, "Contents", ui0_rect(922, 10, 30, 28),
                     UI0IconKind_List,
                     "Contents icon control geometry and identity");
  check_icon_control(&frame, "Find", ui0_rect(960, 10, 30, 28),
                     UI0IconKind_Search,
                     "Find icon control geometry and identity");
  check_icon_control(&frame, "Back", ui0_rect(998, 10, 30, 28),
                     UI0IconKind_ArrowLeft,
                     "Back icon control geometry and identity");
  check_icon_control(&frame, "Forward", ui0_rect(1036, 10, 30, 28),
                     UI0IconKind_ArrowRight,
                     "Forward icon control geometry and identity");
  check_icon_control(&frame, "Full screen", ui0_rect(1084, 10, 30, 28),
                     UI0IconKind_Expand,
                     "Fullscreen icon control geometry and identity");
  check_icon_control(&frame, "Size", ui0_rect(1122, 10, 30, 28),
                     UI0IconKind_TextSize,
                     "Size icon control geometry and identity");
  check_icon_control(&frame, "Spacing", ui0_rect(1160, 10, 30, 28),
                     UI0IconKind_LineSpacing,
                     "Spacing icon control geometry and identity");
  check_icon_control(&frame, "Font", ui0_rect(1198, 10, 30, 28),
                     UI0IconKind_CaseSensitive,
                     "Font icon control geometry and identity");
  check_icon_control(&frame, "Theme", ui0_rect(1236, 10, 30, 28),
                     UI0IconKind_SunMoon,
                     "Theme icon control geometry and identity");
  check_icon_control(&frame, "Annotations", ui0_rect(1274, 10, 30, 28),
                     UI0IconKind_Notebook,
                     "Annotations icon control geometry and identity");
  check_icon_control(&frame, "Bookmark", ui0_rect(1312, 10, 30, 28),
                     UI0IconKind_Bookmark,
                     "Bookmark icon control geometry and identity");
  check_loaded_toolbar_icon_order(&frame);
  {
    struct SemanticControlExpectation
    {
      const char *name;
      ReaderViewSemanticControl control;
    } expectations[] = {
      {"Contents", ReaderViewSemanticControl_Contents},
      {"Find", ReaderViewSemanticControl_Find},
      {"Back", ReaderViewSemanticControl_HistoryBack},
      {"Forward", ReaderViewSemanticControl_HistoryForward},
      {"Full screen", ReaderViewSemanticControl_Fullscreen},
      {"Size", ReaderViewSemanticControl_FontSize},
      {"Spacing", ReaderViewSemanticControl_LineSpacing},
      {"Font", ReaderViewSemanticControl_FontFamily},
      {"Theme", ReaderViewSemanticControl_Theme},
      {"Annotations", ReaderViewSemanticControl_Annotations},
      {"Bookmark", ReaderViewSemanticControl_Bookmark},
    };
    UI0S32 expectation_index;
    for (expectation_index = 0;
         expectation_index < (UI0S32)(sizeof(expectations) /
                                      sizeof(expectations[0]));
         expectation_index += 1)
    {
      const ReaderViewSemanticNode *control_node =
        find_semantic(&frame, expectations[expectation_index].name);
      check(control_node != 0 &&
            control_node->control == expectations[expectation_index].control,
            "toolbar publishes stable portable control identity");
    }
  }
  node = find_semantic_role(&frame, "Reader toolbar",
                            ReaderViewSemantic_Toolbar);
  check(node != 0 &&
        find_draw_for_source(&frame, UI0DrawOp_ControlFill,
                             node ? node->id : 0) == 0 &&
        find_draw_for_source(&frame, UI0DrawOp_ControlBorder,
                             node ? node->id : 0) == 0,
        "reference toolbar is semantic-only without a painted band");
  node = find_semantic(&frame, "Contents");
  {
    const UI0ControlRecord *control = node ?
      find_control_for_source(&storage, node->id) : 0;
    check(control != 0 && control->kind == UI0ControlKind_IconButton &&
          (control->control_flags & UI0Control_Quiet) == 0,
          "reference toolbar buttons retain standard shells");
  }
  node = find_semantic(&frame, "EPUB Reader");
  check(node != 0 && rect_equal(node->rect, ui0_rect(20, 14, 180, 22)) &&
        find_text_binding(&frame, node ? node->id : 0) != 0 &&
        find_text_binding(&frame, node ? node->id : 0)->style ==
          ReaderViewTextStyle_ChromeTitle,
        "portable chrome title uses accepted reference geometry");
  check(find_semantic(&frame, "Host document title") == 0,
        "host document title is not substituted into visible chrome");
  first_hash = frame_contract_hash(&frame);
  check(reader_view_debug_snapshot(&projection, &storage, &frame,
                                   &debug_first),
        "debug snapshot builds");
  check(debug_first.projection_hash != 0 && debug_first.layout_hash != 0 &&
        debug_first.control_hash != 0 && debug_first.draw_hash != 0 &&
        debug_first.semantic_hash != 0 && debug_first.action_hash != 0,
        "debug hashes are nonzero");

  toc_rows[1].key = 999;
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "opaque-key normalization rebuild");
  check(reader_view_debug_snapshot(&projection, &storage, &frame,
                                   &debug_repeat),
        "opaque-key normalization snapshot");
  check(memcmp(&debug_first, &debug_repeat, sizeof(debug_first)) == 0,
        "opaque keys excluded from normalized snapshot");
  toc_rows[1].key = 21;

  projection.labels.contents.data = "Table of contents";
  projection.labels.contents.size = 17;
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "visible-change rebuild");
  check(reader_view_debug_snapshot(&projection, &storage, &frame,
                                   &debug_changed),
        "visible-change snapshot");
  check(debug_changed.projection_hash != debug_first.projection_hash &&
        debug_changed.control_hash == debug_first.control_hash &&
        debug_changed.draw_hash == debug_first.draw_hash &&
        debug_changed.semantic_hash != debug_first.semantic_hash,
        "accessible icon label changes semantic but not visual evidence");
  projection.labels.contents.data = 0;
  projection.labels.contents.size = 0;

  projection.labels.annotation_actions.data = "Localized actions";
  projection.labels.annotation_actions.size = 17;
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "appended panel-label hash rebuild");
  check(reader_view_debug_snapshot(&projection, &storage, &frame,
                                   &debug_changed) &&
        debug_changed.projection_hash != debug_first.projection_hash,
        "debug projection hash includes appended panel/native label fields");
  projection.labels.annotation_actions.data = 0;
  projection.labels.annotation_actions.size = 0;

  layout_input.bounds = ui0_rect(17, 23, 1400, 780);
  check(reader_view_resolve_layout(&state, &layout_input, &layout),
        "shifted-origin layout resolves");
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "shifted-origin rebuild");
  check(reader_view_debug_snapshot(&projection, &storage, &frame,
                                   &debug_shifted),
        "shifted-origin snapshot");
  if (memcmp(&debug_first, &debug_shifted, sizeof(debug_first)) != 0)
  {
    fprintf(stderr,
            "debug origin mismatch projection=%016llx/%016llx layout=%016llx/%016llx control=%016llx/%016llx draw=%016llx/%016llx semantic=%016llx/%016llx action=%016llx/%016llx counts=%d,%d,%d,%d/%d,%d,%d,%d\n",
            (unsigned long long)debug_first.projection_hash,
            (unsigned long long)debug_shifted.projection_hash,
            (unsigned long long)debug_first.layout_hash,
            (unsigned long long)debug_shifted.layout_hash,
            (unsigned long long)debug_first.control_hash,
            (unsigned long long)debug_shifted.control_hash,
            (unsigned long long)debug_first.draw_hash,
            (unsigned long long)debug_shifted.draw_hash,
            (unsigned long long)debug_first.semantic_hash,
            (unsigned long long)debug_shifted.semantic_hash,
            (unsigned long long)debug_first.action_hash,
            (unsigned long long)debug_shifted.action_hash,
            debug_first.control_record_count, debug_first.draw_command_count,
            debug_first.semantic_node_count, debug_first.action_count,
            debug_shifted.control_record_count, debug_shifted.draw_command_count,
            debug_shifted.semantic_node_count, debug_shifted.action_count);
  }
  check(memcmp(&debug_first, &debug_shifted, sizeof(debug_first)) == 0,
        "absolute client origin excluded from normalized snapshot");
  layout_input.bounds = ui0_rect(0, 0, 1400, 780);
  check(reader_view_resolve_layout(&state, &layout_input, &layout),
        "full layout restored after debug checks");
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "full frame restored after debug checks");

  node = find_semantic(&frame, "3 of 10");
  check(node != 0 && node->role == ReaderViewSemantic_Slider &&
        node->control == ReaderViewSemanticControl_Progress &&
        rect_equal(node->rect, ui0_rect(370, 760, 660, 18)),
        "progress semantic spans the exact page width");
  if (node)
  {
    UI0ID progress_id = node->id;
    const ReaderViewSemanticNode *next_node;
    check(count_draw_op_for_source(&frame, UI0DrawOp_SliderThumb,
                                   progress_id) == 0,
          "idle reference progress hides its thumb");
    check(reader_view_accessibility_focus(&state, progress_id),
          "progress accessibility focus queues");
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "progress accessibility focus build");
    node = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_Progress, 0);
    check(node && state.focus_id == progress_id && state.focus_visible &&
          (node->flags & ReaderViewSemantic_Focused) != 0 &&
          count_draw_op_for_source(&frame, UI0DrawOp_SliderThumb,
                                   progress_id) == 0 &&
          count_draw_op_for_source(&frame, UI0DrawOp_FocusRing,
                                   progress_id) == 0,
          "focused reference progress remains visually idle while semantic "
          "focus is published");

    next_node = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_NextPage, 0);
    check(next_node &&
          reader_view_accessibility_focus(&state, next_node->id),
          "next gutter focus queues before Tab traversal");
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "next gutter focus build before Tab traversal");
    memset(&frame_input, 0, sizeof(frame_input));
    frame_input.ui = ui0_input_keyboard(0, 1, 0);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "same-frame Tab traversal to progress builds");
    node = find_semantic_control_source(
      &frame, ReaderViewSemanticControl_Progress, 0);
    check(node && state.focus_id == progress_id && state.focus_visible &&
          (node->flags & ReaderViewSemantic_Focused) != 0 &&
          count_draw_op_for_source(&frame, UI0DrawOp_SliderThumb,
                                   progress_id) == 0 &&
          count_draw_op_for_source(&frame, UI0DrawOp_FocusRing,
                                   progress_id) == 0,
          "same-frame Tab focus preserves frozen hidden progress chrome");
    memset(&frame_input, 0, sizeof(frame_input));
    frame_input.move_horizontal_delta = 1;
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "focused progress keyboard seek build");
    action = find_action(&frame, ReaderViewAction_SeekLocation);
    check(action && action->value == 25 &&
          count_draw_op_for_source(&frame, UI0DrawOp_SliderThumb,
                                   progress_id) == 0 &&
          count_draw_op_for_source(&frame, UI0DrawOp_FocusRing,
                                   progress_id) == 0,
          "keyboard progress seek remains functional without idle thumb or "
          "focus-ring paint");
    memset(&frame_input, 0, sizeof(frame_input));
  }
  node = find_semantic_role(&frame, "3 of 10", ReaderViewSemantic_Status);
  {
    UI0SliderStyle progress_style = ui0_slider_style_from_resolved(&theme);
    UI0S32 footer_height = theme.typography[UI0TypographyRole_Body].line_height;
    UI0S32 footer_gap = theme.spacing[UI0SpacingRole_ControlGap];
    UI0S32 track_y = 760 +
      (18 - (progress_style.track_height < 18 ?
             progress_style.track_height : 18)) / 2;
    UI0Rect expected_footer =
      ui0_rect(370, track_y - footer_gap - footer_height,
               660, footer_height);
    const UI0DrawCommand *label = node ?
      find_draw_for_source(&frame, UI0DrawOp_Text, node->id) : 0;
    check(node != 0 && label != 0 &&
          rect_equal(label->rect, expected_footer) &&
          label->color == theme.colors[UI0ColorRole_TextMuted] &&
          find_text_binding(&frame, node ? node->id : 0) != 0 &&
          find_text_binding(&frame, node ? node->id : 0)->style ==
            ReaderViewTextStyle_ChromeMetadata,
          "progress footer uses exact reference placement and muted color");
  }

  node = find_semantic(&frame, "Previous page");
  check(node != 0 &&
        node->control == ReaderViewSemanticControl_PreviousPage &&
        rect_equal(node->rect, ui0_rect(4, 56, 366, 686)) &&
        count_draw_op_for_source(&frame, UI0DrawOp_Text,
                                 node ? node->id : 0) == 0 &&
        find_icon_for_source(&frame, node ? node->id : 0) == 0,
        "idle previous gutter exposes its full hot rect without paint");
  if (node)
  {
    UI0ID previous_id = node->id;
    frame_input.ui = ui0_input_pointer(10, 100, 1, 1, 0);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "previous gutter hot-area press build");
    check(find_icon_for_source(&frame, previous_id) != 0 &&
          find_icon_for_source(&frame, previous_id)->icon_kind ==
            UI0IconKind_PageCaretLeft,
          "previous gutter icon appears on interaction");
    frame_input.ui = ui0_input_pointer(10, 100, 0, 0, 1);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "previous gutter hot-area release build");
    check(find_action(&frame, ReaderViewAction_PreviousPage) != 0,
          "previous gutter large hot area emits paging action");
    memset(&frame_input, 0, sizeof(frame_input));
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "previous gutter interaction clears");
  }

  node = find_semantic(&frame, "Font");
  check(node != 0, "Font toolbar semantic present");
  if (node)
  {
    UI0Rect font_rect = node->rect;
    frame_input.ui = ui0_input_pointer(font_rect.x + font_rect.w / 2,
                                       font_rect.y + font_rect.h / 2,
                                       1, 1, 0);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "Font popup press build");
    frame_input.ui = ui0_input_pointer(font_rect.x + font_rect.w / 2,
                                       font_rect.y + font_rect.h / 2,
                                       0, 0, 1);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "Font popup release build");
    check(state.popup == ReaderViewPopup_SettingMenu &&
          state.active_setting_kind == ReaderViewSetting_FontFamily,
          "Font toolbar action opens bounded setting popup state");
    memset(&frame_input, 0, sizeof(frame_input));
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "Font popup visible build");
    node = find_semantic_role(&frame, "More", ReaderViewSemantic_Menu);
    check(node != 0 && rect_equal(node->rect,
                                  ui0_rect(1094, 44, 170, 188)),
          "Font popup is exactly anchored six pixels below Font");
    node = find_semantic_role(&frame, "Serif",
                              ReaderViewSemantic_MenuItem);
    check(node != 0 && rect_equal(node->rect,
                                  ui0_rect(1112, 54, 144, 32)),
          "Font popup first row uses accepted body geometry");
    check(node != 0 &&
          find_text_binding(&frame, node ? node->id : 0) != 0 &&
          find_text_binding(&frame, node ? node->id : 0)->style ==
            ReaderViewTextStyle_MenuItem,
          "Font popup row publishes its portable legacy text style");
    check(node != 0 && state.focus_id == node->id && !state.focus_visible,
          "mouse-opened Font popup initializes selected-row focus invisibly");
    if (node)
    {
      UI0Rect row_rect = node->rect;
      frame_input.ui = ui0_input_pointer(row_rect.x + row_rect.w / 2,
                                         row_rect.y + row_rect.h / 2,
                                         1, 1, 0);
      build_input.frame_index += 1;
      check(reader_view_build(&build_input, &storage, &frame),
            "Font choice press build");
      frame_input.ui = ui0_input_pointer(row_rect.x + row_rect.w / 2,
                                         row_rect.y + row_rect.h / 2,
                                         0, 0, 1);
      build_input.frame_index += 1;
      check(reader_view_build(&build_input, &storage, &frame),
            "Font choice release build");
      action = find_action(&frame, ReaderViewAction_SelectSetting);
      check(action != 0 && action->key == 10 &&
            action->setting_kind == ReaderViewSetting_FontFamily &&
            state.popup == ReaderViewPopup_None,
            "Font choice preserves bounded SelectSetting action contract");
    }
    memset(&frame_input, 0, sizeof(frame_input));
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "Font popup interaction clears");
  }

  node = find_semantic(&frame, "Contents");
  check(node != 0, "contents semantic present");
  if (node)
  {
    frame_input.ui = ui0_input_pointer(node->rect.x + node->rect.w / 2,
                                       node->rect.y + node->rect.h / 2,
                                       1, 1, 0);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "contents press build");
    frame_input.ui = ui0_input_pointer(node->rect.x + node->rect.w / 2,
                                       node->rect.y + node->rect.h / 2,
                                       0, 0, 1);
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "contents release build");
    check(state.left_panel == ReaderViewLeftPanel_Contents,
          "contents opens shared left panel");
  }

  check(reader_view_resolve_layout(&state, &layout_input, &layout),
        "contents panel layout resolves");
  memset(&frame_input, 0, sizeof(frame_input));
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "contents panel build");
  node = find_semantic_role(&frame, "One", ReaderViewSemantic_ListItem);
  check(node != 0 && node->role == ReaderViewSemantic_ListItem,
        "contents first row semantic present");
  if (node)
  {
    check(reader_view_accessibility_focus(&state, node->id),
          "contents first row focus queued");
    memset(&frame_input, 0, sizeof(frame_input));
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "contents first row focus build");
    frame_input.move_vertical_delta = 1;
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "contents arrow navigation build");
    node = find_semantic_role(&frame, "Two", ReaderViewSemantic_ListItem);
    check(node && (node->flags & ReaderViewSemantic_Focused),
          "contents arrow moves shared focus");
  }

  state.left_panel = ReaderViewLeftPanel_Find;
  check(reader_view_resolve_layout(&state, &layout_input, &layout),
        "find panel layout resolves");
  memset(&frame_input, 0, sizeof(frame_input));
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "find focused edit seed builds");
  node = find_semantic_control_source(
    &frame, ReaderViewSemanticControl_FindInput, 0);
  check(node != 0 && reader_view_accessibility_focus(&state, node->id),
        "find focused edit queues input focus");
  frame_input.find_text.text = "abc";
  frame_input.find_text.text_len = 3;
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "find edit build");
  action = find_action(&frame, ReaderViewAction_FindChanged);
  check(action != 0 && text_equal(action->text, "abc"),
        "find edit emits borrowed query action");

  memset(&frame_input, 0, sizeof(frame_input));
  state.left_panel = ReaderViewLeftPanel_None;
  check(reader_view_resolve_layout(&state, &layout_input, &layout),
        "accessibility layout resolves");
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "accessibility seed build");
  node = find_semantic(&frame, "Back");
  check(node != 0, "history back semantic present");
  if (node)
  {
    check(reader_view_accessibility_invoke(&state, node->id),
          "accessibility invoke queued");
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "accessibility invoke build");
    check(find_action(&frame, ReaderViewAction_HistoryBack) != 0,
          "accessibility invoke uses action path");
  }

  node = find_semantic(&frame, "3 of 10");
  check(node != 0 && node->role == ReaderViewSemantic_Slider,
        "progress slider semantic present");
  if (node)
  {
    check(reader_view_accessibility_focus(&state, node->id),
          "progress accessibility focus queued");
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "progress accessibility focus build");
    node = find_semantic(&frame, "3 of 10");
    check(node && (node->flags & ReaderViewSemantic_Focused),
          "progress accessibility focus published");
    frame_input.move_horizontal_delta = 10;
    build_input.frame_index += 1;
    check(reader_view_build(&build_input, &storage, &frame),
          "progress accessibility keyboard build");
    action = find_action(&frame, ReaderViewAction_SeekLocation);
    check(action != 0 && action->value > projection.progress.location_index,
          "focused progress keyboard emits seek action");
  }

  projection.selection.status = ready_status();
  projection.selection.selection_key = 50;
  projection.selection.revision = 7;
  projection.selection.flags = ReaderViewSelection_Active |
                               ReaderViewSelection_CanEditNote;
  projection.selection.note_text.data = "Existing note";
  projection.selection.note_text.size = 13;
  check(reader_view_open_note_editor(&state, &projection.selection),
        "host can request shared note editor for authoritative selection");
  memset(&frame_input, 0, sizeof(frame_input));
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "host-requested note editor build");
  check(state.popup == ReaderViewPopup_NoteEditor &&
        text_equal(reader_view_note_draft(&state), "Existing note"),
        "host-requested note editor owns bounded draft state");

  frame_input.note_text.text = "!";
  frame_input.note_text.text_len = 1;
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame) &&
        state.note_dirty &&
        text_equal(reader_view_note_draft(&state), "Existing note!"),
        "note editor records a bounded dirty draft");
  memset(&frame_input, 0, sizeof(frame_input));
  frame_input.escape_pressed = 1;
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame) &&
        state.popup == ReaderViewPopup_NoteEditor && state.note_dirty &&
        text_equal(reader_view_note_draft(&state), "Existing note!") &&
        find_action(&frame, ReaderViewAction_CancelNote) == 0,
        "dirty note Escape preserves the draft and requires explicit Cancel");
  memset(&frame_input, 0, sizeof(frame_input));
  node = find_semantic(&frame, "Cancel note");
  check(node != 0 && reader_view_accessibility_invoke(&state, node->id),
        "note Cancel accessibility invoke queues through the shared control");
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "explicit note Cancel build");
  action = find_action(&frame, ReaderViewAction_CancelNote);
  check(action != 0 && action->key == 50 && action->value == 7 &&
        frame.action_count == 1 &&
        state.popup == ReaderViewPopup_None && !state.note_dirty,
        "explicit note Cancel closes once and returns authoritative identity");
  {
    ReaderViewDebugSnapshot cancel_debug_a;
    ReaderViewDebugSnapshot cancel_debug_b;
    check(reader_view_debug_snapshot(&projection, &storage, &frame,
                                     &cancel_debug_a),
          "explicit note Cancel debug snapshot captures");
    storage.actions[0].value = 7007;
    check(reader_view_debug_snapshot(&projection, &storage, &frame,
                                     &cancel_debug_b) &&
          cancel_debug_a.action_hash == cancel_debug_b.action_hash,
          "explicit note Cancel normalizes opaque revision identity");
    storage.actions[0].value = 7;
  }

  memset(&projection.selection, 0, sizeof(projection.selection));
  projection.selection.status = ready_status();
  reader_view_state_reset_document(&state, projection.document_key);
  memset(&frame_input, 0, sizeof(frame_input));
  check(reader_view_resolve_layout(&state, &layout_input, &layout),
        "determinism layout resolves");
  build_input.frame_index = 10;
  check(reader_view_build(&build_input, &storage, &frame),
        "determinism rebuild");
  check(frame_contract_hash(&frame) == first_hash,
        "deterministic layout and semantic hash");

  toc_rows[1].key = toc_rows[0].key;
  build_input.frame_index += 1;
  check(!reader_view_build(&build_input, &storage, &frame),
        "duplicate projection rejected");
  check((frame.error_flags & ReaderViewFrameError_DuplicateKey) != 0,
        "duplicate projection error");

  if (failures)
  {
    fprintf(stderr, "readerview0 tests: %d failure(s)\n", failures);
    return 1;
  }
  puts("readerview0 tests: passed");
  return 0;
}

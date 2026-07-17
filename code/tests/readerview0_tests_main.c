#include <stdio.h>
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

static const UI0ControlRecord *
find_control_for_source(const ReaderViewFrameStorage *storage, UI0ID source_id)
{
  UI0S32 index;
  for (index = 0; index < READER_VIEW_CONTROL_CAP; ++index)
    if (storage->control_records[index].id == source_id)
      return storage->control_records + index;
  return 0;
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
    check(count_draw_op_for_source(&frame, UI0DrawOp_SliderThumb,
                                   node->id) == 0,
          "idle reference progress hides its thumb");
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
            UI0IconKind_ChevronLeft,
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

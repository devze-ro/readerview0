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
                ReaderViewChoice *font_choices,
                ReaderViewTocRow *toc_rows,
                ReaderViewFindRow *find_rows,
                ReaderViewRightRow *right_rows)
{
  ReaderViewProjection result = minimal_projection();
  memset(settings, 0, sizeof(*settings));
  memset(font_choices, 0, 2 * sizeof(*font_choices));
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
  result.progress.location_count = 100;
  result.progress.location_index = 24;
  result.progress.page_count = 10;
  result.progress.page_index = 2;
  result.progress.can_seek = 1;
  result.progress.chapter.data = "Chapter 1";
  result.progress.chapter.size = 9;
  result.progress.label.data = "3 of 10";
  result.progress.label.size = 7;

  font_choices[0].key = 10;
  font_choices[0].label.data = "Serif";
  font_choices[0].label.size = 5;
  font_choices[0].flags = ReaderViewChoice_Enabled |
                          ReaderViewChoice_Selected;
  font_choices[1].key = 11;
  font_choices[1].label.data = "Sans";
  font_choices[1].label.size = 4;
  font_choices[1].flags = ReaderViewChoice_Enabled;
  settings[0].kind = ReaderViewSetting_FontFamily;
  settings[0].label.data = "Font";
  settings[0].label.size = 4;
  settings[0].status = ready_status();
  settings[0].choices.items = font_choices;
  settings[0].choices.count = 2;
  settings[0].choices.presentation = ReaderViewChoicePresentation_Menu;
  result.settings.items = settings;
  result.settings.count = 1;

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
  ReaderViewSettingControl settings[1];
  ReaderViewChoice font_choices[2];
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

  check(READERVIEW0_API_VERSION == 2,
        "public API version");
  check(READERVIEW0_VERSION_MAJOR == 0 &&
        READERVIEW0_VERSION_MINOR == 2 &&
        READERVIEW0_VERSION_PATCH == 0 &&
        strcmp(READERVIEW0_VERSION_STRING, "0.2.0-dev") == 0,
        "public package version");
  check(READERVIEW0_UI0_REQUIRED_API_VERSION == 90 &&
        UI0_API_VERSION == 90,
        "exact UI0 API version");

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
  check(layout.toolbar_density == ReaderViewToolbar_Full,
        "full toolbar density");

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
  check(layout.toolbar_density == ReaderViewToolbar_Overflow,
        "narrow overflow density");

  projection = full_projection(settings, font_choices, toc_rows,
                               find_rows, right_rows);
  reader_view_state_reset_document(&state, projection.document_key);
  memset(&layout_input, 0, sizeof(layout_input));
  layout_input.bounds = ui0_rect(0, 0, 1280, 800);
  layout_input.features = projection.features;
  layout_input.document_flags = projection.document_flags;
  check(reader_view_resolve_layout(&state, &layout_input, &layout),
        "full layout resolves");
  memset(&frame_input, 0, sizeof(frame_input));
  build_input.frame_index = 10;
  build_input.layout = &layout;
  build_input.projection = &projection;
  check(reader_view_build(&build_input, &storage, &frame), "full build");
  check(frame.draw_command_count > 0, "full draw records");
  check(frame.semantic_node_count > 10, "full semantic records");
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

  projection.labels.open.data = "Open EPUB";
  projection.labels.open.size = 9;
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "visible-change rebuild");
  check(reader_view_debug_snapshot(&projection, &storage, &frame,
                                   &debug_changed),
        "visible-change snapshot");
  check(debug_changed.projection_hash != debug_first.projection_hash &&
        debug_changed.control_hash != debug_first.control_hash &&
        debug_changed.draw_hash != debug_first.draw_hash &&
        debug_changed.semantic_hash != debug_first.semantic_hash,
        "visible text changes normalized evidence");
  projection.labels.open.data = 0;
  projection.labels.open.size = 0;

  layout_input.bounds = ui0_rect(17, 23, 1280, 800);
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
  layout_input.bounds = ui0_rect(0, 0, 1280, 800);
  check(reader_view_resolve_layout(&state, &layout_input, &layout),
        "full layout restored after debug checks");
  build_input.frame_index += 1;
  check(reader_view_build(&build_input, &storage, &frame),
        "full frame restored after debug checks");

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

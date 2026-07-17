#include <string.h>
#include <stdint.h>

enum
{
  RV_TOOLBAR_HEIGHT = READER_VIEW_REFERENCE_TOP_CHROME_HEIGHT,
  RV_PROGRESS_HEIGHT = READER_VIEW_REFERENCE_FOOTER_HEIGHT,
  RV_PANEL_WIDTH = READER_VIEW_REFERENCE_LEFT_PANEL_WIDTH,
  RV_RIGHT_PANEL_WIDTH = READER_VIEW_REFERENCE_RIGHT_PANEL_WIDTH,
  RV_INSET = 12,
  RV_GAP = 8,
  RV_CONTROL_HEIGHT = 32,
  RV_ROW_HEIGHT = 58,
  RV_POPUP_WIDTH = 280,
  RV_NOTE_WIDTH = 512,
  RV_NOTE_HEIGHT = 360,
  RV_TOOLBAR_CONTROL_WIDTH = 30,
  RV_TOOLBAR_CONTROL_HEIGHT = 28,
  RV_TOOLBAR_CONTROL_GAP = 8,
  RV_TOOLBAR_MAJOR_GAP = 18,
  RV_TOOLBAR_CONTROL_COUNT = 12,
  RV_TOOLBAR_SHARED_SLOT_COUNT = 11,
  RV_TOOLBAR_HORIZONTAL_INSET = 20,
  RV_TOOLBAR_TOP_INSET = 10,
  RV_GUTTER_EDGE_INSET = 4,
  RV_GUTTER_PANEL_GAP = 8,
  RV_GUTTER_VISUAL_WIDTH = 44,
  RV_GUTTER_VISUAL_HEIGHT = 88,
  RV_PROGRESS_BOTTOM_INSET = 20,
  RV_PROGRESS_HIT_HEIGHT = 18,
  RV_FONT_POPUP_ANCHOR_X = 86,
  RV_FONT_POPUP_BODY_WIDTH = 144,
  RV_FONT_POPUP_GAP = 6,
  RV_FONT_POPUP_CHOICE_CAP = 5,
  RV_ICON_RECORD_CAP = 64,
};

typedef struct RVIconRecord
{
  UI0S32 control_index;
  UI0IconKind icon_kind;
  UI0Rect rect;
  UI0B32 visible;
} RVIconRecord;

typedef struct RVBuildContext
{
  const ReaderViewBuildInput *input;
  ReaderViewFrameStorage *storage;
  ReaderViewFrame *frame;
  UI0SignalContext signals;
  UI0DrawContext draw;
  UI0SliderContext sliders;
  UI0ScrollContext scrolls;
  UI0S32 control_count;
  UI0S32 text_count;
  UI0S32 semantic_count;
  UI0S32 action_count;
  UI0S32 icon_count;
  RVIconRecord icons[RV_ICON_RECORD_CAP];
  UI0ID toolbar_id;
  UI0ID left_panel_id;
  UI0ID right_panel_id;
  UI0ID popup_id;
  UI0ID modal_id;
  UI0B32 progress_thumb_visible;
} RVBuildContext;

static ReaderViewText
rv_text(const char *data, UI0S32 size)
{
  ReaderViewText result;
  result.data = data;
  result.size = size;
  return result;
}

static ReaderViewText
rv_literal(const char *text)
{
  return rv_text(text, text ? (UI0S32)strlen(text) : 0);
}

static UI0B32
rv_text_valid(ReaderViewText text)
{
  return text.size >= 0 && (text.size == 0 || text.data != 0);
}

static UI0U64
rv_hash_bytes(UI0U64 hash, const void *data, UI0U32 size)
{
  const unsigned char *bytes = (const unsigned char *)data;
  UI0U64 result = hash ? hash : 1469598103934665603ull;
  UI0U32 index;
  for (index = 0; index < size; ++index)
  {
    result ^= (UI0U64)bytes[index];
    result *= 1099511628211ull;
  }
  return result ? result : 1ull;
}

static UI0U64
rv_text_hash(ReaderViewText text)
{
  return rv_hash_bytes(1469598103934665603ull,
                       text.data,
                       (UI0U32)text.size);
}

static UI0ID
rv_id(UI0U64 tag, ReaderViewKey key)
{
  UI0U64 hash = rv_hash_bytes(1469598103934665603ull,
                              &tag,
                              (UI0U32)sizeof(tag));
  return rv_hash_bytes(hash, &key, (UI0U32)sizeof(key));
}

static UI0S32
rv_max(UI0S32 a, UI0S32 b)
{
  return a > b ? a : b;
}

static UI0S32
rv_min(UI0S32 a, UI0S32 b)
{
  return a < b ? a : b;
}

static UI0S32
rv_clamp(UI0S32 value, UI0S32 low, UI0S32 high)
{
  if (value < low) return low;
  if (value > high) return high;
  return value;
}

static UI0B32
rv_i64_fits_s32(int64_t value)
{
  return value >= INT32_MIN && value <= INT32_MAX;
}

static UI0Rect
rv_rect(UI0S32 x, UI0S32 y, UI0S32 w, UI0S32 h)
{
  UI0Rect result;
  result.x = x;
  result.y = y;
  result.w = rv_max(w, 0);
  result.h = rv_max(h, 0);
  return result;
}

static UI0B32
rv_rect_contains(UI0Rect outer, UI0Rect inner)
{
  int64_t outer_right = (int64_t)outer.x + outer.w;
  int64_t outer_bottom = (int64_t)outer.y + outer.h;
  int64_t inner_right = (int64_t)inner.x + inner.w;
  int64_t inner_bottom = (int64_t)inner.y + inner.h;
  return outer.w >= 0 && outer.h >= 0 && inner.w >= 0 && inner.h >= 0 &&
         inner.x >= outer.x && inner.y >= outer.y &&
         inner_right <= outer_right && inner_bottom <= outer_bottom;
}

static UI0B32
rv_has_feature(const ReaderViewProjection *projection,
               ReaderViewFeatureFlags feature)
{
  return (projection->features & feature) != 0;
}

static UI0B32
rv_has_document_flag(const ReaderViewProjection *projection,
                     ReaderViewDocumentFlags flag)
{
  return (projection->document_flags & flag) != 0;
}

static void
rv_history_init(UI0TextInputHistory *history,
                UI0TextInputHistoryEdit *edits,
                UI0U64 edit_cap,
                char *text,
                UI0U64 text_cap,
                char *scratch,
                UI0U64 scratch_cap)
{
  memset(history, 0, sizeof(*history));
  history->edits = edits;
  history->edit_cap = edit_cap;
  history->text_storage = text;
  history->text_cap = text_cap;
  history->scratch_storage = scratch;
  history->scratch_cap = scratch_cap;
}

ReaderViewLabels
reader_view_default_english_labels(void)
{
  ReaderViewLabels labels;
  memset(&labels, 0, sizeof(labels));
  labels.open = rv_literal("Open");
  labels.previous_page = rv_literal("Previous page");
  labels.next_page = rv_literal("Next page");
  labels.back = rv_literal("Back");
  labels.forward = rv_literal("Forward");
  labels.contents = rv_literal("Contents");
  labels.find = rv_literal("Find");
  labels.reading_settings = rv_literal("Reading settings");
  labels.bookmark = rv_literal("Bookmark");
  labels.remove_bookmark = rv_literal("Remove bookmark");
  labels.annotations = rv_literal("Annotations and bookmarks");
  labels.fullscreen = rv_literal("Full screen");
  labels.exit_fullscreen = rv_literal("Exit full screen");
  labels.distraction_free = rv_literal("Distraction-free reading");
  labels.close = rv_literal("Close");
  labels.clear = rv_literal("Clear");
  labels.previous_match = rv_literal("Previous match");
  labels.next_match = rv_literal("Next match");
  labels.export_rows = rv_literal("Export");
  labels.all = rv_literal("All");
  labels.bookmarks = rv_literal("Bookmarks");
  labels.highlights = rv_literal("Highlights");
  labels.notes = rv_literal("Notes");
  labels.go_to = rv_literal("Go to");
  labels.star = rv_literal("Star");
  labels.unstar = rv_literal("Unstar");
  labels.edit_note = rv_literal("Edit note");
  labels.save_note = rv_literal("Save note");
  labels.cancel = rv_literal("Cancel");
  labels.delete_value = rv_literal("Delete");
  labels.copy = rv_literal("Copy");
  labels.dictionary = rv_literal("Dictionary");
  labels.web_lookup = rv_literal("Web lookup");
  labels.translate = rv_literal("Translate");
  labels.more = rv_literal("More");
  return labels;
}

void
reader_view_state_init(ReaderViewState *state)
{
  if (!state) return;
  memset(state, 0, sizeof(*state));
  state->right_filter = ReaderViewRightFilter_All;
  state->active_setting_kind = ReaderViewSetting_FontFamily;
  ui0_scroll_state_init(&state->toc_scroll);
  ui0_scroll_state_init(&state->find_scroll);
  ui0_scroll_state_init(&state->right_scroll);
  ui0_scroll_state_init(&state->settings_scroll);
  ui0_text_input_state_init(&state->find_input);
  ui0_text_area_state_init(&state->note_input);
  rv_history_init(&state->find_history,
                  state->find_history_edits,
                  READER_VIEW_FIND_HISTORY_EDIT_CAP,
                  state->find_history_text,
                  READER_VIEW_FIND_HISTORY_TEXT_CAP,
                  state->find_history_scratch,
                  READER_VIEW_FIND_QUERY_CAP);
  rv_history_init(&state->note_history,
                  state->note_history_edits,
                  READER_VIEW_NOTE_HISTORY_EDIT_CAP,
                  state->note_history_text,
                  READER_VIEW_NOTE_HISTORY_TEXT_CAP,
                  state->note_history_scratch,
                  READER_VIEW_NOTE_DRAFT_CAP);
  state->find_input.history = &state->find_history;
  state->note_input.history = &state->note_history;
}

void
reader_view_state_reset_document(ReaderViewState *state, UI0U64 document_key)
{
  if (!state) return;
  reader_view_state_init(state);
  state->document_key = document_key;
}

ReaderViewText
reader_view_find_query(const ReaderViewState *state)
{
  if (!state) return rv_text(0, 0);
  return rv_text(state->find_query, state->find_query_length);
}

ReaderViewText
reader_view_note_draft(const ReaderViewState *state)
{
  if (!state) return rv_text(0, 0);
  return rv_text(state->note_draft, state->note_draft_length);
}

void
reader_view_frame_storage_init(ReaderViewFrameStorage *storage)
{
  if (storage) memset(storage, 0, sizeof(*storage));
}

UI0B32
reader_view_accessibility_focus(ReaderViewState *state, UI0ID semantic_id)
{
  if (!state || semantic_id == 0) return 0;
  state->pending_accessibility_focus_id = semantic_id;
  return 1;
}

UI0B32
reader_view_accessibility_invoke(ReaderViewState *state, UI0ID semantic_id)
{
  if (!state || semantic_id == 0) return 0;
  state->pending_accessibility_invoke_id = semantic_id;
  return 1;
}

static UI0S32
rv_toolbar_total_width(void)
{
  return RV_TOOLBAR_CONTROL_COUNT * RV_TOOLBAR_CONTROL_WIDTH +
         (RV_TOOLBAR_CONTROL_COUNT - 2) * RV_TOOLBAR_CONTROL_GAP +
         RV_TOOLBAR_MAJOR_GAP;
}

static UI0Rect
rv_toolbar_slot_rect(const ReaderViewLayout *layout, UI0S32 slot)
{
  UI0S32 x;
  if (!layout || slot < 0 || slot >= RV_TOOLBAR_CONTROL_COUNT ||
      !layout->toolbar_visible)
  {
    return rv_rect(0, 0, 0, 0);
  }
  x = layout->shared_toolbar_rect.x +
      slot * (RV_TOOLBAR_CONTROL_WIDTH + RV_TOOLBAR_CONTROL_GAP);
  if (slot >= 4) x += RV_TOOLBAR_MAJOR_GAP - RV_TOOLBAR_CONTROL_GAP;
  return rv_rect(x,
                 layout->toolbar_rect.y + RV_TOOLBAR_TOP_INSET,
                 RV_TOOLBAR_CONTROL_WIDTH,
                 RV_TOOLBAR_CONTROL_HEIGHT);
}

static UI0Rect
rv_gutter_visual_rect(UI0Rect hot_rect)
{
  UI0S32 width = rv_min(RV_GUTTER_VISUAL_WIDTH, hot_rect.w);
  UI0S32 height = rv_min(RV_GUTTER_VISUAL_HEIGHT, hot_rect.h);
  return rv_rect(hot_rect.x + (hot_rect.w - width) / 2,
                 hot_rect.y + (hot_rect.h - height) / 2,
                 width,
                 height);
}

UI0B32
reader_view_resolve_layout(const ReaderViewState *state,
                           const ReaderViewLayoutInput *input,
                           ReaderViewLayout *out_layout)
{
  enum { RV_REFERENCE_MIN_AVAILABLE_WIDTH = 120 };
  ReaderViewLayout result;
  ReaderViewContentGeometry geometry;
  ReaderViewContentGeometryStyle geometry_style;
  UI0Rect available_rect;
  UI0B32 chrome_hidden;
  UI0B32 paging_visible;
  UI0S32 toolbar_start_x;
  UI0S32 body_y;
  UI0S32 body_h;
  UI0S32 left_panel_w;
  UI0S32 right_panel_w;
  UI0S32 left_reserve;
  UI0S32 right_reserve;
  UI0S32 viewport_w;
  UI0S32 previous_limit;
  UI0S32 next_limit;
  int64_t bounds_right;
  int64_t bounds_bottom;
  if (out_layout) memset(out_layout, 0, sizeof(*out_layout));
  if (!state || !input || !out_layout || input->bounds.w <= 0 ||
      input->bounds.h <= 0 || input->host_toolbar_leading_width < 0 ||
      input->host_toolbar_trailing_width < 0)
  {
    return 0;
  }

  bounds_right = (int64_t)input->bounds.x + input->bounds.w;
  bounds_bottom = (int64_t)input->bounds.y + input->bounds.h;
  if (!rv_i64_fits_s32(bounds_right) || !rv_i64_fits_s32(bounds_bottom) ||
      !rv_i64_fits_s32((int64_t)input->bounds.x +
                       RV_TOOLBAR_HORIZONTAL_INSET +
                       rv_toolbar_total_width()) ||
      !rv_i64_fits_s32((int64_t)input->bounds.y + RV_TOOLBAR_HEIGHT))
  {
    return 0;
  }

  memset(&result, 0, sizeof(result));
  memset(&geometry, 0, sizeof(geometry));
  result.bounds = input->bounds;
  chrome_hidden = (input->document_flags &
                   (ReaderViewDocument_Fullscreen |
                    ReaderViewDocument_DistractionFree)) != 0;
  if ((!chrome_hidden &&
       (input->bounds.w < rv_toolbar_total_width() +
                          2 * RV_TOOLBAR_HORIZONTAL_INSET ||
        input->bounds.h < RV_TOOLBAR_HEIGHT + RV_PROGRESS_HEIGHT +
                          2 * READER_VIEW_DEFAULT_CONTENT_INSET_Y +
                          READER_VIEW_DEFAULT_CONTENT_MIN_HEIGHT ||
        input->host_toolbar_leading_width >
          input->bounds.w - 2 * RV_TOOLBAR_HORIZONTAL_INSET)) ||
      (chrome_hidden &&
       (input->bounds.w < READER_VIEW_DEFAULT_PAGE_MIN_WIDTH ||
        input->bounds.h < 2 * READER_VIEW_DEFAULT_CONTENT_INSET_Y +
                          READER_VIEW_DEFAULT_CONTENT_MIN_HEIGHT)))
  {
    return 0;
  }
  result.toolbar_visible = !chrome_hidden;
  result.progress_visible = !chrome_hidden &&
    (input->features & ReaderViewFeature_Progress) != 0 &&
    (input->document_flags & ReaderViewDocument_Open) != 0;
  result.left_panel_visible = !chrome_hidden &&
    state->left_panel != ReaderViewLeftPanel_None;
  result.right_panel_visible = !chrome_hidden && state->right_panel_open;

  if (input->bounds.w >= 1180)
    result.mode = ReaderViewLayout_WideDocked;
  else if (input->bounds.w >= 840)
    result.mode = ReaderViewLayout_SingleDocked;
  else
    result.mode = ReaderViewLayout_Overlay;
  result.toolbar_density = ReaderViewToolbar_Compact;

  result.toolbar_rect = result.toolbar_visible ?
    rv_rect(input->bounds.x, input->bounds.y, input->bounds.w, RV_TOOLBAR_HEIGHT) :
    rv_rect(0, 0, 0, 0);
  toolbar_start_x = rv_max(input->bounds.x + RV_TOOLBAR_HORIZONTAL_INSET,
                           (UI0S32)bounds_right -
                           RV_TOOLBAR_HORIZONTAL_INSET -
                           rv_toolbar_total_width());
  if (result.toolbar_visible)
  {
    result.shared_toolbar_rect = rv_rect(
      toolbar_start_x,
      input->bounds.y + RV_TOOLBAR_TOP_INSET,
      (RV_TOOLBAR_SHARED_SLOT_COUNT - 1) *
        (RV_TOOLBAR_CONTROL_WIDTH + RV_TOOLBAR_CONTROL_GAP) +
        (RV_TOOLBAR_MAJOR_GAP - RV_TOOLBAR_CONTROL_GAP) +
        RV_TOOLBAR_CONTROL_WIDTH,
      RV_TOOLBAR_CONTROL_HEIGHT);
    result.host_toolbar_leading_rect = rv_rect(
      input->bounds.x + RV_TOOLBAR_HORIZONTAL_INSET,
      input->bounds.y + RV_TOOLBAR_TOP_INSET,
      input->host_toolbar_leading_width,
      RV_TOOLBAR_CONTROL_HEIGHT);
    if (input->host_toolbar_trailing_width > 0)
      result.host_toolbar_trailing_rect = rv_toolbar_slot_rect(
        &result, RV_TOOLBAR_CONTROL_COUNT - 1);
  }

  body_y = chrome_hidden ? input->bounds.y :
    input->bounds.y + RV_TOOLBAR_HEIGHT;
  body_h = chrome_hidden ? input->bounds.h :
    rv_max(96, input->bounds.h - RV_TOOLBAR_HEIGHT - RV_PROGRESS_HEIGHT);
  if (!rv_i64_fits_s32((int64_t)body_y + body_h)) return 0;
  result.body_rect = rv_rect(input->bounds.x, body_y, input->bounds.w, body_h);

  left_panel_w = rv_min(RV_PANEL_WIDTH, rv_max(300, input->bounds.w / 3));
  right_panel_w = rv_min(RV_RIGHT_PANEL_WIDTH, rv_max(150, input->bounds.w / 4));
  if (result.left_panel_visible)
    result.left_panel_rect = rv_rect(
      input->bounds.x + READER_VIEW_REFERENCE_PANEL_INSET,
      body_y,
      left_panel_w,
      body_h);
  if (result.right_panel_visible)
    result.right_panel_rect = rv_rect(
      (UI0S32)bounds_right - right_panel_w -
        READER_VIEW_REFERENCE_PANEL_INSET,
      body_y,
      right_panel_w,
      body_h);

  left_reserve = result.left_panel_visible ?
    RV_PANEL_WIDTH + READER_VIEW_REFERENCE_PANEL_PAGE_GAP : 0;
  right_reserve = result.right_panel_visible ?
    RV_RIGHT_PANEL_WIDTH + READER_VIEW_REFERENCE_PANEL_PAGE_GAP : 0;
  viewport_w = rv_max(2 * READER_VIEW_DEFAULT_PAGE_HORIZONTAL_INSET +
                      RV_REFERENCE_MIN_AVAILABLE_WIDTH,
                      input->bounds.w - left_reserve - right_reserve);
  result.viewport_rect = rv_rect(input->bounds.x + left_reserve,
                                 body_y,
                                 viewport_w,
                                 body_h);
  geometry_style = reader_view_default_content_geometry_style();
  geometry_style.page_horizontal_inset = 0;
  available_rect = rv_rect(input->bounds.x + left_reserve +
                             READER_VIEW_DEFAULT_PAGE_HORIZONTAL_INSET,
                           body_y,
                           rv_max(RV_REFERENCE_MIN_AVAILABLE_WIDTH,
                                  input->bounds.w - left_reserve -
                                    right_reserve -
                                    2 * READER_VIEW_DEFAULT_PAGE_HORIZONTAL_INSET),
                           body_h);
  if (!reader_view_resolve_content_geometry(available_rect,
                                            &geometry_style,
                                            &geometry))
  {
    return 0;
  }
  result.page_surface_rect = geometry.page_surface_rect;
  result.content_rect = geometry.content_rect;

  paging_visible = (input->features & ReaderViewFeature_Paging) != 0 &&
    (input->document_flags & ReaderViewDocument_Open) != 0;
  result.previous_gutter_visible = paging_visible;
  result.next_gutter_visible = paging_visible;
  if (paging_visible)
  {
    previous_limit = input->bounds.x + RV_GUTTER_EDGE_INSET;
    if (result.left_panel_visible)
      previous_limit = rv_max(previous_limit,
                              result.left_panel_rect.x +
                              result.left_panel_rect.w +
                              RV_GUTTER_PANEL_GAP);
    next_limit = (UI0S32)bounds_right - RV_GUTTER_EDGE_INSET;
    if (result.right_panel_visible)
      next_limit = rv_min(next_limit,
                          result.right_panel_rect.x - RV_GUTTER_PANEL_GAP);
    result.previous_gutter_rect = rv_rect(
      previous_limit,
      result.page_surface_rect.y,
      result.page_surface_rect.x - previous_limit,
      result.page_surface_rect.h);
    result.next_gutter_rect = rv_rect(
      result.page_surface_rect.x + result.page_surface_rect.w,
      result.page_surface_rect.y,
      next_limit - (result.page_surface_rect.x + result.page_surface_rect.w),
      result.page_surface_rect.h);
    result.previous_gutter_visual_rect =
      rv_gutter_visual_rect(result.previous_gutter_rect);
    result.next_gutter_visual_rect =
      rv_gutter_visual_rect(result.next_gutter_rect);
  }

  if (result.progress_visible)
    result.progress_rect = rv_rect(result.page_surface_rect.x,
                                   (UI0S32)bounds_bottom -
                                     RV_PROGRESS_BOTTOM_INSET,
                                   result.page_surface_rect.w,
                                   RV_PROGRESS_HIT_HEIGHT);
  if ((result.toolbar_visible &&
       (!rv_rect_contains(result.bounds, result.toolbar_rect) ||
        !rv_rect_contains(result.toolbar_rect, result.shared_toolbar_rect) ||
        !rv_rect_contains(result.toolbar_rect,
                          result.host_toolbar_leading_rect) ||
        (input->host_toolbar_trailing_width > 0 &&
         !rv_rect_contains(result.toolbar_rect,
                           result.host_toolbar_trailing_rect)))) ||
      !rv_rect_contains(result.bounds, result.body_rect) ||
      (result.left_panel_visible &&
       !rv_rect_contains(result.body_rect, result.left_panel_rect)) ||
      (result.right_panel_visible &&
       !rv_rect_contains(result.body_rect, result.right_panel_rect)))
  {
    return 0;
  }
  *out_layout = result;
  return 1;
}

ReaderViewContentGeometryStyle
reader_view_default_content_geometry_style(void)
{
  ReaderViewContentGeometryStyle result;
  result.page_horizontal_inset = READER_VIEW_DEFAULT_PAGE_HORIZONTAL_INSET;
  result.page_max_width = READER_VIEW_DEFAULT_PAGE_MAX_WIDTH;
  result.page_min_width = READER_VIEW_DEFAULT_PAGE_MIN_WIDTH;
  result.content_inset_x = READER_VIEW_DEFAULT_CONTENT_INSET_X;
  result.content_inset_y = READER_VIEW_DEFAULT_CONTENT_INSET_Y;
  result.content_min_width = READER_VIEW_DEFAULT_CONTENT_MIN_WIDTH;
  result.content_min_height = READER_VIEW_DEFAULT_CONTENT_MIN_HEIGHT;
  return result;
}

UI0B32
reader_view_resolve_content_geometry(
  UI0Rect viewport_rect,
  const ReaderViewContentGeometryStyle *style,
  ReaderViewContentGeometry *out_geometry)
{
  ReaderViewContentGeometry result;
  ReaderViewContentGeometryStyle resolved_style =
    style ? *style : reader_view_default_content_geometry_style();
  int64_t available_width;
  int64_t page_width;
  int64_t page_x;
  int64_t content_x;
  int64_t content_y;
  int64_t content_width;
  int64_t content_height;

  memset(&result, 0, sizeof(result));
  if (out_geometry)
  {
    *out_geometry = result;
  }
  if (!out_geometry || viewport_rect.w <= 0 || viewport_rect.h <= 0 ||
      resolved_style.page_horizontal_inset < 0 ||
      resolved_style.page_max_width <= 0 ||
      resolved_style.page_min_width <= 0 ||
      resolved_style.page_min_width > resolved_style.page_max_width ||
      resolved_style.content_inset_x < 0 ||
      resolved_style.content_inset_y < 0 ||
      resolved_style.content_min_width <= 0 ||
      resolved_style.content_min_height <= 0)
  {
    return 0;
  }

  available_width = (int64_t)viewport_rect.w -
    2ll * resolved_style.page_horizontal_inset;
  page_width = available_width > resolved_style.page_min_width ?
    available_width : resolved_style.page_min_width;
  if (page_width > resolved_style.page_max_width)
  {
    page_width = resolved_style.page_max_width;
  }
  page_x = (int64_t)viewport_rect.x;
  if ((int64_t)viewport_rect.w > page_width)
  {
    page_x += ((int64_t)viewport_rect.w - page_width) / 2;
  }
  content_x = page_x + resolved_style.content_inset_x;
  content_y = (int64_t)viewport_rect.y + resolved_style.content_inset_y;
  content_width = page_width - 2ll * resolved_style.content_inset_x;
  if (content_width < resolved_style.content_min_width)
  {
    content_width = resolved_style.content_min_width;
  }
  content_height = (int64_t)viewport_rect.h -
    2ll * resolved_style.content_inset_y;
  if (content_height < resolved_style.content_min_height)
  {
    content_height = resolved_style.content_min_height;
  }
  if (!rv_i64_fits_s32(page_x) || !rv_i64_fits_s32(page_width) ||
      !rv_i64_fits_s32(content_x) || !rv_i64_fits_s32(content_y) ||
      !rv_i64_fits_s32(content_width) || !rv_i64_fits_s32(content_height))
  {
    return 0;
  }

  result.viewport_rect = viewport_rect;
  result.page_surface_rect = rv_rect((UI0S32)page_x,
                                     viewport_rect.y,
                                     (UI0S32)page_width,
                                     viewport_rect.h);
  result.content_rect = rv_rect(
    (UI0S32)content_x,
    (UI0S32)content_y,
    (UI0S32)content_width,
    (UI0S32)content_height);
  *out_geometry = result;
  return 1;
}

static UI0B32
rv_key_unique(ReaderViewKey key, const ReaderViewKey *keys, UI0S32 count)
{
  UI0S32 index;
  if (key == 0) return 0;
  for (index = 0; index < count; ++index)
    if (keys[index] == key) return 0;
  return 1;
}

static void
rv_validate_text(ReaderViewText text, ReaderViewFrameErrorFlags *errors)
{
  if (!rv_text_valid(text)) *errors |= ReaderViewFrameError_InvalidText;
}

static void
rv_validate_status(ReaderViewSurfaceStatus status,
                   ReaderViewFrameErrorFlags *errors)
{
  if (status.state < ReaderViewLoad_Unavailable ||
      status.state > ReaderViewLoad_Error)
    *errors |= ReaderViewFrameError_BadInput;
  rv_validate_text(status.message, errors);
  rv_validate_text(status.detail, errors);
}

static void
rv_validate_choices(const ReaderViewChoiceControl *control,
                    ReaderViewFrameErrorFlags *errors)
{
  ReaderViewKey keys[READER_VIEW_CHOICE_CAP];
  UI0S32 index;
  UI0S32 selected_count = 0;
  if (!control || control->count < 0 || control->count > READER_VIEW_CHOICE_CAP ||
      (control->count > 0 && !control->items))
  {
    *errors |= ReaderViewFrameError_ProjectionCap;
    return;
  }
  if (control->presentation < ReaderViewChoicePresentation_Stepper ||
      control->presentation > ReaderViewChoicePresentation_Menu)
    *errors |= ReaderViewFrameError_InvalidChoice;
  for (index = 0; index < control->count; ++index)
  {
    const ReaderViewChoice *choice = control->items + index;
    if (!rv_key_unique(choice->key, keys, index))
      *errors |= ReaderViewFrameError_DuplicateKey;
    keys[index] = choice->key;
    rv_validate_text(choice->label, errors);
    rv_validate_text(choice->detail, errors);
    if ((choice->flags & ReaderViewChoice_Selected) != 0) selected_count += 1;
  }
  if (selected_count > 1) *errors |= ReaderViewFrameError_InvalidChoice;
}

static ReaderViewFrameErrorFlags
rv_validate_projection(const ReaderViewProjection *projection)
{
  ReaderViewFrameErrorFlags errors = ReaderViewFrameError_None;
  ReaderViewKey keys[READER_VIEW_RIGHT_ROW_CAP];
  UI0B32 setting_seen[ReaderViewSetting_Count];
  UI0S32 index;
  if (!projection)
    return ReaderViewFrameError_BadInput;

  memset(setting_seen, 0, sizeof(setting_seen));
  rv_validate_status(projection->content, &errors);
  rv_validate_text(projection->chrome_title, &errors);
  rv_validate_text(projection->document_title, &errors);
  rv_validate_status(projection->progress.status, &errors);
  rv_validate_text(projection->progress.chapter, &errors);
  rv_validate_text(projection->progress.label, &errors);
  if ((projection->progress.location_count == 0 &&
       projection->progress.location_index != 0) ||
      (projection->progress.location_count > 0 &&
       projection->progress.location_index >= projection->progress.location_count) ||
      (projection->progress.page_count == 0 &&
       projection->progress.page_index != 0) ||
      (projection->progress.page_count > 0 &&
       projection->progress.page_index >= projection->progress.page_count))
    errors |= ReaderViewFrameError_InvalidProgress;

  rv_validate_status(projection->settings.status, &errors);
  if (projection->settings.count < 0 ||
      projection->settings.count > READER_VIEW_SETTING_CAP ||
      (projection->settings.count > 0 && !projection->settings.items))
    errors |= ReaderViewFrameError_ProjectionCap;
  else
  {
    for (index = 0; index < projection->settings.count; ++index)
    {
      const ReaderViewSettingControl *setting = projection->settings.items + index;
      if (setting->kind < ReaderViewSetting_FontFamily ||
          setting->kind >= ReaderViewSetting_Count || setting_seen[setting->kind])
        errors |= ReaderViewFrameError_InvalidChoice;
      else
        setting_seen[setting->kind] = 1;
      rv_validate_text(setting->label, &errors);
      rv_validate_text(setting->help, &errors);
      rv_validate_status(setting->status, &errors);
      rv_validate_choices(&setting->choices, &errors);
    }
  }

  rv_validate_status(projection->toc.status, &errors);
  if (projection->toc.row_count < 0 ||
      projection->toc.row_count > READER_VIEW_TOC_ROW_CAP ||
      (projection->toc.row_count > 0 && !projection->toc.rows))
    errors |= ReaderViewFrameError_ProjectionCap;
  else
  {
    for (index = 0; index < projection->toc.row_count; ++index)
    {
      const ReaderViewTocRow *row = projection->toc.rows + index;
      if (!rv_key_unique(row->key, keys, index))
        errors |= ReaderViewFrameError_DuplicateKey;
      keys[index] = row->key;
      rv_validate_text(row->label, &errors);
      rv_validate_text(row->detail, &errors);
    }
  }

  rv_validate_status(projection->find.status, &errors);
  rv_validate_text(projection->find.committed_query, &errors);
  if (projection->find.row_count < 0 ||
      projection->find.row_count > READER_VIEW_FIND_ROW_CAP ||
      (projection->find.row_count > 0 && !projection->find.rows))
    errors |= ReaderViewFrameError_ProjectionCap;
  else
  {
    for (index = 0; index < projection->find.row_count; ++index)
    {
      const ReaderViewFindRow *row = projection->find.rows + index;
      if (!rv_key_unique(row->key, keys, index))
        errors |= ReaderViewFrameError_DuplicateKey;
      keys[index] = row->key;
      rv_validate_text(row->section, &errors);
      rv_validate_text(row->excerpt, &errors);
      rv_validate_text(row->detail, &errors);
      if (row->match_start > (UI0U32)row->excerpt.size ||
          row->match_size > (UI0U32)row->excerpt.size - row->match_start)
        errors |= ReaderViewFrameError_InvalidMatch;
    }
    if (projection->find.active_index < -1 ||
        projection->find.active_index >= projection->find.row_count)
      errors |= ReaderViewFrameError_InvalidMatch;
  }

  rv_validate_status(projection->right.status, &errors);
  if ((projection->right.available_filters &
       ~(ReaderViewRightFilterFlag_All |
         ReaderViewRightFilterFlag_Bookmarks |
         ReaderViewRightFilterFlag_Highlights |
         ReaderViewRightFilterFlag_Notes)) != 0)
    errors |= ReaderViewFrameError_InvalidFilter;
  if (projection->right.row_count < 0 ||
      projection->right.row_count > READER_VIEW_RIGHT_ROW_CAP ||
      (projection->right.row_count > 0 && !projection->right.rows))
    errors |= ReaderViewFrameError_ProjectionCap;
  else
  {
    for (index = 0; index < projection->right.row_count; ++index)
    {
      const ReaderViewRightRow *row = projection->right.rows + index;
      if (!rv_key_unique(row->key, keys, index))
        errors |= ReaderViewFrameError_DuplicateKey;
      keys[index] = row->key;
      if (row->kind < ReaderViewRightRow_Bookmark ||
          row->kind > ReaderViewRightRow_Note)
        errors |= ReaderViewFrameError_BadInput;
      rv_validate_text(row->section, &errors);
      rv_validate_text(row->primary, &errors);
      rv_validate_text(row->secondary, &errors);
    }
  }

  rv_validate_status(projection->selection.status, &errors);
  rv_validate_text(projection->selection.selected_text, &errors);
  rv_validate_text(projection->selection.note_text, &errors);
  if (projection->selection.note_text.size >= READER_VIEW_NOTE_DRAFT_CAP)
    errors |= ReaderViewFrameError_ProjectionCap;
  rv_validate_choices(&projection->selection.highlight_colors, &errors);

#define RV_VALIDATE_LABEL(field) rv_validate_text(projection->labels.field, &errors)
  RV_VALIDATE_LABEL(open);
  RV_VALIDATE_LABEL(previous_page);
  RV_VALIDATE_LABEL(next_page);
  RV_VALIDATE_LABEL(back);
  RV_VALIDATE_LABEL(forward);
  RV_VALIDATE_LABEL(contents);
  RV_VALIDATE_LABEL(find);
  RV_VALIDATE_LABEL(reading_settings);
  RV_VALIDATE_LABEL(bookmark);
  RV_VALIDATE_LABEL(remove_bookmark);
  RV_VALIDATE_LABEL(annotations);
  RV_VALIDATE_LABEL(fullscreen);
  RV_VALIDATE_LABEL(exit_fullscreen);
  RV_VALIDATE_LABEL(distraction_free);
  RV_VALIDATE_LABEL(close);
  RV_VALIDATE_LABEL(clear);
  RV_VALIDATE_LABEL(previous_match);
  RV_VALIDATE_LABEL(next_match);
  RV_VALIDATE_LABEL(export_rows);
  RV_VALIDATE_LABEL(all);
  RV_VALIDATE_LABEL(bookmarks);
  RV_VALIDATE_LABEL(highlights);
  RV_VALIDATE_LABEL(notes);
  RV_VALIDATE_LABEL(go_to);
  RV_VALIDATE_LABEL(star);
  RV_VALIDATE_LABEL(unstar);
  RV_VALIDATE_LABEL(edit_note);
  RV_VALIDATE_LABEL(save_note);
  RV_VALIDATE_LABEL(cancel);
  RV_VALIDATE_LABEL(delete_value);
  RV_VALIDATE_LABEL(copy);
  RV_VALIDATE_LABEL(dictionary);
  RV_VALIDATE_LABEL(web_lookup);
  RV_VALIDATE_LABEL(translate);
  RV_VALIDATE_LABEL(more);
#undef RV_VALIDATE_LABEL
  return errors;
}

static ReaderViewText
rv_label(ReaderViewText supplied, ReaderViewText fallback)
{
  return supplied.data || supplied.size ? supplied : fallback;
}

static ReaderViewLabels
rv_resolve_labels(ReaderViewLabels supplied)
{
  ReaderViewLabels fallback = reader_view_default_english_labels();
#define RV_LABEL(field) supplied.field = rv_label(supplied.field, fallback.field)
  RV_LABEL(open);
  RV_LABEL(previous_page);
  RV_LABEL(next_page);
  RV_LABEL(back);
  RV_LABEL(forward);
  RV_LABEL(contents);
  RV_LABEL(find);
  RV_LABEL(reading_settings);
  RV_LABEL(bookmark);
  RV_LABEL(remove_bookmark);
  RV_LABEL(annotations);
  RV_LABEL(fullscreen);
  RV_LABEL(exit_fullscreen);
  RV_LABEL(distraction_free);
  RV_LABEL(close);
  RV_LABEL(clear);
  RV_LABEL(previous_match);
  RV_LABEL(next_match);
  RV_LABEL(export_rows);
  RV_LABEL(all);
  RV_LABEL(bookmarks);
  RV_LABEL(highlights);
  RV_LABEL(notes);
  RV_LABEL(go_to);
  RV_LABEL(star);
  RV_LABEL(unstar);
  RV_LABEL(edit_note);
  RV_LABEL(save_note);
  RV_LABEL(cancel);
  RV_LABEL(delete_value);
  RV_LABEL(copy);
  RV_LABEL(dictionary);
  RV_LABEL(web_lookup);
  RV_LABEL(translate);
  RV_LABEL(more);
#undef RV_LABEL
  return supplied;
}

static UI0ControlStateFlags
rv_control_state(UI0Signal signal, UI0ControlFlags flags, UI0B32 checked,
                 UI0B32 open)
{
  UI0ControlStateFlags result = UI0ControlState_None;
  if (ui0_signal_has(signal, UI0Signal_Hovered)) result |= UI0ControlState_Hovered;
  if (ui0_signal_has(signal, UI0Signal_Pressed)) result |= UI0ControlState_Pressed;
  if (ui0_signal_has(signal, UI0Signal_Active)) result |= UI0ControlState_Active;
  if (ui0_signal_has(signal, UI0Signal_Focused)) result |= UI0ControlState_Focused;
  if (ui0_signal_has(signal, UI0Signal_Clicked)) result |= UI0ControlState_Clicked;
  if (ui0_signal_has(signal, UI0Signal_KeyboardActivated))
    result |= UI0ControlState_KeyboardActivated;
  if (ui0_signal_has(signal, UI0Signal_BlockedByRoot))
    result |= UI0ControlState_BlockedByRoot;
  if (ui0_signal_has(signal, UI0Signal_FocusVisible))
    result |= UI0ControlState_FocusVisible;
  if ((flags & UI0Control_Disabled) != 0) result |= UI0ControlState_Disabled;
  if ((flags & UI0Control_Selected) != 0) result |= UI0ControlState_Selected;
  if ((flags & UI0Control_Destructive) != 0) result |= UI0ControlState_Destructive;
  if (checked) result |= UI0ControlState_Checked | UI0ControlState_On;
  if (open) result |= UI0ControlState_Open;
  return result;
}

static ReaderViewSemanticFlags
rv_semantic_flags(RVBuildContext *ctx,
                  UI0ID id,
                  UI0B32 enabled,
                  UI0B32 focusable,
                  UI0B32 selected,
                  UI0B32 checked,
                  UI0B32 expanded)
{
  ReaderViewSemanticFlags flags = ReaderViewSemantic_None;
  if (enabled) flags |= ReaderViewSemantic_Enabled;
  if (focusable) flags |= ReaderViewSemantic_Focusable;
  if (ctx->signals.focus_id == id) flags |= ReaderViewSemantic_Focused;
  if (selected) flags |= ReaderViewSemantic_Selected;
  if (checked) flags |= ReaderViewSemantic_Checked;
  if (expanded) flags |= ReaderViewSemantic_Expanded;
  return flags;
}

static UI0B32
rv_add_semantic(RVBuildContext *ctx,
                UI0ID id,
                UI0ID parent_id,
                ReaderViewSemanticRole role,
                ReaderViewSemanticFlags flags,
                UI0Rect rect,
                ReaderViewText name,
                ReaderViewText value,
                ReaderViewKey source_key,
                UI0U64 range_value,
                UI0U64 range_min,
                UI0U64 range_max)
{
  ReaderViewSemanticNode *node;
  if (ctx->semantic_count >= READER_VIEW_SEMANTIC_NODE_CAP)
  {
    ctx->frame->error_flags |= ReaderViewFrameError_RecordCap;
    return 0;
  }
  node = ctx->storage->semantic_nodes + ctx->semantic_count++;
  memset(node, 0, sizeof(*node));
  node->id = id;
  node->parent_id = parent_id;
  node->role = role;
  node->flags = flags;
  node->rect = rect;
  node->name = name;
  node->value = value;
  node->source_key = source_key;
  node->range_value = range_value;
  node->range_min = range_min;
  node->range_max = range_max;
  return 1;
}

static void
rv_set_semantic_control(RVBuildContext *ctx,
                        UI0ID id,
                        ReaderViewSemanticControl control)
{
  UI0S32 index;
  if (!ctx || control == ReaderViewSemanticControl_None) return;
  for (index = ctx->semantic_count - 1; index >= 0; index -= 1)
  {
    ReaderViewSemanticNode *node = ctx->storage->semantic_nodes + index;
    if (node->id == id)
    {
      node->control = control;
      return;
    }
  }
}

static UI0B32
rv_add_binding(RVBuildContext *ctx,
               UI0ID source_id,
               ReaderViewText text,
               ReaderViewTextStyle style)
{
  ReaderViewTextBinding *binding;
  if (text.size == 0) return 1;
  if (ctx->text_count >= READER_VIEW_TEXT_BINDING_CAP)
  {
    ctx->frame->error_flags |= ReaderViewFrameError_RecordCap;
    return 0;
  }
  binding = ctx->storage->text_bindings + ctx->text_count++;
  binding->source_id = source_id;
  binding->text = text;
  binding->style = style;
  return 1;
}

static UI0B32
rv_add_action(RVBuildContext *ctx,
              ReaderViewActionKind kind,
              ReaderViewKey key,
              ReaderViewKey auxiliary_key,
              ReaderViewSettingKind setting_kind,
              ReaderViewRightRowKind right_kind,
              ReaderViewRightFilter filter,
              UI0U64 value,
              ReaderViewText text)
{
  ReaderViewAction *action;
  if (ctx->action_count >= READER_VIEW_ACTION_CAP)
  {
    ctx->frame->error_flags |= ReaderViewFrameError_ActionCap;
    return 0;
  }
  action = ctx->storage->actions + ctx->action_count++;
  memset(action, 0, sizeof(*action));
  action->kind = kind;
  action->key = key;
  action->auxiliary_key = auxiliary_key;
  action->setting_kind = setting_kind;
  action->right_row_kind = right_kind;
  action->right_filter = filter;
  action->value = value;
  action->text = text;
  return 1;
}

static UI0B32
rv_add_surface(RVBuildContext *ctx,
               UI0ID id,
               UI0ID parent_id,
               UI0ControlKind kind,
               ReaderViewSemanticRole role,
               UI0RootKind root,
               UI0Rect rect,
               ReaderViewText name)
{
  UI0ControlRecord *record;
  if (ctx->control_count >= READER_VIEW_CONTROL_CAP)
  {
    ctx->frame->error_flags |= ReaderViewFrameError_RecordCap;
    return 0;
  }
  record = ctx->storage->control_records + ctx->control_count++;
  memset(record, 0, sizeof(*record));
  record->id = id;
  record->kind = kind;
  record->box_index = UI0LayoutInvalidIndex;
  record->root = root;
  record->rect = rect;
  record->clip_rect = rect;
  record->text_rect = rect;
  return rv_add_semantic(ctx,
                         id,
                         parent_id,
                         role,
                         ReaderViewSemantic_Enabled,
                         rect,
                         name,
                         rv_text(0, 0),
                         0,
                         0,
                         0,
                         0);
}

static UI0B32
rv_add_control_with_hit_rect(RVBuildContext *ctx,
                             UI0ID id,
                             UI0ID parent_id,
                             UI0ControlKind kind,
                             ReaderViewSemanticRole role,
                             UI0RootKind root,
                             UI0Rect rect,
                             UI0Rect hit_rect,
                             ReaderViewText label,
                             ReaderViewText value,
                             ReaderViewKey source_key,
                             UI0B32 enabled,
                             UI0B32 selected,
                             UI0B32 checked,
                             UI0B32 open,
                             UI0B32 destructive)
{
  UI0SignalRectSpec signal_spec;
  UI0Signal signal;
  UI0ControlRecord *record;
  UI0ControlFlags control_flags = UI0Control_None;
  UI0B32 invoked;
  memset(&signal_spec, 0, sizeof(signal_spec));
  signal_spec.id = id;
  signal_spec.root = root;
  signal_spec.flags = UI0SignalBox_Clickable | UI0SignalBox_Focusable;
  if (!enabled) signal_spec.flags |= UI0SignalBox_Disabled;
  signal_spec.rect = rect;
  signal_spec.hit_rect = hit_rect;
  signal = ui0_signal_from_rect(&ctx->signals, signal_spec);

  if (!enabled) control_flags |= UI0Control_Disabled;
  if (selected) control_flags |= UI0Control_Selected;
  if (destructive) control_flags |= UI0Control_Destructive;
  if (kind == UI0ControlKind_IconButton || kind == UI0ControlKind_ToolbarItem)
    control_flags |= UI0Control_Quiet;

  if (ctx->control_count >= READER_VIEW_CONTROL_CAP)
  {
    ctx->frame->error_flags |= ReaderViewFrameError_RecordCap;
    return 0;
  }
  record = ctx->storage->control_records + ctx->control_count++;
  memset(record, 0, sizeof(*record));
  record->id = id;
  record->kind = kind;
  record->box_index = UI0LayoutInvalidIndex;
  record->root = root;
  record->control_flags = control_flags;
  record->state = rv_control_state(signal, control_flags, checked, open);
  record->signal_flags = signal.flags;
  record->rect = rect;
  record->clip_rect = rect;
  record->text_rect = rv_rect(rect.x + 8, rect.y, rv_max(0, rect.w - 16), rect.h);
  record->label_hash = rv_text_hash(label);
  record->label_len = label.size;
  record->value = checked;
  record->next_value = !checked;
  if (ctx->input->state->pending_accessibility_focus_id == id)
  {
    ctx->signals.focus_id = id;
    ctx->signals.focus_visible_id = id;
    ctx->input->state->pending_accessibility_focus_id = 0;
    ctx->frame->change_flags |= ReaderViewFrameChange_FocusChanged;
  }
  (void)rv_add_binding(ctx,
                       id,
                       label,
                       kind == UI0ControlKind_MenuItem ?
                         ReaderViewTextStyle_MenuItem :
                         ReaderViewTextStyle_Default);
  (void)rv_add_semantic(ctx,
                        id,
                        parent_id,
                        role,
                        rv_semantic_flags(ctx, id, enabled, 1,
                                          selected, checked, open),
                        hit_rect,
                        label,
                        value,
                        source_key,
                        0,
                        0,
                        0);
  invoked = ui0_signal_has(signal, UI0Signal_Clicked) ||
            ui0_signal_has(signal, UI0Signal_KeyboardActivated) ||
            ctx->input->state->pending_accessibility_invoke_id == id;
  if (ctx->input->state->pending_accessibility_invoke_id == id)
    ctx->input->state->pending_accessibility_invoke_id = 0;
  return invoked;
}

static UI0B32
rv_add_control(RVBuildContext *ctx,
               UI0ID id,
               UI0ID parent_id,
               UI0ControlKind kind,
               ReaderViewSemanticRole role,
               UI0RootKind root,
               UI0Rect rect,
               ReaderViewText label,
               ReaderViewText value,
               ReaderViewKey source_key,
               UI0B32 enabled,
               UI0B32 selected,
               UI0B32 checked,
               UI0B32 open,
               UI0B32 destructive)
{
  return rv_add_control_with_hit_rect(ctx,
                                      id,
                                      parent_id,
                                      kind,
                                      role,
                                      root,
                                      rect,
                                      rect,
                                      label,
                                      value,
                                      source_key,
                                      enabled,
                                      selected,
                                      checked,
                                      open,
                                      destructive);
}

static UI0B32
rv_add_icon_control(RVBuildContext *ctx,
                    UI0ID id,
                    UI0ID parent_id,
                    UI0ControlKind kind,
                    ReaderViewSemanticRole role,
                    UI0RootKind root,
                    UI0Rect rect,
                    UI0Rect hit_rect,
                    ReaderViewText label,
                    ReaderViewText value,
                    ReaderViewKey source_key,
                    UI0B32 enabled,
                    UI0B32 selected,
                    UI0B32 checked,
                    UI0B32 open,
                    UI0B32 destructive,
                    UI0IconKind icon_kind,
                    UI0Rect icon_rect,
                    UI0B32 icon_visible)
{
  UI0S32 control_index = ctx->control_count;
  UI0B32 invoked = rv_add_control_with_hit_rect(ctx,
                                                id,
                                                parent_id,
                                                kind,
                                                role,
                                                root,
                                                rect,
                                                hit_rect,
                                                label,
                                                value,
                                                source_key,
                                                enabled,
                                                selected,
                                                checked,
                                                open,
                                                destructive);
  if (ctx->control_count > control_index)
  {
    UI0ControlRecord *record = ctx->storage->control_records + control_index;
    record->text_rect = rv_rect(0, 0, 0, 0);
    record->label_hash = 0;
    record->label_len = 0;
    if (icon_kind != UI0IconKind_None)
    {
      if (ctx->icon_count >= RV_ICON_RECORD_CAP)
      {
        ctx->frame->error_flags |= ReaderViewFrameError_RecordCap;
      }
      else
      {
        RVIconRecord *icon = ctx->icons + ctx->icon_count++;
        icon->control_index = control_index;
        icon->icon_kind = icon_kind;
        icon->rect = icon_rect;
        icon->visible = icon_visible;
      }
    }
  }
  return invoked;
}

static void
rv_add_text_record_styled(RVBuildContext *ctx,
                          UI0ID id,
                          UI0ID parent_id,
                          UI0Rect rect,
                          ReaderViewText text,
                          ReaderViewTextStyle style,
                          ReaderViewSemanticRole role,
                          ReaderViewSemanticFlags flags,
                          ReaderViewKey source_key)
{
  UI0ControlRecord *record;
  if (ctx->control_count >= READER_VIEW_CONTROL_CAP)
  {
    ctx->frame->error_flags |= ReaderViewFrameError_RecordCap;
    return;
  }
  record = ctx->storage->control_records + ctx->control_count++;
  memset(record, 0, sizeof(*record));
  record->id = id;
  record->kind = UI0ControlKind_Label;
  record->box_index = UI0LayoutInvalidIndex;
  record->root = UI0RootKind_Normal;
  record->rect = rect;
  record->clip_rect = rect;
  record->text_rect = rect;
  record->label_hash = rv_text_hash(text);
  record->label_len = text.size;
  (void)rv_add_binding(ctx, id, text, style);
  (void)rv_add_semantic(ctx, id, parent_id, role, flags, rect, text,
                        rv_text(0, 0), source_key, 0, 0, 0);
}

static UI0S32
rv_next_enabled_choice(const ReaderViewChoiceControl *control, UI0S32 direction)
{
  UI0S32 selected = -1;
  UI0S32 step;
  UI0S32 index;
  if (!control || control->count <= 0) return -1;
  for (index = 0; index < control->count; ++index)
    if ((control->items[index].flags & ReaderViewChoice_Selected) != 0)
      selected = index;
  if (selected < 0) selected = direction > 0 ? -1 : 0;
  for (step = 1; step <= control->count; ++step)
  {
    index = (selected + direction * step) % control->count;
    if (index < 0) index += control->count;
    if ((control->items[index].flags & ReaderViewChoice_Enabled) != 0)
      return index;
  }
  return -1;
}

static const ReaderViewSettingControl *
rv_find_setting(const ReaderViewProjection *projection,
                ReaderViewSettingKind kind)
{
  UI0S32 index;
  for (index = 0; index < projection->settings.count; ++index)
    if (projection->settings.items[index].kind == kind)
      return projection->settings.items + index;
  return 0;
}

static const ReaderViewRightRow *
rv_find_right_row(const ReaderViewProjection *projection, ReaderViewKey key)
{
  UI0S32 index;
  for (index = 0; index < projection->right.row_count; ++index)
    if (projection->right.rows[index].key == key)
      return projection->right.rows + index;
  return 0;
}

static UI0B32
rv_apply_text_input(UI0TextInputBuffer *buffer,
                    UI0TextInputState *state,
                    const UI0TextInputFrameInput *input,
                    UI0B32 *out_committed)
{
  UI0B32 edited = 0;
  *out_committed = 0;
  if (input->select_all) ui0_text_input_select_all(buffer, state);
  if (input->move_start)
  {
    if (input->extend_selection) ui0_text_input_select_to_start(buffer, state);
    else ui0_text_input_move_caret_to_start(buffer, state);
  }
  if (input->move_end)
  {
    if (input->extend_selection) ui0_text_input_select_to_end(buffer, state);
    else ui0_text_input_move_caret_to_end(buffer, state);
  }
  if (input->move_delta)
  {
    if (input->extend_selection)
      ui0_text_input_select_move(buffer, state, input->move_delta);
    else
      ui0_text_input_move_caret(buffer, state, input->move_delta);
  }
  if (input->copy_pressed && input->transfer_buffer)
    (void)ui0_text_input_copy_selection(buffer, state, input->transfer_buffer);
  if (input->cut_pressed && input->transfer_buffer)
    edited |= ui0_text_input_cut_selection(buffer, state, input->transfer_buffer);
  if (input->paste_pressed && input->transfer_buffer)
    edited |= ui0_text_input_try_paste(buffer, state, input->transfer_buffer);
  if (input->undo_pressed) edited |= ui0_text_input_undo(buffer, state);
  if (input->redo_pressed) edited |= ui0_text_input_redo(buffer, state);
  if (input->backspace_pressed) edited |= ui0_text_input_backspace(buffer, state);
  if (input->delete_pressed) edited |= ui0_text_input_delete(buffer, state);
  if (input->text_len > 0 && input->text)
    edited |= ui0_text_input_try_insert_text(buffer, state,
                                             input->text, input->text_len);
  if (input->commit_pressed)
  {
    *out_committed = 1;
    if (input->commit_buffer)
      (void)ui0_text_input_commit(buffer, state, input->commit_buffer,
                                  input->clear_on_commit);
  }
  return edited;
}

static UI0B32
rv_apply_text_area(UI0TextInputBuffer *buffer,
                   UI0TextAreaState *state,
                   const UI0TextAreaFrameInput *input,
                   UI0S32 vertical_delta)
{
  UI0B32 edited = 0;
  UI0S32 delta = input->move_delta;
  if (input->select_all) ui0_text_area_select_all(buffer, state);
  if (input->move_start)
  {
    if (input->extend_selection) ui0_text_area_select_to_start(buffer, state);
    else ui0_text_area_move_caret_to_start(buffer, state);
  }
  if (input->move_end)
  {
    if (input->extend_selection) ui0_text_area_select_to_end(buffer, state);
    else ui0_text_area_move_caret_to_end(buffer, state);
  }
  if (delta)
  {
    if (input->extend_selection) ui0_text_area_select_move(buffer, state, delta);
    else ui0_text_area_move_caret(buffer, state, delta);
  }
  if (vertical_delta || input->move_vertical_delta)
  {
    UI0S32 approximate = (vertical_delta + input->move_vertical_delta) * 40;
    if (input->extend_selection) ui0_text_area_select_move(buffer, state, approximate);
    else ui0_text_area_move_caret(buffer, state, approximate);
  }
  if (input->copy_pressed && input->transfer_buffer)
    (void)ui0_text_area_copy_selection(buffer, state, input->transfer_buffer);
  if (input->cut_pressed && input->transfer_buffer)
    edited |= ui0_text_area_cut_selection(buffer, state, input->transfer_buffer);
  if (input->paste_pressed && input->transfer_buffer)
    edited |= ui0_text_area_try_paste(buffer, state, input->transfer_buffer);
  if (input->undo_pressed) edited |= ui0_text_area_undo(buffer, state);
  if (input->redo_pressed) edited |= ui0_text_area_redo(buffer, state);
  if (input->backspace_pressed) edited |= ui0_text_area_backspace(buffer, state);
  if (input->delete_pressed) edited |= ui0_text_area_delete(buffer, state);
  if (input->text_len > 0 && input->text)
    edited |= ui0_text_area_try_insert_text(buffer, state,
                                            input->text, input->text_len);
  return edited;
}

static void
rv_copy_note_to_state(ReaderViewState *state,
                      const ReaderViewSelectionProjection *selection)
{
  UI0S32 size = rv_min(selection->note_text.size,
                       READER_VIEW_NOTE_DRAFT_CAP - 1);
  if (size > 0) memcpy(state->note_draft, selection->note_text.data, (size_t)size);
  state->note_draft[size] = 0;
  state->note_draft_length = size;
  state->note_selection_key = selection->selection_key;
  state->note_source_revision = selection->revision;
  state->note_dirty = 0;
  state->note_input.caret = size;
  state->note_input.selection_anchor = size;
  ui0_text_input_history_reset(&state->note_history);
}

UI0B32
reader_view_open_note_editor(ReaderViewState *state,
                             const ReaderViewSelectionProjection *selection)
{
  if (!state || !selection ||
      (selection->flags & ReaderViewSelection_Active) == 0 ||
      selection->selection_key == 0 ||
      (selection->flags & (ReaderViewSelection_CanAddNote |
                           ReaderViewSelection_CanEditNote)) == 0 ||
      !rv_text_valid(selection->note_text) ||
      selection->note_text.size >= READER_VIEW_NOTE_DRAFT_CAP)
    return 0;
  rv_copy_note_to_state(state, selection);
  state->popup = ReaderViewPopup_NoteEditor;
  state->restore_focus_id = state->focus_id;
  return 1;
}

static void
rv_open_note_editor(RVBuildContext *ctx)
{
  ReaderViewState *state = ctx->input->state;
  rv_copy_note_to_state(state, &ctx->input->projection->selection);
  state->popup = ReaderViewPopup_NoteEditor;
  state->restore_focus_id = state->focus_id;
  ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged;
}

static void
rv_handle_escape(RVBuildContext *ctx)
{
  ReaderViewState *state = ctx->input->state;
  if (!ctx->input->input->escape_pressed) return;
  if (state->popup == ReaderViewPopup_NoteEditor && state->note_dirty)
    return;
  if (state->popup != ReaderViewPopup_None)
  {
    if (state->popup == ReaderViewPopup_SelectionTools)
      state->dismissed_selection_key = state->last_selection_key;
    state->popup = ReaderViewPopup_None;
    if (state->restore_focus_id) state->focus_id = state->restore_focus_id;
    state->restore_focus_id = 0;
  }
  else if (state->right_panel_open)
    state->right_panel_open = 0;
  else if (state->left_panel != ReaderViewLeftPanel_None)
    state->left_panel = ReaderViewLeftPanel_None;
  else
    return;
  ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged;
}

static void
rv_sync_selection_popup(RVBuildContext *ctx)
{
  ReaderViewState *state = ctx->input->state;
  const ReaderViewSelectionProjection *selection =
    &ctx->input->projection->selection;
  UI0B32 active = (selection->flags & ReaderViewSelection_Active) != 0 &&
                  selection->selection_key != 0;
  if (!active)
  {
    state->last_selection_key = 0;
    state->dismissed_selection_key = 0;
    if (state->popup == ReaderViewPopup_SelectionTools)
      state->popup = ReaderViewPopup_None;
    return;
  }
  if (state->last_selection_key != selection->selection_key)
  {
    state->last_selection_key = selection->selection_key;
    state->dismissed_selection_key = 0;
    if (state->popup == ReaderViewPopup_None)
    {
      state->popup = ReaderViewPopup_SelectionTools;
      ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged;
    }
  }
}

static UI0Rect
rv_centered_rect(UI0Rect bounds, UI0S32 width, UI0S32 height)
{
  width = rv_min(width, rv_max(0, bounds.w - RV_INSET * 2));
  height = rv_min(height, rv_max(0, bounds.h - RV_INSET * 2));
  return rv_rect(bounds.x + (bounds.w - width) / 2,
                 bounds.y + (bounds.h - height) / 2,
                 width,
                 height);
}

static UI0Rect
rv_toolbar_slot(UI0Rect rect, UI0S32 index, UI0S32 count)
{
  UI0S32 width = count > 0 ? rect.w / count : 0;
  UI0S32 x = rect.x + index * width;
  if (index == count - 1) width = rect.x + rect.w - x;
  return rv_rect(x + 2, rect.y, rv_max(0, width - 4), rect.h);
}

static void
rv_toggle_left_panel(RVBuildContext *ctx, ReaderViewLeftPanelMode mode)
{
  ReaderViewState *state = ctx->input->state;
  state->left_panel = state->left_panel == mode ? ReaderViewLeftPanel_None : mode;
  if (state->left_panel != ReaderViewLeftPanel_None)
    state->most_recent_panel = ReaderViewPanel_Left;
  ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged |
                             ReaderViewFrameChange_LayoutChanged;
}

static void
rv_toggle_right_panel(RVBuildContext *ctx)
{
  ReaderViewState *state = ctx->input->state;
  state->right_panel_open = !state->right_panel_open;
  if (state->right_panel_open) state->most_recent_panel = ReaderViewPanel_Right;
  ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged |
                             ReaderViewFrameChange_LayoutChanged;
}

static ReaderViewText
rv_selected_choice_label(const ReaderViewChoiceControl *choices)
{
  UI0S32 index;
  for (index = 0; choices && index < choices->count; ++index)
    if ((choices->items[index].flags & ReaderViewChoice_Selected) != 0)
      return choices->items[index].label;
  return rv_text(0, 0);
}

static void
rv_activate_setting(RVBuildContext *ctx,
                    const ReaderViewSettingControl *setting,
                    UI0ID source_id)
{
  UI0S32 choice_index;
  if (!setting) return;
  if (setting->choices.presentation == ReaderViewChoicePresentation_Menu)
  {
    ctx->input->state->popup = ReaderViewPopup_SettingMenu;
    ctx->input->state->active_setting_kind = setting->kind;
    ctx->input->state->restore_focus_id = source_id;
    ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged;
    return;
  }
  choice_index = rv_next_enabled_choice(&setting->choices, 1);
  if (choice_index >= 0)
    (void)rv_add_action(ctx,
                        ReaderViewAction_SelectSetting,
                        setting->choices.items[choice_index].key,
                        0,
                        setting->kind,
                        ReaderViewRightRow_Bookmark,
                        ReaderViewRightFilter_All,
                        0,
                        rv_text(0, 0));
}

static void
rv_add_text_record(RVBuildContext *ctx,
                   UI0ID id,
                   UI0ID parent_id,
                   UI0Rect rect,
                   ReaderViewText text,
                   ReaderViewSemanticRole role,
                   ReaderViewSemanticFlags flags,
                   ReaderViewKey source_key)
{
  rv_add_text_record_styled(ctx,
                            id,
                            parent_id,
                            rect,
                            text,
                            ReaderViewTextStyle_Default,
                            role,
                            flags,
                            source_key);
}

static UI0Rect
rv_toolbar_icon_rect(UI0Rect rect)
{
  UI0S32 size = rv_clamp(rv_min(rect.w, rect.h) - 10, 16, 18);
  return rv_rect(rect.x + (rect.w - size) / 2,
                 rect.y + (rect.h - size) / 2,
                 size,
                 size);
}

static UI0B32
rv_toolbar_icon_control(RVBuildContext *ctx,
                        UI0U64 tag,
                        ReaderViewSemanticControl semantic_control,
                        UI0S32 slot,
                        ReaderViewText label,
                        ReaderViewText value,
                        UI0B32 enabled,
                        UI0B32 selected,
                        UI0B32 checked,
                        UI0B32 open,
                        UI0IconKind icon_kind)
{
  UI0S32 control_index = ctx->control_count;
  UI0B32 invoked;
  UI0ID id = rv_id(tag, 0);
  UI0Rect rect = rv_toolbar_slot_rect(ctx->input->layout, slot);
  invoked = rv_add_icon_control(ctx,
                                id,
                                ctx->toolbar_id,
                                UI0ControlKind_IconButton,
                                ReaderViewSemantic_Button,
                                UI0RootKind_Normal,
                                rect,
                                rect,
                                label,
                                value,
                                0,
                                enabled,
                                selected,
                                checked,
                                open,
                                0,
                                icon_kind,
                                rv_toolbar_icon_rect(rect),
                                1);
  if (ctx->control_count > control_index)
  {
    ctx->storage->control_records[control_index].control_flags &=
      ~UI0Control_Quiet;
    rv_set_semantic_control(ctx, id, semantic_control);
  }
  return invoked;
}

static void
rv_build_toolbar_setting(RVBuildContext *ctx,
                         ReaderViewLabels labels,
                         ReaderViewSettingKind kind,
                         UI0S32 slot,
                         UI0IconKind icon_kind)
{
  const ReaderViewSettingControl *setting =
    rv_find_setting(ctx->input->projection, kind);
  UI0ID id;
  ReaderViewText label;
  if (!setting) return;
  id = rv_id(30 + (UI0U64)kind, 0);
  label = setting->label.size ? setting->label : labels.reading_settings;
  if (rv_toolbar_icon_control(
        ctx,
        30 + (UI0U64)kind,
        kind == ReaderViewSetting_FontSize ?
          ReaderViewSemanticControl_FontSize :
        kind == ReaderViewSetting_LineSpacing ?
          ReaderViewSemanticControl_LineSpacing :
        kind == ReaderViewSetting_FontFamily ?
          ReaderViewSemanticControl_FontFamily :
          ReaderViewSemanticControl_Theme,
        slot,
        label,
        rv_selected_choice_label(&setting->choices),
        setting->status.state == ReaderViewLoad_Ready,
        0,
        0,
        ctx->input->state->popup == ReaderViewPopup_SettingMenu &&
          ctx->input->state->active_setting_kind == kind,
        icon_kind))
  {
    rv_activate_setting(ctx, setting, id);
  }
}

static void
rv_build_toolbar(RVBuildContext *ctx, ReaderViewLabels labels)
{
  const ReaderViewProjection *projection = ctx->input->projection;
  const ReaderViewLayout *layout = ctx->input->layout;
  UI0B32 document_open = rv_has_document_flag(
    projection, ReaderViewDocument_Open);

  if (!layout->toolbar_visible || layout->shared_toolbar_rect.w <= 0) return;
  ctx->toolbar_id = rv_id(1, 0);
  (void)rv_add_semantic(ctx,
                        ctx->toolbar_id,
                        0,
                        ReaderViewSemantic_Toolbar,
                        ReaderViewSemantic_Enabled,
                        layout->toolbar_rect,
                        rv_literal("Reader toolbar"),
                        rv_text(0, 0),
                        0,
                        0,
                        0,
                        0);
  if (document_open && projection->chrome_title.size > 0)
  {
    rv_add_text_record_styled(ctx,
                              rv_id(2, 0),
                              ctx->toolbar_id,
                              rv_rect(layout->bounds.x + 20,
                                      layout->bounds.y + 14,
                                      180,
                                      22),
                              projection->chrome_title,
                              ReaderViewTextStyle_ChromeTitle,
                              ReaderViewSemantic_Group,
                              ReaderViewSemantic_Enabled,
                              0);
  }

  if (!document_open)
  {
    if (rv_has_feature(projection, ReaderViewFeature_Open) &&
        rv_toolbar_icon_control(
          ctx, 10, ReaderViewSemanticControl_Open, 0,
          labels.open, rv_text(0, 0),
          rv_has_document_flag(projection, ReaderViewDocument_CanOpen),
          0, 0, 0, UI0IconKind_BookOpen))
    {
      (void)rv_add_action(ctx, ReaderViewAction_Open, 0, 0,
                          ReaderViewSetting_FontFamily, ReaderViewRightRow_Bookmark,
                          ReaderViewRightFilter_All, 0, rv_text(0, 0));
    }
    return;
  }

  if (rv_has_feature(projection, ReaderViewFeature_Contents) &&
      rv_toolbar_icon_control(
        ctx, 11, ReaderViewSemanticControl_Contents, 0,
        labels.contents, rv_text(0, 0), 1,
        ctx->input->state->left_panel == ReaderViewLeftPanel_Contents,
        ctx->input->state->left_panel == ReaderViewLeftPanel_Contents,
        ctx->input->state->left_panel == ReaderViewLeftPanel_Contents,
        UI0IconKind_List))
  {
    rv_toggle_left_panel(ctx, ReaderViewLeftPanel_Contents);
  }
  if (rv_has_feature(projection, ReaderViewFeature_Find) &&
      rv_toolbar_icon_control(
        ctx, 12, ReaderViewSemanticControl_Find, 1,
        labels.find, rv_text(0, 0), 1,
        ctx->input->state->left_panel == ReaderViewLeftPanel_Find,
        ctx->input->state->left_panel == ReaderViewLeftPanel_Find,
        ctx->input->state->left_panel == ReaderViewLeftPanel_Find,
        UI0IconKind_Search))
  {
    rv_toggle_left_panel(ctx, ReaderViewLeftPanel_Find);
  }
  if (rv_has_feature(projection, ReaderViewFeature_History))
  {
    if (rv_toolbar_icon_control(
          ctx, 13, ReaderViewSemanticControl_HistoryBack, 2,
          labels.back, rv_text(0, 0),
          rv_has_document_flag(projection, ReaderViewDocument_CanGoBack),
          0, 0, 0, UI0IconKind_ArrowLeft))
      (void)rv_add_action(ctx, ReaderViewAction_HistoryBack, 0, 0,
                          ReaderViewSetting_FontFamily, ReaderViewRightRow_Bookmark,
                          ReaderViewRightFilter_All, 0, rv_text(0, 0));
    if (rv_toolbar_icon_control(
          ctx, 14, ReaderViewSemanticControl_HistoryForward, 3,
          labels.forward, rv_text(0, 0),
          rv_has_document_flag(projection, ReaderViewDocument_CanGoForward),
          0, 0, 0, UI0IconKind_ArrowRight))
      (void)rv_add_action(ctx, ReaderViewAction_HistoryForward, 0, 0,
                          ReaderViewSetting_FontFamily, ReaderViewRightRow_Bookmark,
                          ReaderViewRightFilter_All, 0, rv_text(0, 0));
  }

  if (rv_has_feature(projection, ReaderViewFeature_Fullscreen))
  {
    UI0B32 fullscreen = rv_has_document_flag(
      projection, ReaderViewDocument_Fullscreen);
    ReaderViewText fullscreen_label = fullscreen ?
      labels.exit_fullscreen : labels.fullscreen;
    if (rv_toolbar_icon_control(
          ctx, 16, ReaderViewSemanticControl_Fullscreen, 4,
          fullscreen_label, rv_text(0, 0),
          rv_has_document_flag(projection,
            ReaderViewDocument_CanToggleFullscreen),
          fullscreen, fullscreen, 0,
          fullscreen ? UI0IconKind_Shrink : UI0IconKind_Expand))
      (void)rv_add_action(ctx, ReaderViewAction_ToggleFullscreen, 0, 0,
                          ReaderViewSetting_FontFamily, ReaderViewRightRow_Bookmark,
                          ReaderViewRightFilter_All, 0, rv_text(0, 0));
  }
  else if (rv_has_feature(projection, ReaderViewFeature_DistractionFree))
  {
    UI0B32 active = rv_has_document_flag(
      projection, ReaderViewDocument_DistractionFree);
    if (rv_toolbar_icon_control(
          ctx, 15, ReaderViewSemanticControl_DistractionFree, 4,
          labels.distraction_free, rv_text(0, 0),
          rv_has_document_flag(projection,
            ReaderViewDocument_CanToggleDistraction),
          active, active, 0,
          active ? UI0IconKind_Shrink : UI0IconKind_Expand))
      (void)rv_add_action(ctx, ReaderViewAction_ToggleDistractionFree, 0, 0,
                          ReaderViewSetting_FontFamily, ReaderViewRightRow_Bookmark,
                          ReaderViewRightFilter_All, 0, rv_text(0, 0));
  }

  if (rv_has_feature(projection, ReaderViewFeature_ReadingSettings))
  {
    rv_build_toolbar_setting(ctx, labels, ReaderViewSetting_FontSize,
                             5, UI0IconKind_TextSize);
    rv_build_toolbar_setting(ctx, labels, ReaderViewSetting_LineSpacing,
                             6, UI0IconKind_LineSpacing);
    rv_build_toolbar_setting(ctx, labels, ReaderViewSetting_FontFamily,
                             7, UI0IconKind_CaseSensitive);
    rv_build_toolbar_setting(ctx, labels, ReaderViewSetting_Theme,
                             8, UI0IconKind_SunMoon);
  }

  if (rv_has_feature(projection, ReaderViewFeature_Annotations) &&
      rv_toolbar_icon_control(
        ctx, 17, ReaderViewSemanticControl_Annotations, 9,
        labels.annotations, rv_text(0, 0), 1,
        ctx->input->state->right_panel_open,
        ctx->input->state->right_panel_open,
        ctx->input->state->right_panel_open,
        UI0IconKind_Notebook))
  {
    rv_toggle_right_panel(ctx);
  }

  if (rv_has_feature(projection, ReaderViewFeature_Bookmark))
  {
    UI0B32 bookmarked = rv_has_document_flag(
      projection, ReaderViewDocument_CurrentBookmarked);
    if (rv_toolbar_icon_control(
          ctx, 18, ReaderViewSemanticControl_Bookmark, 10,
          bookmarked ? labels.remove_bookmark : labels.bookmark,
          rv_text(0, 0), 1, bookmarked, bookmarked, 0,
          UI0IconKind_Bookmark))
      (void)rv_add_action(ctx, ReaderViewAction_ToggleBookmark,
                          projection->current_bookmark_key, 0,
                          ReaderViewSetting_FontFamily, ReaderViewRightRow_Bookmark,
                          ReaderViewRightFilter_All, 0, rv_text(0, 0));
  }
}

static UI0Rect
rv_font_popup_rect(const ReaderViewBuildInput *input,
                   const ReaderViewSettingControl *setting)
{
  UI0MenuStyle style = ui0_menu_style_from_resolved(input->theme);
  UI0Rect font_rect = rv_toolbar_slot_rect(input->layout, 7);
  UI0S32 body_padding_left = ui0_flat_row_popup_body_padding_left(
    style.popup_padding_right,
    UI0FlatRowIndicator_DefaultWidth,
    UI0FlatRowIndicator_DefaultGap);
  UI0S32 row_count = setting ?
    rv_min(setting->choices.count, RV_FONT_POPUP_CHOICE_CAP) : 0;
  row_count = rv_max(row_count, 1);
  return rv_rect(font_rect.x - RV_FONT_POPUP_ANCHOR_X - body_padding_left,
                 font_rect.y + font_rect.h + RV_FONT_POPUP_GAP,
                 RV_FONT_POPUP_BODY_WIDTH + body_padding_left +
                   style.popup_padding_right,
                 style.popup_padding_y * 2 +
                   row_count * style.row_height +
                   (row_count - 1) * style.row_gap);
}

static UI0Rect
rv_popup_rect(const ReaderViewBuildInput *input)
{
  const ReaderViewState *state = input->state;
  UI0Rect bounds = input->layout->bounds;
  UI0Rect result = rv_centered_rect(bounds, RV_POPUP_WIDTH, 300);
  if (state->popup == ReaderViewPopup_SettingMenu &&
      state->active_setting_kind == ReaderViewSetting_FontFamily)
  {
    result = rv_font_popup_rect(
      input,
      rv_find_setting(input->projection, ReaderViewSetting_FontFamily));
  }
  else if (state->popup == ReaderViewPopup_SettingMenu ||
           state->popup == ReaderViewPopup_Overflow)
  {
    result.x = bounds.x + bounds.w - result.w - RV_INSET;
    result.y = input->layout->toolbar_rect.y + input->layout->toolbar_rect.h;
    result.h = rv_min(360, bounds.y + bounds.h - result.y - RV_INSET);
  }
  else if (state->popup == ReaderViewPopup_RightRowActions &&
           input->layout->right_panel_visible)
  {
    result.x = input->layout->right_panel_rect.x + RV_INSET;
    result.y = input->layout->right_panel_rect.y + 72;
    result.w = input->layout->right_panel_rect.w - RV_INSET * 2;
  }
  else if (state->popup == ReaderViewPopup_SelectionTools)
  {
    UI0Rect anchor = input->projection->selection.anchor_rect;
    result.w = rv_min(420, bounds.w - RV_INSET * 2);
    result.h = 176;
    result.x = rv_clamp(anchor.x + anchor.w / 2 - result.w / 2,
                        bounds.x + RV_INSET,
                        bounds.x + bounds.w - result.w - RV_INSET);
    result.y = anchor.y - result.h - RV_GAP;
    if (result.y < bounds.y + RV_INSET)
      result.y = rv_min(bounds.y + bounds.h - result.h - RV_INSET,
                        anchor.y + anchor.h + RV_GAP);
  }
  return result;
}

static UI0B32
rv_popup_button(RVBuildContext *ctx,
                UI0U64 tag,
                ReaderViewKey key,
                UI0Rect rect,
                ReaderViewText label,
                UI0B32 enabled,
                UI0B32 selected,
                UI0B32 destructive)
{
  return rv_add_control(ctx, rv_id(tag, key), ctx->popup_id,
                        UI0ControlKind_MenuItem,
                        ReaderViewSemantic_MenuItem,
                        UI0RootKind_Popup,
                        rect, label, rv_text(0, 0), key,
                        enabled, selected, selected, 0, destructive);
}

static UI0Rect
rv_popup_row(UI0Rect popup, UI0S32 row)
{
  return rv_rect(popup.x + RV_GAP,
                 popup.y + RV_GAP + row * (RV_CONTROL_HEIGHT + 4),
                 popup.w - RV_GAP * 2,
                 RV_CONTROL_HEIGHT);
}

static UI0Rect
rv_font_popup_row(const ReaderViewBuildInput *input,
                  UI0Rect popup,
                  UI0S32 row)
{
  UI0MenuStyle style = ui0_menu_style_from_resolved(input->theme);
  UI0FlatRowPopupGeometry geometry;
  UI0FlatRowPopupRowRects rects;
  memset(&geometry, 0, sizeof(geometry));
  geometry.padding_left = style.popup_padding_right;
  geometry.padding_right = style.popup_padding_right;
  geometry.padding_top = style.popup_padding_y;
  geometry.padding_bottom = style.popup_padding_y;
  geometry.row_height = style.row_height;
  geometry.row_gap = style.row_gap;
  geometry.indicator_width = UI0FlatRowIndicator_DefaultWidth;
  geometry.indicator_gap = UI0FlatRowIndicator_DefaultGap;
  rects = ui0_flat_row_popup_row_rects(popup, geometry, row);
  return rects.body_rect;
}

static void
rv_build_setting_popup(RVBuildContext *ctx, UI0Rect popup)
{
  const ReaderViewSettingControl *setting = rv_find_setting(
    ctx->input->projection, ctx->input->state->active_setting_kind);
  UI0S32 index;
  UI0S32 choice_count;
  if (!setting)
  {
    ctx->input->state->popup = ReaderViewPopup_None;
    return;
  }
  choice_count = setting->kind == ReaderViewSetting_FontFamily ?
    rv_min(setting->choices.count, RV_FONT_POPUP_CHOICE_CAP) :
    setting->choices.count;
  if (setting->kind == ReaderViewSetting_FontFamily &&
      ctx->signals.focus_id == ctx->input->state->restore_focus_id)
  {
    for (index = 0; index < choice_count; ++index)
    {
      const ReaderViewChoice *choice = setting->choices.items + index;
      if ((choice->flags & (ReaderViewChoice_Enabled |
                            ReaderViewChoice_Selected)) ==
          (ReaderViewChoice_Enabled | ReaderViewChoice_Selected))
      {
        ctx->signals.focus_id = rv_id(100, choice->key);
        ctx->signals.focus_visible_id = 0;
        break;
      }
    }
  }
  for (index = 0; index < choice_count; ++index)
  {
    const ReaderViewChoice *choice = setting->choices.items + index;
    if (rv_popup_button(ctx, 100, choice->key,
                        setting->kind == ReaderViewSetting_FontFamily ?
                          rv_font_popup_row(ctx->input, popup, index) :
                          rv_popup_row(popup, index),
                        choice->label,
                        (choice->flags & ReaderViewChoice_Enabled) != 0,
                        (choice->flags & ReaderViewChoice_Selected) != 0,
                        0))
    {
      (void)rv_add_action(ctx, ReaderViewAction_SelectSetting,
                          choice->key, 0, setting->kind,
                          ReaderViewRightRow_Bookmark,
                          ReaderViewRightFilter_All, 0, rv_text(0, 0));
      ctx->input->state->popup = ReaderViewPopup_None;
      ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged;
    }
  }
}

static void
rv_build_overflow_popup(RVBuildContext *ctx,
                        UI0Rect popup,
                        ReaderViewLabels labels)
{
  const ReaderViewProjection *projection = ctx->input->projection;
  UI0S32 row = 0;
  UI0S32 index;
  if (rv_has_feature(projection, ReaderViewFeature_History))
  {
    if (rv_popup_button(ctx, 110, 0, rv_popup_row(popup, row++), labels.back,
                        rv_has_document_flag(projection,
                          ReaderViewDocument_CanGoBack), 0, 0))
      (void)rv_add_action(ctx, ReaderViewAction_HistoryBack, 0, 0,
                          ReaderViewSetting_FontFamily, ReaderViewRightRow_Bookmark,
                          ReaderViewRightFilter_All, 0, rv_text(0, 0));
    if (rv_popup_button(ctx, 111, 0, rv_popup_row(popup, row++), labels.forward,
                        rv_has_document_flag(projection,
                          ReaderViewDocument_CanGoForward), 0, 0))
      (void)rv_add_action(ctx, ReaderViewAction_HistoryForward, 0, 0,
                          ReaderViewSetting_FontFamily, ReaderViewRightRow_Bookmark,
                          ReaderViewRightFilter_All, 0, rv_text(0, 0));
  }
  if (rv_has_feature(projection, ReaderViewFeature_ReadingSettings))
  {
    for (index = 0; index < projection->settings.count && row < 7; ++index)
    {
      const ReaderViewSettingControl *setting = projection->settings.items + index;
      UI0ID id = rv_id(120 + (UI0U64)setting->kind, 0);
      if (rv_add_control(ctx, id, ctx->popup_id, UI0ControlKind_MenuItem,
                         ReaderViewSemantic_MenuItem, UI0RootKind_Popup,
                         rv_popup_row(popup, row++), setting->label,
                         rv_selected_choice_label(&setting->choices), 0,
                         setting->status.state == ReaderViewLoad_Ready,
                         0, 0, 0, 0))
        rv_activate_setting(ctx, setting, id);
    }
  }
  if (rv_has_feature(projection, ReaderViewFeature_Annotations) && row < 7)
  {
    if (rv_popup_button(ctx, 130, 0, rv_popup_row(popup, row++),
                        labels.annotations, 1,
                        ctx->input->state->right_panel_open, 0))
    {
      ctx->input->state->popup = ReaderViewPopup_None;
      rv_toggle_right_panel(ctx);
    }
  }
  if (rv_has_feature(projection, ReaderViewFeature_Bookmark) && row < 7)
  {
    UI0B32 bookmarked = rv_has_document_flag(
      projection, ReaderViewDocument_CurrentBookmarked);
    if (rv_popup_button(ctx, 131, 0, rv_popup_row(popup, row++),
                        bookmarked ? labels.remove_bookmark : labels.bookmark,
                        rv_has_document_flag(projection,
                          ReaderViewDocument_Open), bookmarked, 0))
      (void)rv_add_action(ctx, ReaderViewAction_ToggleBookmark,
                          projection->current_bookmark_key, 0,
                          ReaderViewSetting_FontFamily, ReaderViewRightRow_Bookmark,
                          ReaderViewRightFilter_All, 0, rv_text(0, 0));
  }
  if (rv_has_feature(projection, ReaderViewFeature_Fullscreen) && row < 7)
  {
    UI0B32 fullscreen = rv_has_document_flag(
      projection, ReaderViewDocument_Fullscreen);
    if (rv_popup_button(ctx, 132, 0, rv_popup_row(popup, row++),
                        fullscreen ? labels.exit_fullscreen : labels.fullscreen,
                        rv_has_document_flag(projection,
                          ReaderViewDocument_CanToggleFullscreen), fullscreen, 0))
      (void)rv_add_action(ctx, ReaderViewAction_ToggleFullscreen, 0, 0,
                          ReaderViewSetting_FontFamily, ReaderViewRightRow_Bookmark,
                          ReaderViewRightFilter_All, 0, rv_text(0, 0));
  }
  if (rv_has_feature(projection, ReaderViewFeature_DistractionFree) && row < 7)
  {
    UI0B32 active = rv_has_document_flag(
      projection, ReaderViewDocument_DistractionFree);
    if (rv_popup_button(ctx, 133, 0, rv_popup_row(popup, row++),
                        labels.distraction_free,
                        rv_has_document_flag(projection,
                          ReaderViewDocument_CanToggleDistraction), active, 0))
      (void)rv_add_action(ctx, ReaderViewAction_ToggleDistractionFree, 0, 0,
                          ReaderViewSetting_FontFamily, ReaderViewRightRow_Bookmark,
                          ReaderViewRightFilter_All, 0, rv_text(0, 0));
  }
}

static void
rv_build_right_actions_popup(RVBuildContext *ctx,
                             UI0Rect popup,
                             ReaderViewLabels labels)
{
  const ReaderViewRightRow *row = rv_find_right_row(
    ctx->input->projection, ctx->input->state->right_menu_key);
  UI0S32 item = 0;
  if (!row)
  {
    ctx->input->state->popup = ReaderViewPopup_None;
    return;
  }
  if ((row->actions & ReaderViewRightAction_Activate) != 0 &&
      rv_popup_button(ctx, 140, row->key, rv_popup_row(popup, item++),
                      labels.go_to, 1, 0, 0))
    (void)rv_add_action(ctx, ReaderViewAction_ActivateRightRow,
                        row->key, 0, ReaderViewSetting_FontFamily, row->kind,
                        ReaderViewRightFilter_All, 0, rv_text(0, 0));
  if ((row->actions & ReaderViewRightAction_ToggleStar) != 0 &&
      rv_popup_button(ctx, 141, row->key, rv_popup_row(popup, item++),
                      (row->flags & ReaderViewRow_Starred) ?
                        labels.unstar : labels.star, 1, 0, 0))
    (void)rv_add_action(ctx, ReaderViewAction_ToggleRightRowStar,
                        row->key, 0, ReaderViewSetting_FontFamily, row->kind,
                        ReaderViewRightFilter_All, 0, rv_text(0, 0));
  if ((row->actions & ReaderViewRightAction_EditNote) != 0 &&
      rv_popup_button(ctx, 142, row->key, rv_popup_row(popup, item++),
                      labels.edit_note, 1, 0, 0))
  {
    (void)rv_add_action(ctx, ReaderViewAction_EditRightRowNote,
                        row->key, 0, ReaderViewSetting_FontFamily, row->kind,
                        ReaderViewRightFilter_All, 0, rv_text(0, 0));
  }
  if ((row->actions & ReaderViewRightAction_Delete) != 0 &&
      rv_popup_button(ctx, 143, row->key, rv_popup_row(popup, item++),
                      labels.delete_value, 1, 0, 1))
    (void)rv_add_action(ctx, ReaderViewAction_DeleteRightRow,
                        row->key, 0, ReaderViewSetting_FontFamily, row->kind,
                        ReaderViewRightFilter_All, 0, rv_text(0, 0));
}

static void
rv_build_selection_popup(RVBuildContext *ctx,
                         UI0Rect popup,
                         ReaderViewLabels labels)
{
  const ReaderViewSelectionProjection *selection =
    &ctx->input->projection->selection;
  UI0S32 item = 0;
  UI0S32 color_index;
  UI0S32 columns = 4;
  UI0S32 button_w = (popup.w - RV_GAP * 2) / columns;
  if ((selection->flags & ReaderViewSelection_CanCopy) != 0 &&
      rv_popup_button(ctx, 150, selection->selection_key,
                      rv_rect(popup.x + RV_GAP + (item % columns) * button_w,
                              popup.y + RV_GAP + (item / columns) * 38,
                              button_w - 2, RV_CONTROL_HEIGHT),
                      labels.copy, 1, 0, 0))
    (void)rv_add_action(ctx, ReaderViewAction_CopySelection,
                        selection->selection_key, 0,
                        ReaderViewSetting_FontFamily, ReaderViewRightRow_Highlight,
                        ReaderViewRightFilter_All, 0, selection->selected_text);
  item += 1;
  if ((selection->flags & (ReaderViewSelection_CanAddNote |
                           ReaderViewSelection_CanEditNote)) != 0 &&
      rv_popup_button(ctx, 151, selection->selection_key,
                      rv_rect(popup.x + RV_GAP + (item % columns) * button_w,
                              popup.y + RV_GAP + (item / columns) * 38,
                              button_w - 2, RV_CONTROL_HEIGHT),
                      labels.edit_note, 1, 0, 0))
    rv_open_note_editor(ctx);
  item += 1;
  if ((selection->flags & ReaderViewSelection_CanDictionary) != 0 &&
      rv_popup_button(ctx, 152, selection->selection_key,
                      rv_rect(popup.x + RV_GAP + (item % columns) * button_w,
                              popup.y + RV_GAP + (item / columns) * 38,
                              button_w - 2, RV_CONTROL_HEIGHT),
                      labels.dictionary, 1, 0, 0))
    (void)rv_add_action(ctx, ReaderViewAction_DictionarySelection,
                        selection->selection_key, 0,
                        ReaderViewSetting_FontFamily, ReaderViewRightRow_Highlight,
                        ReaderViewRightFilter_All, 0, selection->selected_text);
  item += 1;
  if ((selection->flags & ReaderViewSelection_CanWebLookup) != 0 &&
      rv_popup_button(ctx, 153, selection->selection_key,
                      rv_rect(popup.x + RV_GAP + (item % columns) * button_w,
                              popup.y + RV_GAP + (item / columns) * 38,
                              button_w - 2, RV_CONTROL_HEIGHT),
                      labels.web_lookup, 1, 0, 0))
    (void)rv_add_action(ctx, ReaderViewAction_WebLookupSelection,
                        selection->selection_key, 0,
                        ReaderViewSetting_FontFamily, ReaderViewRightRow_Highlight,
                        ReaderViewRightFilter_All, 0, selection->selected_text);
  item += 1;
  if ((selection->flags & ReaderViewSelection_CanTranslate) != 0 &&
      rv_popup_button(ctx, 154, selection->selection_key,
                      rv_rect(popup.x + RV_GAP + (item % columns) * button_w,
                              popup.y + RV_GAP + (item / columns) * 38,
                              button_w - 2, RV_CONTROL_HEIGHT),
                      labels.translate, 1, 0, 0))
    (void)rv_add_action(ctx, ReaderViewAction_TranslateSelection,
                        selection->selection_key, 0,
                        ReaderViewSetting_FontFamily, ReaderViewRightRow_Highlight,
                        ReaderViewRightFilter_All, 0, selection->selected_text);
  item += 1;
  if ((selection->flags & ReaderViewSelection_CanRemoveHighlight) != 0 &&
      rv_popup_button(ctx, 155, selection->selection_key,
                      rv_rect(popup.x + RV_GAP + (item % columns) * button_w,
                              popup.y + RV_GAP + (item / columns) * 38,
                              button_w - 2, RV_CONTROL_HEIGHT),
                      labels.delete_value, 1, 0, 1))
    (void)rv_add_action(ctx, ReaderViewAction_RemoveHighlight,
                        selection->selection_key, selection->current_color_key,
                        ReaderViewSetting_FontFamily, ReaderViewRightRow_Highlight,
                        ReaderViewRightFilter_All, 0, rv_text(0, 0));
  item += 1;
  for (color_index = 0;
       color_index < selection->highlight_colors.count && color_index < 4;
       ++color_index)
  {
    const ReaderViewChoice *color =
      selection->highlight_colors.items + color_index;
    if (rv_popup_button(ctx, 160, color->key,
                        rv_rect(popup.x + RV_GAP + (item % columns) * button_w,
                                popup.y + RV_GAP + (item / columns) * 38,
                                button_w - 2, RV_CONTROL_HEIGHT),
                        color->label,
                        (color->flags & ReaderViewChoice_Enabled) != 0,
                        color->key == selection->current_color_key, 0))
      (void)rv_add_action(ctx, ReaderViewAction_SetHighlightColor,
                          selection->selection_key, color->key,
                          ReaderViewSetting_FontFamily,
                          ReaderViewRightRow_Highlight,
                          ReaderViewRightFilter_All, 0, rv_text(0, 0));
    item += 1;
  }
}

static UI0B32
rv_rect_intersects(UI0Rect a, UI0Rect b)
{
  return a.x < b.x + b.w && a.x + a.w > b.x &&
         a.y < b.y + b.h && a.y + a.h > b.y;
}

static ReaderViewText
rv_status_text(ReaderViewSurfaceStatus status)
{
  if (status.message.size) return status.message;
  switch (status.state)
  {
    case ReaderViewLoad_Unavailable: return rv_literal("Unavailable");
    case ReaderViewLoad_Empty: return rv_literal("Nothing here");
    case ReaderViewLoad_Loading: return rv_literal("Loading");
    case ReaderViewLoad_Error: return rv_literal("Unable to load");
    case ReaderViewLoad_Ready:
    default: return rv_text(0, 0);
  }
}

static void
rv_add_status(RVBuildContext *ctx,
              UI0ID parent,
              UI0U64 tag,
              UI0Rect rect,
              ReaderViewSurfaceStatus status)
{
  ReaderViewSemanticFlags flags = ReaderViewSemantic_Enabled;
  if (status.state == ReaderViewLoad_Loading) flags |= ReaderViewSemantic_Busy;
  rv_add_text_record(ctx, rv_id(tag, 0), parent, rect,
                     rv_status_text(status), ReaderViewSemantic_Status,
                     flags, 0);
  if (status.detail.size)
    rv_add_text_record(ctx, rv_id(tag + 1, 0), parent,
                       rv_rect(rect.x, rect.y + 24, rect.w, rect.h - 24),
                       status.detail, ReaderViewSemantic_Status, flags, 0);
}

static UI0S32
rv_scroll_region(RVBuildContext *ctx,
                 UI0ID id,
                 UI0Rect viewport,
                 UI0S32 content_h,
                 UI0S32 *scroll_y,
                 UI0ScrollState *state)
{
  UI0ScrollSpec spec;
  UI0ScrollResult result;
  memset(&spec, 0, sizeof(spec));
  spec.id = id;
  spec.root = UI0RootKind_Normal;
  spec.viewport_rect = viewport;
  spec.content_h = content_h;
  spec.scroll_y = *scroll_y;
  spec.wheel_delta_y = ctx->input->input->ui.wheel_delta_y;
  result = ui0_scroll_region(&ctx->scrolls, &ctx->signals, spec, state);
  *scroll_y = result.scroll_y;
  return result.scroll_y;
}

static void
rv_build_toc_panel(RVBuildContext *ctx,
                   UI0Rect panel,
                   UI0Rect list_rect,
                   ReaderViewLabels labels)
{
  const ReaderViewTocProjection *toc = &ctx->input->projection->toc;
  UI0S32 scroll_y;
  UI0S32 index;
  UI0S32 content_h = toc->row_count * RV_ROW_HEIGHT;
  scroll_y = rv_scroll_region(ctx, rv_id(210, 0), list_rect, content_h,
                              &ctx->input->state->toc_scroll_y,
                              &ctx->input->state->toc_scroll);
  if (toc->status.state != ReaderViewLoad_Ready)
  {
    rv_add_status(ctx, ctx->left_panel_id, 211, list_rect, toc->status);
    return;
  }
  if (toc->row_count == 0)
  {
    ReaderViewSurfaceStatus empty = toc->status;
    empty.state = ReaderViewLoad_Empty;
    rv_add_status(ctx, ctx->left_panel_id, 211, list_rect, empty);
    return;
  }
  for (index = 0; index < toc->row_count; ++index)
  {
    const ReaderViewTocRow *row = toc->rows + index;
    UI0Rect row_rect = rv_rect(list_rect.x + rv_min((UI0S32)row->depth, 6) * 12,
                               list_rect.y + index * RV_ROW_HEIGHT - scroll_y,
                               list_rect.w - rv_min((UI0S32)row->depth, 6) * 12 - 12,
                               RV_ROW_HEIGHT - 2);
    UI0ID id = rv_id(212, row->key);
    if (!rv_rect_intersects(row_rect, list_rect)) continue;
    if (rv_add_control(ctx, id, ctx->left_panel_id,
                       UI0ControlKind_TreeRow,
                       ReaderViewSemantic_ListItem,
                       UI0RootKind_Normal,
                       row_rect,
                       row->label,
                       row->detail,
                       row->key,
                       (row->flags & ReaderViewRow_Enabled) != 0,
                       (row->flags & (ReaderViewRow_Selected |
                                      ReaderViewRow_Current)) != 0,
                       0, 0, 0))
    {
      ctx->input->state->active_toc_key = row->key;
      (void)rv_add_action(ctx, ReaderViewAction_ActivateTocRow,
                          row->key, 0, ReaderViewSetting_FontFamily,
                          ReaderViewRightRow_Bookmark,
                          ReaderViewRightFilter_All, 0, rv_text(0, 0));
    }
  }
  (void)panel;
  (void)labels;
}

static void
rv_process_find_input(RVBuildContext *ctx)
{
  ReaderViewState *state = ctx->input->state;
  UI0TextInputBuffer buffer;
  UI0B32 committed = 0;
  UI0B32 edited;
  buffer.data = state->find_query;
  buffer.length = &state->find_query_length;
  buffer.cap = READER_VIEW_FIND_QUERY_CAP;
  edited = rv_apply_text_input(&buffer, &state->find_input,
                               &ctx->input->input->find_text,
                               &committed);
  if (edited)
    (void)rv_add_action(ctx, ReaderViewAction_FindChanged, 0, 0,
                        ReaderViewSetting_FontFamily, ReaderViewRightRow_Bookmark,
                        ReaderViewRightFilter_All, 0,
                        reader_view_find_query(state));
  if (committed)
    (void)rv_add_action(ctx, ReaderViewAction_FindCommitted, 0, 0,
                        ReaderViewSetting_FontFamily, ReaderViewRightRow_Bookmark,
                        ReaderViewRightFilter_All, 0,
                        reader_view_find_query(state));
  if (edited || committed)
    ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged;
}

static void
rv_build_find_panel(RVBuildContext *ctx,
                    UI0Rect panel,
                    UI0Rect list_rect,
                    ReaderViewLabels labels)
{
  const ReaderViewFindProjection *find = &ctx->input->projection->find;
  ReaderViewState *state = ctx->input->state;
  UI0Rect input_rect = rv_rect(panel.x + RV_INSET, panel.y + 54,
                               panel.w - RV_INSET * 2 - 58,
                               RV_CONTROL_HEIGHT);
  UI0Rect clear_rect = rv_rect(input_rect.x + input_rect.w + 4, input_rect.y,
                               54, RV_CONTROL_HEIGHT);
  UI0Rect step_rect = rv_rect(panel.x + RV_INSET, input_rect.y + 38,
                              panel.w - RV_INSET * 2, RV_CONTROL_HEIGHT);
  UI0S32 scroll_y;
  UI0S32 index;
  UI0S32 content_h;
  rv_process_find_input(ctx);
  (void)rv_add_control(ctx, rv_id(220, 0), ctx->left_panel_id,
                       UI0ControlKind_TextInput,
                       ReaderViewSemantic_SearchBox,
                       UI0RootKind_Normal,
                       input_rect,
                       reader_view_find_query(state),
                       find->committed_query,
                       0, 1, 0, 0, 0, 0);
  if (rv_add_control(ctx, rv_id(221, 0), ctx->left_panel_id,
                     UI0ControlKind_TextButton, ReaderViewSemantic_Button,
                     UI0RootKind_Normal, clear_rect, labels.clear,
                     rv_text(0, 0), 0, state->find_query_length > 0,
                     0, 0, 0, 0))
  {
    state->find_query_length = 0;
    state->find_query[0] = 0;
    state->find_input.caret = 0;
    state->find_input.selection_anchor = 0;
    ui0_text_input_history_reset(&state->find_history);
    (void)rv_add_action(ctx, ReaderViewAction_FindChanged, 0, 0,
                        ReaderViewSetting_FontFamily, ReaderViewRightRow_Bookmark,
                        ReaderViewRightFilter_All, 0,
                        reader_view_find_query(state));
  }
  if (rv_add_control(ctx, rv_id(222, 0), ctx->left_panel_id,
                     UI0ControlKind_IconButton, ReaderViewSemantic_Button,
                     UI0RootKind_Normal,
                     rv_rect(step_rect.x, step_rect.y, step_rect.w / 2 - 2,
                             step_rect.h),
                     labels.previous_match, rv_text(0, 0), 0,
                     find->can_step_previous, 0, 0, 0, 0))
    (void)rv_add_action(ctx, ReaderViewAction_FindPrevious, 0, 0,
                        ReaderViewSetting_FontFamily, ReaderViewRightRow_Bookmark,
                        ReaderViewRightFilter_All, 0, rv_text(0, 0));
  if (rv_add_control(ctx, rv_id(223, 0), ctx->left_panel_id,
                     UI0ControlKind_IconButton, ReaderViewSemantic_Button,
                     UI0RootKind_Normal,
                     rv_rect(step_rect.x + step_rect.w / 2 + 2, step_rect.y,
                             step_rect.w - step_rect.w / 2 - 2, step_rect.h),
                     labels.next_match, rv_text(0, 0), 0,
                     find->can_step_next, 0, 0, 0, 0))
    (void)rv_add_action(ctx, ReaderViewAction_FindNext, 0, 0,
                        ReaderViewSetting_FontFamily, ReaderViewRightRow_Bookmark,
                        ReaderViewRightFilter_All, 0, rv_text(0, 0));

  if (find->status.state != ReaderViewLoad_Ready)
  {
    rv_add_status(ctx, ctx->left_panel_id, 224, list_rect, find->status);
    return;
  }
  content_h = find->row_count * RV_ROW_HEIGHT;
  scroll_y = rv_scroll_region(ctx, rv_id(225, 0), list_rect, content_h,
                              &state->find_scroll_y, &state->find_scroll);
  if (find->row_count == 0)
  {
    ReaderViewSurfaceStatus empty = find->status;
    empty.state = ReaderViewLoad_Empty;
    rv_add_status(ctx, ctx->left_panel_id, 226, list_rect, empty);
  }
  for (index = 0; index < find->row_count; ++index)
  {
    const ReaderViewFindRow *row = find->rows + index;
    UI0Rect row_rect = rv_rect(list_rect.x,
                               list_rect.y + index * RV_ROW_HEIGHT - scroll_y,
                               list_rect.w - 12,
                               RV_ROW_HEIGHT - 2);
    if (!rv_rect_intersects(row_rect, list_rect)) continue;
    if (rv_add_control(ctx, rv_id(227, row->key), ctx->left_panel_id,
                       UI0ControlKind_ListRow,
                       ReaderViewSemantic_ListItem,
                       UI0RootKind_Normal,
                       row_rect,
                       row->excerpt,
                       row->section,
                       row->key,
                       (row->flags & ReaderViewRow_Enabled) != 0,
                       index == find->active_index ||
                       (row->flags & ReaderViewRow_Selected) != 0,
                       0, 0, 0))
    {
      state->active_find_key = row->key;
      (void)rv_add_action(ctx, ReaderViewAction_ActivateFindRow,
                          row->key, 0, ReaderViewSetting_FontFamily,
                          ReaderViewRightRow_Bookmark,
                          ReaderViewRightFilter_All, 0, rv_text(0, 0));
    }
  }
}

static void
rv_build_left_panel(RVBuildContext *ctx, ReaderViewLabels labels)
{
  ReaderViewState *state = ctx->input->state;
  UI0Rect panel = ctx->input->layout->left_panel_rect;
  UI0Rect tabs;
  UI0Rect list_rect;
  if (!ctx->input->layout->left_panel_visible || panel.w <= 0) return;
  ctx->left_panel_id = rv_id(200, 0);
  (void)rv_add_surface(ctx, ctx->left_panel_id, 0,
                       UI0ControlKind_PanelSurface,
                       ReaderViewSemantic_Panel,
                       UI0RootKind_Normal,
                       panel, labels.contents);
  tabs = rv_rect(panel.x + RV_INSET, panel.y + RV_GAP,
                 panel.w - RV_INSET * 2 - 36, RV_CONTROL_HEIGHT);
  if (rv_has_feature(ctx->input->projection, ReaderViewFeature_Contents) &&
      rv_add_control(ctx, rv_id(201, 0), ctx->left_panel_id,
                     UI0ControlKind_SegmentItem, ReaderViewSemantic_Tab,
                     UI0RootKind_Normal,
                     rv_rect(tabs.x, tabs.y, tabs.w / 2, tabs.h),
                     labels.contents, rv_text(0, 0), 0, 1,
                     state->left_panel == ReaderViewLeftPanel_Contents,
                     state->left_panel == ReaderViewLeftPanel_Contents,
                     0, 0))
    state->left_panel = ReaderViewLeftPanel_Contents;
  if (rv_has_feature(ctx->input->projection, ReaderViewFeature_Find) &&
      rv_add_control(ctx, rv_id(202, 0), ctx->left_panel_id,
                     UI0ControlKind_SegmentItem, ReaderViewSemantic_Tab,
                     UI0RootKind_Normal,
                     rv_rect(tabs.x + tabs.w / 2, tabs.y,
                             tabs.w - tabs.w / 2, tabs.h),
                     labels.find, rv_text(0, 0), 0, 1,
                     state->left_panel == ReaderViewLeftPanel_Find,
                     state->left_panel == ReaderViewLeftPanel_Find,
                     0, 0))
    state->left_panel = ReaderViewLeftPanel_Find;
  if (rv_add_control(ctx, rv_id(203, 0), ctx->left_panel_id,
                     UI0ControlKind_IconButton, ReaderViewSemantic_Button,
                     UI0RootKind_Normal,
                     rv_rect(panel.x + panel.w - RV_INSET - 32,
                             panel.y + RV_GAP, 32, RV_CONTROL_HEIGHT),
                     labels.close, rv_text(0, 0), 0, 1, 0, 0, 0, 0))
    rv_toggle_left_panel(ctx, state->left_panel);
  list_rect = rv_rect(panel.x + RV_INSET,
                      panel.y + (state->left_panel == ReaderViewLeftPanel_Find ? 132 : 52),
                      panel.w - RV_INSET * 2,
                      panel.h - (state->left_panel == ReaderViewLeftPanel_Find ? 144 : 64));
  if (state->left_panel == ReaderViewLeftPanel_Find)
    rv_build_find_panel(ctx, panel, list_rect, labels);
  else
    rv_build_toc_panel(ctx, panel, list_rect, labels);
}

static ReaderViewRightFilterFlags
rv_filter_flag(ReaderViewRightFilter filter)
{
  switch (filter)
  {
    case ReaderViewRightFilter_All: return ReaderViewRightFilterFlag_All;
    case ReaderViewRightFilter_Bookmarks: return ReaderViewRightFilterFlag_Bookmarks;
    case ReaderViewRightFilter_Highlights: return ReaderViewRightFilterFlag_Highlights;
    case ReaderViewRightFilter_Notes: return ReaderViewRightFilterFlag_Notes;
    default: return 0;
  }
}

static ReaderViewText
rv_filter_label(ReaderViewLabels labels, ReaderViewRightFilter filter)
{
  switch (filter)
  {
    case ReaderViewRightFilter_All: return labels.all;
    case ReaderViewRightFilter_Bookmarks: return labels.bookmarks;
    case ReaderViewRightFilter_Highlights: return labels.highlights;
    case ReaderViewRightFilter_Notes: return labels.notes;
    default: return labels.all;
  }
}

static void
rv_build_right_panel(RVBuildContext *ctx, ReaderViewLabels labels)
{
  const ReaderViewRightProjection *right = &ctx->input->projection->right;
  ReaderViewState *state = ctx->input->state;
  UI0Rect panel = ctx->input->layout->right_panel_rect;
  UI0Rect filter_rect;
  UI0Rect list_rect;
  UI0S32 filter_count = 0;
  UI0S32 filter_slot = 0;
  UI0S32 filter;
  UI0S32 scroll_y;
  UI0S32 content_h;
  UI0S32 index;
  if (!ctx->input->layout->right_panel_visible || panel.w <= 0) return;
  ctx->right_panel_id = rv_id(300, 0);
  (void)rv_add_surface(ctx, ctx->right_panel_id, 0,
                       UI0ControlKind_PanelSurface,
                       ReaderViewSemantic_Panel,
                       UI0RootKind_Normal,
                       panel, labels.annotations);
  rv_add_text_record(ctx, rv_id(301, 0), ctx->right_panel_id,
                     rv_rect(panel.x + RV_INSET, panel.y + RV_GAP,
                             panel.w - 88, RV_CONTROL_HEIGHT),
                     labels.annotations, ReaderViewSemantic_Group,
                     ReaderViewSemantic_Enabled, 0);
  if (rv_has_feature(ctx->input->projection, ReaderViewFeature_Export) &&
      rv_add_control(ctx, rv_id(302, 0), ctx->right_panel_id,
                     UI0ControlKind_TextButton, ReaderViewSemantic_Button,
                     UI0RootKind_Normal,
                     rv_rect(panel.x + panel.w - 78, panel.y + RV_GAP,
                             36, RV_CONTROL_HEIGHT),
                     labels.export_rows, rv_text(0, 0), 0,
                     right->status.state == ReaderViewLoad_Ready &&
                     right->row_count > 0, 0, 0, 0, 0))
    (void)rv_add_action(ctx, ReaderViewAction_ExportRightRows, 0, 0,
                        ReaderViewSetting_FontFamily, ReaderViewRightRow_Bookmark,
                        state->right_filter, 0, rv_text(0, 0));
  if (rv_add_control(ctx, rv_id(303, 0), ctx->right_panel_id,
                     UI0ControlKind_IconButton, ReaderViewSemantic_Button,
                     UI0RootKind_Normal,
                     rv_rect(panel.x + panel.w - 38, panel.y + RV_GAP,
                             30, RV_CONTROL_HEIGHT),
                     labels.close, rv_text(0, 0), 0, 1, 0, 0, 0, 0))
    rv_toggle_right_panel(ctx);

  for (filter = ReaderViewRightFilter_All;
       filter <= ReaderViewRightFilter_Notes; ++filter)
    if ((right->available_filters &
         rv_filter_flag((ReaderViewRightFilter)filter)) != 0)
      filter_count += 1;
  if (filter_count == 0) filter_count = 1;
  filter_rect = rv_rect(panel.x + RV_INSET, panel.y + 48,
                        panel.w - RV_INSET * 2, RV_CONTROL_HEIGHT);
  for (filter = ReaderViewRightFilter_All;
       filter <= ReaderViewRightFilter_Notes; ++filter)
  {
    ReaderViewRightFilter value = (ReaderViewRightFilter)filter;
    if ((right->available_filters & rv_filter_flag(value)) == 0) continue;
    if (rv_add_control(ctx, rv_id(310 + (UI0U64)filter, 0),
                       ctx->right_panel_id,
                       UI0ControlKind_SegmentItem,
                       ReaderViewSemantic_Tab,
                       UI0RootKind_Normal,
                       rv_toolbar_slot(filter_rect, filter_slot++, filter_count),
                       rv_filter_label(labels, value), rv_text(0, 0), 0, 1,
                       state->right_filter == value,
                       state->right_filter == value, 0, 0))
    {
      state->right_filter = value;
      (void)rv_add_action(ctx, ReaderViewAction_RightFilterChanged,
                          0, 0, ReaderViewSetting_FontFamily,
                          ReaderViewRightRow_Bookmark, value, 0,
                          rv_text(0, 0));
      ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged;
    }
  }
  list_rect = rv_rect(panel.x + RV_INSET, panel.y + 88,
                      panel.w - RV_INSET * 2, panel.h - 100);
  if (right->status.state != ReaderViewLoad_Ready)
  {
    rv_add_status(ctx, ctx->right_panel_id, 320, list_rect, right->status);
    return;
  }
  content_h = right->row_count * RV_ROW_HEIGHT;
  scroll_y = rv_scroll_region(ctx, rv_id(322, 0), list_rect, content_h,
                              &state->right_scroll_y, &state->right_scroll);
  if (right->row_count == 0)
  {
    ReaderViewSurfaceStatus empty = right->status;
    empty.state = ReaderViewLoad_Empty;
    rv_add_status(ctx, ctx->right_panel_id, 323, list_rect, empty);
  }
  for (index = 0; index < right->row_count; ++index)
  {
    const ReaderViewRightRow *row = right->rows + index;
    UI0Rect row_rect = rv_rect(list_rect.x,
                               list_rect.y + index * RV_ROW_HEIGHT - scroll_y,
                               list_rect.w - 12,
                               RV_ROW_HEIGHT - 2);
    UI0Rect main_rect = rv_rect(row_rect.x, row_rect.y,
                                row_rect.w - 64, row_rect.h);
    if (!rv_rect_intersects(row_rect, list_rect)) continue;
    if (rv_add_control(ctx, rv_id(324, row->key), ctx->right_panel_id,
                       UI0ControlKind_ListRow,
                       ReaderViewSemantic_ListItem,
                       UI0RootKind_Normal,
                       main_rect,
                       row->primary,
                       row->section,
                       row->key,
                       (row->flags & ReaderViewRow_Enabled) != 0,
                       (row->flags & ReaderViewRow_Selected) != 0,
                       0, 0, 0))
    {
      state->active_right_key = row->key;
      if ((row->actions & ReaderViewRightAction_Activate) != 0)
        (void)rv_add_action(ctx, ReaderViewAction_ActivateRightRow,
                            row->key, 0, ReaderViewSetting_FontFamily, row->kind,
                            state->right_filter, 0, rv_text(0, 0));
    }
    if ((row->actions & ReaderViewRightAction_ToggleStar) != 0 &&
        rv_add_control(ctx, rv_id(325, row->key), ctx->right_panel_id,
                       UI0ControlKind_IconButton,
                       ReaderViewSemantic_ToggleButton,
                       UI0RootKind_Normal,
                       rv_rect(row_rect.x + row_rect.w - 62, row_rect.y,
                               30, row_rect.h),
                       (row->flags & ReaderViewRow_Starred) ?
                         labels.unstar : labels.star,
                       rv_text(0, 0), row->key, 1, 0,
                       (row->flags & ReaderViewRow_Starred) != 0, 0, 0))
      (void)rv_add_action(ctx, ReaderViewAction_ToggleRightRowStar,
                          row->key, 0, ReaderViewSetting_FontFamily, row->kind,
                          state->right_filter, 0, rv_text(0, 0));
    if (rv_add_control(ctx, rv_id(326, row->key), ctx->right_panel_id,
                       UI0ControlKind_MenuTrigger,
                       ReaderViewSemantic_Button,
                       UI0RootKind_Normal,
                       rv_rect(row_rect.x + row_rect.w - 30, row_rect.y,
                               30, row_rect.h),
                       labels.more, rv_text(0, 0), row->key,
                       row->actions != ReaderViewRightAction_None, 0, 0,
                       state->popup == ReaderViewPopup_RightRowActions &&
                       state->right_menu_key == row->key, 0))
    {
      state->right_menu_key = row->key;
      state->right_menu_kind = row->kind;
      state->popup = ReaderViewPopup_RightRowActions;
      state->restore_focus_id = rv_id(326, row->key);
      ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged;
    }
  }
}

static UI0Rect
rv_gutter_icon_rect(UI0Rect visual_rect)
{
  UI0S32 width = rv_min(8, visual_rect.w);
  UI0S32 height = rv_min(14, visual_rect.h);
  return rv_rect(visual_rect.x + (visual_rect.w - width) / 2,
                 visual_rect.y + (visual_rect.h - height) / 2,
                 width,
                 height);
}

static UI0B32
rv_gutter_icon_control(RVBuildContext *ctx,
                       UI0U64 tag,
                       ReaderViewSemanticControl semantic_control,
                       UI0Rect hot_rect,
                       UI0Rect visual_rect,
                       ReaderViewText label,
                       UI0B32 enabled,
                       UI0IconKind icon_kind)
{
  UI0S32 control_index;
  UI0S32 icon_index;
  UI0B32 invoked;
  if (hot_rect.w <= 0 || hot_rect.h <= 0 ||
      visual_rect.w <= 0 || visual_rect.h <= 0)
  {
    return 0;
  }
  control_index = ctx->control_count;
  icon_index = ctx->icon_count;
  invoked = rv_add_icon_control(ctx,
                                rv_id(tag, 0),
                                0,
                                UI0ControlKind_IconButton,
                                ReaderViewSemantic_Button,
                                UI0RootKind_Normal,
                                visual_rect,
                                hot_rect,
                                label,
                                rv_text(0, 0),
                                0,
                                enabled,
                                0,
                                0,
                                0,
                                0,
                                icon_kind,
                                rv_gutter_icon_rect(visual_rect),
                                0);
  if (ctx->control_count > control_index && ctx->icon_count > icon_index)
  {
    UI0ControlStateFlags state =
      ctx->storage->control_records[control_index].state;
    ctx->icons[icon_index].visible = enabled &&
      (state & (UI0ControlState_Hovered |
                UI0ControlState_Pressed |
                UI0ControlState_FocusVisible)) != 0;
  }
  if (ctx->control_count > control_index)
  {
    rv_set_semantic_control(ctx, rv_id(tag, 0), semantic_control);
  }
  return invoked;
}

static void
rv_build_paging_and_progress(RVBuildContext *ctx, ReaderViewLabels labels)
{
  const ReaderViewProjection *projection = ctx->input->projection;
  const ReaderViewLayout *layout = ctx->input->layout;
  if (layout->previous_gutter_visible &&
      rv_gutter_icon_control(
        ctx, 400, ReaderViewSemanticControl_PreviousPage,
        layout->previous_gutter_rect,
        layout->previous_gutter_visual_rect,
        labels.previous_page,
        rv_has_document_flag(projection,
          ReaderViewDocument_CanGoPreviousPage),
        UI0IconKind_ChevronLeft))
    (void)rv_add_action(ctx, ReaderViewAction_PreviousPage, 0, 0,
                        ReaderViewSetting_FontFamily, ReaderViewRightRow_Bookmark,
                        ReaderViewRightFilter_All, 0, rv_text(0, 0));
  if (layout->next_gutter_visible &&
      rv_gutter_icon_control(
        ctx, 401, ReaderViewSemanticControl_NextPage,
        layout->next_gutter_rect,
        layout->next_gutter_visual_rect,
        labels.next_page,
        rv_has_document_flag(projection,
          ReaderViewDocument_CanGoNextPage),
        UI0IconKind_ChevronRight))
    (void)rv_add_action(ctx, ReaderViewAction_NextPage, 0, 0,
                        ReaderViewSetting_FontFamily, ReaderViewRightRow_Bookmark,
                        ReaderViewRightFilter_All, 0, rv_text(0, 0));
  if (layout->progress_visible && projection->progress.location_count > 0)
  {
    UI0SliderRectSpec spec;
    UI0SliderResult result;
    UI0SliderStyle slider_style =
      ui0_slider_style_from_resolved(ctx->input->theme);
    UI0TypographyToken footer_typography =
      ctx->input->theme->typography[UI0TypographyRole_Body];
    UI0S32 footer_height = rv_max(footer_typography.line_height, 1);
    UI0S32 footer_gap = rv_max(
      ctx->input->theme->spacing[UI0SpacingRole_ControlGap], 0);
    UI0S32 visual_track_y = layout->progress_rect.y +
      (layout->progress_rect.h -
       rv_min(slider_style.track_height, layout->progress_rect.h)) / 2;
    UI0U64 slider_count_u64 = projection->progress.location_count;
    UI0S32 slider_max = slider_count_u64 > 2147483647ull ?
      2147483647 : (UI0S32)slider_count_u64;
    UI0S32 slider_value = projection->progress.location_index + 1ull >
      (UI0U64)slider_max ? slider_max :
      (UI0S32)(projection->progress.location_index + 1ull);
    UI0Rect slider_rect = layout->progress_rect;
    memset(&spec, 0, sizeof(spec));
    spec.id = rv_id(402, 0);
    spec.root = UI0RootKind_Normal;
    spec.rect = slider_rect;
    spec.hit_rect = slider_rect;
    spec.clip_rect = layout->progress_rect;
    spec.value = slider_value;
    spec.min_value = 1;
    spec.max_value = slider_max;
    spec.step = 1;
    if (ctx->input->state->pending_accessibility_focus_id == spec.id)
    {
      ctx->input->state->pending_accessibility_focus_id = 0;
      if (projection->progress.can_seek)
      {
        ctx->signals.focus_id = spec.id;
        ctx->signals.focus_visible_id = spec.id;
        ctx->frame->change_flags |= ReaderViewFrameChange_FocusChanged;
      }
    }
    if (ctx->input->state->pending_accessibility_invoke_id == spec.id)
    {
      ctx->input->state->pending_accessibility_invoke_id = 0;
      if (projection->progress.can_seek)
      {
        ctx->signals.focus_id = spec.id;
        ctx->signals.focus_visible_id = spec.id;
        ctx->frame->change_flags |= ReaderViewFrameChange_FocusChanged;
      }
    }
    spec.keyboard_delta = ctx->signals.focus_id == spec.id ?
      ctx->input->input->move_horizontal_delta : 0;
    if (!projection->progress.can_seek) spec.flags |= UI0Slider_Disabled;
    result = ui0_slider_rect(&ctx->sliders, &ctx->signals, spec);
    ctx->progress_thumb_visible = projection->progress.can_seek &&
      (result.state & (UI0SliderState_Hovered |
                       UI0SliderState_Pressed |
                       UI0SliderState_Active |
                       UI0SliderState_Dragged)) != 0;
    (void)rv_add_semantic(ctx, spec.id, 0, ReaderViewSemantic_Slider,
                          rv_semantic_flags(ctx, spec.id,
                                            projection->progress.can_seek, 1,
                                            0, 0, 0),
                          slider_rect, projection->progress.label,
                          projection->progress.chapter, 0,
                          projection->progress.location_index, 0,
                          projection->progress.location_count - 1);
    rv_set_semantic_control(ctx, spec.id, ReaderViewSemanticControl_Progress);
    if (result.changed)
    {
      UI0U64 location = result.next_value > 0 ?
        (UI0U64)result.next_value - 1ull : 0;
      (void)rv_add_action(ctx, ReaderViewAction_SeekLocation, 0, 0,
                          ReaderViewSetting_FontFamily, ReaderViewRightRow_Bookmark,
                          ReaderViewRightFilter_All, location, rv_text(0, 0));
    }
    rv_add_text_record_styled(ctx, rv_id(403, 0), 0,
                              rv_rect(layout->page_surface_rect.x,
                                      visual_track_y - footer_gap -
                                        footer_height,
                                      layout->page_surface_rect.w,
                                      footer_height),
                              projection->progress.label,
                              ReaderViewTextStyle_ChromeMetadata,
                              ReaderViewSemantic_Status,
                              ReaderViewSemantic_Enabled, 0);
  }
}

static void
rv_filter_reference_chrome_draws(RVBuildContext *ctx)
{
  UI0S32 write_index = 0;
  UI0ID previous_id = rv_id(400, 0);
  UI0ID next_id = rv_id(401, 0);
  UI0ID progress_id = rv_id(402, 0);
  UI0ID progress_label_id = rv_id(403, 0);
  for (UI0S32 read_index = 0;
       read_index < ctx->draw.command_count;
       ++read_index)
  {
    UI0DrawCommand command = ctx->draw.commands[read_index];
    UI0B32 gutter_shell =
      (command.source_id == previous_id || command.source_id == next_id) &&
      (command.op == UI0DrawOp_ControlFill ||
       command.op == UI0DrawOp_ControlBorder ||
       command.op == UI0DrawOp_Text);
    UI0B32 hidden_progress_thumb = command.source_id == progress_id &&
      !ctx->progress_thumb_visible &&
      (command.op == UI0DrawOp_SliderThumb ||
       command.op == UI0DrawOp_FocusRing);
    if (gutter_shell || hidden_progress_thumb) continue;
    if (command.source_id == progress_label_id &&
        command.op == UI0DrawOp_Text)
    {
      command.color = ctx->input->theme->colors[UI0ColorRole_TextMuted];
    }
    ctx->draw.commands[write_index++] = command;
  }
  ctx->draw.command_count = write_index;
}

static void
rv_build_note_editor(RVBuildContext *ctx,
                     UI0Rect modal,
                     ReaderViewLabels labels)
{
  ReaderViewState *state = ctx->input->state;
  const ReaderViewSelectionProjection *selection =
    &ctx->input->projection->selection;
  UI0TextInputBuffer buffer;
  UI0Rect editor_rect;
  UI0Rect button_row;
  UI0B32 edited;
  ctx->modal_id = rv_id(500, state->note_selection_key);
  (void)rv_add_surface(ctx, ctx->modal_id, 0,
                       UI0ControlKind_ModalSurface,
                       ReaderViewSemantic_Dialog,
                       UI0RootKind_Modal,
                       modal, labels.edit_note);
  rv_add_text_record(ctx, rv_id(501, state->note_selection_key),
                     ctx->modal_id,
                     rv_rect(modal.x + RV_INSET, modal.y + RV_INSET,
                             modal.w - RV_INSET * 2, 30),
                     labels.edit_note, ReaderViewSemantic_Group,
                     ReaderViewSemantic_Enabled, state->note_selection_key);
  editor_rect = rv_rect(modal.x + RV_INSET, modal.y + 48,
                        modal.w - RV_INSET * 2, modal.h - 108);
  buffer.data = state->note_draft;
  buffer.length = &state->note_draft_length;
  buffer.cap = READER_VIEW_NOTE_DRAFT_CAP;
  edited = rv_apply_text_area(&buffer, &state->note_input,
                              &ctx->input->input->note_text,
                              ctx->input->input->move_vertical_delta);
  if (edited)
  {
    state->note_dirty = 1;
    ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged;
  }
  if (state->note_selection_key != selection->selection_key ||
      state->note_source_revision != selection->revision)
    ctx->frame->error_flags |= ReaderViewFrameError_StaleNoteRevision;
  (void)rv_add_control(ctx, rv_id(502, state->note_selection_key),
                       ctx->modal_id, UI0ControlKind_TextArea,
                       ReaderViewSemantic_TextArea, UI0RootKind_Modal,
                       editor_rect, reader_view_note_draft(state),
                       rv_text(0, 0), state->note_selection_key,
                       1, 0, 0, 0, 0);
  button_row = rv_rect(modal.x + RV_INSET,
                       modal.y + modal.h - 48,
                       modal.w - RV_INSET * 2,
                       RV_CONTROL_HEIGHT);
  if (rv_add_control(ctx, rv_id(503, state->note_selection_key),
                     ctx->modal_id, UI0ControlKind_TextButton,
                     ReaderViewSemantic_Button, UI0RootKind_Modal,
                     rv_rect(button_row.x, button_row.y,
                             button_row.w / 3 - 3, button_row.h),
                     labels.save_note, rv_text(0, 0),
                     state->note_selection_key,
                     state->note_dirty &&
                     state->note_selection_key == selection->selection_key &&
                     state->note_source_revision == selection->revision,
                     0, 0, 0, 0))
  {
    if (state->note_selection_key != selection->selection_key ||
        state->note_source_revision != selection->revision)
      ctx->frame->error_flags |= ReaderViewFrameError_StaleNoteRevision;
    else
      (void)rv_add_action(ctx, ReaderViewAction_SaveNote,
                          state->note_selection_key, 0,
                          ReaderViewSetting_FontFamily, ReaderViewRightRow_Note,
                          ReaderViewRightFilter_All,
                          state->note_source_revision,
                          reader_view_note_draft(state));
  }
  if (rv_add_control(ctx, rv_id(504, state->note_selection_key),
                     ctx->modal_id, UI0ControlKind_TextButton,
                     ReaderViewSemantic_Button, UI0RootKind_Modal,
                     rv_rect(button_row.x + button_row.w / 3,
                             button_row.y, button_row.w / 3 - 3,
                             button_row.h),
                     labels.cancel, rv_text(0, 0),
                     state->note_selection_key, 1, 0, 0, 0, 0))
  {
    state->popup = ReaderViewPopup_None;
    state->note_dirty = 0;
    state->focus_id = state->restore_focus_id;
    state->restore_focus_id = 0;
    ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged;
  }
  if (rv_add_control(ctx, rv_id(505, state->note_selection_key),
                     ctx->modal_id, UI0ControlKind_TextButton,
                     ReaderViewSemantic_Button, UI0RootKind_Modal,
                     rv_rect(button_row.x + (button_row.w * 2) / 3,
                             button_row.y,
                             button_row.w - (button_row.w * 2) / 3,
                             button_row.h),
                     labels.delete_value, rv_text(0, 0),
                     state->note_selection_key,
                     (selection->flags & ReaderViewSelection_CanDeleteNote) != 0,
                     0, 0, 0, 1))
    (void)rv_add_action(ctx, ReaderViewAction_DeleteNote,
                        state->note_selection_key, 0,
                        ReaderViewSetting_FontFamily, ReaderViewRightRow_Note,
                        ReaderViewRightFilter_All,
                        state->note_source_revision, rv_text(0, 0));
}

static void
rv_apply_semantic_focus_navigation(RVBuildContext *ctx)
{
  const ReaderViewInput *input;
  ReaderViewSemanticNode *nodes;
  UI0S32 current = -1;
  UI0S32 target = -1;
  UI0S32 direction;
  if (!ctx || !ctx->input || !ctx->input->input) return;
  input = ctx->input->input;
  if (input->move_vertical_delta == 0 &&
      input->range_move == ReaderViewRangeMove_None)
    return;
  nodes = ctx->storage->semantic_nodes;
  for (UI0S32 index = 0; index < ctx->semantic_count; index += 1)
  {
    if (nodes[index].id == ctx->signals.focus_id)
    {
      current = index;
      break;
    }
  }
  if (current < 0 ||
      (nodes[current].role != ReaderViewSemantic_ListItem &&
       nodes[current].role != ReaderViewSemantic_MenuItem &&
       nodes[current].role != ReaderViewSemantic_Tab))
    return;

  direction = input->move_vertical_delta < 0 ? -1 : 1;
  if (input->range_move == ReaderViewRangeMove_First) direction = 1;
  if (input->range_move == ReaderViewRangeMove_Last) direction = -1;
  UI0S32 start = input->range_move == ReaderViewRangeMove_First ? 0 :
                 input->range_move == ReaderViewRangeMove_Last ?
                   ctx->semantic_count - 1 : current + direction;
  UI0S32 end = direction > 0 ? ctx->semantic_count : -1;
  for (UI0S32 index = start; index != end; index += direction)
  {
    ReaderViewSemanticNode *candidate = nodes + index;
    if (candidate->parent_id == nodes[current].parent_id &&
        candidate->role == nodes[current].role &&
        (candidate->flags & (ReaderViewSemantic_Enabled |
                             ReaderViewSemantic_Focusable)) ==
          (ReaderViewSemantic_Enabled | ReaderViewSemantic_Focusable) &&
        (candidate->flags & ReaderViewSemantic_Offscreen) == 0)
    {
      target = index;
      break;
    }
  }
  if (target < 0 || target == current) return;
  nodes[current].flags &= ~ReaderViewSemantic_Focused;
  nodes[target].flags |= ReaderViewSemantic_Focused;
  ctx->signals.focus_id = nodes[target].id;
  ctx->signals.focus_visible_id = nodes[target].id;
  ctx->frame->change_flags |= ReaderViewFrameChange_FocusChanged;
}

static void
rv_build_popup(RVBuildContext *ctx,
               ReaderViewPopupKind root_popup,
               UI0Rect popup,
               ReaderViewLabels labels)
{
  if (root_popup == ReaderViewPopup_None ||
      ctx->input->state->popup != root_popup)
    return;
  if (root_popup == ReaderViewPopup_NoteEditor)
  {
    rv_build_note_editor(ctx, popup, labels);
    return;
  }
  ctx->popup_id = rv_id(600, (UI0U64)root_popup);
  (void)rv_add_surface(ctx, ctx->popup_id, 0,
                       UI0ControlKind_PopupSurface,
                       root_popup == ReaderViewPopup_SelectionTools ?
                         ReaderViewSemantic_Group : ReaderViewSemantic_Menu,
                       UI0RootKind_Popup,
                       popup, labels.more);
  switch (root_popup)
  {
    case ReaderViewPopup_SettingMenu:
      rv_build_setting_popup(ctx, popup);
      break;
    case ReaderViewPopup_Overflow:
      rv_build_overflow_popup(ctx, popup, labels);
      break;
    case ReaderViewPopup_RightRowActions:
      rv_build_right_actions_popup(ctx, popup, labels);
      break;
    case ReaderViewPopup_SelectionTools:
      rv_build_selection_popup(ctx, popup, labels);
      break;
    default:
      break;
  }
}

static void
rv_build_content_status(RVBuildContext *ctx)
{
  const ReaderViewProjection *projection = ctx->input->projection;
  UI0Rect viewport = ctx->input->layout->viewport_rect;
  if (projection->content.state == ReaderViewLoad_Ready) return;
  rv_add_status(ctx, 0, 700,
                rv_centered_rect(viewport, rv_min(420, viewport.w), 96),
                projection->content);
}

static UI0B32
rv_build_input_valid(const ReaderViewBuildInput *input,
                     ReaderViewFrameStorage *storage,
                     ReaderViewFrame *out_frame)
{
  return input && storage && out_frame && input->state && input->layout &&
         input->projection && input->input && input->theme;
}

UI0B32
reader_view_build(const ReaderViewBuildInput *input,
                  ReaderViewFrameStorage *storage,
                  ReaderViewFrame *out_frame)
{
  RVBuildContext ctx;
  ReaderViewFrameErrorFlags projection_errors;
  ReaderViewLabels labels;
  ReaderViewPopupKind root_popup;
  UI0Rect root_rect;
  UI0ID old_focus;
  UI0B32 popup_dismiss;
  if (!out_frame) return 0;
  memset(out_frame, 0, sizeof(*out_frame));
  if (!rv_build_input_valid(input, storage, out_frame))
  {
    out_frame->error_flags = ReaderViewFrameError_BadInput;
    return 0;
  }
  reader_view_frame_storage_init(storage);
  out_frame->layout = *input->layout;
  projection_errors = rv_validate_projection(input->projection);
  if (projection_errors != ReaderViewFrameError_None)
  {
    out_frame->error_flags = projection_errors;
    out_frame->change_flags = ReaderViewFrameChange_ProjectionInvalid;
    return 0;
  }
  if (input->state->document_key != input->projection->document_key)
  {
    if (input->state->document_key == 0)
      reader_view_state_reset_document(input->state,
                                       input->projection->document_key);
    else
    {
      out_frame->error_flags = ReaderViewFrameError_StaleDocumentState;
      out_frame->change_flags = ReaderViewFrameChange_ProjectionInvalid;
      return 0;
    }
  }

  memset(&ctx, 0, sizeof(ctx));
  ctx.input = input;
  ctx.storage = storage;
  ctx.frame = out_frame;
  labels = rv_resolve_labels(input->projection->labels);
  rv_sync_selection_popup(&ctx);
  root_popup = input->state->popup;
  root_rect = root_popup == ReaderViewPopup_NoteEditor ?
    rv_centered_rect(input->layout->bounds, RV_NOTE_WIDTH, RV_NOTE_HEIGHT) :
    rv_popup_rect(input);

  ui0_signal_context_init(&ctx.signals);
  ctx.signals.hot_id = input->state->hot_id;
  ctx.signals.active_id = input->state->active_id;
  ctx.signals.focus_id = input->state->focus_id;
  ctx.signals.focus_visible_id = input->state->focus_visible ?
    input->state->focus_id : 0;
  ui0_signal_begin_frame(&ctx.signals, input->input->ui,
                         storage->signal_records, READER_VIEW_SIGNAL_CAP);
  ctx.signals.frame_index = input->frame_index;
  ui0_signal_set_root(&ctx.signals, UI0RootKind_Popup, root_rect,
                      root_popup != ReaderViewPopup_None &&
                      root_popup != ReaderViewPopup_NoteEditor);
  ui0_signal_set_root(&ctx.signals, UI0RootKind_Modal, root_rect,
                      root_popup == ReaderViewPopup_NoteEditor);
  ui0_signal_resolve_roots(&ctx.signals);
  popup_dismiss = ctx.signals.popup_dismiss_requested;

  ui0_slider_context_init(&ctx.sliders);
  ui0_slider_begin_frame(&ctx.sliders, storage->slider_records,
                         READER_VIEW_SLIDER_CAP);
  ctx.sliders.frame_index = input->frame_index;
  ctx.sliders.style = ui0_slider_style_from_resolved(input->theme);
  ui0_scroll_context_init(&ctx.scrolls);
  ui0_scroll_begin_frame(&ctx.scrolls, storage->scroll_records,
                         READER_VIEW_SCROLL_CAP);
  ctx.scrolls.frame_index = input->frame_index;

  if (popup_dismiss && root_popup != ReaderViewPopup_NoteEditor)
  {
    if (root_popup == ReaderViewPopup_SelectionTools)
      input->state->dismissed_selection_key = input->state->last_selection_key;
    input->state->popup = ReaderViewPopup_None;
    out_frame->change_flags |= ReaderViewFrameChange_StateChanged;
  }
  rv_handle_escape(&ctx);
  old_focus = ctx.signals.focus_id;
  rv_build_toolbar(&ctx, labels);
  rv_build_paging_and_progress(&ctx, labels);
  rv_build_content_status(&ctx);
  rv_build_left_panel(&ctx, labels);
  rv_build_right_panel(&ctx, labels);
  rv_build_popup(&ctx, root_popup, root_rect, labels);
  rv_apply_semantic_focus_navigation(&ctx);

  ui0_signal_end_frame(&ctx.signals);
  input->state->hot_id = ctx.signals.hot_id;
  input->state->active_id = ctx.signals.active_id;
  input->state->focus_id = ctx.signals.focus_id;
  input->state->focus_visible =
    ctx.signals.focus_visible_id == ctx.signals.focus_id &&
    ctx.signals.focus_id != 0;
  if (old_focus != ctx.signals.focus_id)
    out_frame->change_flags |= ReaderViewFrameChange_FocusChanged;

  ui0_draw_context_init(&ctx.draw);
  ui0_draw_begin_frame(&ctx.draw, storage->draw_commands,
                       READER_VIEW_DRAW_COMMAND_CAP,
                       ui0_draw_theme_from_resolved(input->theme));
  ctx.draw.frame_index = input->frame_index;
  (void)ui0_draw_controls(&ctx.draw, storage->control_records,
                          ctx.control_count);
  (void)ui0_slider_draw_records(&ctx.draw, &ctx.sliders);
  (void)ui0_scroll_draw_records(&ctx.draw, &ctx.scrolls);
  for (UI0S32 icon_index = 0; icon_index < ctx.icon_count; ++icon_index)
  {
    const RVIconRecord *icon = ctx.icons + icon_index;
    if (icon->visible && icon->control_index >= 0 &&
        icon->control_index < ctx.control_count)
    {
      (void)ui0_draw_push_icon(&ctx.draw,
                               storage->control_records + icon->control_index,
                               icon->icon_kind,
                               icon->rect);
    }
  }
  rv_filter_reference_chrome_draws(&ctx);

  if ((ctx.signals.error_flags & UI0SignalError_NoRecordCap) != 0 ||
      (ctx.sliders.error_flags & UI0SliderError_NoRecordCap) != 0 ||
      (ctx.scrolls.error_flags & UI0ScrollError_NoRecordCap) != 0 ||
      (ctx.draw.error_flags & UI0DrawError_NoCommandCap) != 0)
    out_frame->error_flags |= ReaderViewFrameError_RecordCap;
  if ((ctx.signals.error_flags & UI0SignalError_BadInput) != 0 ||
      (ctx.sliders.error_flags & UI0SliderError_BadInput) != 0 ||
      (ctx.scrolls.error_flags & UI0ScrollError_BadInput) != 0 ||
      (ctx.draw.error_flags & UI0DrawError_BadInput) != 0)
    out_frame->error_flags |= ReaderViewFrameError_BadInput;

  out_frame->draw_commands = storage->draw_commands;
  out_frame->draw_command_count = ctx.draw.command_count;
  out_frame->text_bindings = storage->text_bindings;
  out_frame->text_binding_count = ctx.text_count;
  out_frame->semantic_nodes = storage->semantic_nodes;
  out_frame->semantic_node_count = ctx.semantic_count;
  out_frame->actions = storage->actions;
  out_frame->action_count = ctx.action_count;
  return out_frame->error_flags == ReaderViewFrameError_None;
}

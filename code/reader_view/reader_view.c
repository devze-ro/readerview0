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
  RV_NAV_RAIL_WIDTH = 72,
  RV_NAV_RAIL_ROW_HEIGHT = 34,
  RV_NAV_RAIL_ROW_STEP = 42,
  RV_NAV_CONTENTS_START_Y = 52,
  RV_NAV_CONTENTS_SEMANTIC_ROW_HEIGHT = 32,
  RV_NAV_CONTENTS_VISUAL_CLIP_Y = 48,
  RV_NAV_CONTENTS_VISUAL_ROW_HEIGHT = 32,
  RV_NAV_CONTENTS_VISUAL_ROW_GAP = 2,
  RV_NAV_CONTENTS_VISUAL_INDICATOR_WIDTH = 3,
  RV_NAV_CONTENTS_VISUAL_INDICATOR_GAP = 7,
  RV_NAV_CONTENTS_VISUAL_PADDING_X = 10,
  RV_NAV_CONTENTS_VISUAL_INDENT = 20,
  RV_NAV_CONTENTS_VISUAL_EXPANDER = 12,
  RV_NAV_CONTENTS_VISUAL_EXPANDER_GAP = 6,
  RV_NAV_CONTENTS_EMPTY_Y = 56,
  RV_NAV_CONTENTS_EMPTY_HEIGHT = 22,
  RV_NAV_FIND_ROW_HEIGHT = 88,
  RV_NAV_FIND_START_Y = 116,
  RV_NAV_FIND_STATUS_Y = 90,
  RV_NAV_FIND_STATUS_HEIGHT = 18,
  RV_NAV_FIND_DIVIDER_HEIGHT = 1,
  RV_NAV_BOTTOM_PAD = 10,
  RV_NAV_TITLE_HEIGHT = 24,
  RV_NAV_INPUT_HEIGHT = 34,
  RV_RIGHT_ROW_HEIGHT = 58,
  RV_RIGHT_ROW_GAP = 8,
  RV_RIGHT_SECTION_HEIGHT = 26,
  RV_RIGHT_SECTION_LABEL_HEIGHT = 20,
  RV_RIGHT_LIST_START_Y = 50,
  RV_RIGHT_FILTER_POPUP_HEIGHT = 136,
  RV_RIGHT_FILTER_ROW_HEIGHT = 29,
  RV_RIGHT_FILTER_ROW_GAP = 3,
  RV_RIGHT_FILTER_TEXT_PADDING_X = 10,
  RV_RIGHT_FILTER_ICON_SIZE = 14,
  RV_FIND_CARET_HEIGHT = 20,
  RV_RIGHT_TEXT_LINE_HEIGHT = 20,
  RV_RIGHT_TEXT_STACK_GAP = 6,
  RV_RIGHT_TEXT_BASELINE_OFFSET = 4,
  RV_POPUP_WIDTH = 280,
  RV_NOTE_WIDTH = 520,
  RV_NOTE_HEIGHT = 360,
  RV_NOTE_ANCHOR_GAP = 12,
  RV_NOTE_TEXT_PADDING_X = 8,
  RV_NOTE_TEXT_PADDING_TOP = 13,
  RV_NOTE_TEXT_PADDING_BOTTOM = 7,
  RV_NOTE_TEXT_LAYOUT_PADDING_Y = 10,
  RV_NOTE_TEXT_ROW_ID_BASE = 4096,
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
  RV_GUTTER_CARET_WIDTH = 18,
  RV_GUTTER_CARET_HEIGHT = 32,
  RV_TEXT_CARET_BLINK_VISIBLE_FRAMES = 30,
  RV_TEXT_CARET_BLINK_HIDDEN_FRAMES = 30,
  RV_PROGRESS_BOTTOM_INSET = 20,
  RV_PROGRESS_HIT_HEIGHT = 18,
  RV_FONT_POPUP_ANCHOR_X = 86,
  RV_FONT_POPUP_BODY_WIDTH = 144,
  RV_FONT_POPUP_GAP = 6,
  RV_FONT_POPUP_CHOICE_CAP = 5,
  RV_ICON_RECORD_CAP = 64,
  RV_VISUAL_FILL_CAP = 64,
  RV_TEXT_OVERRIDE_CAP = 64,
};

typedef struct RVIconRecord
{
  UI0S32 control_index;
  UI0IconKind icon_kind;
  UI0Rect rect;
  UI0B32 visible;
} RVIconRecord;

typedef struct RVVisualFillRecord
{
  UI0ID source_id;
  UI0Color color;
} RVVisualFillRecord;

typedef struct RVTextOverrideRecord
{
  UI0ID source_id;
  UI0TextAlignX align_x;
  UI0TypographyRole typography_role;
  UI0Color color;
} RVTextOverrideRecord;

typedef struct RVBuildContext
{
  const ReaderViewBuildInput *input;
  ReaderViewFrameStorage *storage;
  ReaderViewFrame *frame;
  UI0LayoutContext input_layout;
  UI0SignalContext signals;
  UI0DrawContext draw;
  UI0TextInputContext text_inputs;
  UI0TextAreaContext text_areas;
  UI0SidenavContext sidenav_visuals;
  UI0SliderContext sliders;
  UI0ScrollContext scrolls;
  UI0S32 control_count;
  UI0S32 text_count;
  UI0S32 semantic_count;
  UI0S32 action_count;
  UI0S32 icon_count;
  RVIconRecord icons[RV_ICON_RECORD_CAP];
  UI0S32 visual_fill_count;
  RVVisualFillRecord visual_fills[RV_VISUAL_FILL_CAP];
  UI0S32 text_override_count;
  RVTextOverrideRecord text_overrides[RV_TEXT_OVERRIDE_CAP];
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

static UI0B32
rv_text_same(ReaderViewText a, ReaderViewText b)
{
  return a.size == b.size &&
         (a.size == 0 || memcmp(a.data, b.data, (size_t)a.size) == 0);
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

static void
rv_patch_built_focus(RVBuildContext *ctx,
                     UI0ID id,
                     UI0B32 focused,
                     UI0B32 focus_visible)
{
  UI0S32 index;
  if (!ctx || id == 0) return;
  for (index = 0; index < ctx->control_count; ++index)
  {
    UI0ControlRecord *record = ctx->storage->control_records + index;
    if (record->id != id) continue;
    record->state &= ~(UI0ControlState_Focused |
                       UI0ControlState_FocusVisible);
    record->signal_flags &= ~(UI0Signal_Focused | UI0Signal_FocusVisible);
    if (focused)
    {
      record->state |= UI0ControlState_Focused;
      record->signal_flags |= UI0Signal_Focused;
      if (focus_visible)
      {
        record->state |= UI0ControlState_FocusVisible;
        record->signal_flags |= UI0Signal_FocusVisible;
      }
    }
  }
  for (index = 0; index < ctx->sidenav_visuals.record_count; ++index)
  {
    UI0SidenavRecord *record = ctx->sidenav_visuals.records + index;
    if (record->id != id) continue;
    record->state &= ~(UI0SidenavState_Focused |
                       UI0SidenavState_FocusVisible);
    record->signal_flags &= ~(UI0Signal_Focused | UI0Signal_FocusVisible);
    if (focused)
    {
      record->state |= UI0SidenavState_Focused;
      record->signal_flags |= UI0Signal_Focused;
      if (focus_visible)
      {
        record->state |= UI0SidenavState_FocusVisible;
        record->signal_flags |= UI0Signal_FocusVisible;
      }
    }
  }
  for (index = 0; index < ctx->text_inputs.record_count; ++index)
  {
    UI0TextInputRecord *record = ctx->text_inputs.records + index;
    if (record->id != id) continue;
    record->state &= ~(UI0TextInputState_Focused |
                       UI0TextInputState_FocusVisible);
    record->signal_flags &= ~(UI0Signal_Focused | UI0Signal_FocusVisible);
    if (focused)
    {
      record->state |= UI0TextInputState_Focused;
      record->signal_flags |= UI0Signal_Focused;
      if (focus_visible)
      {
        record->state |= UI0TextInputState_FocusVisible;
        record->signal_flags |= UI0Signal_FocusVisible;
      }
    }
  }
  for (index = 0; index < ctx->text_areas.record_count; ++index)
  {
    UI0TextAreaRecord *record = ctx->text_areas.records + index;
    if (record->id != id) continue;
    record->state &= ~(UI0TextAreaState_Focused |
                       UI0TextAreaState_FocusVisible);
    record->signal_flags &= ~(UI0Signal_Focused | UI0Signal_FocusVisible);
    if (focused)
    {
      record->state |= UI0TextAreaState_Focused;
      record->signal_flags |= UI0Signal_Focused;
      if (focus_visible)
      {
        record->state |= UI0TextAreaState_FocusVisible;
        record->signal_flags |= UI0Signal_FocusVisible;
      }
    }
  }
  for (index = 0; index < ctx->sliders.record_count; ++index)
  {
    UI0SliderRecord *record = ctx->sliders.records + index;
    if (record->id != id) continue;
    record->state &= ~(UI0SliderState_Focused |
                       UI0SliderState_FocusVisible);
    record->signal_flags &= ~(UI0Signal_Focused | UI0Signal_FocusVisible);
    if (focused)
    {
      record->state |= UI0SliderState_Focused;
      record->signal_flags |= UI0Signal_Focused;
      if (focus_visible)
      {
        record->state |= UI0SliderState_FocusVisible;
        record->signal_flags |= UI0Signal_FocusVisible;
      }
    }
  }
  for (index = 0; index < ctx->semantic_count; ++index)
  {
    ReaderViewSemanticNode *node = ctx->storage->semantic_nodes + index;
    if (node->id != id) continue;
    node->flags &= ~ReaderViewSemantic_Focused;
    if (focused &&
        (node->flags & ReaderViewSemantic_Focusable) != 0)
      node->flags |= ReaderViewSemantic_Focused;
  }
  if (id == rv_id(400, 0) || id == rv_id(401, 0))
  {
    for (index = 0; index < ctx->icon_count; ++index)
    {
      RVIconRecord *icon = ctx->icons + index;
      UI0ControlRecord *record;
      if (icon->control_index < 0 ||
          icon->control_index >= ctx->control_count)
        continue;
      record = ctx->storage->control_records + icon->control_index;
      if (record->id != id) continue;
      icon->visible =
        (record->state & (UI0ControlState_Hovered |
                          UI0ControlState_Pressed |
                          UI0ControlState_Focused |
                          UI0ControlState_FocusVisible)) != 0;
    }
  }
}

static void
rv_move_focus(RVBuildContext *ctx, UI0ID id, UI0B32 focus_visible)
{
  UI0ID old_id;
  UI0B32 old_visible;
  if (!ctx) return;
  old_id = ctx->signals.focus_id;
  old_visible = old_id != 0 &&
    ctx->signals.focus_visible_id == old_id;
  rv_patch_built_focus(ctx, old_id, 0, 0);
  ctx->signals.focus_id = id;
  ctx->signals.focus_visible_id = id && focus_visible ? id : 0;
  rv_patch_built_focus(ctx, id, id != 0, id != 0 && focus_visible);
  if (old_id != id || old_visible != (id != 0 && focus_visible))
    ctx->frame->change_flags |= ReaderViewFrameChange_FocusChanged;
}

static UI0B32
rv_control_kind_focusable(UI0ControlKind kind)
{
  return kind == UI0ControlKind_TextButton ||
         kind == UI0ControlKind_IconButton ||
         kind == UI0ControlKind_Checkbox ||
         kind == UI0ControlKind_Toggle ||
         kind == UI0ControlKind_SegmentItem ||
         kind == UI0ControlKind_MenuItem ||
         kind == UI0ControlKind_SelectTrigger ||
         kind == UI0ControlKind_SelectOption ||
         kind == UI0ControlKind_AccordionHeader ||
         kind == UI0ControlKind_ListRow ||
         kind == UI0ControlKind_TableHeader ||
         kind == UI0ControlKind_TableCell ||
         kind == UI0ControlKind_TreeRow ||
         kind == UI0ControlKind_TextInput ||
         kind == UI0ControlKind_TextArea ||
         kind == UI0ControlKind_Slider ||
         kind == UI0ControlKind_ToolbarItem ||
         kind == UI0ControlKind_SidenavRow ||
         kind == UI0ControlKind_MenuTrigger;
}

static void
rv_unblock_built_normal_root(RVBuildContext *ctx)
{
  UI0S32 index;
  if (!ctx) return;
  for (index = 0; index < ctx->control_count; ++index)
  {
    UI0ControlRecord *record = ctx->storage->control_records + index;
    UI0S32 semantic_index;
    if (record->root != UI0RootKind_Normal) continue;
    record->state &= ~UI0ControlState_BlockedByRoot;
    record->signal_flags &= ~UI0Signal_BlockedByRoot;
    if (!rv_control_kind_focusable(record->kind) ||
        (record->control_flags & UI0Control_Disabled) != 0)
      continue;
    for (semantic_index = 0;
         semantic_index < ctx->semantic_count;
         ++semantic_index)
    {
      ReaderViewSemanticNode *node =
        ctx->storage->semantic_nodes + semantic_index;
      if (node->id == record->id)
        node->flags |= ReaderViewSemantic_Enabled |
                       ReaderViewSemantic_Focusable;
    }
  }
  for (index = 0; index < ctx->text_inputs.record_count; ++index)
  {
    ctx->text_inputs.records[index].state &=
      ~UI0TextInputState_BlockedByRoot;
    ctx->text_inputs.records[index].signal_flags &=
      ~UI0Signal_BlockedByRoot;
  }
  for (index = 0; index < ctx->sidenav_visuals.record_count; ++index)
  {
    ctx->sidenav_visuals.records[index].state &=
      ~UI0SidenavState_BlockedByRoot;
    ctx->sidenav_visuals.records[index].signal_flags &=
      ~UI0Signal_BlockedByRoot;
  }
  for (index = 0; index < ctx->sliders.record_count; ++index)
  {
    UI0SliderRecord *record = ctx->sliders.records + index;
    UI0S32 semantic_index;
    record->signal_flags &= ~UI0Signal_BlockedByRoot;
    if ((record->flags & UI0Slider_Disabled) != 0) continue;
    for (semantic_index = 0;
         semantic_index < ctx->semantic_count;
         ++semantic_index)
    {
      ReaderViewSemanticNode *node =
        ctx->storage->semantic_nodes + semantic_index;
      if (node->id == record->id)
        node->flags |= ReaderViewSemantic_Enabled |
                       ReaderViewSemantic_Focusable;
    }
  }
}

static UI0B32
rv_right_row_starts_section(const ReaderViewRightProjection *right,
                            UI0S32 index);
static UI0S32
rv_right_content_height(const ReaderViewRightProjection *right);
static void
rv_close_popup_and_restore_focus(RVBuildContext *ctx);
static void
rv_clear_prior_popup_interaction(ReaderViewState *state,
                                 UI0ID restore_id);
static void
rv_reset_scroll_interaction(UI0ScrollState *scroll,
                            ReaderViewFrame *frame);
static UI0B32 rv_rect_intersects(UI0Rect a, UI0Rect b);

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
rv_text_caret_is_visible(UI0U64 frame_index)
{
  UI0U64 visible = RV_TEXT_CARET_BLINK_VISIBLE_FRAMES;
  UI0U64 period = visible + RV_TEXT_CARET_BLINK_HIDDEN_FRAMES;
  UI0U64 phase;
  if (frame_index == 0 || visible == 0 || period == 0) return 1;
  phase = (frame_index - 1) % period;
  return phase < visible;
}

static void
rv_apply_reference_find_caret_height(UI0TextInputRecord *record)
{
  UI0S32 height;
  if (!record || record->rect.h <= 0 ||
      record->caret_rect.w <= 0 || record->caret_rect.h <= 0)
    return;
  height = rv_min(RV_FIND_CARET_HEIGHT, record->rect.h);
  record->caret_rect.y = record->rect.y + (record->rect.h - height) / 2;
  record->caret_rect.h = height;
}

/*
 * Returns round-half-up(value * multiplier / divisor) without forming the
 * product. The progress adapter calls this with value <= divisor, so every
 * accumulated quotient is bounded by multiplier even at UINT64_MAX.
 */
static UI0U64
rv_round_mul_div_u64(UI0U64 value, UI0U64 multiplier, UI0U64 divisor)
{
  UI0U64 quotient = 0;
  UI0U64 remainder = 0;
  UI0U64 add_quotient;
  UI0U64 add_remainder;

  if (value == 0 || multiplier == 0 || divisor == 0) return 0;
  if (value > divisor) value = divisor;
  add_quotient = value / divisor;
  add_remainder = value % divisor;

  while (multiplier != 0)
  {
    if ((multiplier & 1ull) != 0)
    {
      quotient += add_quotient;
      if (add_remainder != 0 &&
          remainder >= divisor - add_remainder)
      {
        remainder -= divisor - add_remainder;
        quotient += 1;
      }
      else
      {
        remainder += add_remainder;
      }
    }

    multiplier >>= 1;
    if (multiplier == 0) break;

    add_quotient += add_quotient;
    if (add_remainder != 0 &&
        add_remainder >= divisor - add_remainder)
    {
      add_remainder -= divisor - add_remainder;
      add_quotient += 1;
    }
    else
    {
      add_remainder += add_remainder;
    }
  }

  if (remainder != 0 && remainder >= divisor - remainder)
    quotient += 1;
  return quotient;
}

static UI0S32
rv_progress_slider_max(UI0U64 location_count)
{
  return location_count > (UI0U64)INT32_MAX ?
    INT32_MAX : (UI0S32)location_count;
}

static UI0S32
rv_progress_slider_value(UI0U64 location_index, UI0U64 location_count)
{
  UI0U64 location_max;
  UI0U64 slider_offset;
  const UI0U64 slider_span = (UI0U64)INT32_MAX - 1ull;

  if (location_count == 0) return 1;
  location_max = location_count - 1ull;
  if (location_index > location_max) location_index = location_max;
  if (location_count <= (UI0U64)INT32_MAX)
    return (UI0S32)(location_index + 1ull);

  slider_offset = rv_round_mul_div_u64(location_index,
                                       slider_span,
                                       location_max);
  return (UI0S32)(slider_offset + 1ull);
}

static UI0U64
rv_progress_location_from_slider(UI0S32 slider_value,
                                 UI0U64 location_count)
{
  UI0S32 slider_max;
  const UI0U64 slider_span = (UI0U64)INT32_MAX - 1ull;

  if (location_count <= 1ull) return 0;
  slider_max = rv_progress_slider_max(location_count);
  slider_value = rv_clamp(slider_value, 1, slider_max);
  if (location_count <= (UI0U64)INT32_MAX)
    return (UI0U64)(slider_value - 1);

  return rv_round_mul_div_u64((UI0U64)(slider_value - 1),
                              location_count - 1ull,
                              slider_span);
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
  labels.star = rv_literal("Add star");
  labels.unstar = rv_literal("Remove star");
  labels.edit_note = rv_literal("Edit note");
  labels.save_note = rv_literal("Save note");
  labels.cancel = rv_literal("Cancel");
  labels.delete_value = rv_literal("Delete");
  labels.copy = rv_literal("Copy");
  labels.dictionary = rv_literal("Dictionary");
  labels.web_lookup = rv_literal("Web lookup");
  labels.translate = rv_literal("Translate");
  labels.more = rv_literal("More");
  labels.close_navigation = rv_literal("Close navigation");
  labels.search_input = rv_literal("Search input");
  labels.clear_search = rv_literal("Clear search");
  labels.annotation_actions = rv_literal("Annotation actions");
  labels.annotation_filters = rv_literal("Annotation filters");
  labels.export_annotations = rv_literal("Export annotations");
  labels.close_annotations = rv_literal("Close annotations");
  labels.delete_bookmark = rv_literal("Delete bookmark");
  labels.delete_note = rv_literal("Delete note");
  labels.delete_highlight = rv_literal("Delete highlight");
  labels.contents_short = rv_literal("TOC");
  labels.contents_panel_title = rv_literal("Table of Contents");
  labels.find_panel_title = rv_literal("Search");
  labels.filter_annotations = rv_literal("Filter annotations");
  labels.no_contents = rv_literal("No contents");
  labels.find_prompt = rv_literal("Type and press Enter");
  labels.no_matches = rv_literal("No matches");
  labels.no_annotations = rv_literal("No annotations");
  labels.no_bookmarks = rv_literal("No bookmarks");
  labels.no_highlights = rv_literal("No highlights");
  labels.no_notes = rv_literal("No notes");
  labels.find_placeholder = rv_literal("Search in book");
  labels.note_title = rv_literal("Note");
  labels.add_note_title = rv_literal("Add Note");
  labels.note_text = rv_literal("Note text");
  labels.note_placeholder = rv_literal("Type a note");
  labels.save = rv_literal("Save");
  labels.cancel_note = rv_literal("Cancel note");
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
    ((state->left_panel == ReaderViewLeftPanel_Contents &&
      (input->features & ReaderViewFeature_Contents) != 0) ||
     (state->left_panel == ReaderViewLeftPanel_Find &&
      (input->features & ReaderViewFeature_Find) != 0));
  result.right_panel_visible = !chrome_hidden && state->right_panel_open &&
    (input->features & ReaderViewFeature_Annotations) != 0;

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
      if ((row->flags & ~(ReaderViewRow_Enabled |
                          ReaderViewRow_Current |
                          ReaderViewRow_Selected)) != 0)
        errors |= ReaderViewFrameError_BadInput;
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
      if ((row->flags & ~(ReaderViewRow_Enabled |
                          ReaderViewRow_Current |
                          ReaderViewRow_Selected)) != 0)
        errors |= ReaderViewFrameError_BadInput;
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
      if ((row->flags & ~(ReaderViewRow_Enabled |
                          ReaderViewRow_Current |
                          ReaderViewRow_Selected |
                          ReaderViewRow_Starred |
                          ReaderViewRow_AttachedToPrevious)) != 0 ||
          (row->actions & ~(ReaderViewRightAction_Activate |
                            ReaderViewRightAction_ToggleStar |
                            ReaderViewRightAction_EditNote |
                            ReaderViewRightAction_Delete)) != 0)
        errors |= ReaderViewFrameError_BadInput;
      if (row->kind != ReaderViewRightRow_Note &&
          (row->actions & ReaderViewRightAction_EditNote) != 0)
        errors |= ReaderViewFrameError_BadInput;
      rv_validate_text(row->section, &errors);
      rv_validate_text(row->primary, &errors);
      rv_validate_text(row->secondary, &errors);
      if ((row->flags & ReaderViewRow_AttachedToPrevious) != 0)
      {
        if (index == 0 || row->kind != ReaderViewRightRow_Note ||
            projection->right.rows[index - 1].kind !=
              ReaderViewRightRow_Highlight ||
            (row->section.size > 0 &&
             (!rv_text_valid(row->section) ||
              !rv_text_valid(projection->right.rows[index - 1].section) ||
              !rv_text_same(row->section,
                            projection->right.rows[index - 1].section))) ||
            row->color_key == 0 ||
            projection->right.rows[index - 1].color_key == 0 ||
            row->color_key != projection->right.rows[index - 1].color_key ||
            row->rail_color == 0 ||
            projection->right.rows[index - 1].rail_color == 0 ||
            row->rail_color !=
              projection->right.rows[index - 1].rail_color)
          errors |= ReaderViewFrameError_InvalidAttachment;
      }
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
  RV_VALIDATE_LABEL(close_navigation);
  RV_VALIDATE_LABEL(search_input);
  RV_VALIDATE_LABEL(clear_search);
  RV_VALIDATE_LABEL(annotation_actions);
  RV_VALIDATE_LABEL(annotation_filters);
  RV_VALIDATE_LABEL(export_annotations);
  RV_VALIDATE_LABEL(close_annotations);
  RV_VALIDATE_LABEL(delete_bookmark);
  RV_VALIDATE_LABEL(delete_note);
  RV_VALIDATE_LABEL(delete_highlight);
  RV_VALIDATE_LABEL(contents_short);
  RV_VALIDATE_LABEL(contents_panel_title);
  RV_VALIDATE_LABEL(find_panel_title);
  RV_VALIDATE_LABEL(filter_annotations);
  RV_VALIDATE_LABEL(no_contents);
  RV_VALIDATE_LABEL(find_prompt);
  RV_VALIDATE_LABEL(no_matches);
  RV_VALIDATE_LABEL(no_annotations);
  RV_VALIDATE_LABEL(no_bookmarks);
  RV_VALIDATE_LABEL(no_highlights);
  RV_VALIDATE_LABEL(no_notes);
  RV_VALIDATE_LABEL(find_placeholder);
  RV_VALIDATE_LABEL(note_title);
  RV_VALIDATE_LABEL(add_note_title);
  RV_VALIDATE_LABEL(note_text);
  RV_VALIDATE_LABEL(note_placeholder);
  RV_VALIDATE_LABEL(save);
  RV_VALIDATE_LABEL(cancel_note);
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
  RV_LABEL(close_navigation);
  RV_LABEL(search_input);
  RV_LABEL(clear_search);
  RV_LABEL(annotation_actions);
  RV_LABEL(annotation_filters);
  RV_LABEL(export_annotations);
  RV_LABEL(close_annotations);
  RV_LABEL(delete_bookmark);
  RV_LABEL(delete_note);
  RV_LABEL(delete_highlight);
  RV_LABEL(contents_short);
  RV_LABEL(contents_panel_title);
  RV_LABEL(find_panel_title);
  RV_LABEL(filter_annotations);
  RV_LABEL(no_contents);
  RV_LABEL(find_prompt);
  RV_LABEL(no_matches);
  RV_LABEL(no_annotations);
  RV_LABEL(no_bookmarks);
  RV_LABEL(no_highlights);
  RV_LABEL(no_notes);
  RV_LABEL(find_placeholder);
  RV_LABEL(note_title);
  RV_LABEL(add_note_title);
  RV_LABEL(note_text);
  RV_LABEL(note_placeholder);
  RV_LABEL(save);
  RV_LABEL(cancel_note);
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
  if (focusable && ctx->signals.focus_id == id)
    flags |= ReaderViewSemantic_Focused;
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

static void
rv_set_semantic_rect(RVBuildContext *ctx, UI0ID id, UI0Rect rect)
{
  UI0S32 index;
  if (!ctx) return;
  for (index = ctx->semantic_count - 1; index >= 0; index -= 1)
  {
    ReaderViewSemanticNode *node = ctx->storage->semantic_nodes + index;
    if (node->id == id)
    {
      node->rect = rect;
      return;
    }
  }
}

static void
rv_add_semantic_flags_for_id(RVBuildContext *ctx,
                             UI0ID id,
                             ReaderViewSemanticFlags flags)
{
  UI0S32 index;
  if (!ctx) return;
  for (index = ctx->semantic_count - 1; index >= 0; index -= 1)
  {
    ReaderViewSemanticNode *node = ctx->storage->semantic_nodes + index;
    if (node->id == id)
    {
      node->flags |= flags;
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
  memset(binding, 0, sizeof(*binding));
  binding->source_id = source_id;
  binding->text = text;
  binding->style = style;
  return 1;
}

static void
rv_set_text_binding_match(RVBuildContext *ctx,
                          UI0ID source_id,
                          UI0U32 match_start,
                          UI0U32 match_size)
{
  UI0S32 index;
  if (!ctx || match_size == 0) return;
  for (index = ctx->text_count - 1; index >= 0; --index)
  {
    ReaderViewTextBinding *binding = ctx->storage->text_bindings + index;
    if (binding->source_id == source_id)
    {
      binding->match_start = match_start;
      binding->match_size = match_size;
      return;
    }
  }
}

static void
rv_set_control_visual_text(RVBuildContext *ctx,
                           UI0ID source_id,
                           ReaderViewText text)
{
  UI0S32 index;
  if (!ctx) return;
  for (index = ctx->control_count - 1; index >= 0; --index)
  {
    UI0ControlRecord *record = ctx->storage->control_records + index;
    if (record->id == source_id)
    {
      record->label_hash = rv_text_hash(text);
      record->label_len = text.size;
      break;
    }
  }
  for (index = ctx->text_count - 1; index >= 0; --index)
  {
    ReaderViewTextBinding *binding = ctx->storage->text_bindings + index;
    if (binding->source_id == source_id)
    {
      binding->text = text;
      break;
    }
  }
}

static void
rv_set_semantic_value(RVBuildContext *ctx,
                      UI0ID id,
                      ReaderViewText value)
{
  UI0S32 index;
  if (!ctx) return;
  for (index = ctx->semantic_count - 1; index >= 0; --index)
  {
    ReaderViewSemanticNode *node = ctx->storage->semantic_nodes + index;
    if (node->id == id)
    {
      node->value = value;
      return;
    }
  }
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
  UI0B32 blocked;
  UI0B32 pending_focus;
  UI0B32 pending_invoke;
  UI0B32 invoked;
  memset(&signal_spec, 0, sizeof(signal_spec));
  signal_spec.id = id;
  signal_spec.root = root;
  signal_spec.flags = UI0SignalBox_Clickable | UI0SignalBox_Focusable;
  if (!enabled) signal_spec.flags |= UI0SignalBox_Disabled;
  signal_spec.rect = rect;
  signal_spec.hit_rect = hit_rect;
  signal = ui0_signal_from_rect(&ctx->signals, signal_spec);
  blocked = ui0_signal_has(signal, UI0Signal_BlockedByRoot);
  pending_focus = ctx->input->state->pending_accessibility_focus_id == id;
  pending_invoke = ctx->input->state->pending_accessibility_invoke_id == id;

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
  if (pending_focus)
  {
    ctx->input->state->pending_accessibility_focus_id = 0;
    if (!blocked)
      rv_move_focus(ctx, id, 1);
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
                        rv_semantic_flags(ctx, id, enabled && !blocked,
                                          !blocked,
                                          selected, checked, open),
                        hit_rect,
                        label,
                        value,
                        source_key,
                        0,
                        0,
                        0);
  invoked = enabled && !blocked &&
            (ui0_signal_has(signal, UI0Signal_Clicked) ||
             ui0_signal_has(signal, UI0Signal_KeyboardActivated) ||
             pending_invoke);
  if (pending_invoke && enabled && !blocked)
    rv_move_focus(ctx, id, 1);
  if (pending_invoke)
    ctx->input->state->pending_accessibility_invoke_id = 0;
  return invoked;
}

static void
rv_add_visual_fill(RVBuildContext *ctx,
                   UI0ID id,
                   UI0Rect rect,
                   UI0Rect clip_rect,
                   UI0Color color)
{
  UI0ControlRecord *record;
  RVVisualFillRecord *visual;
  if (ctx->control_count >= READER_VIEW_CONTROL_CAP ||
      ctx->visual_fill_count >= RV_VISUAL_FILL_CAP)
  {
    ctx->frame->error_flags |= ReaderViewFrameError_RecordCap;
    return;
  }
  record = ctx->storage->control_records + ctx->control_count++;
  memset(record, 0, sizeof(*record));
  record->id = id;
  record->kind = UI0ControlKind_Surface;
  record->box_index = UI0LayoutInvalidIndex;
  record->root = UI0RootKind_Normal;
  record->rect = rect;
  record->clip_rect = clip_rect;
  record->text_rect = rv_rect(0, 0, 0, 0);
  visual = ctx->visual_fills + ctx->visual_fill_count++;
  visual->source_id = id;
  visual->color = color;
}

static UI0ControlRecord *
rv_control_record_for_id(RVBuildContext *ctx, UI0ID id)
{
  UI0S32 index;
  if (!ctx || id == 0) return 0;
  for (index = ctx->control_count - 1; index >= 0; --index)
  {
    UI0ControlRecord *record = ctx->storage->control_records + index;
    if (record->id == id) return record;
  }
  return 0;
}

static void
rv_make_control_nonquiet(RVBuildContext *ctx, UI0ID id)
{
  UI0ControlRecord *record = rv_control_record_for_id(ctx, id);
  if (record) record->control_flags &= ~UI0Control_Quiet;
}

static void
rv_make_control_primary(RVBuildContext *ctx, UI0ID id)
{
  UI0ControlRecord *record = rv_control_record_for_id(ctx, id);
  if (record)
  {
    record->control_flags &= ~UI0Control_Quiet;
    record->control_flags |= UI0Control_Primary;
  }
}

static void
rv_make_control_quiet(RVBuildContext *ctx, UI0ID id)
{
  UI0ControlRecord *record = rv_control_record_for_id(ctx, id);
  if (record)
  {
    record->control_flags &= ~UI0Control_Primary;
    record->control_flags |= UI0Control_Quiet;
  }
}

static UI0SidenavStateFlags
rv_sidenav_state_from_control(const UI0ControlRecord *control)
{
  UI0SidenavStateFlags result = UI0SidenavState_None;
  if (!control) return result;
  if (control->state & UI0ControlState_Hovered)
    result |= UI0SidenavState_Hovered;
  if (control->state & UI0ControlState_Pressed)
    result |= UI0SidenavState_Pressed;
  if (control->state & UI0ControlState_Active)
    result |= UI0SidenavState_Active;
  if (control->state & UI0ControlState_Focused)
    result |= UI0SidenavState_Focused;
  if (control->state & UI0ControlState_Disabled)
    result |= UI0SidenavState_Disabled;
  if (control->state & UI0ControlState_Clicked)
    result |= UI0SidenavState_Clicked;
  if (control->state & UI0ControlState_KeyboardActivated)
    result |= UI0SidenavState_KeyboardActivated;
  if (control->state & UI0ControlState_BlockedByRoot)
    result |= UI0SidenavState_BlockedByRoot;
  if (control->state & UI0ControlState_FocusVisible)
    result |= UI0SidenavState_FocusVisible;
  return result;
}

static void
rv_add_sidenav_visual(RVBuildContext *ctx,
                      UI0ID id,
                      UI0ID sidenav_id,
                      UI0S32 item_index,
                      UI0S32 depth,
                      UI0Rect outer_rect,
                      UI0Rect body_rect,
                      UI0Rect clip_rect,
                      UI0Rect text_rect,
                      UI0Rect current_rect,
                      UI0Rect expander_rect,
                      ReaderViewText label,
                      UI0B32 selected,
                      UI0B32 current,
                      UI0B32 has_children,
                      UI0B32 expanded)
{
  UI0SidenavRecord *record;
  UI0ControlRecord *control;
  if (!ctx || ctx->sidenav_visuals.record_count >=
              ctx->sidenav_visuals.record_cap)
  {
    if (ctx) ctx->frame->error_flags |= ReaderViewFrameError_RecordCap;
    return;
  }
  control = rv_control_record_for_id(ctx, id);
  record = ctx->sidenav_visuals.records +
           ctx->sidenav_visuals.record_count++;
  memset(record, 0, sizeof(*record));
  record->id = id;
  record->sidenav_id = sidenav_id;
  record->item_index = item_index;
  record->visible_index = item_index;
  record->depth = depth;
  record->root = UI0RootKind_Normal;
  record->state = rv_sidenav_state_from_control(control);
  record->signal_flags = control ? control->signal_flags : 0;
  record->rect = body_rect;
  record->hit_rect = ui0_rect_intersect(outer_rect, clip_rect);
  record->clip_rect = clip_rect;
  record->text_rect = text_rect;
  record->current_rect = current_rect;
  record->expander_rect = expander_rect;
  record->label_hash = rv_text_hash(label);
  record->label_len = label.size;
  if (selected) record->state |= UI0SidenavState_Selected;
  if (current) record->state |= UI0SidenavState_Current;
  if (has_children) record->state |= UI0SidenavState_HasChildren;
  if (expanded) record->state |= UI0SidenavState_Expanded;
}

static UI0B32
rv_has_sidenav_visual(const RVBuildContext *ctx, UI0ID id)
{
  UI0S32 index;
  if (!ctx || id == 0) return 0;
  for (index = 0; index < ctx->sidenav_visuals.record_count; ++index)
  {
    if (ctx->sidenav_visuals.records[index].id == id) return 1;
  }
  return 0;
}

static void
rv_add_text_override(RVBuildContext *ctx,
                     UI0ID id,
                     UI0TextAlignX align_x,
                     UI0TypographyRole typography_role,
                     UI0Color color)
{
  RVTextOverrideRecord *record;
  if (ctx->text_override_count >= RV_TEXT_OVERRIDE_CAP)
  {
    ctx->frame->error_flags |= ReaderViewFrameError_RecordCap;
    return;
  }
  record = ctx->text_overrides + ctx->text_override_count++;
  record->source_id = id;
  record->align_x = align_x;
  record->typography_role = typography_role;
  record->color = color;
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
  ui0_text_area_state_init(&state->note_input);
  state->note_input.history = &state->note_history;
  state->note_input.caret = size;
  state->note_input.selection_anchor = size;
  ui0_text_input_history_reset(&state->note_history);
}

UI0B32
reader_view_open_note_editor(ReaderViewState *state,
                             const ReaderViewSelectionProjection *selection)
{
  if (!state || !selection ||
      selection->status.state != ReaderViewLoad_Ready ||
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
  UI0ID restore_id = state->popup == ReaderViewPopup_SelectionTools ?
    state->restore_focus_id : ctx->signals.focus_id;
  rv_copy_note_to_state(state, &ctx->input->projection->selection);
  state->popup = ReaderViewPopup_NoteEditor;
  state->restore_focus_id = restore_id;
  ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged;
}

static void
rv_handle_escape(RVBuildContext *ctx)
{
  ReaderViewState *state = ctx->input->state;
  UI0ID restore_id = 0;
  if (!ctx->input->input->escape_pressed) return;
  if (state->popup == ReaderViewPopup_NoteEditor && state->note_dirty)
    return;
  if (state->popup != ReaderViewPopup_None)
  {
    ReaderViewPopupKind dismissed_popup = state->popup;
    UI0S32 popup_index;
    if (state->popup == ReaderViewPopup_SelectionTools)
      state->dismissed_selection_key = state->last_selection_key;
    if (dismissed_popup == ReaderViewPopup_RightFilter)
      state->right_filter_menu_flags = 0;
    restore_id = state->restore_focus_id;
    for (popup_index = 0;
         popup_index < state->prior_popup_item_count;
         ++popup_index)
    {
      UI0ID id = state->prior_popup_item_ids[popup_index];
      if (ctx->signals.hot_id == id) ctx->signals.hot_id = 0;
      if (ctx->signals.active_id == id) ctx->signals.active_id = 0;
    }
    rv_clear_prior_popup_interaction(state, restore_id);
    state->popup = ReaderViewPopup_None;
    state->restore_focus_id = 0;
    if (dismissed_popup == ReaderViewPopup_RightRowActions)
    {
      state->right_menu_key = 0;
      state->right_menu_actions = ReaderViewRightAction_None;
    }
  }
  else if (state->left_panel != ReaderViewLeftPanel_None)
  {
    state->left_panel = ReaderViewLeftPanel_None;
    state->pending_left_panel_focus = ReaderViewLeftPanel_None;
    rv_reset_scroll_interaction(&state->toc_scroll, ctx->frame);
    rv_reset_scroll_interaction(&state->find_scroll, ctx->frame);
    restore_id = state->left_panel_restore_focus_id;
    state->left_panel_restore_focus_id = 0;
    ctx->frame->change_flags |= ReaderViewFrameChange_LayoutChanged;
  }
  else
    return;
  rv_move_focus(ctx, restore_id, restore_id != 0);
  ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged;
}

static void
rv_clear_prior_popup_interaction(ReaderViewState *state,
                                 UI0ID restore_id)
{
  UI0S32 index;
  UI0B32 focus_was_visible;
  if (!state) return;
  focus_was_visible = state->focus_id != 0 && state->focus_visible;
  for (index = 0; index < state->prior_popup_item_count; ++index)
  {
    UI0ID id = state->prior_popup_item_ids[index];
    if (state->focus_id == id)
    {
      state->focus_id = restore_id;
      state->focus_visible = restore_id != 0 && focus_was_visible;
    }
    if (state->hot_id == id) state->hot_id = 0;
    if (state->active_id == id) state->active_id = 0;
    if (state->pending_accessibility_focus_id == id)
      state->pending_accessibility_focus_id = 0;
    if (state->pending_accessibility_invoke_id == id)
      state->pending_accessibility_invoke_id = 0;
  }
  state->prior_popup_item_count = 0;
  state->prior_popup_kind = ReaderViewPopup_None;
}

UI0B32
reader_view_close_note_editor(ReaderViewState *state)
{
  UI0ID restore_id;
  UI0B32 focus_was_visible;
  UI0U64 tag;
  if (!state || state->popup != ReaderViewPopup_NoteEditor) return 0;
  restore_id = state->restore_focus_id;
  focus_was_visible = state->focus_id != 0 && state->focus_visible;
  rv_clear_prior_popup_interaction(state, restore_id);
  for (tag = 500; tag <= 505; ++tag)
  {
    UI0ID id = rv_id(tag, state->note_selection_key);
    if (state->focus_id == id)
    {
      state->focus_id = restore_id;
      state->focus_visible = restore_id != 0 && focus_was_visible;
    }
    if (state->hot_id == id) state->hot_id = 0;
    if (state->active_id == id) state->active_id = 0;
    if (state->pending_accessibility_focus_id == id)
      state->pending_accessibility_focus_id = 0;
    if (state->pending_accessibility_invoke_id == id)
      state->pending_accessibility_invoke_id = 0;
  }
  state->popup = ReaderViewPopup_None;
  state->restore_focus_id = 0;
  state->note_dirty = 0;
  return 1;
}

static void
rv_sync_selection_popup(RVBuildContext *ctx)
{
  ReaderViewState *state = ctx->input->state;
  const ReaderViewSelectionProjection *selection =
    &ctx->input->projection->selection;
  UI0B32 active = selection->status.state == ReaderViewLoad_Ready &&
                  (selection->flags & ReaderViewSelection_Active) != 0 &&
                  selection->selection_key != 0;
  if (!active)
  {
    if (state->popup == ReaderViewPopup_SelectionTools)
    {
      UI0ID restore_id = state->restore_focus_id;
      UI0ID old_focus = state->focus_id;
      rv_clear_prior_popup_interaction(state, restore_id);
      state->popup = ReaderViewPopup_None;
      state->restore_focus_id = 0;
      ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged;
      if (state->focus_id != old_focus)
        ctx->frame->change_flags |= ReaderViewFrameChange_FocusChanged;
    }
    state->last_selection_key = 0;
    state->dismissed_selection_key = 0;
    return;
  }
  if (state->last_selection_key != selection->selection_key)
  {
    if (state->popup == ReaderViewPopup_SelectionTools)
    {
      UI0ID old_focus = state->focus_id;
      rv_clear_prior_popup_interaction(state, state->restore_focus_id);
      if (state->focus_id != old_focus)
        ctx->frame->change_flags |= ReaderViewFrameChange_FocusChanged;
      ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged;
    }
    state->last_selection_key = selection->selection_key;
    state->dismissed_selection_key = 0;
    if (state->popup == ReaderViewPopup_None)
    {
      state->popup = ReaderViewPopup_SelectionTools;
      state->restore_focus_id = state->focus_id;
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
rv_focus_left_panel(RVBuildContext *ctx, ReaderViewLeftPanelMode mode)
{
  const ReaderViewTocProjection *toc = &ctx->input->projection->toc;
  ReaderViewState *state = ctx->input->state;
  UI0ID target = 0;
  UI0B32 focus_visible =
    ctx->signals.focus_id == 0 ||
    ctx->signals.focus_visible_id == ctx->signals.focus_id;
  if (mode == ReaderViewLeftPanel_Find)
  {
    target = rv_id(220, 0);
  }
  else if (mode == ReaderViewLeftPanel_Contents)
  {
    UI0S32 index;
    for (index = 0; index < toc->row_count; ++index)
    {
      const ReaderViewTocRow *row = toc->rows + index;
      if ((row->flags & ReaderViewRow_Enabled) != 0 &&
          (row->flags & (ReaderViewRow_Current |
                         ReaderViewRow_Selected)) != 0)
      {
        state->active_toc_key = row->key;
        target = rv_id(212, row->key);
        break;
      }
    }
    if (target == 0)
    {
      for (index = 0; index < toc->row_count; ++index)
      {
        const ReaderViewTocRow *row = toc->rows + index;
        if ((row->flags & ReaderViewRow_Enabled) != 0)
        {
          state->active_toc_key = row->key;
          target = rv_id(212, row->key);
          break;
        }
      }
    }
    if (target == 0) target = rv_id(201, 0);
  }
  if (target)
    rv_move_focus(ctx, target, focus_visible);
}

static void
rv_close_left_panel(RVBuildContext *ctx)
{
  ReaderViewState *state = ctx->input->state;
  UI0B32 focus_visible =
    ctx->signals.focus_id != 0 &&
    ctx->signals.focus_visible_id == ctx->signals.focus_id;
  state->left_panel = ReaderViewLeftPanel_None;
  state->pending_left_panel_focus = ReaderViewLeftPanel_None;
  rv_reset_scroll_interaction(&state->toc_scroll, ctx->frame);
  rv_reset_scroll_interaction(&state->find_scroll, ctx->frame);
  if (state->left_panel_restore_focus_id)
    rv_move_focus(ctx, state->left_panel_restore_focus_id, focus_visible);
  state->left_panel_restore_focus_id = 0;
  ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged |
                             ReaderViewFrameChange_LayoutChanged;
}

static void
rv_toggle_left_panel(RVBuildContext *ctx,
                     ReaderViewLeftPanelMode mode,
                     UI0ID source_id)
{
  ReaderViewState *state = ctx->input->state;
  if (state->left_panel == mode)
  {
    rv_close_left_panel(ctx);
    return;
  }
  rv_reset_scroll_interaction(&state->toc_scroll, ctx->frame);
  rv_reset_scroll_interaction(&state->find_scroll, ctx->frame);
  state->left_panel = mode;
  state->most_recent_panel = ReaderViewPanel_Left;
  if (source_id) state->left_panel_restore_focus_id = source_id;
  if (mode == ReaderViewLeftPanel_Contents)
    state->toc_scroll_y = 0;
  else if (mode == ReaderViewLeftPanel_Find)
    state->find_scroll_y = 0;
  state->pending_left_panel_focus = mode;
  if (ctx->input->layout->left_panel_visible)
  {
    rv_focus_left_panel(ctx, mode);
    state->pending_left_panel_focus = ReaderViewLeftPanel_None;
  }
  ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged |
                             ReaderViewFrameChange_LayoutChanged;
}

static void
rv_close_right_panel(RVBuildContext *ctx)
{
  ReaderViewState *state = ctx->input->state;
  UI0B32 focus_visible =
    ctx->signals.focus_id != 0 &&
    ctx->signals.focus_visible_id == ctx->signals.focus_id;
  state->right_panel_open = 0;
  rv_reset_scroll_interaction(&state->right_scroll, ctx->frame);
  if (state->right_panel_restore_focus_id)
    rv_move_focus(ctx, state->right_panel_restore_focus_id, focus_visible);
  state->right_panel_restore_focus_id = 0;
  ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged |
                             ReaderViewFrameChange_LayoutChanged;
}

static void
rv_toggle_right_panel(RVBuildContext *ctx, UI0ID source_id)
{
  ReaderViewState *state = ctx->input->state;
  if (state->right_panel_open)
  {
    rv_close_right_panel(ctx);
    return;
  }
  state->right_panel_open = 1;
  rv_reset_scroll_interaction(&state->right_scroll, ctx->frame);
  state->most_recent_panel = ReaderViewPanel_Right;
  state->right_scroll_y = 0;
  if (source_id) state->right_panel_restore_focus_id = source_id;
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
  UI0B32 contents_open =
    ctx->input->state->left_panel == ReaderViewLeftPanel_Contents;
  UI0B32 find_open =
    ctx->input->state->left_panel == ReaderViewLeftPanel_Find;
  UI0B32 annotations_open = ctx->input->state->right_panel_open;

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
        contents_open,
        0,
        0,
        UI0IconKind_List))
  {
    rv_toggle_left_panel(ctx, ReaderViewLeftPanel_Contents, rv_id(11, 0));
  }
  if (contents_open)
    rv_add_semantic_flags_for_id(ctx, rv_id(11, 0),
                                 ReaderViewSemantic_Expanded);
  if (rv_has_feature(projection, ReaderViewFeature_Find) &&
      rv_toolbar_icon_control(
        ctx, 12, ReaderViewSemanticControl_Find, 1,
        labels.find, rv_text(0, 0), 1,
        find_open,
        0,
        0,
        UI0IconKind_Search))
  {
    rv_toggle_left_panel(ctx, ReaderViewLeftPanel_Find, rv_id(12, 0));
  }
  if (find_open)
    rv_add_semantic_flags_for_id(ctx, rv_id(12, 0),
                                 ReaderViewSemantic_Expanded);
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
        annotations_open,
        0,
        0,
        UI0IconKind_Notebook))
  {
    rv_toggle_right_panel(ctx, rv_id(17, 0));
  }
  if (annotations_open)
    rv_add_semantic_flags_for_id(ctx, rv_id(17, 0),
                                 ReaderViewSemantic_Expanded);

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

static UI0S32
rv_right_action_popup_count(const ReaderViewRightRow *row)
{
  UI0S32 result = 0;
  if (!row) return 0;
  if ((row->actions & ReaderViewRightAction_Activate) != 0) result += 1;
  if (row->kind == ReaderViewRightRow_Highlight &&
      (row->actions & ReaderViewRightAction_ToggleStar) != 0)
    result += 1;
  if (row->kind == ReaderViewRightRow_Note &&
      (row->actions & ReaderViewRightAction_EditNote) != 0)
    result += 1;
  if ((row->actions & ReaderViewRightAction_Delete) != 0) result += 1;
  return result;
}

static UI0B32
rv_right_row_menu_rect(const ReaderViewBuildInput *input,
                       ReaderViewKey key,
                       UI0Rect *out_rect)
{
  const ReaderViewRightProjection *right;
  UI0Rect panel;
  UI0Rect list_rect;
  UI0S32 row_y;
  UI0S32 scroll_y;
  UI0S32 index;
  if (out_rect) *out_rect = rv_rect(0, 0, 0, 0);
  if (!input || !input->state || !input->layout || !input->projection ||
      !out_rect || key == 0 || !input->layout->right_panel_visible)
    return 0;
  right = &input->projection->right;
  panel = input->layout->right_panel_rect;
  list_rect = rv_rect(panel.x + 10,
                      panel.y + RV_RIGHT_LIST_START_Y,
                      rv_max(panel.w - 20, 1),
                      rv_max(panel.h - RV_RIGHT_LIST_START_Y - 10, 0));
  scroll_y = rv_clamp(input->state->right_scroll_y, 0,
                      rv_max(rv_right_content_height(right) - list_rect.h, 0));
  row_y = list_rect.y - scroll_y;
  for (index = 0; index < right->row_count; ++index)
  {
    UI0Rect entry_rect;
    if (rv_right_row_starts_section(right, index))
      row_y += RV_RIGHT_SECTION_HEIGHT;
    entry_rect = rv_rect(list_rect.x, row_y,
                         list_rect.w, RV_RIGHT_ROW_HEIGHT);
    if (right->rows[index].key == key)
    {
      UI0Rect visible_menu;
      *out_rect = rv_rect(entry_rect.x + entry_rect.w - 38,
                          entry_rect.y + (entry_rect.h - 28) / 2,
                          30, 28);
      visible_menu = ui0_rect_intersect(*out_rect, list_rect);
      return visible_menu.w > 0 && visible_menu.h > 0;
    }
    row_y += RV_RIGHT_ROW_HEIGHT;
    if (index + 1 < right->row_count &&
        (right->rows[index + 1].flags &
         ReaderViewRow_AttachedToPrevious) == 0)
      row_y += RV_RIGHT_ROW_GAP;
  }
  return 0;
}

static UI0B32
rv_right_row_owns_id(UI0ID id,
                     ReaderViewKey key,
                     UI0ID restore_id)
{
  if (id == 0 || key == 0) return 0;
  return id == restore_id || id == rv_id(324, key) ||
         id == rv_id(325, key) || id == rv_id(326, key) ||
         id == rv_id(140, key) || id == rv_id(141, key) ||
         id == rv_id(142, key) || id == rv_id(143, key);
}

static UI0B32
rv_close_stale_right_action_popup(ReaderViewState *state,
                                  UI0B32 clear_active_row)
{
  ReaderViewKey key;
  UI0ID restore_id;
  UI0B32 close_popup;
  UI0B32 focus_changed = 0;
  if (!state || state->right_menu_key == 0) return 0;
  key = state->right_menu_key;
  close_popup = state->popup == ReaderViewPopup_RightRowActions;
  restore_id = close_popup ? state->restore_focus_id : 0;
  if (rv_right_row_owns_id(state->focus_id, key, restore_id))
  {
    state->focus_id = 0;
    state->focus_visible = 0;
    focus_changed = 1;
  }
  if (rv_right_row_owns_id(state->hot_id, key, restore_id))
    state->hot_id = 0;
  if (rv_right_row_owns_id(state->active_id, key, restore_id))
    state->active_id = 0;
  if (rv_right_row_owns_id(
        state->pending_accessibility_focus_id, key, restore_id))
    state->pending_accessibility_focus_id = 0;
  if (rv_right_row_owns_id(
        state->pending_accessibility_invoke_id, key, restore_id))
    state->pending_accessibility_invoke_id = 0;
  if (clear_active_row && state->active_right_key == key)
    state->active_right_key = 0;
  if (close_popup)
  {
    state->popup = ReaderViewPopup_None;
    state->restore_focus_id = 0;
  }
  state->right_menu_key = 0;
  state->right_menu_kind = ReaderViewRightRow_Bookmark;
  state->right_menu_actions = ReaderViewRightAction_None;
  return focus_changed;
}

static UI0B32
rv_right_filter_option_owns_id(UI0ID id)
{
  return id != 0 &&
    (id == rv_id(135, (ReaderViewKey)ReaderViewRightFilter_All) ||
     id == rv_id(135, (ReaderViewKey)ReaderViewRightFilter_Bookmarks) ||
     id == rv_id(135, (ReaderViewKey)ReaderViewRightFilter_Highlights) ||
     id == rv_id(135, (ReaderViewKey)ReaderViewRightFilter_Notes));
}

static UI0B32
rv_close_stale_right_filter_popup(ReaderViewState *state)
{
  UI0ID restore_id;
  UI0B32 focus_changed = 0;
  if (!state || state->popup != ReaderViewPopup_RightFilter) return 0;
  restore_id = state->restore_focus_id;
  if (rv_right_filter_option_owns_id(state->focus_id))
  {
    if (state->focus_id != restore_id) focus_changed = 1;
    state->focus_id = restore_id;
    if (!restore_id) state->focus_visible = 0;
  }
  if (rv_right_filter_option_owns_id(state->hot_id)) state->hot_id = 0;
  if (rv_right_filter_option_owns_id(state->active_id)) state->active_id = 0;
  if (rv_right_filter_option_owns_id(
        state->pending_accessibility_focus_id))
    state->pending_accessibility_focus_id = 0;
  if (rv_right_filter_option_owns_id(
        state->pending_accessibility_invoke_id))
    state->pending_accessibility_invoke_id = 0;
  state->popup = ReaderViewPopup_None;
  state->restore_focus_id = 0;
  state->right_filter_menu_flags = 0;
  return focus_changed;
}

static UI0Rect
rv_right_actions_popup_rect(const ReaderViewBuildInput *input,
                            const ReaderViewRightRow *row)
{
  UI0MenuStyle style = ui0_menu_style_from_resolved(input->theme);
  UI0Rect panel = input->layout->right_panel_rect;
  UI0Rect button = rv_rect(panel.x + panel.w - 48,
                           panel.y + RV_RIGHT_LIST_START_Y,
                           30, 28);
  UI0S32 count = rv_max(rv_right_action_popup_count(row), 1);
  UI0S32 body_padding_left = ui0_flat_row_popup_body_padding_left(
    style.popup_padding_right,
    UI0FlatRowIndicator_DefaultWidth,
    UI0FlatRowIndicator_DefaultGap);
  UI0S32 legacy_body_width = rv_max(
    rv_min(176, rv_max(138, panel.w - 34)) - 16, 1);
  UI0S32 menu_width = rv_max(legacy_body_width + body_padding_left +
                              style.popup_padding_right, 1);
  UI0S32 menu_height = rv_max(
    style.popup_padding_y * 2 + count * style.row_height +
      (count - 1) * style.row_gap, 1);
  UI0S32 menu_x;
  UI0S32 menu_y;
  (void)rv_right_row_menu_rect(input,
                               input->state->right_menu_key,
                               &button);
  menu_x = button.x - menu_width + button.w;
  menu_y = button.y + button.h + 4;
  if (menu_y + menu_height > panel.y + panel.h - 8)
    menu_y = button.y - menu_height - 4;
  menu_x = rv_clamp(menu_x,
                    panel.x + 8,
                    panel.x + panel.w - menu_width - 8);
  menu_y = rv_clamp(menu_y,
                    panel.y + 42,
                    panel.y + panel.h - menu_height - 8);
  return rv_rect(menu_x, menu_y, menu_width, menu_height);
}

static UI0Rect
rv_right_action_popup_row(const ReaderViewBuildInput *input,
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
  else if (state->popup == ReaderViewPopup_RightFilter &&
           input->layout->right_panel_visible)
  {
    result.x = input->layout->right_panel_rect.x + 10;
    result.y = input->layout->right_panel_rect.y + 40;
    result.w = rv_max(input->layout->right_panel_rect.w - 20, 80);
    result.h = RV_RIGHT_FILTER_POPUP_HEIGHT;
  }
  else if (state->popup == ReaderViewPopup_RightRowActions &&
           input->layout->right_panel_visible)
  {
    result = rv_right_actions_popup_rect(
      input,
      rv_find_right_row(input->projection, state->right_menu_key));
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
        rv_move_focus(ctx, rv_id(100, choice->key), 0);
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
      rv_close_popup_and_restore_focus(ctx);
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
      UI0ID restore_id = ctx->input->state->restore_focus_id;
      ctx->input->state->popup = ReaderViewPopup_None;
      ctx->input->state->restore_focus_id = 0;
      rv_toggle_right_panel(ctx, restore_id);
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

static ReaderViewRightFilterFlags
rv_filter_flag(ReaderViewRightFilter filter);

static ReaderViewText
rv_filter_label(ReaderViewLabels labels, ReaderViewRightFilter filter);

static ReaderViewText
rv_filter_label_with_count(RVBuildContext *ctx,
                           ReaderViewLabels labels,
                           ReaderViewRightFilter filter);

static void
rv_build_right_filter_popup(RVBuildContext *ctx,
                            UI0Rect popup,
                            ReaderViewLabels labels)
{
  static const ReaderViewRightFilter order[] = {
    ReaderViewRightFilter_All,
    ReaderViewRightFilter_Highlights,
    ReaderViewRightFilter_Notes,
    ReaderViewRightFilter_Bookmarks,
  };
  const ReaderViewRightProjection *right =
    &ctx->input->projection->right;
  ReaderViewState *state = ctx->input->state;
  UI0S32 index;
  UI0S32 slot = 0;
  UI0B32 initialize_focus =
    ctx->signals.focus_id == state->restore_focus_id;
  UI0B32 selected_available =
    (right->available_filters & rv_filter_flag(state->right_filter)) != 0;
  for (index = 0; index < (UI0S32)(sizeof(order) / sizeof(order[0])); ++index)
  {
    ReaderViewRightFilter value = order[index];
    UI0ID id;
    UI0Rect row_rect;
    if ((right->available_filters & rv_filter_flag(value)) == 0) continue;
    id = rv_id(135, (ReaderViewKey)value);
    row_rect = rv_rect(popup.x + 18,
                       popup.y + 4 +
                         slot * (RV_RIGHT_FILTER_ROW_HEIGHT +
                                 RV_RIGHT_FILTER_ROW_GAP),
                       rv_max(popup.w - 26, 1),
                       RV_RIGHT_FILTER_ROW_HEIGHT);
    if (initialize_focus &&
        ((selected_available && state->right_filter == value) ||
         (!selected_available && slot == 0)))
    {
      rv_move_focus(ctx, id, state->focus_visible);
      initialize_focus = 0;
    }
    if (rv_popup_button(ctx, 135, (ReaderViewKey)value,
                        row_rect,
                        rv_filter_label_with_count(ctx, labels, value),
                        1, state->right_filter == value, 0))
    {
      state->right_filter = value;
      state->right_scroll_y = 0;
      (void)rv_add_action(ctx, ReaderViewAction_RightFilterChanged,
                          0, 0, ReaderViewSetting_FontFamily,
                          ReaderViewRightRow_Bookmark, value, 0,
                          rv_text(0, 0));
      rv_close_popup_and_restore_focus(ctx);
    }
    {
      UI0ControlRecord *option = rv_control_record_for_id(ctx, id);
      if (option)
        option->text_rect = rv_rect(
          row_rect.x + RV_RIGHT_FILTER_TEXT_PADDING_X,
          row_rect.y,
          rv_max(row_rect.w - RV_RIGHT_FILTER_TEXT_PADDING_X * 2, 1),
          row_rect.h);
    }
    rv_set_semantic_control(ctx, id,
                            ReaderViewSemanticControl_RightFilterOption);
    slot += 1;
  }
}

static void
rv_close_popup_and_restore_focus(RVBuildContext *ctx)
{
  ReaderViewState *state;
  UI0ID restore_id;
  UI0B32 focus_visible;
  ReaderViewPopupKind dismissed;
  UI0S32 index;
  if (!ctx || !ctx->input || !ctx->input->state) return;
  state = ctx->input->state;
  dismissed = state->popup;
  restore_id = state->restore_focus_id;
  focus_visible = ctx->signals.focus_id != 0 &&
    ctx->signals.focus_visible_id == ctx->signals.focus_id;
  for (index = 0; index < ctx->control_count; ++index)
  {
    UI0ControlRecord *record = ctx->storage->control_records + index;
    if (record->root == UI0RootKind_Normal) continue;
    if (state->hot_id == record->id) state->hot_id = 0;
    if (state->active_id == record->id) state->active_id = 0;
    if (ctx->signals.hot_id == record->id) ctx->signals.hot_id = 0;
    if (ctx->signals.active_id == record->id) ctx->signals.active_id = 0;
    if (state->pending_accessibility_focus_id == record->id)
      state->pending_accessibility_focus_id = 0;
    if (state->pending_accessibility_invoke_id == record->id)
      state->pending_accessibility_invoke_id = 0;
  }
  rv_clear_prior_popup_interaction(state, restore_id);
  state->popup = ReaderViewPopup_None;
  state->restore_focus_id = 0;
  if (dismissed == ReaderViewPopup_RightFilter)
    state->right_filter_menu_flags = 0;
  ui0_signal_set_root(&ctx->signals, UI0RootKind_Popup,
                      rv_rect(0, 0, 0, 0), 0);
  ui0_signal_set_root(&ctx->signals, UI0RootKind_Modal,
                      rv_rect(0, 0, 0, 0), 0);
  ui0_signal_resolve_roots(&ctx->signals);
  rv_unblock_built_normal_root(ctx);
  rv_move_focus(ctx, restore_id, focus_visible);
  ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged;
}

static void
rv_initialize_popup_item_focus(RVBuildContext *ctx,
                               UI0ID id,
                               UI0B32 *initialize_focus)
{
  UI0B32 focus_visible;
  if (!ctx || !initialize_focus || !*initialize_focus) return;
  focus_visible = ctx->signals.focus_id != 0 &&
    ctx->signals.focus_visible_id == ctx->signals.focus_id;
  rv_move_focus(ctx, id, focus_visible);
  *initialize_focus = 0;
}

static ReaderViewText
rv_right_delete_label(ReaderViewLabels labels,
                      ReaderViewRightRowKind kind)
{
  switch (kind)
  {
    case ReaderViewRightRow_Bookmark: return labels.delete_bookmark;
    case ReaderViewRightRow_Note: return labels.delete_note;
    case ReaderViewRightRow_Highlight: return labels.delete_highlight;
    default: return labels.delete_value;
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
  UI0B32 initialize_focus =
    ctx->signals.focus_id == ctx->input->state->restore_focus_id;
  if (!row)
  {
    ctx->input->state->popup = ReaderViewPopup_None;
    return;
  }
  if ((row->actions & ReaderViewRightAction_Activate) != 0)
  {
    rv_initialize_popup_item_focus(ctx, rv_id(140, row->key),
                                   &initialize_focus);
    if (rv_popup_button(ctx, 140, row->key,
                        rv_right_action_popup_row(ctx->input, popup, item++),
                        labels.go_to, 1, 0, 0))
    {
      rv_close_popup_and_restore_focus(ctx);
      (void)rv_add_action(ctx, ReaderViewAction_ActivateRightRow,
                          row->key, 0, ReaderViewSetting_FontFamily, row->kind,
                          ReaderViewRightFilter_All, 0, rv_text(0, 0));
    }
    rv_set_semantic_control(ctx, rv_id(140, row->key),
                            ReaderViewSemanticControl_RightActionGoTo);
  }
  if (row->kind == ReaderViewRightRow_Highlight &&
      (row->actions & ReaderViewRightAction_ToggleStar) != 0)
  {
    rv_initialize_popup_item_focus(ctx, rv_id(141, row->key),
                                   &initialize_focus);
    if (rv_popup_button(ctx, 141, row->key,
                        rv_right_action_popup_row(ctx->input, popup, item++),
                        (row->flags & ReaderViewRow_Starred) ?
                          labels.unstar : labels.star, 1, 0, 0))
    {
      rv_close_popup_and_restore_focus(ctx);
      (void)rv_add_action(ctx, ReaderViewAction_ToggleRightRowStar,
                          row->key, 0, ReaderViewSetting_FontFamily, row->kind,
                          ReaderViewRightFilter_All, 0, rv_text(0, 0));
    }
    rv_set_semantic_control(ctx, rv_id(141, row->key),
                            ReaderViewSemanticControl_RightActionToggleStar);
  }
  if (row->kind == ReaderViewRightRow_Note &&
      (row->actions & ReaderViewRightAction_EditNote) != 0)
  {
    rv_initialize_popup_item_focus(ctx, rv_id(142, row->key),
                                   &initialize_focus);
    if (rv_popup_button(ctx, 142, row->key,
                        rv_right_action_popup_row(ctx->input, popup, item++),
                        labels.edit_note, 1, 0, 0))
    {
      rv_close_popup_and_restore_focus(ctx);
      (void)rv_add_action(ctx, ReaderViewAction_EditRightRowNote,
                          row->key, 0, ReaderViewSetting_FontFamily, row->kind,
                          ReaderViewRightFilter_All, 0, rv_text(0, 0));
    }
    rv_set_semantic_control(ctx, rv_id(142, row->key),
                            ReaderViewSemanticControl_RightActionEditNote);
  }
  if ((row->actions & ReaderViewRightAction_Delete) != 0)
  {
    rv_initialize_popup_item_focus(ctx, rv_id(143, row->key),
                                   &initialize_focus);
    if (rv_popup_button(ctx, 143, row->key,
                        rv_right_action_popup_row(ctx->input, popup, item++),
                        rv_right_delete_label(labels, row->kind), 1, 0, 1))
    {
      rv_close_popup_and_restore_focus(ctx);
      (void)rv_add_action(ctx, ReaderViewAction_DeleteRightRow,
                          row->key, 0, ReaderViewSetting_FontFamily, row->kind,
                          ReaderViewRightFilter_All, 0, rv_text(0, 0));
    }
    rv_set_semantic_control(ctx, rv_id(143, row->key),
                            ReaderViewSemanticControl_RightActionDelete);
  }
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
rv_hidden_scroll_region(RVBuildContext *ctx,
                        UI0Rect viewport,
                        UI0S32 content_h,
                        UI0S32 *scroll_y,
                        UI0ScrollState *obsolete_interaction)
{
  UI0S32 old_scroll_y;
  UI0S32 next_scroll_y;
  UI0S32 max_scroll_y;
  int64_t requested;
  if (!ctx || !scroll_y) return 0;
  old_scroll_y = *scroll_y;
  max_scroll_y = ui0_scroll_max_y(viewport.h, content_h);
  next_scroll_y = rv_clamp(old_scroll_y, 0, max_scroll_y);
  rv_reset_scroll_interaction(obsolete_interaction, ctx->frame);
  if (ctx->signals.input_root == UI0RootKind_Normal &&
      ctx->signals.input.wheel_delta_y != 0 &&
      ui0_rect_contains_point(viewport,
                              ctx->signals.input.pointer_x,
                              ctx->signals.input.pointer_y))
  {
    requested = (int64_t)next_scroll_y +
                (int64_t)ctx->signals.input.wheel_delta_y;
    if (requested < 0)
      next_scroll_y = 0;
    else if (requested > (int64_t)max_scroll_y)
      next_scroll_y = max_scroll_y;
    else
      next_scroll_y = (UI0S32)requested;
  }
  *scroll_y = next_scroll_y;
  if (next_scroll_y != old_scroll_y)
    ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged;
  return next_scroll_y;
}

static void
rv_build_toc_panel(RVBuildContext *ctx,
                   UI0Rect panel,
                   UI0Rect list_rect,
                   ReaderViewLabels labels)
{
  const ReaderViewTocProjection *toc = &ctx->input->projection->toc;
  ReaderViewState *state = ctx->input->state;
  UI0S32 scroll_y;
  UI0S32 index;
  UI0S32 visual_stride = RV_NAV_CONTENTS_VISUAL_ROW_HEIGHT +
                         RV_NAV_CONTENTS_VISUAL_ROW_GAP;
  UI0S32 content_h = toc->row_count > 0 ?
    toc->row_count * RV_NAV_CONTENTS_VISUAL_ROW_HEIGHT +
      (toc->row_count - 1) * RV_NAV_CONTENTS_VISUAL_ROW_GAP : 0;
  UI0Rect visual_clip = rv_rect(
    list_rect.x,
    panel.y + RV_NAV_CONTENTS_VISUAL_CLIP_Y,
    list_rect.w,
    rv_max(panel.h - RV_NAV_CONTENTS_VISUAL_CLIP_Y -
             RV_NAV_BOTTOM_PAD, 0));
  if (toc->status.state != ReaderViewLoad_Ready)
  {
    rv_add_status(ctx, ctx->left_panel_id, 211, list_rect, toc->status);
    return;
  }
  if (toc->row_count == 0)
  {
    UI0ID empty_id = rv_id(211, 0);
    rv_add_text_record(
      ctx, empty_id, ctx->left_panel_id,
      rv_rect(list_rect.x, panel.y + RV_NAV_CONTENTS_EMPTY_Y,
              list_rect.w, RV_NAV_CONTENTS_EMPTY_HEIGHT),
      labels.no_contents,
      ReaderViewSemantic_Status, ReaderViewSemantic_Enabled, 0);
    rv_add_text_override(ctx, empty_id, UI0TextAlignX_Start,
                         UI0TypographyRole_Body,
                         ctx->input->theme->colors[UI0ColorRole_TextMuted]);
    return;
  }
  for (index = 0; index < toc->row_count; ++index)
  {
    const ReaderViewTocRow *row = toc->rows + index;
    if (ctx->signals.focus_id == rv_id(212, row->key))
    {
      UI0S32 row_top = index * visual_stride;
      UI0S32 row_bottom = row_top + RV_NAV_CONTENTS_VISUAL_ROW_HEIGHT;
      UI0S32 old_scroll_y = state->toc_scroll_y;
      if (row_top < state->toc_scroll_y)
        state->toc_scroll_y = row_top;
      else if (row_bottom > state->toc_scroll_y + visual_clip.h)
        state->toc_scroll_y = rv_max(0, row_bottom - visual_clip.h);
      if (state->toc_scroll_y != old_scroll_y)
        ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged;
      break;
    }
  }
  scroll_y = rv_hidden_scroll_region(ctx, visual_clip, content_h,
                                     &state->toc_scroll_y,
                                     &state->toc_scroll);
  for (index = 0; index < toc->row_count; ++index)
  {
    const ReaderViewTocRow *row = toc->rows + index;
    UI0Rect visual_outer = rv_rect(
      visual_clip.x,
      visual_clip.y + index * visual_stride - scroll_y,
      visual_clip.w,
      RV_NAV_CONTENTS_VISUAL_ROW_HEIGHT);
    UI0Rect semantic_rect = rv_rect(
      list_rect.x,
      list_rect.y + index * RV_NAV_CONTENTS_SEMANTIC_ROW_HEIGHT - scroll_y,
      list_rect.w,
      RV_NAV_CONTENTS_SEMANTIC_ROW_HEIGHT);
    UI0Rect row_rect = visual_outer;
    UI0Rect hit_rect = ui0_rect_intersect(visual_outer, visual_clip);
    UI0ID id = rv_id(212, row->key);
    UI0S32 control_index = ctx->control_count;
    if (hit_rect.w <= 0 || hit_rect.h <= 0) continue;
    if (rv_add_control_with_hit_rect(
          ctx, id, ctx->left_panel_id,
          UI0ControlKind_SidenavRow,
          ReaderViewSemantic_ListItem,
          UI0RootKind_Normal,
          row_rect, hit_rect,
          row->label, row->detail, row->key,
          (row->flags & ReaderViewRow_Enabled) != 0,
          (row->flags & (ReaderViewRow_Selected |
                         ReaderViewRow_Current)) != 0,
          0, 0, 0))
    {
      state->active_toc_key = row->key;
      (void)rv_add_action(ctx, ReaderViewAction_ActivateTocRow,
                          row->key, 0, ReaderViewSetting_FontFamily,
                          ReaderViewRightRow_Bookmark,
                          ReaderViewRightFilter_All, 0, rv_text(0, 0));
    }
    if (ctx->control_count > control_index)
    {
      UI0ControlRecord *record =
        ctx->storage->control_records + control_index;
      UI0Rect visual_body = ui0_flat_row_body_rect(
        visual_outer,
        RV_NAV_CONTENTS_VISUAL_INDICATOR_WIDTH,
        RV_NAV_CONTENTS_VISUAL_INDICATOR_GAP);
      uint64_t indent_u64 =
        (uint64_t)row->depth * RV_NAV_CONTENTS_VISUAL_INDENT;
      UI0S32 maximum_indent = rv_max(
        visual_body.w -
          (RV_NAV_CONTENTS_VISUAL_PADDING_X * 2 +
           RV_NAV_CONTENTS_VISUAL_EXPANDER +
           RV_NAV_CONTENTS_VISUAL_EXPANDER_GAP + 1), 0);
      UI0S32 visual_indent = indent_u64 > (uint64_t)maximum_indent ?
        maximum_indent : (UI0S32)indent_u64;
      UI0S32 expander_x = visual_body.x +
        RV_NAV_CONTENTS_VISUAL_PADDING_X + visual_indent;
      UI0B32 has_children = index + 1 < toc->row_count &&
        toc->rows[index + 1].depth > row->depth;
      UI0Rect expander_rect = has_children ?
        rv_rect(expander_x,
                visual_body.y +
                  (visual_body.h - RV_NAV_CONTENTS_VISUAL_EXPANDER) / 2,
                RV_NAV_CONTENTS_VISUAL_EXPANDER,
                RV_NAV_CONTENTS_VISUAL_EXPANDER) :
        rv_rect(0, 0, 0, 0);
      UI0S32 text_x = expander_x +
        RV_NAV_CONTENTS_VISUAL_EXPANDER +
        RV_NAV_CONTENTS_VISUAL_EXPANDER_GAP;
      UI0S32 text_right = visual_body.x + visual_body.w -
        RV_NAV_CONTENTS_VISUAL_PADDING_X;
      record->clip_rect = hit_rect;
      record->text_rect = rv_rect(text_x,
                                  visual_body.y,
                                  rv_max(text_right - text_x, 0),
                                  visual_body.h);
      rv_add_sidenav_visual(
        ctx,
        id,
        rv_id(213, 0),
        index,
        row->depth > 0x7fffffffu ?
          0x7fffffff : (UI0S32)row->depth,
        visual_outer,
        visual_body,
        visual_clip,
        record->text_rect,
        ui0_flat_row_indicator_rect(
          visual_body,
          RV_NAV_CONTENTS_VISUAL_INDICATOR_WIDTH,
          RV_NAV_CONTENTS_VISUAL_INDICATOR_GAP),
        expander_rect,
        row->label,
        (row->flags & (ReaderViewRow_Selected |
                       ReaderViewRow_Current)) != 0,
        (row->flags & ReaderViewRow_Current) != 0,
        has_children,
        has_children);
    }
    rv_set_semantic_rect(ctx, id, semantic_rect);
    rv_set_semantic_control(ctx, id, ReaderViewSemanticControl_TocRow);
    if ((row->flags & ReaderViewRow_Current) != 0)
      rv_add_semantic_flags_for_id(ctx, id, ReaderViewSemantic_Current);
  }
}

static void
rv_add_find_input(RVBuildContext *ctx,
                  UI0Rect semantic_rect,
                  UI0Rect field_rect,
                  ReaderViewLabels labels)
{
  ReaderViewState *state = ctx->input->state;
  UI0ID input_id = rv_id(220, 0);
  UI0BoxDesc box_desc;
  UI0S32 box_index;
  UI0TextInputSpec spec;
  UI0TextInputBuffer buffer;
  UI0TextInputResult result;
  UI0ControlRecord *control;
  ReaderViewText query;
  ReaderViewText display_text;
  UI0B32 blocked;

  if (state->pending_accessibility_focus_id == input_id)
  {
    state->pending_accessibility_focus_id = 0;
    if (state->popup == ReaderViewPopup_None)
      rv_move_focus(ctx, input_id, 1);
  }
  /* SearchBox invocation remains a bounded no-op, as before this adoption. */
  if (state->pending_accessibility_invoke_id == input_id)
    state->pending_accessibility_invoke_id = 0;

  box_desc = ui0_box_desc("reader.find.input",
                          UI0LayoutInvalidIndex,
                          UI0Axis_X,
                          ui0_size_fixed(field_rect.w),
                          ui0_size_fixed(field_rect.h));
  box_index = ui0_layout_add_box(&ctx->input_layout, &box_desc);
  if (box_index == UI0LayoutInvalidIndex ||
      !ui0_layout_solve(&ctx->input_layout, box_index, field_rect))
  {
    ctx->frame->error_flags |= ReaderViewFrameError_RecordCap;
    return;
  }

  memset(&spec, 0, sizeof(spec));
  spec.id = input_id;
  spec.box_index = box_index;
  spec.root = UI0RootKind_Normal;
  spec.placeholder = labels.find_placeholder.data;
  spec.placeholder_len = labels.find_placeholder.size;
  spec.frame_input = ctx->input->input->find_text;
  buffer.data = state->find_query;
  buffer.length = &state->find_query_length;
  buffer.cap = READER_VIEW_FIND_QUERY_CAP;
  result = ui0_text_input(&ctx->text_inputs,
                          &ctx->signals,
                          &ctx->input_layout,
                          spec,
                          &buffer,
                          &state->find_input);
  if (ctx->text_inputs.record_count > 0)
    rv_apply_reference_find_caret_height(
      ctx->storage->text_input_records +
      (ctx->text_inputs.record_count - 1));
  query = reader_view_find_query(state);
  if (query.size == 0 && ctx->text_inputs.record_count > 0)
  {
    UI0TextInputRecord *record = ctx->storage->text_input_records +
      (ctx->text_inputs.record_count - 1);
    /* Frozen a6b paints `Search in book|` while the empty input is focused. */
    record->state |= UI0TextInputState_PlaceholderVisible;
    record->text_draw_rect.w = record->placeholder_width;
    result.state |= UI0TextInputState_PlaceholderVisible;
  }
  display_text = query.size > 0 ? query : labels.find_placeholder;
  blocked = (result.state & UI0TextInputState_BlockedByRoot) != 0;

  if (ctx->control_count >= READER_VIEW_CONTROL_CAP)
  {
    ctx->frame->error_flags |= ReaderViewFrameError_RecordCap;
    return;
  }
  control = ctx->storage->control_records + ctx->control_count++;
  memset(control, 0, sizeof(*control));
  control->id = input_id;
  control->kind = UI0ControlKind_TextInput;
  control->box_index = box_index;
  control->root = UI0RootKind_Normal;
  control->state = rv_control_state(result.signal, UI0Control_None, 0, 0);
  control->signal_flags = result.signal.flags;
  control->rect = semantic_rect;
  control->clip_rect = field_rect;
  if (ctx->text_inputs.record_count > 0)
    control->text_rect = ctx->storage->text_input_records[0].text_rect;
  control->label_hash = rv_text_hash(display_text);
  control->label_len = display_text.size;
  (void)rv_add_binding(ctx, input_id, display_text,
                       ReaderViewTextStyle_Default);
  (void)rv_add_semantic(ctx,
                        input_id,
                        ctx->left_panel_id,
                        ReaderViewSemantic_SearchBox,
                        rv_semantic_flags(ctx, input_id,
                                          !blocked, !blocked,
                                          0, 0, 0),
                        semantic_rect,
                        labels.search_input,
                        query,
                        0, 0, 0, 0);
  rv_set_semantic_control(ctx, input_id,
                          ReaderViewSemanticControl_FindInput);

  if (result.edited)
    (void)rv_add_action(ctx, ReaderViewAction_FindChanged, 0, 0,
                        ReaderViewSetting_FontFamily,
                        ReaderViewRightRow_Bookmark,
                        ReaderViewRightFilter_All, 0, query);
  if (result.committed)
    (void)rv_add_action(ctx, ReaderViewAction_FindCommitted, 0, 0,
                        ReaderViewSetting_FontFamily,
                        ReaderViewRightRow_Bookmark,
                        ReaderViewRightFilter_All, 0, query);
  if (result.edited || result.committed)
    ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged;
}

static void
rv_sync_cleared_find_input(RVBuildContext *ctx,
                           UI0ID input_id,
                           ReaderViewText placeholder)
{
  ReaderViewState *state = ctx->input->state;
  UI0ControlRecord *control = rv_control_record_for_id(ctx, input_id);
  UI0TextInputRecord *record = ctx->text_inputs.record_count > 0 ?
    ctx->storage->text_input_records : 0;
  UI0S32 index;
  if (control)
  {
    control->state &= ~(UI0ControlState_Focused |
                        UI0ControlState_FocusVisible);
    if (ctx->signals.focus_id == input_id)
      control->state |= UI0ControlState_Focused;
    if (ctx->signals.focus_visible_id == input_id)
      control->state |= UI0ControlState_FocusVisible;
  }
  if (record && record->id == input_id)
  {
    record->state &= ~(UI0TextInputState_Focused |
                       UI0TextInputState_FocusVisible |
                       UI0TextInputState_HasSelection |
                       UI0TextInputState_TextClipped |
                       UI0TextInputState_SelectionClipped |
                       UI0TextInputState_CaretClipped);
    if (ctx->signals.focus_id == input_id)
      record->state |= UI0TextInputState_Focused;
    if (ctx->signals.focus_visible_id == input_id)
      record->state |= UI0TextInputState_FocusVisible;
    record->state |= UI0TextInputState_PlaceholderVisible;
    record->text_hash = rv_text_hash(rv_text("", 0));
    record->text_len = 0;
    record->caret = 0;
    record->selection_start = 0;
    record->selection_end = 0;
    record->scroll_x = 0;
    record->full_text_width = 0;
    record->visible_text_width = 0;
    record->text_draw_rect = rv_rect(record->text_rect.x,
                                     record->rect.y,
                                     record->placeholder_width,
                                     record->rect.h);
    record->selection_rect = rv_rect(0, 0, 0, 0);
    record->caret_rect = ui0_text_input_caret_rect(
      ctx->text_inputs.style.measure,
      record->text_rect,
      0,
      state->find_query,
      0,
      0,
      ctx->text_inputs.style.caret_width);
    rv_apply_reference_find_caret_height(record);
  }
  rv_set_control_visual_text(ctx, input_id, placeholder);
  rv_set_semantic_value(ctx, input_id, reader_view_find_query(state));
  for (index = ctx->semantic_count - 1; index >= 0; --index)
  {
    ReaderViewSemanticNode *node = ctx->storage->semantic_nodes + index;
    if (node->id == input_id)
    {
      node->flags &= ~ReaderViewSemantic_Focused;
      if (ctx->signals.focus_id == input_id)
        node->flags |= ReaderViewSemantic_Focused;
      break;
    }
  }
}

static void
rv_build_find_panel(RVBuildContext *ctx,
                    UI0Rect panel,
                    UI0Rect list_rect,
                    ReaderViewLabels labels)
{
  const ReaderViewFindProjection *find = &ctx->input->projection->find;
  ReaderViewState *state = ctx->input->state;
  UI0Rect body = rv_rect(panel.x + RV_NAV_RAIL_WIDTH,
                         panel.y,
                         rv_max(panel.w - RV_NAV_RAIL_WIDTH, 1),
                         panel.h);
  UI0Rect input_rect = rv_rect(body.x + 20, body.y + 48,
                               rv_max(body.w - 40, 1),
                               RV_NAV_INPUT_HEIGHT);
  UI0Rect input_hit_rect = rv_rect(input_rect.x, input_rect.y,
                                   rv_max(input_rect.w - 34, 1),
                                   input_rect.h);
  UI0Rect clear_rect = rv_rect(input_rect.x + input_rect.w - 30,
                               input_rect.y + 5,
                               24, 24);
  UI0S32 scroll_y;
  UI0S32 index;
  UI0S32 content_h;
  UI0Rect content_clip;
  UI0ID input_id = rv_id(220, 0);
  UI0ID clear_id = rv_id(221, 0);
  UI0S32 clear_control_index;
  UI0B32 clear_accessibility_invoke;
  ReaderViewText query;
  ReaderViewText ready_status;
  rv_add_find_input(ctx, input_rect, input_hit_rect, labels);
  query = reader_view_find_query(state);
  ready_status = find->status.message;
  clear_control_index = ctx->control_count;
  clear_accessibility_invoke =
    state->pending_accessibility_invoke_id == clear_id;
  if (rv_add_icon_control(ctx, clear_id, ctx->left_panel_id,
                          UI0ControlKind_IconButton,
                          ReaderViewSemantic_Button,
                          UI0RootKind_Normal,
                          clear_rect, clear_rect,
                          labels.clear_search, rv_text(0, 0), 0,
                          state->find_query_length > 0,
                          0, 0, 0, 0,
                          UI0IconKind_Close,
                          rv_toolbar_icon_rect(clear_rect),
                          state->find_query_length > 0))
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
    rv_move_focus(ctx, input_id,
      clear_accessibility_invoke ||
      (ctx->control_count > clear_control_index &&
       (ctx->storage->control_records[clear_control_index].state &
        UI0ControlState_KeyboardActivated) != 0));
    rv_sync_cleared_find_input(ctx, input_id, labels.find_placeholder);
    query = reader_view_find_query(state);
    ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged;
  }
  rv_make_control_nonquiet(ctx, clear_id);
  rv_set_semantic_control(ctx, clear_id,
                          ReaderViewSemanticControl_FindClear);

  if (find->status.state == ReaderViewLoad_Ready && ready_status.size == 0 &&
      find->row_count == 0)
  {
    if (query.size == 0)
      ready_status = labels.find_prompt;
    else if (find->total_count == 0)
      ready_status = labels.no_matches;
  }
  if (find->status.state == ReaderViewLoad_Ready && ready_status.size > 0)
  {
    UI0ID status_id = rv_id(223, 0);
    rv_add_text_record(ctx, status_id, ctx->left_panel_id,
                       rv_rect(body.x + 20,
                               body.y + RV_NAV_FIND_STATUS_Y,
                               rv_max(body.w - 40, 1),
                               RV_NAV_FIND_STATUS_HEIGHT),
                       ready_status,
                       ReaderViewSemantic_Status,
                       ReaderViewSemantic_Enabled, 0);
    rv_add_text_override(ctx, status_id, UI0TextAlignX_Start,
                         UI0TypographyRole_Body,
                         ctx->input->theme->colors[UI0ColorRole_TextMuted]);
  }

  if (find->status.state != ReaderViewLoad_Ready)
  {
    rv_add_status(ctx, ctx->left_panel_id, 224, list_rect, find->status);
    return;
  }
  content_h = find->row_count * RV_NAV_FIND_ROW_HEIGHT;
  for (index = 0; index < find->row_count; ++index)
  {
    if (ctx->signals.focus_id == rv_id(227, find->rows[index].key))
    {
      UI0S32 old_scroll_y = state->find_scroll_y;
      UI0S32 row_top = index * RV_NAV_FIND_ROW_HEIGHT;
      UI0S32 row_bottom = row_top + RV_NAV_FIND_ROW_HEIGHT;
      if (row_top < state->find_scroll_y)
        state->find_scroll_y = row_top;
      else if (row_bottom > state->find_scroll_y + list_rect.h)
        state->find_scroll_y = rv_max(0, row_bottom - list_rect.h);
      if (state->find_scroll_y != old_scroll_y)
        ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged;
      break;
    }
  }
  scroll_y = rv_hidden_scroll_region(ctx, list_rect, content_h,
                                     &state->find_scroll_y,
                                     &state->find_scroll);
  content_clip = list_rect;
  for (index = 0; index < find->row_count; ++index)
  {
    const ReaderViewFindRow *row = find->rows + index;
    UI0Rect row_rect = rv_rect(list_rect.x,
                               list_rect.y +
                                 index * RV_NAV_FIND_ROW_HEIGHT - scroll_y,
                               list_rect.w,
                               RV_NAV_FIND_ROW_HEIGHT);
    UI0Rect hit_rect = ui0_rect_intersect(row_rect, content_clip);
    UI0Rect section_rect = rv_rect(row_rect.x + 12, row_rect.y + 8,
                                   rv_max(row_rect.w - 24, 1), 18);
    UI0Rect excerpt_rect = rv_rect(row_rect.x + 12, row_rect.y + 32,
                                   rv_max(row_rect.w - 24, 1),
                                   rv_max(row_rect.h - 42, 1));
    UI0ID row_id = rv_id(227, row->key);
    UI0S32 control_index = ctx->control_count;
    if (hit_rect.w <= 0 || hit_rect.h <= 0) continue;
    if (rv_add_control_with_hit_rect(
          ctx, row_id, ctx->left_panel_id,
          UI0ControlKind_SidenavRow,
          ReaderViewSemantic_ListItem,
          UI0RootKind_Normal,
          row_rect, hit_rect,
          row->excerpt, row->section, row->key,
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
    if (ctx->control_count > control_index)
    {
      UI0ControlRecord *record =
        ctx->storage->control_records + control_index;
      record->clip_rect = hit_rect;
      record->text_rect = rv_rect(0, 0, 0, 0);
    }
    rv_add_visual_fill(
      ctx, rv_id(231, row->key),
      rv_rect(row_rect.x,
              row_rect.y + row_rect.h - RV_NAV_FIND_DIVIDER_HEIGHT,
              row_rect.w, RV_NAV_FIND_DIVIDER_HEIGHT),
      content_clip,
      ctx->input->theme->colors[UI0ColorRole_BorderMuted]);
    rv_set_semantic_control(ctx, row_id, ReaderViewSemanticControl_FindRow);
    if (row->section.size > 0)
    {
      rv_add_text_record_styled(ctx, rv_id(228, row->key), row_id,
                                section_rect, row->section,
                                ReaderViewTextStyle_ChromeMetadata,
                                ReaderViewSemantic_Group,
                                ReaderViewSemantic_Enabled, row->key);
      rv_add_text_override(ctx, rv_id(228, row->key),
                           UI0TextAlignX_End,
                           UI0TypographyRole_Metadata,
                           ctx->input->theme->colors[UI0ColorRole_TextMuted]);
      {
        UI0ControlRecord *section_record =
          rv_control_record_for_id(ctx, rv_id(228, row->key));
        if (section_record)
          section_record->clip_rect = ui0_rect_intersect(
            section_record->clip_rect, content_clip);
      }
    }
    rv_add_text_record(ctx, rv_id(230, row->key), row_id,
                       excerpt_rect, row->excerpt,
                       ReaderViewSemantic_Group,
                       ReaderViewSemantic_Enabled, row->key);
    {
      UI0ControlRecord *excerpt_record =
        rv_control_record_for_id(ctx, rv_id(230, row->key));
      if (excerpt_record)
        excerpt_record->clip_rect = ui0_rect_intersect(
          excerpt_record->clip_rect, content_clip);
    }
    rv_set_text_binding_match(ctx, rv_id(230, row->key),
                              row->match_start, row->match_size);
  }
}

static void
rv_build_left_panel(RVBuildContext *ctx, ReaderViewLabels labels)
{
  ReaderViewState *state = ctx->input->state;
  UI0Rect panel = ctx->input->layout->left_panel_rect;
  UI0Rect body;
  UI0Rect list_rect;
  UI0Rect contents_tab;
  UI0Rect find_tab;
  UI0Rect close_rect;
  ReaderViewText panel_name;
  if (!ctx->input->layout->left_panel_visible || panel.w <= 0) return;
  if (state->pending_left_panel_focus == state->left_panel)
  {
    rv_focus_left_panel(ctx, state->left_panel);
    state->pending_left_panel_focus = ReaderViewLeftPanel_None;
    ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged;
  }
  ctx->left_panel_id = rv_id(200, 0);
  panel_name = state->left_panel == ReaderViewLeftPanel_Find ?
    labels.find_panel_title : labels.contents_panel_title;
  (void)rv_add_surface(ctx, ctx->left_panel_id, 0,
                       UI0ControlKind_PanelSurface,
                       ReaderViewSemantic_Panel,
                       UI0RootKind_Normal,
                       panel, panel_name);
  rv_add_visual_fill(ctx, rv_id(205, 0),
                     rv_rect(panel.x + RV_NAV_RAIL_WIDTH - 1,
                             panel.y + 12, 1,
                             rv_max(panel.h - 24, 1)),
                     panel,
                     ctx->input->theme->colors[UI0ColorRole_BorderMuted]);
  rv_add_visual_fill(ctx, rv_id(206, 0),
                     rv_rect(panel.x + panel.w - 1,
                             panel.y, 1, panel.h),
                     panel,
                     ctx->input->theme->colors[UI0ColorRole_BorderMuted]);
  body = rv_rect(panel.x + RV_NAV_RAIL_WIDTH,
                 panel.y,
                 rv_max(panel.w - RV_NAV_RAIL_WIDTH, 1),
                 panel.h);
  contents_tab = rv_rect(panel.x + 8, panel.y + 12,
                         rv_max(RV_NAV_RAIL_WIDTH - 16, 1),
                         RV_NAV_RAIL_ROW_HEIGHT);
  find_tab = rv_rect(contents_tab.x,
                     contents_tab.y + RV_NAV_RAIL_ROW_STEP,
                     contents_tab.w,
                     contents_tab.h);
  close_rect = rv_rect(body.x + body.w - 34, body.y + 10, 24, 24);
  if (rv_has_feature(ctx->input->projection, ReaderViewFeature_Contents) &&
      rv_add_control(ctx, rv_id(201, 0), ctx->left_panel_id,
                     UI0ControlKind_SegmentItem, ReaderViewSemantic_Tab,
                     UI0RootKind_Normal,
                     contents_tab,
                     labels.contents, rv_text(0, 0), 0, 1,
                     state->left_panel == ReaderViewLeftPanel_Contents,
                     state->left_panel == ReaderViewLeftPanel_Contents,
                     0, 0))
  {
    if (state->left_panel != ReaderViewLeftPanel_Contents)
      rv_toggle_left_panel(ctx, ReaderViewLeftPanel_Contents, 0);
  }
  rv_set_control_visual_text(ctx, rv_id(201, 0), labels.contents_short);
  rv_set_semantic_control(ctx, rv_id(201, 0),
                          ReaderViewSemanticControl_LeftContentsTab);
  if (rv_has_feature(ctx->input->projection, ReaderViewFeature_Find) &&
      rv_add_control(ctx, rv_id(202, 0), ctx->left_panel_id,
                     UI0ControlKind_SegmentItem, ReaderViewSemantic_Tab,
                     UI0RootKind_Normal,
                     find_tab,
                     labels.find, rv_text(0, 0), 0, 1,
                     state->left_panel == ReaderViewLeftPanel_Find,
                     state->left_panel == ReaderViewLeftPanel_Find,
                     0, 0))
  {
    if (state->left_panel != ReaderViewLeftPanel_Find)
      rv_toggle_left_panel(ctx, ReaderViewLeftPanel_Find, 0);
  }
  rv_set_semantic_control(ctx, rv_id(202, 0),
                          ReaderViewSemanticControl_LeftFindTab);
  {
    UI0Rect rail_rows[2] = {contents_tab, find_tab};
    ReaderViewText rail_labels[2] = {labels.contents_short, labels.find};
    UI0ID rail_ids[2] = {rv_id(201, 0), rv_id(202, 0)};
    UI0B32 rail_selected[2] = {
      state->left_panel == ReaderViewLeftPanel_Contents,
      state->left_panel == ReaderViewLeftPanel_Find,
    };
    UI0B32 rail_enabled[2] = {
      rv_has_feature(ctx->input->projection, ReaderViewFeature_Contents),
      rv_has_feature(ctx->input->projection, ReaderViewFeature_Find),
    };
    UI0S32 rail_index;
    for (rail_index = 0; rail_index < 2; ++rail_index)
    {
      UI0Rect outer;
      UI0Rect visual_body;
      UI0Rect text_rect;
      if (!rail_enabled[rail_index]) continue;
      outer = rail_rows[rail_index];
      visual_body = ui0_flat_row_body_rect(outer, 3, 4);
      text_rect = rv_rect(visual_body.x + 8 + 1,
                          visual_body.y,
                          rv_max(visual_body.w - 8 - 1 - 8, 0),
                          visual_body.h);
      rv_add_sidenav_visual(
        ctx,
        rail_ids[rail_index],
        rv_id(207, 0),
        rail_index,
        0,
        outer,
        visual_body,
        panel,
        text_rect,
        ui0_flat_row_indicator_rect(visual_body, 3, 4),
        rv_rect(0, 0, 0, 0),
        rail_labels[rail_index],
        rail_selected[rail_index],
        rail_selected[rail_index],
        0,
        0);
    }
  }
  if (rv_add_icon_control(ctx, rv_id(203, 0), ctx->left_panel_id,
                          UI0ControlKind_IconButton,
                          ReaderViewSemantic_Button,
                          UI0RootKind_Normal,
                          close_rect, close_rect,
                          labels.close_navigation, rv_text(0, 0), 0,
                          1, 0, 0, 0, 0,
                          UI0IconKind_Close,
                          rv_toolbar_icon_rect(close_rect), 1))
    rv_close_left_panel(ctx);
  rv_make_control_nonquiet(ctx, rv_id(203, 0));
  rv_set_semantic_control(ctx, rv_id(203, 0),
                          ReaderViewSemanticControl_LeftPanelClose);

  rv_add_text_record_styled(ctx, rv_id(204, 0), ctx->left_panel_id,
                            rv_rect(body.x + 20, body.y + 12,
                                    rv_max(body.w - 64, 1),
                                    RV_NAV_TITLE_HEIGHT),
                            panel_name,
                            ReaderViewTextStyle_ChromeTitle,
                            ReaderViewSemantic_Group,
                            ReaderViewSemantic_Enabled, 0);
  list_rect = rv_rect(body.x + 20,
                      panel.y +
                        (state->left_panel == ReaderViewLeftPanel_Find ?
                           RV_NAV_FIND_START_Y : RV_NAV_CONTENTS_START_Y),
                      rv_max(body.w - 40, 1),
                      rv_max(panel.h -
                        (state->left_panel == ReaderViewLeftPanel_Find ?
                           RV_NAV_FIND_START_Y : RV_NAV_CONTENTS_START_Y) -
                        RV_NAV_BOTTOM_PAD, 0));
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

static ReaderViewText
rv_right_empty_label(ReaderViewLabels labels, ReaderViewRightFilter filter)
{
  switch (filter)
  {
    case ReaderViewRightFilter_Bookmarks: return labels.no_bookmarks;
    case ReaderViewRightFilter_Highlights: return labels.no_highlights;
    case ReaderViewRightFilter_Notes: return labels.no_notes;
    case ReaderViewRightFilter_All:
    default: return labels.no_annotations;
  }
}

static ReaderViewText
rv_filter_label_with_count(RVBuildContext *ctx,
                           ReaderViewLabels labels,
                           ReaderViewRightFilter filter)
{
  const ReaderViewRightProjection *right;
  ReaderViewText base;
  UI0U64 count;
  UI0S32 slot;
  char digits[20];
  UI0S32 digit_count = 0;
  UI0S32 index;
  char *buffer;
  if (!ctx || !ctx->input || !ctx->input->projection || !ctx->storage)
    return rv_filter_label(labels, filter);
  right = &ctx->input->projection->right;
  base = rv_filter_label(labels, filter);
  switch (filter)
  {
    case ReaderViewRightFilter_All:
      slot = 0;
      count = right->all_count;
      break;
    case ReaderViewRightFilter_Highlights:
      slot = 1;
      count = right->highlight_count;
      break;
    case ReaderViewRightFilter_Notes:
      slot = 2;
      count = right->note_count;
      break;
    case ReaderViewRightFilter_Bookmarks:
      slot = 3;
      count = right->bookmark_count;
      break;
    default:
      return base;
  }
  do
  {
    digits[digit_count++] = (char)('0' + (count % 10));
    count /= 10;
  } while (count != 0 && digit_count < (UI0S32)sizeof(digits));
  if (base.size < 0 ||
      base.size > READER_VIEW_RIGHT_FILTER_LABEL_CAP - 3 - digit_count)
  {
    ctx->frame->error_flags |= ReaderViewFrameError_RecordCap;
    return base;
  }
  buffer = ctx->storage->right_filter_labels[slot];
  for (index = 0; index < base.size; ++index)
    buffer[index] = base.data[index];
  buffer[base.size] = ' ';
  buffer[base.size + 1] = '(';
  for (index = 0; index < digit_count; ++index)
    buffer[base.size + 2 + index] = digits[digit_count - index - 1];
  buffer[base.size + 2 + digit_count] = ')';
  return rv_text(buffer, base.size + 3 + digit_count);
}

static UI0B32
rv_right_row_starts_section(const ReaderViewRightProjection *right,
                            UI0S32 index)
{
  if (!right || index < 0 || index >= right->row_count ||
      right->rows[index].section.size == 0)
    return 0;
  return index == 0 ||
         !rv_text_same(right->rows[index - 1].section,
                       right->rows[index].section);
}

static UI0S32
rv_right_content_height(const ReaderViewRightProjection *right)
{
  UI0S32 index;
  UI0S32 result = 0;
  if (!right) return 0;
  for (index = 0; index < right->row_count; ++index)
  {
    if (rv_right_row_starts_section(right, index))
      result += RV_RIGHT_SECTION_HEIGHT;
    result += RV_RIGHT_ROW_HEIGHT;
    if (index + 1 < right->row_count &&
        (right->rows[index + 1].flags &
         ReaderViewRow_AttachedToPrevious) == 0)
      result += RV_RIGHT_ROW_GAP;
  }
  return result;
}

static void
rv_build_right_panel(RVBuildContext *ctx, ReaderViewLabels labels)
{
  const ReaderViewRightProjection *right = &ctx->input->projection->right;
  ReaderViewState *state = ctx->input->state;
  UI0Rect panel = ctx->input->layout->right_panel_rect;
  UI0Rect list_rect;
  UI0Rect filter_rect;
  UI0Rect export_rect;
  UI0Rect close_rect;
  UI0S32 scroll_y;
  UI0S32 content_h;
  UI0S32 index;
  UI0S32 row_y;
  UI0Rect content_clip;
  if (!ctx->input->layout->right_panel_visible || panel.w <= 0) return;
  ctx->right_panel_id = rv_id(300, 0);
  (void)rv_add_surface(ctx, ctx->right_panel_id, 0,
                       UI0ControlKind_PanelSurface,
                       ReaderViewSemantic_Panel,
                       UI0RootKind_Normal,
                       panel, labels.annotations);
  rv_add_text_record_styled(ctx, rv_id(301, 0), ctx->right_panel_id,
                            rv_rect(panel.x + 88, panel.y + 8,
                                    rv_max(panel.w - 134, 1), 28),
                            labels.annotations,
                            ReaderViewTextStyle_ChromeTitle,
                            ReaderViewSemantic_Group,
                            ReaderViewSemantic_Enabled, 0);
  filter_rect = rv_rect(panel.x + 10, panel.y + 10, 24, 24);
  export_rect = rv_rect(panel.x + 44, panel.y + 10, 24, 24);
  close_rect = rv_rect(panel.x + panel.w - 34, panel.y + 10, 24, 24);

  if (rv_add_icon_control(ctx, rv_id(304, 0), ctx->right_panel_id,
                          UI0ControlKind_SelectTrigger,
                          ReaderViewSemantic_Button,
                          UI0RootKind_Normal,
                          filter_rect, filter_rect,
                          labels.filter_annotations,
                          rv_filter_label(labels, state->right_filter),
                          (ReaderViewKey)state->right_filter,
                          right->available_filters != 0,
                          0, 0,
                          state->popup == ReaderViewPopup_RightFilter,
                          0, UI0IconKind_Filter,
                          rv_rect(
                            filter_rect.x +
                              (filter_rect.w - RV_RIGHT_FILTER_ICON_SIZE) / 2,
                            filter_rect.y +
                              (filter_rect.h - RV_RIGHT_FILTER_ICON_SIZE) / 2,
                            RV_RIGHT_FILTER_ICON_SIZE,
                            RV_RIGHT_FILTER_ICON_SIZE), 1))
  {
    state->popup = ReaderViewPopup_RightFilter;
    state->restore_focus_id = rv_id(304, 0);
    state->right_filter_menu_flags = right->available_filters;
    ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged;
  }
  rv_set_semantic_control(ctx, rv_id(304, 0),
                          ReaderViewSemanticControl_RightFilter);
  if (rv_has_feature(ctx->input->projection, ReaderViewFeature_Export) &&
      rv_add_icon_control(ctx, rv_id(302, 0), ctx->right_panel_id,
                          UI0ControlKind_IconButton,
                          ReaderViewSemantic_Button,
                          UI0RootKind_Normal,
                          export_rect, export_rect,
                          labels.export_annotations, rv_text(0, 0), 0,
                          right->status.state == ReaderViewLoad_Ready &&
                          right->row_count > 0,
                          0, 0, 0, 0,
                          UI0IconKind_Upload,
                          rv_toolbar_icon_rect(export_rect), 1))
    (void)rv_add_action(ctx, ReaderViewAction_ExportRightRows, 0, 0,
                        ReaderViewSetting_FontFamily, ReaderViewRightRow_Bookmark,
                        state->right_filter, 0, rv_text(0, 0));
  rv_set_semantic_control(ctx, rv_id(302, 0),
                          ReaderViewSemanticControl_RightExport);
  rv_make_control_nonquiet(ctx, rv_id(302, 0));
  if (rv_add_icon_control(ctx, rv_id(303, 0), ctx->right_panel_id,
                          UI0ControlKind_IconButton,
                          ReaderViewSemantic_Button,
                          UI0RootKind_Normal,
                          close_rect, close_rect,
                          labels.close_annotations, rv_text(0, 0), 0,
                          1, 0, 0, 0, 0,
                          UI0IconKind_Close,
                          rv_toolbar_icon_rect(close_rect), 1))
    rv_close_right_panel(ctx);
  rv_make_control_nonquiet(ctx, rv_id(303, 0));
  rv_set_semantic_control(ctx, rv_id(303, 0),
                          ReaderViewSemanticControl_RightPanelClose);

  list_rect = rv_rect(panel.x + 10,
                      panel.y + RV_RIGHT_LIST_START_Y,
                      rv_max(panel.w - 20, 1),
                      rv_max(panel.h - RV_RIGHT_LIST_START_Y - 10, 0));
  if (right->status.state != ReaderViewLoad_Ready)
  {
    rv_add_status(ctx, ctx->right_panel_id, 320, list_rect, right->status);
    return;
  }
  if (right->row_count == 0)
  {
    UI0ID empty_id = rv_id(323, 0);
    rv_add_text_record(
      ctx, empty_id, ctx->right_panel_id,
      rv_rect(panel.x + 10, panel.y + 88, rv_max(panel.w - 20, 1), 24),
      rv_right_empty_label(labels, state->right_filter),
      ReaderViewSemantic_Status, ReaderViewSemantic_Enabled, 0);
    rv_add_text_override(ctx, empty_id, UI0TextAlignX_Start,
                         UI0TypographyRole_Body,
                         ctx->input->theme->colors[UI0ColorRole_TextMuted]);
    return;
  }
  content_h = rv_right_content_height(right);
  row_y = 0;
  for (index = 0; index < right->row_count; ++index)
  {
    UI0S32 entry_top;
    UI0S32 entry_bottom;
    if (rv_right_row_starts_section(right, index))
      row_y += RV_RIGHT_SECTION_HEIGHT;
    entry_top = row_y;
    entry_bottom = entry_top + RV_RIGHT_ROW_HEIGHT;
    if (ctx->signals.focus_id == rv_id(324, right->rows[index].key) ||
        ctx->signals.focus_id == rv_id(325, right->rows[index].key) ||
        ctx->signals.focus_id == rv_id(326, right->rows[index].key))
    {
      UI0S32 old_scroll_y = state->right_scroll_y;
      if (entry_top < state->right_scroll_y)
        state->right_scroll_y = entry_top;
      else if (entry_bottom > state->right_scroll_y + list_rect.h)
        state->right_scroll_y = rv_max(0, entry_bottom - list_rect.h);
      if (state->right_scroll_y != old_scroll_y)
        ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged;
      break;
    }
    row_y += RV_RIGHT_ROW_HEIGHT;
    if (index + 1 < right->row_count &&
        (right->rows[index + 1].flags &
         ReaderViewRow_AttachedToPrevious) == 0)
      row_y += RV_RIGHT_ROW_GAP;
  }
  scroll_y = rv_hidden_scroll_region(ctx, list_rect, content_h,
                                     &state->right_scroll_y,
                                     &state->right_scroll);
  content_clip = list_rect;
  row_y = list_rect.y - scroll_y;
  for (index = 0; index < right->row_count; ++index)
  {
    const ReaderViewRightRow *row = right->rows + index;
    UI0Rect entry_rect;
    UI0Rect visual_row;
    UI0Rect star_rect;
    UI0Rect menu_rect;
    UI0Rect star_hit;
    UI0Rect menu_hit;
    UI0Rect main_hit;
    UI0Rect main_physical_rect;
    UI0Rect visible_rect;
    UI0ID row_id = rv_id(324, row->key);
    UI0S32 control_index;
    UI0S32 secondary_text_y = 0;
    UI0S32 primary_text_y;
    UI0S32 primary_text_height = 16;
    if (rv_right_row_starts_section(right, index))
    {
      UI0Rect section_rect = rv_rect(list_rect.x, row_y,
                                     list_rect.w,
                                     RV_RIGHT_SECTION_LABEL_HEIGHT);
      if (rv_rect_intersects(section_rect, list_rect))
      {
        rv_add_text_record_styled(ctx, rv_id(321, row->key),
                                  ctx->right_panel_id,
                                  section_rect, row->section,
                                  ReaderViewTextStyle_ChromeTitle,
                                  ReaderViewSemantic_Group,
                                  ReaderViewSemantic_Enabled, row->key);
        {
          UI0ControlRecord *section_record =
            rv_control_record_for_id(ctx, rv_id(321, row->key));
          if (section_record)
            section_record->clip_rect = ui0_rect_intersect(
              section_record->clip_rect, content_clip);
        }
      }
      row_y += RV_RIGHT_SECTION_HEIGHT;
    }
    entry_rect = rv_rect(list_rect.x, row_y,
                         list_rect.w, RV_RIGHT_ROW_HEIGHT);
    visual_row = rv_rect(entry_rect.x + 5, entry_rect.y,
                         rv_max(entry_rect.w - 5, 1), entry_rect.h);
    star_rect = rv_rect(entry_rect.x + entry_rect.w - 58,
                        entry_rect.y + (entry_rect.h - 20) / 2,
                        20, 20);
    menu_rect = rv_rect(entry_rect.x + entry_rect.w - 38,
                        entry_rect.y + (entry_rect.h - 28) / 2,
                        30, 28);
    star_hit = ui0_rect_intersect(star_rect, content_clip);
    menu_hit = ui0_rect_intersect(menu_rect, content_clip);
    main_physical_rect = rv_rect(
      visual_row.x, visual_row.y,
      rv_max(((row->actions & ReaderViewRightAction_ToggleStar) != 0 ?
                star_rect.x : menu_rect.x) - visual_row.x, 1),
      visual_row.h);
    main_hit = ui0_rect_intersect(main_physical_rect, content_clip);
    visible_rect = ui0_rect_intersect(entry_rect, content_clip);
    primary_text_y = visual_row.y + (visual_row.h - primary_text_height) / 2;
    if (row->secondary.size > 0)
    {
      secondary_text_y = visual_row.y +
        (visual_row.h -
         (RV_RIGHT_TEXT_LINE_HEIGHT * 2 + RV_RIGHT_TEXT_STACK_GAP)) / 2 +
        RV_RIGHT_TEXT_BASELINE_OFFSET;
      primary_text_y = secondary_text_y + RV_RIGHT_TEXT_LINE_HEIGHT +
        RV_RIGHT_TEXT_STACK_GAP;
      primary_text_height = RV_RIGHT_TEXT_LINE_HEIGHT;
    }
    row_y += RV_RIGHT_ROW_HEIGHT;
    if (index + 1 < right->row_count &&
        (right->rows[index + 1].flags &
         ReaderViewRow_AttachedToPrevious) == 0)
      row_y += RV_RIGHT_ROW_GAP;
    if (visible_rect.w <= 0 || visible_rect.h <= 0) continue;
    control_index = ctx->control_count;
    if (rv_add_control_with_hit_rect(
          ctx, row_id, ctx->right_panel_id,
          UI0ControlKind_SidenavRow,
          ReaderViewSemantic_ListItem,
          UI0RootKind_Normal,
          visual_row, main_hit,
          row->primary, row->secondary, row->key,
          (row->flags & ReaderViewRow_Enabled) != 0,
          (row->flags & ReaderViewRow_Selected) != 0 ||
            state->active_right_key == row->key,
          0, 0, 0))
    {
      state->active_right_key = row->key;
      if ((row->actions & ReaderViewRightAction_Activate) != 0)
        (void)rv_add_action(ctx, ReaderViewAction_ActivateRightRow,
                            row->key, 0, ReaderViewSetting_FontFamily, row->kind,
                            state->right_filter, 0, rv_text(0, 0));
    }
    if (ctx->control_count > control_index)
    {
      UI0ControlRecord *record =
        ctx->storage->control_records + control_index;
      record->clip_rect = visible_rect;
      record->text_rect = rv_rect(0, 0, 0, 0);
    }
    rv_set_semantic_rect(ctx, row_id, entry_rect);
    rv_set_semantic_control(ctx, row_id,
                            ReaderViewSemanticControl_RightRow);
    if ((row->flags & ReaderViewRow_Current) != 0)
      rv_add_semantic_flags_for_id(ctx, row_id, ReaderViewSemantic_Current);
    if (row->rail_color != 0)
      rv_add_visual_fill(ctx, rv_id(327, row->key),
                         rv_rect(entry_rect.x,
                                 entry_rect.y +
                                   ((row->flags &
                                     ReaderViewRow_AttachedToPrevious) != 0 ?
                                      0 : 2),
                                 4, rv_max(entry_rect.h - 2, 1)),
                         visible_rect,
                         row->rail_color);
    if (row->secondary.size > 0)
    {
      rv_add_text_record_styled(
        ctx, rv_id(328, row->key), row_id,
        rv_rect(visual_row.x + 8, secondary_text_y,
                rv_max(star_rect.x - visual_row.x - 16, 1),
                RV_RIGHT_TEXT_LINE_HEIGHT),
        row->secondary, ReaderViewTextStyle_ChromeMetadata,
        ReaderViewSemantic_Group, ReaderViewSemantic_Enabled, row->key);
      rv_add_text_override(ctx, rv_id(328, row->key),
                           UI0TextAlignX_Start,
                           UI0TypographyRole_Metadata,
                           ctx->input->theme->colors[
                             UI0ColorRole_TextSecondary]);
      {
        UI0ControlRecord *secondary_record =
          rv_control_record_for_id(ctx, rv_id(328, row->key));
        if (secondary_record)
          secondary_record->clip_rect = ui0_rect_intersect(
            secondary_record->clip_rect, visible_rect);
      }
    }
    rv_add_text_record(
      ctx, rv_id(329, row->key), row_id,
      rv_rect(visual_row.x + 8,
              primary_text_y,
              rv_max(star_rect.x - visual_row.x - 16, 1),
              primary_text_height),
      row->primary, ReaderViewSemantic_Group,
      ReaderViewSemantic_Enabled, row->key);
    {
      UI0ControlRecord *primary_record =
        rv_control_record_for_id(ctx, rv_id(329, row->key));
      if (primary_record)
        primary_record->clip_rect = ui0_rect_intersect(
          primary_record->clip_rect, visible_rect);
    }
    if (star_hit.w > 0 && star_hit.h > 0 &&
        (row->actions & ReaderViewRightAction_ToggleStar) != 0 &&
        rv_add_icon_control(ctx, rv_id(325, row->key),
                            ctx->right_panel_id,
                            UI0ControlKind_IconButton,
                            ReaderViewSemantic_ToggleButton,
                            UI0RootKind_Normal,
                            star_rect, star_hit,
                            (row->flags & ReaderViewRow_Starred) ?
                              labels.unstar : labels.star,
                            rv_text(0, 0), row->key, 1, 0,
                            (row->flags & ReaderViewRow_Starred) != 0,
                            0, 0, UI0IconKind_Star,
                            rv_rect(star_rect.x + 3, star_rect.y + 3,
                                    rv_max(star_rect.w - 6, 1),
                                    rv_max(star_rect.h - 6, 1)), 1))
      (void)rv_add_action(ctx, ReaderViewAction_ToggleRightRowStar,
                          row->key, 0, ReaderViewSetting_FontFamily, row->kind,
                          state->right_filter, 0, rv_text(0, 0));
    {
      UI0ControlRecord *star_control =
        rv_control_record_for_id(ctx, rv_id(325, row->key));
      if (star_control)
        star_control->clip_rect = ui0_rect_intersect(
          star_control->clip_rect, visible_rect);
    }
    rv_set_semantic_control(ctx, rv_id(325, row->key),
                            ReaderViewSemanticControl_RightRowStar);
    if (menu_hit.w > 0 && menu_hit.h > 0 &&
        rv_add_control_with_hit_rect(
          ctx, rv_id(326, row->key), ctx->right_panel_id,
          UI0ControlKind_MenuTrigger, ReaderViewSemantic_Button,
          UI0RootKind_Normal,
          menu_rect, menu_hit,
          labels.annotation_actions, rv_text(0, 0), row->key,
          rv_right_action_popup_count(row) > 0,
          0, 0,
          state->popup == ReaderViewPopup_RightRowActions &&
          state->right_menu_key == row->key,
          0))
    {
      state->right_menu_key = row->key;
      state->right_menu_kind = row->kind;
      state->right_menu_actions = row->actions;
      state->popup = ReaderViewPopup_RightRowActions;
      state->restore_focus_id = rv_id(326, row->key);
      ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged;
    }
    {
      UI0ControlRecord *menu_control =
        rv_control_record_for_id(ctx, rv_id(326, row->key));
      if (menu_control)
      {
        menu_control->clip_rect = ui0_rect_intersect(
          menu_control->clip_rect, visible_rect);
        menu_control->text_rect = rv_rect(menu_rect.x + 4,
                                          menu_rect.y,
                                          rv_max(menu_rect.w - 8, 1),
                                          menu_rect.h);
      }
    }
    rv_set_control_visual_text(ctx, rv_id(326, row->key),
                               rv_literal("..."));
    rv_set_semantic_control(ctx, rv_id(326, row->key),
                            ReaderViewSemanticControl_RightRowMenu);
    {
      UI0ControlRecord *row_control =
        rv_control_record_for_id(ctx, row_id);
      UI0ControlRecord *star_control =
        rv_control_record_for_id(ctx, rv_id(325, row->key));
      UI0ControlRecord *menu_control =
        rv_control_record_for_id(ctx, rv_id(326, row->key));
      UI0ControlStateFlags child_visual_state = UI0ControlState_None;
      UI0ControlStateFlags visual_state_mask =
        UI0ControlState_Hovered |
        UI0ControlState_Pressed |
        UI0ControlState_Active;
      if (star_control)
        child_visual_state |= star_control->state & visual_state_mask;
      if (menu_control)
        child_visual_state |= menu_control->state & visual_state_mask;
      if (row_control) row_control->state |= child_visual_state;
    }
  }
}

static UI0Rect
rv_gutter_icon_rect(UI0Rect visual_rect)
{
  UI0S32 width = rv_min(RV_GUTTER_CARET_WIDTH, visual_rect.w);
  UI0S32 height = rv_min(RV_GUTTER_CARET_HEIGHT, visual_rect.h);
  return rv_rect(visual_rect.x + (visual_rect.w - width) / 2,
                 visual_rect.y + (visual_rect.h - height) / 2,
                 width,
                 height);
}

static UI0Rect
rv_note_editor_rect(const ReaderViewBuildInput *input)
{
  UI0Rect bounds;
  UI0Rect body;
  UI0Rect content;
  UI0Rect anchor;
  UI0S32 width;
  UI0S32 height;
  int64_t min_x;
  int64_t max_x;
  int64_t editor_x;
  int64_t min_y;
  int64_t max_y;
  int64_t below_y;
  int64_t above_y;
  int64_t editor_y;
  UI0B32 has_anchor;
  if (!input || !input->layout || !input->projection)
    return rv_rect(0, 0, 0, 0);

  bounds = input->layout->bounds;
  body = input->layout->body_rect;
  content = input->layout->content_rect;
  if (body.w <= 0 || body.h <= 0) body = bounds;
  if (content.w <= 0 || content.h <= 0) content = input->layout->viewport_rect;
  if (content.w <= 0 || content.h <= 0) content = bounds;

  width = rv_min(RV_NOTE_WIDTH,
                 rv_max(380, bounds.w / 2));
  width = rv_min(width, rv_max(bounds.w - RV_INSET * 2, 1));
  height = rv_min(RV_NOTE_HEIGHT, rv_max(body.h, 1));

  anchor = input->projection->selection.anchor_rect;
  has_anchor = anchor.w > 0 && anchor.h > 0 &&
    rv_i64_fits_s32((int64_t)anchor.x + anchor.w) &&
    rv_i64_fits_s32((int64_t)anchor.y + anchor.h);
  if (!has_anchor)
  {
    anchor = rv_rect(bounds.x + bounds.w / 2,
                     body.y + 48, 1, 1);
  }

  min_x = (int64_t)bounds.x + RV_INSET;
  {
    int64_t content_min_x = (int64_t)content.x - 12;
    if (content_min_x > min_x) min_x = content_min_x;
  }
  max_x = (int64_t)bounds.x + bounds.w - width - RV_INSET;
  {
    int64_t content_max_x =
      (int64_t)content.x + content.w - width + 12;
    if (content_max_x < max_x) max_x = content_max_x;
  }
  if (max_x < min_x) max_x = min_x;
  editor_x = (int64_t)anchor.x + anchor.w / 2 - width / 2;
  if (editor_x < min_x) editor_x = min_x;
  if (editor_x > max_x) editor_x = max_x;

  min_y = body.y;
  max_y = (int64_t)body.y + body.h - height;
  if (max_y < min_y) max_y = min_y;
  below_y = (int64_t)anchor.y + anchor.h + RV_NOTE_ANCHOR_GAP;
  above_y = (int64_t)anchor.y - height - RV_NOTE_ANCHOR_GAP;
  editor_y = below_y <= max_y ? below_y :
    (above_y >= min_y ? above_y : below_y);
  if (editor_y < min_y) editor_y = min_y;
  if (editor_y > max_y) editor_y = max_y;

  if (!rv_i64_fits_s32(editor_x) || !rv_i64_fits_s32(editor_y))
    return rv_centered_rect(bounds, width, height);
  return rv_rect((UI0S32)editor_x, (UI0S32)editor_y, width, height);
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
    UI0ControlRecord *record =
      ctx->storage->control_records + control_index;
    UI0ID id = rv_id(tag, 0);
    UI0B32 pointer_over =
      (record->signal_flags & UI0Signal_BlockedByRoot) == 0 &&
      ctx->input->input->ui.pointer_x >= hot_rect.x &&
      ctx->input->input->ui.pointer_y >= hot_rect.y &&
      ctx->input->input->ui.pointer_x < hot_rect.x + hot_rect.w &&
      ctx->input->input->ui.pointer_y < hot_rect.y + hot_rect.h;
    if (pointer_over &&
        (ctx->input->input->ui.flags & UI0Input_PointerPressed) != 0)
    {
      rv_move_focus(ctx, id, 0);
    }
    if ((record->signal_flags & UI0Signal_BlockedByRoot) == 0 &&
        ctx->signals.focus_id == id)
    {
      record->state |= UI0ControlState_Focused;
      rv_add_semantic_flags_for_id(ctx, id, ReaderViewSemantic_Focused);
      if (ctx->signals.focus_visible_id == id)
        record->state |= UI0ControlState_FocusVisible;
    }
    ctx->icons[icon_index].visible =
      pointer_over ||
      (record->state & (UI0ControlState_Hovered |
                        UI0ControlState_Pressed |
                        UI0ControlState_Focused |
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
        UI0IconKind_PageCaretLeft))
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
        UI0IconKind_PageCaretRight))
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
    UI0S32 slider_max = rv_progress_slider_max(slider_count_u64);
    UI0S32 slider_value = rv_progress_slider_value(
      projection->progress.location_index, slider_count_u64);
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
      if (projection->progress.can_seek &&
          ctx->input->state->popup == ReaderViewPopup_None)
        rv_move_focus(ctx, spec.id, 1);
    }
    if (ctx->input->state->pending_accessibility_invoke_id == spec.id)
    {
      ctx->input->state->pending_accessibility_invoke_id = 0;
      if (projection->progress.can_seek &&
          ctx->input->state->popup == ReaderViewPopup_None)
        rv_move_focus(ctx, spec.id, 1);
    }
    spec.keyboard_delta = ctx->signals.focus_id == spec.id ?
      ctx->input->input->move_horizontal_delta : 0;
    if (!projection->progress.can_seek) spec.flags |= UI0Slider_Disabled;
    result = ui0_slider_rect(&ctx->sliders, &ctx->signals, spec);
    {
      UI0B32 blocked =
        (result.signal.flags & UI0Signal_BlockedByRoot) != 0;
    ctx->progress_thumb_visible = projection->progress.can_seek &&
      !blocked &&
      (result.state & (UI0SliderState_Hovered |
                       UI0SliderState_Pressed |
                       UI0SliderState_Active |
                       UI0SliderState_Dragged)) != 0;
    (void)rv_add_semantic(ctx, spec.id, 0, ReaderViewSemantic_Slider,
                          rv_semantic_flags(ctx, spec.id,
                                            projection->progress.can_seek &&
                                              !blocked,
                                            !blocked,
                                            0, 0, 0),
                          slider_rect, projection->progress.label,
                          projection->progress.chapter, 0,
                          projection->progress.location_index, 0,
                          projection->progress.location_count - 1);
    }
    rv_set_semantic_control(ctx, spec.id, ReaderViewSemanticControl_Progress);
    if (result.changed)
    {
      UI0U64 location = rv_progress_location_from_slider(
        result.next_value, slider_count_u64);
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

static UI0B32
rv_source_is_find_row(const RVBuildContext *ctx, UI0ID id)
{
  const ReaderViewFindProjection *find;
  UI0S32 index;
  if (!ctx || !ctx->input || !ctx->input->projection) return 0;
  find = &ctx->input->projection->find;
  for (index = 0; index < find->row_count; ++index)
  {
    if (rv_id(227, find->rows[index].key) == id) return 1;
  }
  return 0;
}

static const ReaderViewRightRow *
rv_right_row_for_draw_source(const RVBuildContext *ctx,
                             UI0ID id,
                             UI0U64 tag)
{
  const ReaderViewRightProjection *right;
  UI0S32 index;
  if (!ctx || !ctx->input || !ctx->input->projection) return 0;
  right = &ctx->input->projection->right;
  for (index = 0; index < right->row_count; ++index)
  {
    if (rv_id(tag, right->rows[index].key) == id)
      return right->rows + index;
  }
  return 0;
}

static UI0B32
rv_point_in_rect(UI0S32 x, UI0S32 y, UI0Rect rect)
{
  return rect.w > 0 && rect.h > 0 &&
         x >= rect.x && y >= rect.y &&
         x < rect.x + rect.w && y < rect.y + rect.h;
}

static UI0B32
rv_gutter_pointer_down(const RVBuildContext *ctx, UI0ID id)
{
  UI0Rect hot_rect;
  const ReaderViewLayout *layout;
  if (!ctx || !ctx->input || !ctx->input->layout ||
      !ctx->input->input ||
      (ctx->input->input->ui.flags & UI0Input_PointerDown) == 0)
    return 0;
  layout = ctx->input->layout;
  if (id == rv_id(400, 0))
    hot_rect = layout->previous_gutter_rect;
  else if (id == rv_id(401, 0))
    hot_rect = layout->next_gutter_rect;
  else
    return 0;
  return rv_point_in_rect(ctx->input->input->ui.pointer_x,
                          ctx->input->input->ui.pointer_y,
                          hot_rect);
}

static void
rv_filter_reference_chrome_draws(RVBuildContext *ctx)
{
  UI0S32 write_index = 0;
  UI0ID previous_id = rv_id(400, 0);
  UI0ID next_id = rv_id(401, 0);
  UI0ID progress_id = rv_id(402, 0);
  UI0ID progress_label_id = rv_id(403, 0);
  UI0ID contents_tab_id = rv_id(201, 0);
  UI0ID find_tab_id = rv_id(202, 0);
  UI0ID left_title_id = rv_id(204, 0);
  UI0ID right_title_id = rv_id(301, 0);
  UI0ID right_filter_id = rv_id(304, 0);
  UI0ID find_input_id = rv_id(220, 0);
  for (UI0S32 read_index = 0;
       read_index < ctx->draw.command_count;
       ++read_index)
  {
    UI0DrawCommand command = ctx->draw.commands[read_index];
    UI0S32 visual_index;
    UI0S32 text_override_index;
    UI0B32 visual_fill = 0;
    UI0Color visual_color = 0;
    const ReaderViewRightRow *right_row =
      rv_right_row_for_draw_source(ctx, command.source_id, 324);
    const ReaderViewRightRow *star_row =
      rv_right_row_for_draw_source(ctx, command.source_id, 325);
    for (visual_index = 0;
         visual_index < ctx->visual_fill_count;
         ++visual_index)
    {
      if (ctx->visual_fills[visual_index].source_id == command.source_id)
      {
        visual_fill = 1;
        visual_color = ctx->visual_fills[visual_index].color;
        break;
      }
    }
    UI0B32 gutter_shell =
      (command.source_id == previous_id || command.source_id == next_id) &&
      (command.op == UI0DrawOp_ControlFill ||
       command.op == UI0DrawOp_ControlBorder ||
       command.op == UI0DrawOp_Text);
    UI0B32 hidden_progress_thumb = command.source_id == progress_id &&
      !ctx->progress_thumb_visible &&
      (command.op == UI0DrawOp_SliderThumb ||
       command.op == UI0DrawOp_FocusRing);
    UI0B32 left_panel_shell = command.source_id == ctx->left_panel_id &&
      (command.op == UI0DrawOp_ControlFill ||
       command.op == UI0DrawOp_ControlBorder);
    UI0B32 star_shell = star_row &&
      (command.op == UI0DrawOp_ControlFill ||
       command.op == UI0DrawOp_ControlBorder);
    UI0B32 right_row_menu_owner = right_row &&
      ctx->input->state->popup == ReaderViewPopup_RightRowActions &&
      ctx->input->state->right_menu_key == right_row->key;
    UI0B32 right_row_shell = right_row &&
      (command.op == UI0DrawOp_ControlBorder ||
       (command.op == UI0DrawOp_ControlFill &&
        !right_row_menu_owner &&
        (command.flags & (UI0DrawFlag_Hovered | UI0DrawFlag_Active)) == 0));
    UI0B32 manual_find_input = command.source_id == find_input_id;
    if (manual_find_input ||
        (command.source_id == right_filter_id &&
         command.op == UI0DrawOp_FocusRing) ||
        gutter_shell || hidden_progress_thumb || left_panel_shell ||
        star_shell || right_row_shell ||
        rv_has_sidenav_visual(ctx, command.source_id) ||
        (visual_fill && command.op != UI0DrawOp_ControlFill) ||
        ((command.source_id == contents_tab_id ||
          command.source_id == find_tab_id) &&
         command.op == UI0DrawOp_SegmentJoin))
      continue;
    if (visual_fill && command.op == UI0DrawOp_ControlFill)
    {
      command.color = visual_color;
      command.stroke_color = visual_color;
      command.flags |= UI0DrawFlag_RadiusExplicit;
      command.corner_radius = 0;
    }
    if (right_row && command.op == UI0DrawOp_ControlFill)
    {
      command.color =
        ctx->input->theme->colors[UI0ColorRole_SurfaceElevated];
      command.stroke_color = command.color;
    }
    if (rv_source_is_find_row(ctx, command.source_id) &&
        command.op == UI0DrawOp_ControlFill)
    {
      command.rect.y += 4;
      command.rect.h = rv_max(command.rect.h - 8, 1);
      command.clip_rect = ui0_rect_intersect(command.clip_rect,
                                              command.rect);
    }
    if (star_row && command.op == UI0DrawOp_Icon)
    {
      command.color =
        (command.flags & UI0DrawFlag_Active) != 0 ?
          ctx->input->theme->colors[UI0ColorRole_TextPrimary] :
        (star_row->flags & ReaderViewRow_Starred) != 0 ?
          ctx->input->theme->colors[UI0ColorRole_Focus] :
          ctx->input->theme->colors[UI0ColorRole_TextMuted];
      command.stroke_color =
        (star_row->flags & ReaderViewRow_Starred) != 0 ?
          ctx->input->theme->colors[UI0ColorRole_Badge] :
          ctx->input->theme->colors[UI0ColorRole_SurfaceElevated];
      /* The icon raster preblends against the visible row/star fill. */
    }
    if ((command.source_id == previous_id ||
         command.source_id == next_id) &&
        command.op == UI0DrawOp_Icon)
    {
      command.color = rv_gutter_pointer_down(ctx, command.source_id) ?
        ctx->input->theme->colors[UI0ColorRole_TextSecondary] :
        ctx->input->theme->colors[UI0ColorRole_TextMuted];
      command.stroke_color =
        ctx->input->theme->colors[UI0ColorRole_Surface];
      command.clip_rect = ctx->input->layout->bounds;
    }
    if ((command.source_id == previous_id ||
         command.source_id == next_id) &&
        command.op == UI0DrawOp_FocusRing)
    {
      UI0Rect visual = command.source_id == previous_id ?
        ctx->input->layout->previous_gutter_visual_rect :
        ctx->input->layout->next_gutter_visual_rect;
      command.rect = rv_rect(visual.x - 2, visual.y - 2,
                             visual.w + 4, visual.h + 4);
      command.clip_rect = ctx->input->layout->bounds;
      command.stroke_width = rv_max(command.stroke_width, 1);
      command.flags |= UI0DrawFlag_RadiusExplicit;
      command.corner_radius = 4;
    }
    if (command.source_id == right_filter_id &&
        command.op == UI0DrawOp_ControlBorder &&
        (command.flags & UI0DrawFlag_Disabled) == 0 &&
        (command.flags & (UI0DrawFlag_Open |
                          UI0DrawFlag_Focused |
                          UI0DrawFlag_FocusVisible |
                          UI0DrawFlag_Active)) != 0)
    {
      command.color = ctx->input->theme->colors[UI0ColorRole_Focus];
    }
    if (command.source_id == progress_label_id &&
        command.op == UI0DrawOp_Text)
    {
      command.color = ctx->input->theme->colors[UI0ColorRole_TextMuted];
    }
    if ((command.source_id == left_title_id ||
         command.source_id == right_title_id) &&
        command.op == UI0DrawOp_Text)
    {
      UI0TypographyToken typography =
        ctx->input->theme->typography[UI0TypographyRole_SectionTitle];
      command.typography_role = UI0TypographyRole_SectionTitle;
      command.has_typography_role = 1;
      command.typography_char_width = typography.char_width;
      command.typography_line_height = typography.line_height;
    }
    if (command.op == UI0DrawOp_Text)
    {
      for (text_override_index = 0;
           text_override_index < ctx->text_override_count;
           ++text_override_index)
      {
        const RVTextOverrideRecord *override =
          ctx->text_overrides + text_override_index;
        if (override->source_id == command.source_id)
        {
          UI0TypographyToken typography =
            ctx->input->theme->typography[override->typography_role];
          command.text_align_x = override->align_x;
          command.text_align_y = UI0TextAlignY_Center;
          command.has_text_alignment = 1;
          command.typography_role = override->typography_role;
          command.has_typography_role = 1;
          command.typography_char_width = typography.char_width;
          command.typography_line_height = typography.line_height;
          command.color = override->color;
          break;
        }
      }
    }
    ctx->draw.commands[write_index++] = command;
  }
  ctx->draw.command_count = write_index;
}

static void
rv_restore_find_caret_clip(RVBuildContext *ctx, UI0S32 draw_start)
{
  UI0ID find_input_id = rv_id(220, 0);
  UI0Rect field_clip = rv_rect(0, 0, 0, 0);
  UI0S32 record_index;
  UI0S32 draw_index;
  if (!ctx || draw_start < 0 || draw_start > ctx->draw.command_count) return;
  for (record_index = 0;
       record_index < ctx->text_inputs.record_count;
       ++record_index)
  {
    const UI0TextInputRecord *record =
      ctx->text_inputs.records + record_index;
    if (record->id == find_input_id)
    {
      field_clip = record->clip_rect;
      break;
    }
  }
  if (field_clip.w <= 0 || field_clip.h <= 0) return;
  for (draw_index = draw_start;
       draw_index < ctx->draw.command_count;
       ++draw_index)
  {
    UI0DrawCommand *command = ctx->draw.commands + draw_index;
    if (command->source_id == find_input_id &&
        command->op == UI0DrawOp_TextCaret)
      command->clip_rect = field_clip;
  }
}

static void
rv_add_note_editor_corner_masks(RVBuildContext *ctx,
                                const UI0TextAreaRecord *record)
{
  UI0Rect corners[4];
  UI0S32 index;
  if (!ctx || !ctx->input || !ctx->input->state || !record ||
      ctx->input->state->popup != ReaderViewPopup_NoteEditor ||
      record->id == 0 || record->rect.w <= 0 || record->rect.h <= 0)
    return;
  corners[0] = rv_rect(record->rect.x, record->rect.y, 1, 1);
  corners[1] = rv_rect(record->rect.x + record->rect.w - 1,
                       record->rect.y, 1, 1);
  corners[2] = rv_rect(record->rect.x,
                       record->rect.y + record->rect.h - 1, 1, 1);
  corners[3] = rv_rect(record->rect.x + record->rect.w - 1,
                       record->rect.y + record->rect.h - 1, 1, 1);
  for (index = 0; index < 4; ++index)
  {
    UI0DrawCommand command;
    memset(&command, 0, sizeof(command));
    command.op = UI0DrawOp_ControlFill;
    command.source_id = record->id;
    command.source_kind = UI0ControlKind_TextArea;
    command.source_index = record->box_index;
    command.rect = corners[index];
    command.clip_rect = record->clip_rect;
    command.color =
      ctx->input->theme->colors[UI0ColorRole_SurfaceElevated];
    command.stroke_color = command.color;
    command.flags = UI0DrawFlag_RadiusExplicit;
    command.corner_radius = 0;
    (void)ui0_draw_push_command(&ctx->draw, command);
  }
}

static void
rv_build_note_editor(RVBuildContext *ctx,
                     UI0Rect modal,
                     ReaderViewLabels labels)
{
  ReaderViewState *state = ctx->input->state;
  const ReaderViewSelectionProjection *selection =
    &ctx->input->projection->selection;
  UI0BoxDesc box_desc;
  UI0TextInputBuffer buffer;
  UI0TextAreaSpec text_spec;
  UI0TextAreaResult text_result;
  UI0Rect editor_rect;
  UI0Rect delete_rect;
  UI0Rect cancel_rect;
  UI0Rect save_rect;
  UI0S32 box_index;
  UI0S32 row_index;
  UI0S32 saved_pointer_y;
  UI0B32 pointer_y_mapped;
  UI0B32 matching;
  UI0B32 editing;
  UI0B32 delete_invoked;
  UI0B32 cancel_invoked;
  UI0B32 save_invoked;
  ReaderViewText title;
  ReaderViewText cancel_visual;
  UI0ID editor_id = rv_id(502, state->note_selection_key);
  UI0ID delete_id = rv_id(505, state->note_selection_key);
  UI0ID cancel_id = rv_id(504, state->note_selection_key);
  UI0ID save_id = rv_id(503, state->note_selection_key);
  ctx->modal_id = rv_id(500, state->note_selection_key);
  matching = state->note_selection_key == selection->selection_key &&
    state->note_source_revision == selection->revision;
  editing = (selection->flags & ReaderViewSelection_CanEditNote) != 0;
  title = editing ? labels.note_title : labels.add_note_title;
  cancel_visual = editing ? labels.close : labels.cancel;

  (void)rv_add_surface(ctx, ctx->modal_id, 0,
                       UI0ControlKind_ModalSurface,
                       ReaderViewSemantic_Dialog,
                       UI0RootKind_Modal,
                       modal, title);
  rv_add_text_record(ctx, rv_id(501, state->note_selection_key),
                     ctx->modal_id,
                     rv_rect(modal.x + 14, modal.y + 14,
                             rv_max(modal.w - 28, 1), 24),
                     title, ReaderViewSemantic_Group,
                     ReaderViewSemantic_Enabled, state->note_selection_key);

  editor_rect = rv_rect(modal.x + 14, modal.y + 50,
                        rv_max(modal.w - 28, 1),
                        rv_max(modal.h - 112, 80));
  delete_rect = rv_rect(modal.x + 14,
                        modal.y + modal.h - 46,
                        74, 30);
  cancel_rect = rv_rect(modal.x + modal.w - 148,
                        modal.y + modal.h - 46,
                        62, 30);
  save_rect = rv_rect(modal.x + modal.w - 76,
                      modal.y + modal.h - 46,
                      62, 30);

  if (state->pending_accessibility_focus_id == editor_id)
  {
    state->pending_accessibility_focus_id = 0;
    rv_move_focus(ctx, editor_id, 1);
  }
  if (state->pending_accessibility_invoke_id == editor_id)
  {
    state->pending_accessibility_invoke_id = 0;
    rv_move_focus(ctx, editor_id, 1);
  }

  box_desc = ui0_box_desc("reader.note_editor.text",
                          UI0LayoutInvalidIndex,
                          UI0Axis_X,
                          ui0_size_fixed(editor_rect.w),
                          ui0_size_fixed(editor_rect.h));
  box_desc.flags = UI0BoxFlag_Clip;
  box_index = ui0_layout_add_box(&ctx->input_layout, &box_desc);
  if (box_index == UI0LayoutInvalidIndex ||
      !ui0_layout_solve(&ctx->input_layout, box_index, editor_rect))
  {
    ctx->frame->error_flags |= ReaderViewFrameError_RecordCap;
    return;
  }

  buffer.data = state->note_draft;
  buffer.length = &state->note_draft_length;
  buffer.cap = READER_VIEW_NOTE_DRAFT_CAP;
  memset(&text_spec, 0, sizeof(text_spec));
  text_spec.id = editor_id;
  text_spec.box_index = box_index;
  text_spec.root = UI0RootKind_Modal;
  text_spec.flags = UI0TextArea_Wrap;
  text_spec.text = state->note_draft;
  text_spec.text_len = state->note_draft_length;
  text_spec.placeholder = labels.note_placeholder.data;
  text_spec.placeholder_len = labels.note_placeholder.size;
  text_spec.frame_input = ctx->input->input->note_text;
  ctx->text_areas.style.wrap_width = rv_max(
    editor_rect.w - ctx->text_areas.style.padding_x * 2, 8);
  saved_pointer_y = ctx->signals.input.pointer_y;
  pointer_y_mapped =
    ctx->signals.input.pointer_x >= editor_rect.x + RV_NOTE_TEXT_PADDING_X &&
    ctx->signals.input.pointer_x <
      editor_rect.x + editor_rect.w - RV_NOTE_TEXT_PADDING_X &&
    saved_pointer_y >= editor_rect.y &&
    saved_pointer_y < editor_rect.y + editor_rect.h;
  if (pointer_y_mapped)
  {
    ctx->signals.input.pointer_y = rv_max(
      editor_rect.y,
      saved_pointer_y -
        (RV_NOTE_TEXT_PADDING_TOP - RV_NOTE_TEXT_LAYOUT_PADDING_Y));
  }
  text_result = ui0_text_area_edit(&ctx->text_areas,
                                   &ctx->signals,
                                   &ctx->input_layout,
                                   text_spec,
                                   &buffer,
                                   &state->note_input);
  ctx->signals.input.pointer_y = saved_pointer_y;
  if (ctx->text_areas.record_count > 0)
  {
    UI0TextAreaRecord *record =
      ctx->text_areas.records + ctx->text_areas.record_count - 1;
    UI0S32 line_height = ctx->input->note_text_metrics.line_height;
    UI0S32 visible_row_cap = rv_max(
      (editor_rect.h - RV_NOTE_TEXT_PADDING_TOP -
       RV_NOTE_TEXT_PADDING_BOTTOM) / line_height,
      1);
    UI0S32 maximum_scroll = rv_max(
      ((record->content_h + line_height - 1) / line_height -
       visible_row_cap) * line_height,
      0);
    UI0S32 quantized_scroll = record->scroll_y <= 0 ? 0 :
      ((record->scroll_y + line_height / 2) / line_height) * line_height;
    UI0S32 geometry_delta;
    UI0S32 original_row_count;
    UI0S32 record_row_index;
    UI0S32 row_write_count;
    UI0S32 row_visual_bottom;
    UI0S32 original_selection_count;
    UI0S32 selection_write_count;
    UI0S32 selection_index;
    quantized_scroll = rv_clamp(quantized_scroll, 0, maximum_scroll);
    geometry_delta =
      RV_NOTE_TEXT_PADDING_TOP - RV_NOTE_TEXT_LAYOUT_PADDING_Y -
      (quantized_scroll - record->scroll_y);
    state->note_input.scroll_y = quantized_scroll;
    record->rect = editor_rect;
    record->clip_rect = editor_rect;
    record->text_rect = rv_rect(
      editor_rect.x + RV_NOTE_TEXT_PADDING_X,
      editor_rect.y + RV_NOTE_TEXT_PADDING_TOP,
      rv_max(editor_rect.w - RV_NOTE_TEXT_PADDING_X * 2, 1),
      rv_max(editor_rect.h - RV_NOTE_TEXT_PADDING_TOP -
             RV_NOTE_TEXT_PADDING_BOTTOM, 1));
    record->scroll_y = quantized_scroll;
    record->scroll_record.scroll_y = quantized_scroll;
    record->scroll_record.max_scroll_y = maximum_scroll;
    original_row_count = record->row_count;
    row_write_count = 0;
    row_visual_bottom = record->text_rect.y + visible_row_cap * line_height;
    for (record_row_index = 0;
         record_row_index < original_row_count;
         ++record_row_index)
    {
      UI0S32 read_index = record->row_start + record_row_index;
      UI0S32 write_index = record->row_start + row_write_count;
      UI0TextAreaRowRecord row_record;
      if (read_index < 0 ||
          read_index >= ctx->text_areas.row_record_count)
        break;
      row_record = ctx->text_areas.row_records[read_index];
      row_record.rect.y += geometry_delta;
      if (row_record.rect.y < record->text_rect.y ||
          row_record.rect.y >= row_visual_bottom ||
          row_write_count >= visible_row_cap)
        continue;
      ctx->text_areas.row_records[write_index] = row_record;
      row_write_count += 1;
    }
    record->row_count = row_write_count;
    record->visible_row_count = row_write_count;
    original_selection_count = record->selection_count;
    selection_write_count = 0;
    for (selection_index = 0;
         selection_index < original_selection_count;
         ++selection_index)
    {
      UI0S32 read_index =
        record->selection_start_record + selection_index;
      UI0S32 write_index =
        record->selection_start_record + selection_write_count;
      UI0TextAreaSelectionRecord *selection_record;
      UI0TextAreaSelectionRecord adjusted;
      if (read_index < 0 ||
          read_index >= ctx->text_areas.selection_record_count)
        break;
      adjusted = ctx->text_areas.selection_records[read_index];
      adjusted.rect.y += geometry_delta + 1;
      adjusted.rect.h = rv_max(line_height - 2, 1);
      if (adjusted.rect.y < record->text_rect.y + 1 ||
          adjusted.rect.y >= row_visual_bottom)
        continue;
      selection_record = ctx->text_areas.selection_records + write_index;
      *selection_record = adjusted;
      selection_record->rect = ui0_rect_intersect(
        selection_record->rect, record->text_rect);
      selection_write_count += 1;
    }
    record->selection_count = selection_write_count;
    if (record->caret_rect.w > 0 && record->caret_rect.h > 0)
    {
      record->caret_rect.y += geometry_delta + 1;
      record->caret_rect.h = rv_max(line_height - 2, 1);
      record->caret_rect = ui0_rect_intersect(record->caret_rect,
                                               record->text_rect);
    }
    if (state->note_draft_length == 0)
    {
      record->state |= UI0TextAreaState_PlaceholderVisible;
      record->placeholder_rect = rv_rect(
        editor_rect.x + RV_NOTE_TEXT_PADDING_X,
        editor_rect.y + RV_NOTE_TEXT_PADDING_BOTTOM,
        rv_max(editor_rect.w - RV_NOTE_TEXT_PADDING_X * 2, 1),
        rv_max(editor_rect.h - RV_NOTE_TEXT_PADDING_BOTTOM * 2, 1));
    }
  }
  if (text_result.edited)
  {
    state->note_dirty = 1;
    ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged;
  }
  if (!matching)
    ctx->frame->error_flags |= ReaderViewFrameError_StaleNoteRevision;
  (void)rv_add_semantic(
    ctx, editor_id, ctx->modal_id,
    ReaderViewSemantic_TextArea,
    rv_semantic_flags(ctx, editor_id,
                      (text_result.state &
                       UI0TextAreaState_BlockedByRoot) == 0,
                      (text_result.state &
                       UI0TextAreaState_BlockedByRoot) == 0,
                      0, 0, 0),
    editor_rect, labels.note_text, reader_view_note_draft(state),
    state->note_selection_key, 0, 0, 0);
  (void)rv_add_binding(ctx, editor_id, labels.note_placeholder,
                       ReaderViewTextStyle_NoteEditor);
  for (row_index = 0;
       ctx->text_areas.record_count > 0 &&
       row_index < ctx->text_areas.records[0].row_count;
       ++row_index)
  {
    UI0S32 storage_row_index =
      ctx->text_areas.records[0].row_start + row_index;
    const UI0TextAreaRowRecord *row =
      ctx->text_areas.row_records + storage_row_index;
    UI0S32 start = rv_clamp(row->byte_start, 0, state->note_draft_length);
    UI0S32 end = rv_clamp(row->byte_end, start, state->note_draft_length);
    (void)rv_add_binding(
      ctx, rv_id(RV_NOTE_TEXT_ROW_ID_BASE + (UI0U64)storage_row_index,
                 state->note_selection_key),
      rv_text(state->note_draft + start, end - start),
      ReaderViewTextStyle_NoteEditor);
  }

  delete_invoked = 0;
  if ((selection->flags & ReaderViewSelection_CanDeleteNote) != 0)
  {
    delete_invoked = rv_add_control(
      ctx, delete_id, ctx->modal_id,
      UI0ControlKind_TextButton, ReaderViewSemantic_Button,
      UI0RootKind_Modal, delete_rect,
      labels.delete_note, rv_text(0, 0), state->note_selection_key,
      matching, 0, 0, 0, 1);
    rv_set_control_visual_text(ctx, delete_id, labels.delete_value);
  }
  if (delete_invoked)
    (void)rv_add_action(ctx, ReaderViewAction_DeleteNote,
                        state->note_selection_key, 0,
                        ReaderViewSetting_FontFamily, ReaderViewRightRow_Note,
                        ReaderViewRightFilter_All,
                        state->note_source_revision, rv_text(0, 0));

  cancel_invoked = rv_add_control(
    ctx, cancel_id, ctx->modal_id,
    UI0ControlKind_TextButton, ReaderViewSemantic_Button,
    UI0RootKind_Modal, cancel_rect,
    labels.cancel_note, rv_text(0, 0), state->note_selection_key,
    1, 0, 0, 0, 0);
  rv_make_control_quiet(ctx, cancel_id);
  rv_set_control_visual_text(ctx, cancel_id, cancel_visual);
  if (cancel_invoked)
  {
    (void)rv_add_action(ctx, ReaderViewAction_CancelNote,
                        state->note_selection_key, 0,
                        ReaderViewSetting_FontFamily, ReaderViewRightRow_Note,
                        ReaderViewRightFilter_All,
                        state->note_source_revision, rv_text(0, 0));
    state->note_dirty = 0;
    rv_close_popup_and_restore_focus(ctx);
  }

  save_invoked = rv_add_control(
    ctx, save_id, ctx->modal_id,
    UI0ControlKind_TextButton, ReaderViewSemantic_Button,
    UI0RootKind_Modal, save_rect,
    labels.save_note, rv_text(0, 0), state->note_selection_key,
    matching, 0, 0, 0, 0);
  rv_make_control_primary(ctx, save_id);
  rv_set_control_visual_text(ctx, save_id, labels.save);
  if (save_invoked)
    (void)rv_add_action(ctx, ReaderViewAction_SaveNote,
                        state->note_selection_key, 0,
                        ReaderViewSetting_FontFamily, ReaderViewRightRow_Note,
                        ReaderViewRightFilter_All,
                        state->note_source_revision,
                        reader_view_note_draft(state));
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
  rv_move_focus(ctx, nodes[target].id, 1);
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
                       popup,
                       root_popup == ReaderViewPopup_RightFilter ?
                         labels.annotation_filters :
                       root_popup == ReaderViewPopup_RightRowActions ?
                         labels.annotation_actions : labels.more);
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
    case ReaderViewPopup_RightFilter:
      rv_build_right_filter_popup(ctx, popup, labels);
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

typedef struct RVPopupItemSet
{
  UI0ID ids[READER_VIEW_POPUP_ITEM_SNAPSHOT_CAP];
  UI0S32 count;
} RVPopupItemSet;

static void
rv_popup_item_append(RVPopupItemSet *set, UI0ID id)
{
  if (!set || id == 0 ||
      set->count >= READER_VIEW_POPUP_ITEM_SNAPSHOT_CAP)
    return;
  set->ids[set->count++] = id;
}

static UI0B32
rv_popup_item_contains(const RVPopupItemSet *set, UI0ID id)
{
  UI0S32 index;
  if (!set || id == 0) return 0;
  for (index = 0; index < set->count; ++index)
    if (set->ids[index] == id) return 1;
  return 0;
}

static RVPopupItemSet
rv_popup_items(const ReaderViewBuildInput *input, UI0B32 focusable_only)
{
  RVPopupItemSet result;
  const ReaderViewProjection *projection;
  const ReaderViewState *state;
  UI0S32 index;
  memset(&result, 0, sizeof(result));
  if (!input || !input->projection || !input->state) return result;
  projection = input->projection;
  state = input->state;
  switch (state->popup)
  {
    case ReaderViewPopup_SettingMenu:
    {
      const ReaderViewSettingControl *setting = rv_find_setting(
        projection, state->active_setting_kind);
      UI0S32 count = setting ? setting->choices.count : 0;
      if (setting && setting->kind == ReaderViewSetting_FontFamily)
        count = rv_min(count, RV_FONT_POPUP_CHOICE_CAP);
      for (index = 0; setting && index < count; ++index)
      {
        const ReaderViewChoice *choice = setting->choices.items + index;
        if (!focusable_only ||
            (choice->flags & ReaderViewChoice_Enabled) != 0)
          rv_popup_item_append(&result, rv_id(100, choice->key));
      }
    } break;

    case ReaderViewPopup_Overflow:
    {
      UI0S32 row = 0;
      if (rv_has_feature(projection, ReaderViewFeature_History))
      {
        if (!focusable_only || rv_has_document_flag(
              projection, ReaderViewDocument_CanGoBack))
          rv_popup_item_append(&result, rv_id(110, 0));
        row += 1;
        if (!focusable_only || rv_has_document_flag(
              projection, ReaderViewDocument_CanGoForward))
          rv_popup_item_append(&result, rv_id(111, 0));
        row += 1;
      }
      if (rv_has_feature(projection, ReaderViewFeature_ReadingSettings))
      {
        for (index = 0; index < projection->settings.count && row < 7;
             ++index, ++row)
        {
          const ReaderViewSettingControl *setting =
            projection->settings.items + index;
          if (!focusable_only ||
              setting->status.state == ReaderViewLoad_Ready)
            rv_popup_item_append(
              &result, rv_id(120 + (UI0U64)setting->kind, 0));
        }
      }
      if (rv_has_feature(projection, ReaderViewFeature_Annotations) && row < 7)
      {
        rv_popup_item_append(&result, rv_id(130, 0));
        row += 1;
      }
      if (rv_has_feature(projection, ReaderViewFeature_Bookmark) && row < 7)
      {
        if (!focusable_only || rv_has_document_flag(
              projection, ReaderViewDocument_Open))
          rv_popup_item_append(&result, rv_id(131, 0));
        row += 1;
      }
      if (rv_has_feature(projection, ReaderViewFeature_Fullscreen) && row < 7)
      {
        if (!focusable_only || rv_has_document_flag(
              projection, ReaderViewDocument_CanToggleFullscreen))
          rv_popup_item_append(&result, rv_id(132, 0));
        row += 1;
      }
      if (rv_has_feature(projection, ReaderViewFeature_DistractionFree) &&
          row < 7)
      {
        if (!focusable_only || rv_has_document_flag(
              projection, ReaderViewDocument_CanToggleDistraction))
          rv_popup_item_append(&result, rv_id(133, 0));
      }
    } break;

    case ReaderViewPopup_RightFilter:
    {
      static const ReaderViewRightFilter order[] = {
        ReaderViewRightFilter_All,
        ReaderViewRightFilter_Highlights,
        ReaderViewRightFilter_Notes,
        ReaderViewRightFilter_Bookmarks,
      };
      for (index = 0; index < (UI0S32)(sizeof(order) / sizeof(order[0]));
           ++index)
      {
        ReaderViewRightFilter value = order[index];
        if ((projection->right.available_filters & rv_filter_flag(value)) != 0)
          rv_popup_item_append(&result, rv_id(135, (ReaderViewKey)value));
      }
    } break;

    case ReaderViewPopup_RightRowActions:
    {
      const ReaderViewRightRow *row = rv_find_right_row(
        projection, state->right_menu_key);
      if (row && (row->actions & ReaderViewRightAction_Activate) != 0)
        rv_popup_item_append(&result, rv_id(140, row->key));
      if (row && row->kind == ReaderViewRightRow_Highlight &&
          (row->actions & ReaderViewRightAction_ToggleStar) != 0)
        rv_popup_item_append(&result, rv_id(141, row->key));
      if (row && row->kind == ReaderViewRightRow_Note &&
          (row->actions & ReaderViewRightAction_EditNote) != 0)
        rv_popup_item_append(&result, rv_id(142, row->key));
      if (row && (row->actions & ReaderViewRightAction_Delete) != 0)
        rv_popup_item_append(&result, rv_id(143, row->key));
    } break;

    case ReaderViewPopup_SelectionTools:
    {
      const ReaderViewSelectionProjection *selection = &projection->selection;
      ReaderViewSelectionFlags flags = selection->flags;
      if ((flags & ReaderViewSelection_CanCopy) != 0)
        rv_popup_item_append(&result, rv_id(150, selection->selection_key));
      if ((flags & (ReaderViewSelection_CanAddNote |
                    ReaderViewSelection_CanEditNote)) != 0)
        rv_popup_item_append(&result, rv_id(151, selection->selection_key));
      if ((flags & ReaderViewSelection_CanDictionary) != 0)
        rv_popup_item_append(&result, rv_id(152, selection->selection_key));
      if ((flags & ReaderViewSelection_CanWebLookup) != 0)
        rv_popup_item_append(&result, rv_id(153, selection->selection_key));
      if ((flags & ReaderViewSelection_CanTranslate) != 0)
        rv_popup_item_append(&result, rv_id(154, selection->selection_key));
      if ((flags & ReaderViewSelection_CanRemoveHighlight) != 0)
        rv_popup_item_append(&result, rv_id(155, selection->selection_key));
      for (index = 0;
           index < selection->highlight_colors.count && index < 4;
           ++index)
      {
        const ReaderViewChoice *color =
          selection->highlight_colors.items + index;
        if (!focusable_only ||
            (color->flags & ReaderViewChoice_Enabled) != 0)
          rv_popup_item_append(&result, rv_id(160, color->key));
      }
    } break;

    case ReaderViewPopup_NoteEditor:
    {
      const ReaderViewSelectionProjection *selection = &projection->selection;
      UI0B32 matching = state->note_selection_key == selection->selection_key &&
        state->note_source_revision == selection->revision;
      rv_popup_item_append(&result, rv_id(502, state->note_selection_key));
      if ((selection->flags & ReaderViewSelection_CanDeleteNote) != 0 &&
          (!focusable_only || matching))
        rv_popup_item_append(&result,
                             rv_id(505, state->note_selection_key));
      rv_popup_item_append(&result, rv_id(504, state->note_selection_key));
      if (!focusable_only || matching)
        rv_popup_item_append(&result, rv_id(503, state->note_selection_key));
    } break;

    default:
      break;
  }
  return result;
}

static UI0ID
rv_first_popup_focus_id(const ReaderViewBuildInput *input,
                        const RVPopupItemSet *focusable)
{
  const ReaderViewState *state;
  UI0S32 index;
  if (!input || !input->state || !input->projection ||
      !focusable || focusable->count == 0)
    return 0;
  state = input->state;
  if (state->popup == ReaderViewPopup_SettingMenu)
  {
    const ReaderViewSettingControl *setting = rv_find_setting(
      input->projection, state->active_setting_kind);
    UI0S32 count = setting ? setting->choices.count : 0;
    if (setting && setting->kind == ReaderViewSetting_FontFamily)
      count = rv_min(count, RV_FONT_POPUP_CHOICE_CAP);
    for (index = 0; setting && index < count; ++index)
    {
      const ReaderViewChoice *choice = setting->choices.items + index;
      UI0ID id = rv_id(100, choice->key);
      if ((choice->flags & ReaderViewChoice_Selected) != 0 &&
          rv_popup_item_contains(focusable, id))
        return id;
    }
  }
  if (state->popup == ReaderViewPopup_RightFilter)
  {
    UI0ID selected = rv_id(135, (ReaderViewKey)state->right_filter);
    if (rv_popup_item_contains(focusable, selected)) return selected;
  }
  return focusable->ids[0];
}

static UI0B32
rv_prior_row_id(const ReaderViewKey *keys,
                UI0S32 count,
                UI0U64 tag,
                UI0ID id,
                ReaderViewKey *out_key)
{
  UI0S32 index;
  if (out_key) *out_key = 0;
  if (!keys || count < 0 || id == 0) return 0;
  for (index = 0; index < count; ++index)
  {
    if (rv_id(tag, keys[index]) == id)
    {
      if (out_key) *out_key = keys[index];
      return 1;
    }
  }
  return 0;
}

static const ReaderViewTocRow *
rv_current_toc_row(const ReaderViewBuildInput *input, ReaderViewKey key)
{
  UI0S32 index;
  if (!input || !input->state || !input->layout || !input->projection ||
      key == 0 || !input->layout->left_panel_visible ||
      input->state->left_panel != ReaderViewLeftPanel_Contents ||
      input->projection->toc.status.state != ReaderViewLoad_Ready)
    return 0;
  for (index = 0; index < input->projection->toc.row_count; ++index)
    if (input->projection->toc.rows[index].key == key)
      return input->projection->toc.rows + index;
  return 0;
}

static const ReaderViewFindRow *
rv_current_find_row(const ReaderViewBuildInput *input, ReaderViewKey key)
{
  UI0S32 index;
  if (!input || !input->state || !input->layout || !input->projection ||
      key == 0 || !input->layout->left_panel_visible ||
      input->state->left_panel != ReaderViewLeftPanel_Find ||
      input->projection->find.status.state != ReaderViewLoad_Ready)
    return 0;
  for (index = 0; index < input->projection->find.row_count; ++index)
    if (input->projection->find.rows[index].key == key)
      return input->projection->find.rows + index;
  return 0;
}

static const ReaderViewRightRow *
rv_current_right_row(const ReaderViewBuildInput *input, ReaderViewKey key)
{
  const ReaderViewRightRow *row;
  if (!input || !input->state || !input->layout || !input->projection ||
      key == 0 || !input->layout->right_panel_visible ||
      input->projection->right.status.state != ReaderViewLoad_Ready)
    return 0;
  row = rv_find_right_row(input->projection, key);
  return row;
}

static ReaderViewKey
rv_default_toc_key(const ReaderViewBuildInput *input)
{
  const ReaderViewTocProjection *toc;
  UI0S32 index;
  if (!input || !input->projection || !input->layout || !input->state ||
      !input->layout->left_panel_visible ||
      input->state->left_panel != ReaderViewLeftPanel_Contents)
    return 0;
  toc = &input->projection->toc;
  if (toc->status.state != ReaderViewLoad_Ready) return 0;
  for (index = 0; index < toc->row_count; ++index)
    if ((toc->rows[index].flags & ReaderViewRow_Enabled) != 0 &&
        (toc->rows[index].flags & (ReaderViewRow_Current |
                                   ReaderViewRow_Selected)) != 0)
      return toc->rows[index].key;
  for (index = 0; index < toc->row_count; ++index)
    if ((toc->rows[index].flags & ReaderViewRow_Enabled) != 0)
      return toc->rows[index].key;
  return 0;
}

static ReaderViewKey
rv_default_find_key(const ReaderViewBuildInput *input)
{
  const ReaderViewFindProjection *find;
  UI0S32 index;
  if (!input || !input->projection || !input->layout || !input->state ||
      !input->layout->left_panel_visible ||
      input->state->left_panel != ReaderViewLeftPanel_Find)
    return 0;
  find = &input->projection->find;
  if (find->status.state != ReaderViewLoad_Ready) return 0;
  if (find->active_index >= 0 && find->active_index < find->row_count &&
      (find->rows[find->active_index].flags & ReaderViewRow_Enabled) != 0)
    return find->rows[find->active_index].key;
  for (index = 0; index < find->row_count; ++index)
    if ((find->rows[index].flags & (ReaderViewRow_Enabled |
                                    ReaderViewRow_Selected)) ==
        (ReaderViewRow_Enabled | ReaderViewRow_Selected))
      return find->rows[index].key;
  for (index = 0; index < find->row_count; ++index)
    if ((find->rows[index].flags & ReaderViewRow_Enabled) != 0)
      return find->rows[index].key;
  return 0;
}

static void
rv_state_rehome_focus(ReaderViewState *state,
                      ReaderViewFrame *frame,
                      UI0ID target)
{
  if (!state || !frame || state->focus_id == target) return;
  state->focus_id = target;
  if (target == 0) state->focus_visible = 0;
  frame->change_flags |= ReaderViewFrameChange_FocusChanged;
}

static void
rv_reconcile_dynamic_rows(const ReaderViewBuildInput *input,
                          ReaderViewFrame *frame)
{
  ReaderViewState *state;
  ReaderViewKey key;
  ReaderViewKey target_key;
  UI0ID target_id;
  UI0ID *ids[5];
  UI0S32 id_index;
  if (!input || !input->state || !input->projection || !frame) return;
  state = input->state;

  target_key = rv_default_toc_key(input);
  if (state->active_toc_key != 0 &&
      (!rv_current_toc_row(input, state->active_toc_key) ||
       (rv_current_toc_row(input, state->active_toc_key)->flags &
        ReaderViewRow_Enabled) == 0))
  {
    state->active_toc_key = target_key;
    frame->change_flags |= ReaderViewFrameChange_StateChanged;
  }
  ids[0] = &state->focus_id;
  ids[1] = &state->hot_id;
  ids[2] = &state->active_id;
  ids[3] = &state->pending_accessibility_focus_id;
  ids[4] = &state->pending_accessibility_invoke_id;
  for (id_index = 0; id_index < 5; ++id_index)
  {
    if (rv_prior_row_id(state->prior_toc_row_keys,
                        state->prior_toc_row_count, 212,
                        *ids[id_index], &key) &&
        (!rv_current_toc_row(input, key) ||
         (rv_current_toc_row(input, key)->flags & ReaderViewRow_Enabled) == 0))
    {
      if (id_index == 0)
      {
        target_id = target_key ? rv_id(212, target_key) :
          (input->layout->left_panel_visible &&
           state->left_panel == ReaderViewLeftPanel_Contents ?
             rv_id(201, 0) : state->left_panel_restore_focus_id);
        rv_state_rehome_focus(state, frame, target_id);
      }
      else
        *ids[id_index] = 0;
      frame->change_flags |= ReaderViewFrameChange_StateChanged;
    }
  }

  target_key = rv_default_find_key(input);
  if (state->active_find_key != 0 &&
      (!rv_current_find_row(input, state->active_find_key) ||
       (rv_current_find_row(input, state->active_find_key)->flags &
        ReaderViewRow_Enabled) == 0))
  {
    state->active_find_key = target_key;
    frame->change_flags |= ReaderViewFrameChange_StateChanged;
  }
  for (id_index = 0; id_index < 5; ++id_index)
  {
    if (rv_prior_row_id(state->prior_find_row_keys,
                        state->prior_find_row_count, 227,
                        *ids[id_index], &key) &&
        (!rv_current_find_row(input, key) ||
         (rv_current_find_row(input, key)->flags & ReaderViewRow_Enabled) == 0))
    {
      if (id_index == 0)
      {
        target_id = input->layout->left_panel_visible &&
          state->left_panel == ReaderViewLeftPanel_Find ?
            rv_id(220, 0) : state->left_panel_restore_focus_id;
        rv_state_rehome_focus(state, frame, target_id);
      }
      else
        *ids[id_index] = 0;
      frame->change_flags |= ReaderViewFrameChange_StateChanged;
    }
  }

  if (state->active_right_key != 0 &&
      !rv_current_right_row(input, state->active_right_key))
  {
    state->active_right_key = 0;
    frame->change_flags |= ReaderViewFrameChange_StateChanged;
  }
  for (id_index = 0; id_index < 5; ++id_index)
  {
    UI0B32 prior_right =
      rv_prior_row_id(state->prior_right_row_keys,
                      state->prior_right_row_count, 324,
                      *ids[id_index], &key) ||
      rv_prior_row_id(state->prior_right_row_keys,
                      state->prior_right_row_count, 325,
                      *ids[id_index], &key) ||
      rv_prior_row_id(state->prior_right_row_keys,
                      state->prior_right_row_count, 326,
                      *ids[id_index], &key);
    const ReaderViewRightRow *row = prior_right ?
      rv_current_right_row(input, key) : 0;
    UI0B32 current = row != 0;
    if (current && *ids[id_index] == rv_id(324, key))
      current = (row->flags & ReaderViewRow_Enabled) != 0;
    else if (current && *ids[id_index] == rv_id(325, key))
      current = (row->actions & ReaderViewRightAction_ToggleStar) != 0;
    else if (current && *ids[id_index] == rv_id(326, key))
      current = rv_right_action_popup_count(row) > 0;
    if (prior_right && !current)
    {
      if (id_index == 0)
      {
        target_id = 0;
        rv_state_rehome_focus(state, frame, target_id);
      }
      else
        *ids[id_index] = 0;
      frame->change_flags |= ReaderViewFrameChange_StateChanged;
    }
  }
}

static UI0B32
rv_left_fixed_id(UI0ID id)
{
  return id != 0 &&
    (id == rv_id(201, 0) || id == rv_id(202, 0) ||
     id == rv_id(203, 0) || id == rv_id(220, 0) ||
     id == rv_id(221, 0));
}

static UI0B32
rv_right_fixed_id(UI0ID id)
{
  return id != 0 &&
    (id == rv_id(302, 0) || id == rv_id(303, 0) ||
     id == rv_id(304, 0));
}

static UI0B32
rv_prior_left_row_id(const ReaderViewState *state, UI0ID id)
{
  return state &&
    (rv_prior_row_id(state->prior_toc_row_keys,
                     state->prior_toc_row_count, 212, id, 0) ||
     rv_prior_row_id(state->prior_find_row_keys,
                     state->prior_find_row_count, 227, id, 0));
}

static UI0B32
rv_prior_right_row_id(const ReaderViewState *state, UI0ID id)
{
  return state &&
    (rv_prior_row_id(state->prior_right_row_keys,
                     state->prior_right_row_count, 324, id, 0) ||
     rv_prior_row_id(state->prior_right_row_keys,
                     state->prior_right_row_count, 325, id, 0) ||
     rv_prior_row_id(state->prior_right_row_keys,
                     state->prior_right_row_count, 326, id, 0));
}

static void
rv_reconcile_feature_owners(const ReaderViewBuildInput *input,
                            ReaderViewFrame *frame)
{
  ReaderViewState *state;
  UI0ID *ids[5];
  UI0S32 index;
  UI0B32 left_supported;
  UI0B32 right_supported;
  if (!input || !input->state || !input->projection || !frame) return;
  state = input->state;
  if (!rv_has_document_flag(input->projection, ReaderViewDocument_Open))
    return;
  ids[0] = &state->focus_id;
  ids[1] = &state->hot_id;
  ids[2] = &state->active_id;
  ids[3] = &state->pending_accessibility_focus_id;
  ids[4] = &state->pending_accessibility_invoke_id;
  left_supported =
    state->left_panel == ReaderViewLeftPanel_None ||
    (state->left_panel == ReaderViewLeftPanel_Contents &&
     rv_has_feature(input->projection, ReaderViewFeature_Contents)) ||
    (state->left_panel == ReaderViewLeftPanel_Find &&
     rv_has_feature(input->projection, ReaderViewFeature_Find));
  if (!left_supported)
  {
    for (index = 0; index < 5; ++index)
    {
      if (rv_left_fixed_id(*ids[index]) ||
          rv_prior_left_row_id(state, *ids[index]))
      {
        if (index == 0)
          rv_state_rehome_focus(state, frame, 0);
        else
          *ids[index] = 0;
      }
    }
    state->left_panel = ReaderViewLeftPanel_None;
    state->pending_left_panel_focus = ReaderViewLeftPanel_None;
    state->left_panel_restore_focus_id = 0;
    state->active_toc_key = 0;
    state->active_find_key = 0;
    frame->change_flags |= ReaderViewFrameChange_StateChanged |
                           ReaderViewFrameChange_LayoutChanged;
  }

  right_supported = !state->right_panel_open ||
    rv_has_feature(input->projection, ReaderViewFeature_Annotations);
  if (!right_supported)
  {
    for (index = 0; index < 5; ++index)
    {
      if (rv_right_fixed_id(*ids[index]) ||
          rv_prior_right_row_id(state, *ids[index]))
      {
        if (index == 0)
          rv_state_rehome_focus(state, frame, 0);
        else
          *ids[index] = 0;
      }
    }
    state->right_panel_open = 0;
    state->right_panel_restore_focus_id = 0;
    state->active_right_key = 0;
    frame->change_flags |= ReaderViewFrameChange_StateChanged |
                           ReaderViewFrameChange_LayoutChanged;
  }
}

static void
rv_reset_scroll_interaction(UI0ScrollState *scroll,
                            ReaderViewFrame *frame)
{
  if (!scroll || !frame ||
      (scroll->active_thumb_id == 0 &&
       scroll->drag_start_pointer_y == 0 &&
       scroll->drag_start_scroll_y == 0))
    return;
  memset(scroll, 0, sizeof(*scroll));
  frame->change_flags |= ReaderViewFrameChange_StateChanged;
}

static void
rv_reconcile_scroll_owners(const ReaderViewBuildInput *input,
                           ReaderViewFrame *frame)
{
  ReaderViewState *state;
  UI0B32 toc_owner;
  UI0B32 find_owner;
  UI0B32 right_owner;
  if (!input || !input->state || !input->layout ||
      !input->projection || !frame)
    return;
  state = input->state;
  toc_owner = input->layout->left_panel_visible &&
    state->left_panel == ReaderViewLeftPanel_Contents &&
    input->projection->toc.status.state == ReaderViewLoad_Ready &&
    input->projection->toc.row_count > 0;
  find_owner = input->layout->left_panel_visible &&
    state->left_panel == ReaderViewLeftPanel_Find &&
    input->projection->find.status.state == ReaderViewLoad_Ready &&
    input->projection->find.row_count > 0;
  right_owner = input->layout->right_panel_visible &&
    state->right_panel_open &&
    input->projection->right.status.state == ReaderViewLoad_Ready &&
    input->projection->right.row_count > 0;
  if (!toc_owner) rv_reset_scroll_interaction(&state->toc_scroll, frame);
  if (!find_owner) rv_reset_scroll_interaction(&state->find_scroll, frame);
  if (!right_owner) rv_reset_scroll_interaction(&state->right_scroll, frame);
}

static UI0B32
rv_popup_owner_available(const ReaderViewBuildInput *input,
                         const RVPopupItemSet *present,
                         const RVPopupItemSet *focusable)
{
  const ReaderViewState *state;
  const ReaderViewSettingControl *setting;
  UI0Rect menu_rect;
  if (!input || !input->state || !input->layout || !input->projection ||
      !present || !focusable || present->count == 0 || focusable->count == 0)
    return 0;
  state = input->state;
  switch (state->popup)
  {
    case ReaderViewPopup_SettingMenu:
      setting = rv_find_setting(input->projection,
                                state->active_setting_kind);
      return input->layout->toolbar_visible &&
        input->layout->shared_toolbar_rect.w > 0 &&
        rv_has_feature(input->projection,
                       ReaderViewFeature_ReadingSettings) &&
        setting && setting->status.state == ReaderViewLoad_Ready;
    case ReaderViewPopup_Overflow:
      return input->layout->toolbar_visible &&
        input->layout->shared_toolbar_rect.w > 0;
    case ReaderViewPopup_RightFilter:
      return input->layout->right_panel_visible &&
        input->projection->right.status.state == ReaderViewLoad_Ready &&
        input->projection->right.available_filters != 0;
    case ReaderViewPopup_RightRowActions:
      return input->layout->right_panel_visible &&
        input->projection->right.status.state == ReaderViewLoad_Ready &&
        rv_right_row_menu_rect(input, state->right_menu_key, &menu_rect);
    case ReaderViewPopup_SelectionTools:
      return rv_has_feature(input->projection,
                            ReaderViewFeature_SelectionTools) &&
        input->projection->selection.status.state ==
               ReaderViewLoad_Ready &&
        (input->projection->selection.flags & ReaderViewSelection_Active) != 0 &&
        input->projection->selection.selection_key != 0;
    case ReaderViewPopup_NoteEditor:
      return state->note_selection_key != 0;
    default:
      return 0;
  }
}

static void
rv_reconcile_popup_state(const ReaderViewBuildInput *input,
                         ReaderViewFrame *frame)
{
  ReaderViewState *state;
  RVPopupItemSet present;
  RVPopupItemSet focusable;
  UI0ID old_focus;
  UI0ID target;
  UI0S32 index;
  if (!input || !input->state || !frame) return;
  state = input->state;
  old_focus = state->focus_id;
  if (state->popup == ReaderViewPopup_None)
  {
    if (state->prior_popup_item_count > 0)
    {
      rv_clear_prior_popup_interaction(state, state->restore_focus_id);
      state->restore_focus_id = 0;
      frame->change_flags |= ReaderViewFrameChange_StateChanged;
      if (state->focus_id != old_focus)
        frame->change_flags |= ReaderViewFrameChange_FocusChanged;
    }
    return;
  }

  present = rv_popup_items(input, 0);
  focusable = rv_popup_items(input, 1);
  if (state->prior_popup_kind != ReaderViewPopup_None &&
      state->prior_popup_kind != state->popup)
  {
    rv_clear_prior_popup_interaction(state, state->restore_focus_id);
    frame->change_flags |= ReaderViewFrameChange_StateChanged;
  }

  if (!rv_popup_owner_available(input, &present, &focusable))
  {
    target = 0;
    if (rv_popup_item_contains(&present, state->focus_id))
      rv_state_rehome_focus(state, frame, target);
    rv_clear_prior_popup_interaction(state, target);
    if (state->popup == ReaderViewPopup_SelectionTools)
      state->dismissed_selection_key = state->last_selection_key;
    if (state->popup == ReaderViewPopup_RightFilter)
      state->right_filter_menu_flags = 0;
    if (state->popup == ReaderViewPopup_RightRowActions)
    {
      state->right_menu_key = 0;
      state->right_menu_actions = ReaderViewRightAction_None;
    }
    state->popup = ReaderViewPopup_None;
    state->restore_focus_id = 0;
    frame->change_flags |= ReaderViewFrameChange_StateChanged;
    if (state->focus_id != old_focus)
      frame->change_flags |= ReaderViewFrameChange_FocusChanged;
    return;
  }

  for (index = 0; index < state->prior_popup_item_count; ++index)
  {
    UI0ID prior_id = state->prior_popup_item_ids[index];
    if (rv_popup_item_contains(&present, prior_id)) continue;
    if (state->hot_id == prior_id) state->hot_id = 0;
    if (state->active_id == prior_id) state->active_id = 0;
    if (state->pending_accessibility_focus_id == prior_id)
      state->pending_accessibility_focus_id = 0;
    if (state->pending_accessibility_invoke_id == prior_id)
      state->pending_accessibility_invoke_id = 0;
  }
  if (!rv_popup_item_contains(&focusable, state->focus_id))
  {
    target = rv_first_popup_focus_id(input, &focusable);
    rv_state_rehome_focus(state, frame, target);
    if (state->popup == ReaderViewPopup_NoteEditor && target != 0)
      state->focus_visible = 1;
  }
  if (state->focus_id != old_focus)
    frame->change_flags |= ReaderViewFrameChange_FocusChanged;
}

static void
rv_capture_state_snapshots(RVBuildContext *ctx,
                           ReaderViewPopupKind built_popup)
{
  ReaderViewState *state;
  const ReaderViewProjection *projection;
  UI0S32 index;
  if (!ctx || !ctx->input || !ctx->input->state ||
      !ctx->input->projection)
    return;
  state = ctx->input->state;
  projection = ctx->input->projection;
  state->prior_toc_row_count = 0;
  for (index = 0; index < projection->toc.row_count; ++index)
  {
    ReaderViewKey key = projection->toc.rows[index].key;
    if (state->prior_toc_row_count < READER_VIEW_TOC_ROW_CAP)
      state->prior_toc_row_keys[state->prior_toc_row_count++] = key;
  }
  state->prior_find_row_count = 0;
  for (index = 0; index < projection->find.row_count; ++index)
  {
    ReaderViewKey key = projection->find.rows[index].key;
    if (state->prior_find_row_count < READER_VIEW_FIND_ROW_CAP)
      state->prior_find_row_keys[state->prior_find_row_count++] = key;
  }
  state->prior_right_row_count = 0;
  for (index = 0; index < projection->right.row_count; ++index)
  {
    ReaderViewKey key = projection->right.rows[index].key;
    if (state->prior_right_row_count < READER_VIEW_RIGHT_ROW_CAP)
      state->prior_right_row_keys[state->prior_right_row_count++] = key;
  }

  state->prior_popup_item_count = 0;
  state->prior_popup_kind = ReaderViewPopup_None;
  if (built_popup != ReaderViewPopup_None &&
      state->popup != ReaderViewPopup_None)
  {
    state->prior_popup_kind = built_popup;
    if (built_popup == ReaderViewPopup_NoteEditor &&
        state->prior_popup_item_count <
          READER_VIEW_POPUP_ITEM_SNAPSHOT_CAP)
    {
      state->prior_popup_item_ids[state->prior_popup_item_count++] =
        rv_id(502, state->note_selection_key);
    }
    for (index = 0; index < ctx->control_count &&
         state->prior_popup_item_count <
           READER_VIEW_POPUP_ITEM_SNAPSHOT_CAP; ++index)
    {
      const UI0ControlRecord *record =
        ctx->storage->control_records + index;
      if (record->root == UI0RootKind_Normal ||
          record->kind == UI0ControlKind_PopupSurface ||
          record->kind == UI0ControlKind_ModalSurface)
        continue;
      state->prior_popup_item_ids[state->prior_popup_item_count++] =
        record->id;
    }
  }
}

static UI0U32
rv_find_text_codepoint(const char *text,
                       UI0S32 byte_count,
                       UI0S32 index,
                       UI0S32 *out_size)
{
  unsigned char byte;
  UI0U32 codepoint;
  UI0S32 size = 1;
  if (out_size) *out_size = 0;
  if (!text || !out_size || index < 0 || index >= byte_count) return 0;
  byte = (unsigned char)text[index];
  codepoint = byte;
  if ((byte & 0x80u) == 0)
  {
    *out_size = 1;
    return codepoint;
  }
  if ((byte & 0xe0u) == 0xc0u && index + 1 < byte_count &&
      (((unsigned char)text[index + 1]) & 0xc0u) == 0x80u)
  {
    codepoint = ((UI0U32)(byte & 0x1fu) << 6) |
                (UI0U32)(((unsigned char)text[index + 1]) & 0x3fu);
    size = 2;
  }
  else if ((byte & 0xf0u) == 0xe0u && index + 2 < byte_count &&
           (((unsigned char)text[index + 1]) & 0xc0u) == 0x80u &&
           (((unsigned char)text[index + 2]) & 0xc0u) == 0x80u)
  {
    codepoint = ((UI0U32)(byte & 0x0fu) << 12) |
                ((UI0U32)(((unsigned char)text[index + 1]) & 0x3fu) << 6) |
                (UI0U32)(((unsigned char)text[index + 2]) & 0x3fu);
    size = 3;
  }
  else if ((byte & 0xf8u) == 0xf0u && index + 3 < byte_count &&
           (((unsigned char)text[index + 1]) & 0xc0u) == 0x80u &&
           (((unsigned char)text[index + 2]) & 0xc0u) == 0x80u &&
           (((unsigned char)text[index + 3]) & 0xc0u) == 0x80u)
  {
    codepoint = ((UI0U32)(byte & 0x07u) << 18) |
                ((UI0U32)(((unsigned char)text[index + 1]) & 0x3fu) << 12) |
                ((UI0U32)(((unsigned char)text[index + 2]) & 0x3fu) << 6) |
                (UI0U32)(((unsigned char)text[index + 3]) & 0x3fu);
    size = 4;
  }
  *out_size = size;
  return codepoint;
}

static UI0S32
rv_find_codepoint_advance(const ReaderViewFindTextMetrics *metrics,
                          UI0U32 codepoint)
{
  UI0S32 index;
  if (!metrics) return 0;
  for (index = 0; index < metrics->advance_count; ++index)
    if (metrics->advances[index].codepoint == codepoint)
      return metrics->advances[index].advance;
  return metrics->fallback_advance;
}

static UI0S32
rv_note_codepoint_advance(const ReaderViewNoteTextMetrics *metrics,
                          UI0U32 codepoint)
{
  UI0S32 index;
  if (!metrics) return 0;
  for (index = 0; index < metrics->advance_count; ++index)
    if (metrics->advances[index].codepoint == codepoint)
      return metrics->advances[index].advance;
  return metrics->fallback_advance;
}

static UI0S32
rv_find_text_measure(void *user_data,
                     const char *text,
                     UI0S32 byte_count)
{
  const ReaderViewFindTextMetrics *metrics =
    (const ReaderViewFindTextMetrics *)user_data;
  UI0S32 index = 0;
  UI0S32 current_width = 0;
  UI0S32 max_width = 0;
  if (!metrics || !text || byte_count <= 0) return 0;
  while (index < byte_count)
  {
    UI0S32 size;
    UI0U32 codepoint = rv_find_text_codepoint(text, byte_count, index, &size);
    UI0S32 advance;
    if (size <= 0) break;
    index += size;
    if (codepoint == (UI0U32)'\r') continue;
    if (codepoint == (UI0U32)'\n')
    {
      max_width = rv_max(max_width, current_width);
      current_width = 0;
      continue;
    }
    advance = rv_find_codepoint_advance(metrics, codepoint);
    current_width = current_width > 0x7fffffff - advance ?
      0x7fffffff : current_width + advance;
  }
  return rv_max(max_width, current_width);
}

static UI0S32
rv_note_text_measure(void *user_data,
                     const char *text,
                     UI0S32 byte_count)
{
  const ReaderViewNoteTextMetrics *metrics =
    (const ReaderViewNoteTextMetrics *)user_data;
  UI0S32 index = 0;
  UI0S32 current_width = 0;
  UI0S32 max_width = 0;
  if (!metrics || !text || byte_count <= 0) return 0;
  while (index < byte_count)
  {
    UI0S32 size;
    UI0U32 codepoint = rv_find_text_codepoint(text, byte_count, index, &size);
    UI0S32 advance;
    if (size <= 0) break;
    index += size;
    if (codepoint == (UI0U32)'\r') continue;
    if (codepoint == (UI0U32)'\n')
    {
      max_width = rv_max(max_width, current_width);
      current_width = 0;
      continue;
    }
    advance = rv_note_codepoint_advance(metrics, codepoint);
    current_width = current_width > 0x7fffffff - advance ?
      0x7fffffff : current_width + advance;
  }
  return rv_max(max_width, current_width);
}

static UI0B32
rv_codepoint_metrics_valid(const ReaderViewCodepointAdvance *advances,
                           UI0S32 advance_count,
                           UI0S32 fallback_advance,
                           UI0S32 cap,
                           UI0B32 required,
                           UI0B32 present)
{
  UI0S32 index;
  UI0S32 earlier;
  if (((required || present) &&
       (fallback_advance <= 0 || fallback_advance > 0x100000)) ||
      advance_count < 0 || advance_count > cap ||
      (advance_count > 0 && !advances))
    return 0;
  for (index = 0; index < advance_count; ++index)
  {
    const ReaderViewCodepointAdvance *item = advances + index;
    if (item->codepoint == 0 || item->codepoint > 0x10ffffu ||
        (item->codepoint >= 0xd800u && item->codepoint <= 0xdfffu) ||
        item->advance < 0 || item->advance > 0x100000)
      return 0;
    for (earlier = 0; earlier < index; ++earlier)
      if (advances[earlier].codepoint == item->codepoint)
        return 0;
  }
  return 1;
}

static UI0B32
rv_build_input_valid(const ReaderViewBuildInput *input,
                     ReaderViewFrameStorage *storage,
                     ReaderViewFrame *out_frame)
{
  UI0B32 metrics_required;
  UI0B32 metrics_present;
  UI0B32 note_metrics_required;
  UI0B32 note_metrics_present;
  if (!input || !storage || !out_frame || !input->state || !input->layout ||
      !input->projection || !input->input || !input->theme)
    return 0;
  metrics_required =
    (input->projection->features & ReaderViewFeature_Find) != 0;
  metrics_present = input->find_text_metrics.advances != 0 ||
    input->find_text_metrics.advance_count != 0 ||
    input->find_text_metrics.fallback_advance != 0;
  if (!rv_codepoint_metrics_valid(
        input->find_text_metrics.advances,
        input->find_text_metrics.advance_count,
        input->find_text_metrics.fallback_advance,
        READER_VIEW_FIND_CODEPOINT_ADVANCE_CAP,
        metrics_required,
        metrics_present))
    return 0;
  note_metrics_required = input->state->popup == ReaderViewPopup_NoteEditor;
  note_metrics_present = input->note_text_metrics.advances != 0 ||
    input->note_text_metrics.advance_count != 0 ||
    input->note_text_metrics.fallback_advance != 0 ||
    input->note_text_metrics.pixel_height != 0 ||
    input->note_text_metrics.line_height != 0;
  if (!rv_codepoint_metrics_valid(
        input->note_text_metrics.advances,
        input->note_text_metrics.advance_count,
        input->note_text_metrics.fallback_advance,
        READER_VIEW_NOTE_CODEPOINT_ADVANCE_CAP,
        note_metrics_required,
        note_metrics_present) ||
      ((note_metrics_required || note_metrics_present) &&
       (input->note_text_metrics.pixel_height <= 0 ||
        input->note_text_metrics.pixel_height > 0x100000 ||
        input->note_text_metrics.line_height <
          input->note_text_metrics.pixel_height ||
        input->note_text_metrics.line_height > 0x100000)))
    return 0;
  return 1;
}

static UI0B32
rv_published_focusable_id(const RVBuildContext *ctx, UI0ID id)
{
  UI0S32 index;
  if (!ctx || id == 0) return 0;
  for (index = 0; index < ctx->semantic_count; ++index)
  {
    const ReaderViewSemanticNode *node =
      ctx->storage->semantic_nodes + index;
    if (node->id == id &&
        (node->flags & ReaderViewSemantic_Focusable) != 0)
      return 1;
  }
  return 0;
}

static UI0B32
rv_published_signal_id(const RVBuildContext *ctx, UI0ID id)
{
  UI0S32 index;
  if (!ctx || id == 0) return 0;
  for (index = 0; index < ctx->signals.record_count; ++index)
    if (ctx->signals.records[index].id == id) return 1;
  return 0;
}

static void
rv_reconcile_published_interaction(RVBuildContext *ctx)
{
  if (!ctx) return;
  if (ctx->signals.focus_id != 0 &&
      !rv_published_focusable_id(ctx, ctx->signals.focus_id))
    rv_move_focus(ctx, 0, 0);
  if (ctx->signals.hot_id != 0 &&
      !rv_published_signal_id(ctx, ctx->signals.hot_id))
  {
    ctx->signals.hot_id = 0;
    ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged;
  }
  if (ctx->signals.active_id != 0 &&
      !rv_published_signal_id(ctx, ctx->signals.active_id))
  {
    ctx->signals.active_id = 0;
    ctx->frame->change_flags |= ReaderViewFrameChange_StateChanged;
  }
}

static UI0B32
rv_consume_accessibility_requests(ReaderViewState *state)
{
  if (!state ||
      (state->pending_accessibility_focus_id == 0 &&
       state->pending_accessibility_invoke_id == 0))
    return 0;
  state->pending_accessibility_focus_id = 0;
  state->pending_accessibility_invoke_id = 0;
  return 1;
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
  UI0ID pre_end_focus;
  UI0B32 pre_end_focus_visible;
  UI0B32 popup_dismiss;
  UI0S32 late_control_index;
  if (!out_frame)
  {
    if (input) (void)rv_consume_accessibility_requests(input->state);
    return 0;
  }
  memset(out_frame, 0, sizeof(*out_frame));
  if (!rv_build_input_valid(input, storage, out_frame))
  {
    out_frame->error_flags = ReaderViewFrameError_BadInput;
    if (input && rv_consume_accessibility_requests(input->state))
      out_frame->change_flags |= ReaderViewFrameChange_StateChanged;
    return 0;
  }
  reader_view_frame_storage_init(storage);
  out_frame->layout = *input->layout;
  projection_errors = rv_validate_projection(input->projection);
  if (projection_errors != ReaderViewFrameError_None)
  {
    out_frame->error_flags = projection_errors;
    out_frame->change_flags = ReaderViewFrameChange_ProjectionInvalid;
    if (rv_consume_accessibility_requests(input->state))
      out_frame->change_flags |= ReaderViewFrameChange_StateChanged;
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
      if (rv_consume_accessibility_requests(input->state))
        out_frame->change_flags |= ReaderViewFrameChange_StateChanged;
      return 0;
    }
  }
  old_focus = input->state->focus_id;

  memset(&ctx, 0, sizeof(ctx));
  ctx.input = input;
  ctx.storage = storage;
  ctx.frame = out_frame;
  ui0_layout_begin(&ctx.input_layout,
                   storage->layout_boxes,
                   READER_VIEW_LAYOUT_BOX_CAP);
  ui0_text_input_context_init(&ctx.text_inputs);
  ui0_text_input_begin_frame(&ctx.text_inputs,
                             storage->text_input_records,
                             READER_VIEW_TEXT_INPUT_CAP);
  ctx.text_inputs.frame_index = input->frame_index;
  ctx.text_inputs.style = ui0_text_input_style_from_resolved(input->theme);
  ui0_text_area_context_init(&ctx.text_areas);
  ui0_text_area_begin_frame(
    &ctx.text_areas,
    storage->text_area_records,
    READER_VIEW_TEXT_AREA_CAP,
    storage->note_text_area_row_records,
    READER_VIEW_TEXT_AREA_ROW_CAP,
    storage->note_text_area_selection_records,
    READER_VIEW_TEXT_AREA_SELECTION_CAP,
    storage->note_text_area_layout_rows,
    READER_VIEW_TEXT_AREA_ROW_CAP);
  ctx.text_areas.frame_index = input->frame_index;
  ctx.text_areas.style = ui0_text_area_style_from_resolved(input->theme);
  if (input->note_text_metrics.fallback_advance > 0)
  {
    ctx.text_areas.style.measure.measure = rv_note_text_measure;
    ctx.text_areas.style.measure.user_data =
      (void *)(const void *)&input->note_text_metrics;
    ctx.text_areas.style.measure.fallback_char_width =
      input->note_text_metrics.fallback_advance;
    ctx.text_areas.style.padding_x = RV_NOTE_TEXT_PADDING_X;
    ctx.text_areas.style.padding_y = RV_NOTE_TEXT_LAYOUT_PADDING_Y;
    ctx.text_areas.style.line_height = input->note_text_metrics.line_height;
    ctx.text_areas.style.caret_width = 1;
  }
  if (input->find_text_metrics.fallback_advance > 0)
  {
    ctx.text_inputs.style.measure.measure = rv_find_text_measure;
    ctx.text_inputs.style.measure.user_data =
      (void *)(const void *)&input->find_text_metrics;
    ctx.text_inputs.style.measure.fallback_char_width =
      input->find_text_metrics.fallback_advance;
  }
  labels = rv_resolve_labels(input->projection->labels);
  rv_reconcile_feature_owners(input, out_frame);
  rv_reconcile_scroll_owners(input, out_frame);
  rv_sync_selection_popup(&ctx);
  rv_reconcile_dynamic_rows(input, out_frame);
  if (input->state->right_menu_key != 0)
  {
    const ReaderViewRightRow *menu_row = rv_find_right_row(
      input->projection, input->state->right_menu_key);
    UI0B32 row_identity_changed = !menu_row ||
      menu_row->kind != input->state->right_menu_kind;
    UI0B32 action_membership_changed =
      input->state->popup == ReaderViewPopup_RightRowActions && menu_row &&
      menu_row->actions != input->state->right_menu_actions;
    UI0B32 empty_open_popup =
      input->state->popup == ReaderViewPopup_RightRowActions &&
      rv_right_action_popup_count(menu_row) == 0;
    if (row_identity_changed || action_membership_changed || empty_open_popup)
    {
      UI0B32 focus_changed = rv_close_stale_right_action_popup(
        input->state, row_identity_changed);
      out_frame->change_flags |= ReaderViewFrameChange_StateChanged;
      if (focus_changed)
        out_frame->change_flags |= ReaderViewFrameChange_FocusChanged;
    }
  }
  if (input->state->popup == ReaderViewPopup_RightFilter &&
      input->state->right_filter_menu_flags !=
        input->projection->right.available_filters)
  {
    UI0B32 focus_changed =
      rv_close_stale_right_filter_popup(input->state);
    out_frame->change_flags |= ReaderViewFrameChange_StateChanged;
    if (focus_changed)
      out_frame->change_flags |= ReaderViewFrameChange_FocusChanged;
  }
  rv_reconcile_popup_state(input, out_frame);
  root_popup = input->state->popup;
  root_rect = root_popup == ReaderViewPopup_NoteEditor ?
    rv_note_editor_rect(input) :
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
  ui0_sidenav_context_init(&ctx.sidenav_visuals);
  ui0_sidenav_begin_frame(
    &ctx.sidenav_visuals,
    storage->reference_sidenav_records,
    READER_VIEW_REFERENCE_SIDENAV_RECORD_CAP);
  ctx.sidenav_visuals.frame_index = input->frame_index;

  if (popup_dismiss && root_popup != ReaderViewPopup_NoteEditor)
  {
    UI0ID restore_id = input->state->restore_focus_id;
    UI0S32 popup_index;
    UI0B32 focus_visible =
      ctx.signals.focus_id != 0 &&
      ctx.signals.focus_visible_id == ctx.signals.focus_id;
    if (root_popup == ReaderViewPopup_SelectionTools)
      input->state->dismissed_selection_key = input->state->last_selection_key;
    for (popup_index = 0;
         popup_index < input->state->prior_popup_item_count;
         ++popup_index)
    {
      UI0ID id = input->state->prior_popup_item_ids[popup_index];
      if (ctx.signals.hot_id == id) ctx.signals.hot_id = 0;
      if (ctx.signals.active_id == id) ctx.signals.active_id = 0;
    }
    rv_clear_prior_popup_interaction(input->state, restore_id);
    input->state->popup = ReaderViewPopup_None;
    input->state->restore_focus_id = 0;
    if (root_popup == ReaderViewPopup_RightFilter)
      input->state->right_filter_menu_flags = 0;
    if (root_popup == ReaderViewPopup_RightRowActions)
    {
      input->state->right_menu_key = 0;
      input->state->right_menu_actions = ReaderViewRightAction_None;
    }
    rv_move_focus(&ctx, restore_id, focus_visible);
    out_frame->change_flags |= ReaderViewFrameChange_StateChanged;
  }
  rv_handle_escape(&ctx);
  if (input->state->popup != root_popup)
  {
    ui0_signal_set_root(&ctx.signals, UI0RootKind_Popup,
                        rv_rect(0, 0, 0, 0), 0);
    ui0_signal_set_root(&ctx.signals, UI0RootKind_Modal,
                        rv_rect(0, 0, 0, 0), 0);
    ui0_signal_resolve_roots(&ctx.signals);
  }
  rv_build_toolbar(&ctx, labels);
  rv_build_paging_and_progress(&ctx, labels);
  rv_build_content_status(&ctx);
  rv_build_left_panel(&ctx, labels);
  rv_build_right_panel(&ctx, labels);
  rv_build_popup(&ctx, root_popup, root_rect, labels);
  rv_apply_semantic_focus_navigation(&ctx);

  pre_end_focus = ctx.signals.focus_id;
  pre_end_focus_visible = pre_end_focus != 0 &&
    ctx.signals.focus_visible_id == pre_end_focus;
  ui0_signal_end_frame(&ctx.signals);
  rv_reconcile_published_interaction(&ctx);
  rv_patch_built_focus(&ctx, pre_end_focus, 0, 0);
  rv_patch_built_focus(
    &ctx, ctx.signals.focus_id, ctx.signals.focus_id != 0,
    ctx.signals.focus_id != 0 &&
      ctx.signals.focus_visible_id == ctx.signals.focus_id);
  if (pre_end_focus != ctx.signals.focus_id ||
      pre_end_focus_visible !=
        (ctx.signals.focus_id != 0 &&
         ctx.signals.focus_visible_id == ctx.signals.focus_id))
    out_frame->change_flags |= ReaderViewFrameChange_FocusChanged;
  input->state->hot_id = ctx.signals.hot_id;
  input->state->active_id = ctx.signals.active_id;
  input->state->focus_id = ctx.signals.focus_id;
  input->state->focus_visible =
    ctx.signals.focus_visible_id == ctx.signals.focus_id &&
    ctx.signals.focus_id != 0;
  if (old_focus != ctx.signals.focus_id)
    out_frame->change_flags |= ReaderViewFrameChange_FocusChanged;
  if (rv_consume_accessibility_requests(input->state))
  {
    out_frame->change_flags |= ReaderViewFrameChange_StateChanged;
  }
  rv_capture_state_snapshots(&ctx, root_popup);

  if (root_popup == ReaderViewPopup_RightRowActions &&
      input->state->right_menu_key != 0)
  {
    UI0ControlRecord *owner = rv_control_record_for_id(
      &ctx, rv_id(324, input->state->right_menu_key));
    if (owner) owner->state |= UI0ControlState_Hovered;
  }

  ui0_draw_context_init(&ctx.draw);
  ui0_draw_begin_frame(&ctx.draw, storage->draw_commands,
                       READER_VIEW_DRAW_COMMAND_CAP,
                       ui0_draw_theme_from_resolved(input->theme));
  ctx.draw.frame_index = input->frame_index;
  late_control_index = ctx.control_count;
  for (UI0S32 control_index = 0;
       control_index < ctx.control_count;
       ++control_index)
  {
    if (storage->control_records[control_index].root != UI0RootKind_Normal)
    {
      late_control_index = control_index;
      break;
    }
  }
  (void)ui0_draw_controls(&ctx.draw, storage->control_records,
                          late_control_index);
  (void)ui0_slider_draw_records(&ctx.draw, &ctx.sliders);
  for (UI0S32 icon_index = 0; icon_index < ctx.icon_count; ++icon_index)
  {
    const RVIconRecord *icon = ctx.icons + icon_index;
    if (icon->visible && icon->control_index >= 0 &&
        icon->control_index < late_control_index)
    {
      (void)ui0_draw_push_icon(&ctx.draw,
                               storage->control_records + icon->control_index,
                               icon->icon_kind,
                               icon->rect);
    }
  }
  rv_filter_reference_chrome_draws(&ctx);
  ui0_draw_set_text_caret_visible(
    &ctx.draw, rv_text_caret_is_visible(input->frame_index));
  {
    UI0S32 text_input_draw_start = ctx.draw.command_count;
    (void)ui0_text_input_draw_records(&ctx.draw,
                                      storage->text_input_records,
                                      ctx.text_inputs.record_count);
    rv_restore_find_caret_clip(&ctx, text_input_draw_start);
  }
  (void)ui0_sidenav_draw_records(&ctx.draw, &ctx.sidenav_visuals);
  (void)ui0_scroll_draw_records(&ctx.draw, &ctx.scrolls);
  (void)ui0_draw_controls(
    &ctx.draw,
    storage->control_records + late_control_index,
    ctx.control_count - late_control_index);
  for (UI0S32 icon_index = 0; icon_index < ctx.icon_count; ++icon_index)
  {
    const RVIconRecord *icon = ctx.icons + icon_index;
    if (icon->visible && icon->control_index >= late_control_index &&
        icon->control_index < ctx.control_count)
    {
      (void)ui0_draw_push_icon(&ctx.draw,
                               storage->control_records + icon->control_index,
                               icon->icon_kind,
                               icon->rect);
    }
  }
  {
    UI0S32 text_area_draw_start = ctx.draw.command_count;
    (void)ui0_text_area_draw_records(&ctx.draw, &ctx.text_areas);
    if (ctx.text_areas.record_count > 0 &&
        input->state->popup == ReaderViewPopup_NoteEditor)
    {
      const UI0TextAreaRecord *record = ctx.text_areas.records;
      UI0S32 text_area_draw_index;
      for (text_area_draw_index = text_area_draw_start;
           text_area_draw_index < ctx.draw.command_count;
           ++text_area_draw_index)
      {
        UI0DrawCommand *command =
          ctx.draw.commands + text_area_draw_index;
        if (command->source_id != record->id) continue;
        if (command->op == UI0DrawOp_Text)
        {
          if (input->state->note_draft_length > 0 &&
              command->source_index >= record->row_start &&
              command->source_index < record->row_start + record->row_count)
          {
            command->source_id = rv_id(
              RV_NOTE_TEXT_ROW_ID_BASE + (UI0U64)command->source_index,
              input->state->note_selection_key);
          }
          command->typography_role = UI0TypographyRole_Body;
          command->has_typography_role = 1;
          command->typography_char_width =
            input->note_text_metrics.fallback_advance;
          command->typography_line_height =
            input->note_text_metrics.pixel_height;
        }
        else if (command->op == UI0DrawOp_TextCaret)
        {
          command->color =
            input->theme->colors[UI0ColorRole_Focus];
        }
      }
      rv_add_note_editor_corner_masks(&ctx, record);
    }
  }

  if ((ctx.signals.error_flags & UI0SignalError_NoRecordCap) != 0 ||
      (ctx.sliders.error_flags & UI0SliderError_NoRecordCap) != 0 ||
      (ctx.scrolls.error_flags & UI0ScrollError_NoRecordCap) != 0 ||
      (ctx.text_inputs.error_flags & UI0TextInputError_NoRecordCap) != 0 ||
      (ctx.text_areas.error_flags &
       (UI0TextAreaError_RowCapExceeded |
        UI0TextAreaError_NoRecordCap |
        UI0TextAreaError_NoRowCap |
        UI0TextAreaError_NoSelectionCap)) != 0 ||
      (ctx.sidenav_visuals.error_flags & UI0SidenavError_NoRecordCap) != 0 ||
      (ctx.draw.error_flags & UI0DrawError_NoCommandCap) != 0)
    out_frame->error_flags |= ReaderViewFrameError_RecordCap;
  if ((ctx.signals.error_flags & UI0SignalError_BadInput) != 0 ||
      (ctx.sliders.error_flags & UI0SliderError_BadInput) != 0 ||
      (ctx.scrolls.error_flags & UI0ScrollError_BadInput) != 0 ||
      (ctx.text_inputs.error_flags & UI0TextInputError_BadInput) != 0 ||
      (ctx.text_areas.error_flags &
       (UI0TextAreaError_BadInput |
        UI0TextAreaError_HistoryIncompatible)) != 0 ||
      (ctx.sidenav_visuals.error_flags & UI0SidenavError_BadInput) != 0 ||
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

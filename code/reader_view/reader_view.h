#ifndef READERVIEW0_READER_VIEW_H
#define READERVIEW0_READER_VIEW_H

enum
{
  READER_VIEW_FIND_QUERY_CAP = 128,
  READER_VIEW_FIND_CODEPOINT_ADVANCE_CAP = 256,
  READER_VIEW_NOTE_CODEPOINT_ADVANCE_CAP = 256,
  READER_VIEW_NOTE_DRAFT_CAP = 512,
  READER_VIEW_FIND_HISTORY_EDIT_CAP = 16,
  READER_VIEW_FIND_HISTORY_TEXT_CAP = 512,
  READER_VIEW_NOTE_HISTORY_EDIT_CAP = 64,
  READER_VIEW_NOTE_HISTORY_TEXT_CAP = 4096,
  READER_VIEW_TOC_ROW_CAP = 64,
  READER_VIEW_FIND_ROW_CAP = 64,
  READER_VIEW_RIGHT_ROW_CAP = 128,
  READER_VIEW_SETTING_CAP = 4,
  READER_VIEW_CHOICE_CAP = 16,
  READER_VIEW_HIGHLIGHT_COLOR_CAP = 8,
  READER_VIEW_ACTION_CAP = 32,
  READER_VIEW_LAYOUT_BOX_CAP = 640,
  READER_VIEW_SIGNAL_CAP = 384,
  READER_VIEW_CONTROL_CAP = 384,
  READER_VIEW_DRAW_COMMAND_CAP = 2560,
  READER_VIEW_POPUP_CAP = 6,
  READER_VIEW_PANEL_CAP = 4,
  READER_VIEW_TOOLBAR_SLOT_CAP = 20,
  READER_VIEW_LIST_RECORD_CAP = 208,
  READER_VIEW_SIDENAV_RECORD_CAP = 64,
  READER_VIEW_REFERENCE_SIDENAV_RECORD_CAP = READER_VIEW_TOC_ROW_CAP + 2,
  READER_VIEW_TEXT_INPUT_CAP = 1,
  READER_VIEW_TEXT_AREA_CAP = 1,
  READER_VIEW_TEXT_AREA_ROW_CAP = READER_VIEW_NOTE_DRAFT_CAP,
  READER_VIEW_TEXT_AREA_SELECTION_CAP = 64,
  READER_VIEW_SLIDER_CAP = 2,
  READER_VIEW_SCROLL_CAP = 4,
  READER_VIEW_POPUP_ITEM_SNAPSHOT_CAP = 32,
  READER_VIEW_TEXT_BINDING_CAP = 640,
  READER_VIEW_RIGHT_FILTER_LABEL_CAP = 256,
  READER_VIEW_SEMANTIC_NODE_CAP = 384,
  READER_VIEW_DEFAULT_PAGE_HORIZONTAL_INSET = 24,
  READER_VIEW_DEFAULT_PAGE_MAX_WIDTH = 660,
  READER_VIEW_DEFAULT_PAGE_MIN_WIDTH = 160,
  READER_VIEW_DEFAULT_CONTENT_INSET_X = 52,
  READER_VIEW_DEFAULT_CONTENT_INSET_Y = 68,
  READER_VIEW_DEFAULT_CONTENT_MIN_WIDTH = 80,
  READER_VIEW_DEFAULT_CONTENT_MIN_HEIGHT = 48,
  READER_VIEW_REFERENCE_TOP_CHROME_HEIGHT = 56,
  READER_VIEW_REFERENCE_FOOTER_HEIGHT = 38,
  READER_VIEW_REFERENCE_LEFT_PANEL_WIDTH = 420,
  READER_VIEW_REFERENCE_RIGHT_PANEL_WIDTH = 320,
  READER_VIEW_REFERENCE_PANEL_INSET = 12,
  READER_VIEW_REFERENCE_PANEL_PAGE_GAP = 14,
};

typedef struct ReaderViewText
{
  const char *data;
  UI0S32 size;
} ReaderViewText;

/*
 * Concrete font measurement remains host-owned. The host supplies bounded
 * per-codepoint advances from the same system-UI face used to paint Find text.
 * Readerview0 uses only these portable values for Find-input geometry and to
 * select a bounded one-line result excerpt that includes its valid match; the
 * host still owns glyph measurement and rasterization. No caller executable
 * indirection or provider boundary is exposed. Missing codepoints use the
 * caller-measured fallback advance.
 */
typedef struct ReaderViewCodepointAdvance
{
  UI0U32 codepoint;
  UI0S32 advance;
} ReaderViewCodepointAdvance;

typedef struct ReaderViewFindTextMetrics
{
  const ReaderViewCodepointAdvance *advances;
  UI0S32 advance_count;
  UI0S32 fallback_advance;
} ReaderViewFindTextMetrics;

/*
 * The note editor uses the accepted system-UI face at a caller-owned pixel
 * height. Advances and scalar metrics are borrowed for one build only. The
 * boundary remains values-only: no font object, executable indirection, or
 * native handle crosses into readerview0.
 */
typedef struct ReaderViewNoteTextMetrics
{
  const ReaderViewCodepointAdvance *advances;
  UI0S32 advance_count;
  UI0S32 fallback_advance;
  UI0S32 pixel_height;
  UI0S32 line_height;
} ReaderViewNoteTextMetrics;

typedef UI0U64 ReaderViewKey;

typedef enum ReaderViewLoadState
{
  ReaderViewLoad_Unavailable,
  ReaderViewLoad_Empty,
  ReaderViewLoad_Loading,
  ReaderViewLoad_Ready,
  ReaderViewLoad_Error,
} ReaderViewLoadState;

typedef struct ReaderViewSurfaceStatus
{
  ReaderViewLoadState state;
  ReaderViewText message;
  ReaderViewText detail;
} ReaderViewSurfaceStatus;

typedef UI0U32 ReaderViewChoiceFlags;
enum
{
  ReaderViewChoice_None     = 0,
  ReaderViewChoice_Enabled  = 1u << 0,
  ReaderViewChoice_Selected = 1u << 1,
};

typedef struct ReaderViewChoice
{
  ReaderViewKey key;
  ReaderViewText label;
  ReaderViewText detail;
  ReaderViewChoiceFlags flags;
} ReaderViewChoice;

typedef enum ReaderViewChoicePresentation
{
  ReaderViewChoicePresentation_Stepper,
  ReaderViewChoicePresentation_Segments,
  ReaderViewChoicePresentation_Menu,
} ReaderViewChoicePresentation;

typedef struct ReaderViewChoiceControl
{
  const ReaderViewChoice *items;
  UI0S32 count;
  ReaderViewChoicePresentation presentation;
} ReaderViewChoiceControl;

typedef UI0U32 ReaderViewRowFlags;
enum
{
  ReaderViewRow_None     = 0,
  ReaderViewRow_Enabled  = 1u << 0,
  ReaderViewRow_Current  = 1u << 1,
  ReaderViewRow_Selected = 1u << 2,
  ReaderViewRow_Starred  = 1u << 3,
  /* The row is a note visually attached to the preceding highlight row. */
  ReaderViewRow_AttachedToPrevious = 1u << 4,
};

typedef struct ReaderViewTocRow
{
  ReaderViewKey key;
  UI0U32 depth;
  ReaderViewText label;
  ReaderViewText detail;
  ReaderViewRowFlags flags;
} ReaderViewTocRow;

typedef struct ReaderViewFindRow
{
  ReaderViewKey key;
  ReaderViewText section;
  ReaderViewText excerpt;
  ReaderViewText detail;
  UI0U32 match_start;
  UI0U32 match_size;
  ReaderViewRowFlags flags;
} ReaderViewFindRow;

typedef enum ReaderViewRightRowKind
{
  ReaderViewRightRow_Bookmark,
  ReaderViewRightRow_Highlight,
  ReaderViewRightRow_Note,
} ReaderViewRightRowKind;

typedef UI0U32 ReaderViewRightActionFlags;
enum
{
  ReaderViewRightAction_None       = 0,
  ReaderViewRightAction_Activate   = 1u << 0,
  /*
   * Enables the frozen inline star for every right-row kind. The same action
   * is also present in the popup only for Highlight rows; Bookmark and Note
   * popup membership is kind-specific.
   */
  ReaderViewRightAction_ToggleStar = 1u << 1,
  /* Valid only for Note rows and exposed in the Note action popup. */
  ReaderViewRightAction_EditNote   = 1u << 2,
  ReaderViewRightAction_Delete     = 1u << 3,
};

typedef struct ReaderViewRightRow
{
  ReaderViewKey key;
  ReaderViewRightRowKind kind;
  ReaderViewText section;
  ReaderViewText primary;
  ReaderViewText secondary;
  ReaderViewKey color_key;
  ReaderViewRowFlags flags;
  ReaderViewRightActionFlags actions;
  /* Append-only caller-resolved visual; color_key remains identity. */
  UI0Color rail_color;
} ReaderViewRightRow;

typedef UI0U64 ReaderViewFeatureFlags;
enum
{
  ReaderViewFeature_None            = 0,
  ReaderViewFeature_Open            = 1ull << 0,
  ReaderViewFeature_Paging          = 1ull << 1,
  ReaderViewFeature_History         = 1ull << 2,
  ReaderViewFeature_Contents        = 1ull << 3,
  ReaderViewFeature_Find            = 1ull << 4,
  ReaderViewFeature_Progress        = 1ull << 5,
  ReaderViewFeature_ReadingSettings = 1ull << 6,
  ReaderViewFeature_Bookmark        = 1ull << 7,
  ReaderViewFeature_Annotations     = 1ull << 8,
  ReaderViewFeature_SelectionTools  = 1ull << 9,
  ReaderViewFeature_Fullscreen      = 1ull << 10,
  ReaderViewFeature_DistractionFree = 1ull << 11,
  ReaderViewFeature_Lookup          = 1ull << 12,
  ReaderViewFeature_Export          = 1ull << 13,
};

typedef UI0U32 ReaderViewDocumentFlags;
enum
{
  ReaderViewDocument_None                  = 0,
  ReaderViewDocument_Open                  = 1u << 0,
  ReaderViewDocument_CanOpen               = 1u << 1,
  ReaderViewDocument_CanGoPreviousPage     = 1u << 2,
  ReaderViewDocument_CanGoNextPage         = 1u << 3,
  ReaderViewDocument_CanGoBack             = 1u << 4,
  ReaderViewDocument_CanGoForward          = 1u << 5,
  ReaderViewDocument_CurrentBookmarked     = 1u << 6,
  ReaderViewDocument_Fullscreen            = 1u << 7,
  ReaderViewDocument_DistractionFree       = 1u << 8,
  ReaderViewDocument_CanToggleFullscreen   = 1u << 9,
  ReaderViewDocument_CanToggleDistraction  = 1u << 10,
};

typedef enum ReaderViewSettingKind
{
  ReaderViewSetting_FontFamily,
  ReaderViewSetting_FontSize,
  ReaderViewSetting_LineSpacing,
  ReaderViewSetting_Theme,
  ReaderViewSetting_Count,
} ReaderViewSettingKind;

typedef struct ReaderViewSettingControl
{
  ReaderViewSettingKind kind;
  ReaderViewText label;
  ReaderViewText help;
  ReaderViewSurfaceStatus status;
  ReaderViewChoiceControl choices;
} ReaderViewSettingControl;

typedef struct ReaderViewReadingSettingsProjection
{
  ReaderViewSurfaceStatus status;
  const ReaderViewSettingControl *items;
  UI0S32 count;
} ReaderViewReadingSettingsProjection;

typedef struct ReaderViewProgressProjection
{
  ReaderViewSurfaceStatus status;
  UI0U64 location_index;
  UI0U64 location_count;
  UI0U64 page_index;
  UI0U64 page_count;
  ReaderViewText chapter;
  ReaderViewText label;
  UI0B32 can_seek;
} ReaderViewProgressProjection;

typedef struct ReaderViewTocProjection
{
  ReaderViewSurfaceStatus status;
  const ReaderViewTocRow *rows;
  UI0S32 row_count;
  UI0U64 total_count;
} ReaderViewTocProjection;

typedef struct ReaderViewFindProjection
{
  ReaderViewSurfaceStatus status;
  ReaderViewText committed_query;
  const ReaderViewFindRow *rows;
  UI0S32 row_count;
  UI0U64 total_count;
  UI0S32 active_index;
  UI0B32 has_more;
  UI0B32 can_step_previous;
  UI0B32 can_step_next;
} ReaderViewFindProjection;

typedef UI0U32 ReaderViewRightFilterFlags;
enum
{
  ReaderViewRightFilterFlag_All        = 1u << 0,
  ReaderViewRightFilterFlag_Bookmarks  = 1u << 1,
  ReaderViewRightFilterFlag_Highlights = 1u << 2,
  ReaderViewRightFilterFlag_Notes      = 1u << 3,
};

typedef struct ReaderViewRightProjection
{
  ReaderViewSurfaceStatus status;
  const ReaderViewRightRow *rows;
  UI0S32 row_count;
  UI0U64 total_count;
  UI0B32 has_more;
  ReaderViewRightFilterFlags available_filters;
  /* Explicit unfiltered totals used by the frozen annotation-filter labels. */
  UI0U64 all_count;
  UI0U64 bookmark_count;
  UI0U64 highlight_count;
  UI0U64 note_count;
} ReaderViewRightProjection;

typedef UI0U32 ReaderViewSelectionFlags;
enum
{
  ReaderViewSelection_None               = 0,
  ReaderViewSelection_Active             = 1u << 0,
  ReaderViewSelection_CanCopy            = 1u << 1,
  ReaderViewSelection_CanHighlight       = 1u << 2,
  ReaderViewSelection_CanRemoveHighlight = 1u << 3,
  ReaderViewSelection_CanAddNote         = 1u << 4,
  ReaderViewSelection_CanEditNote        = 1u << 5,
  ReaderViewSelection_CanDeleteNote      = 1u << 6,
  ReaderViewSelection_CanDictionary      = 1u << 7,
  ReaderViewSelection_CanWebLookup       = 1u << 8,
  ReaderViewSelection_CanTranslate       = 1u << 9,
};

typedef struct ReaderViewSelectionProjection
{
  ReaderViewSurfaceStatus status;
  ReaderViewKey selection_key;
  UI0U64 revision;
  ReaderViewText selected_text;
  ReaderViewText note_text;
  ReaderViewKey current_color_key;
  ReaderViewSelectionFlags flags;
  ReaderViewChoiceControl highlight_colors;
  UI0Rect anchor_rect;
} ReaderViewSelectionProjection;

typedef struct ReaderViewLabels
{
  ReaderViewText open;
  ReaderViewText previous_page;
  ReaderViewText next_page;
  ReaderViewText back;
  ReaderViewText forward;
  ReaderViewText contents;
  ReaderViewText find;
  ReaderViewText reading_settings;
  ReaderViewText bookmark;
  ReaderViewText remove_bookmark;
  ReaderViewText annotations;
  ReaderViewText fullscreen;
  ReaderViewText exit_fullscreen;
  ReaderViewText distraction_free;
  ReaderViewText close;
  ReaderViewText clear;
  ReaderViewText previous_match;
  ReaderViewText next_match;
  ReaderViewText export_rows;
  ReaderViewText all;
  ReaderViewText bookmarks;
  ReaderViewText highlights;
  ReaderViewText notes;
  ReaderViewText go_to;
  ReaderViewText star;
  ReaderViewText unstar;
  ReaderViewText edit_note;
  ReaderViewText save_note;
  ReaderViewText cancel;
  ReaderViewText delete_value;
  ReaderViewText copy;
  ReaderViewText dictionary;
  ReaderViewText web_lookup;
  ReaderViewText translate;
  ReaderViewText more;
  /* API 3 panel-specific labels preserve portable names and exact chrome. */
  ReaderViewText close_navigation;
  ReaderViewText search_input;
  ReaderViewText clear_search;
  ReaderViewText annotation_actions;
  ReaderViewText annotation_filters;
  ReaderViewText export_annotations;
  ReaderViewText close_annotations;
  ReaderViewText delete_bookmark;
  ReaderViewText delete_note;
  ReaderViewText delete_highlight;
  ReaderViewText contents_short;
  ReaderViewText contents_panel_title;
  ReaderViewText find_panel_title;
  ReaderViewText filter_annotations;
  ReaderViewText no_contents;
  ReaderViewText find_prompt;
  ReaderViewText no_matches;
  ReaderViewText no_annotations;
  ReaderViewText no_bookmarks;
  ReaderViewText no_highlights;
  ReaderViewText no_notes;
  /* Frozen Find field placeholder; distinct from the ready-status prompt. */
  ReaderViewText find_placeholder;
  /* API 3 frozen inline-note composition and native names. */
  ReaderViewText note_title;
  ReaderViewText add_note_title;
  ReaderViewText note_text;
  ReaderViewText note_placeholder;
  ReaderViewText save;
  ReaderViewText cancel_note;
} ReaderViewLabels;

typedef struct ReaderViewProjection
{
  UI0U64 document_key;
  ReaderViewFeatureFlags features;
  ReaderViewDocumentFlags document_flags;
  ReaderViewSurfaceStatus content;
  /* Portable fixed chrome title; distinct from the host document title. */
  ReaderViewText chrome_title;
  ReaderViewText document_title;
  ReaderViewKey current_bookmark_key;
  ReaderViewLabels labels;
  ReaderViewProgressProjection progress;
  ReaderViewReadingSettingsProjection settings;
  ReaderViewTocProjection toc;
  ReaderViewFindProjection find;
  ReaderViewRightProjection right;
  ReaderViewSelectionProjection selection;
} ReaderViewProjection;

typedef enum ReaderViewLeftPanelMode
{
  ReaderViewLeftPanel_None,
  ReaderViewLeftPanel_Contents,
  ReaderViewLeftPanel_Find,
} ReaderViewLeftPanelMode;

typedef enum ReaderViewRightFilter
{
  ReaderViewRightFilter_All,
  ReaderViewRightFilter_Bookmarks,
  ReaderViewRightFilter_Highlights,
  ReaderViewRightFilter_Notes,
} ReaderViewRightFilter;

typedef enum ReaderViewPopupKind
{
  ReaderViewPopup_None,
  ReaderViewPopup_SettingMenu,
  ReaderViewPopup_Overflow,
  ReaderViewPopup_RightRowActions,
  ReaderViewPopup_SelectionTools,
  ReaderViewPopup_NoteEditor,
  /* Appended in API 3 so the existing popup values remain stable. */
  ReaderViewPopup_RightFilter,
} ReaderViewPopupKind;

typedef enum ReaderViewPanelSide
{
  ReaderViewPanel_None,
  ReaderViewPanel_Left,
  ReaderViewPanel_Right,
} ReaderViewPanelSide;

typedef struct ReaderViewState
{
  UI0U64 document_key;
  ReaderViewLeftPanelMode left_panel;
  UI0B32 right_panel_open;
  ReaderViewRightFilter right_filter;
  ReaderViewPopupKind popup;
  ReaderViewSettingKind active_setting_kind;
  ReaderViewPanelSide most_recent_panel;

  UI0S32 toc_scroll_y;
  UI0S32 find_scroll_y;
  UI0S32 right_scroll_y;
  UI0S32 settings_scroll_y;
  UI0ScrollState toc_scroll;
  UI0ScrollState find_scroll;
  UI0ScrollState right_scroll;
  UI0ScrollState settings_scroll;

  ReaderViewKey active_toc_key;
  ReaderViewKey active_find_key;
  ReaderViewKey active_right_key;
  ReaderViewKey right_menu_key;
  ReaderViewRightRowKind right_menu_kind;
  ReaderViewKey last_selection_key;
  ReaderViewKey dismissed_selection_key;

  UI0ID hot_id;
  UI0ID active_id;
  UI0ID focus_id;
  UI0ID restore_focus_id;
  UI0B32 focus_visible;
  UI0ID pending_accessibility_focus_id;
  UI0ID pending_accessibility_invoke_id;

  UI0TextInputState find_input;
  UI0TextInputHistory find_history;
  UI0TextInputHistoryEdit
    find_history_edits[READER_VIEW_FIND_HISTORY_EDIT_CAP];
  char find_history_text[READER_VIEW_FIND_HISTORY_TEXT_CAP];
  char find_history_scratch[READER_VIEW_FIND_QUERY_CAP];
  char find_query[READER_VIEW_FIND_QUERY_CAP];
  UI0S32 find_query_length;

  UI0TextAreaState note_input;
  UI0TextInputHistory note_history;
  UI0TextInputHistoryEdit
    note_history_edits[READER_VIEW_NOTE_HISTORY_EDIT_CAP];
  char note_history_text[READER_VIEW_NOTE_HISTORY_TEXT_CAP];
  char note_history_scratch[READER_VIEW_NOTE_DRAFT_CAP];
  char note_draft[READER_VIEW_NOTE_DRAFT_CAP];
  UI0S32 note_draft_length;
  ReaderViewKey note_selection_key;
  UI0U64 note_source_revision;
  UI0B32 note_dirty;

  /* API 3 panel focus-restoration state is appended for layout stability. */
  UI0ID left_panel_restore_focus_id;
  UI0ID right_panel_restore_focus_id;
  /* API 3 popup-membership snapshot is also append-only. */
  ReaderViewRightActionFlags right_menu_actions;
  ReaderViewRightFilterFlags right_filter_menu_flags;
  /*
   * API 3 bounded prior-frame identity snapshots let projection refreshes
   * retire vanished controls without retaining caller behavior or allocating.
   */
  ReaderViewKey prior_toc_row_keys[READER_VIEW_TOC_ROW_CAP];
  UI0S32 prior_toc_row_count;
  ReaderViewKey prior_find_row_keys[READER_VIEW_FIND_ROW_CAP];
  UI0S32 prior_find_row_count;
  ReaderViewKey prior_right_row_keys[READER_VIEW_RIGHT_ROW_CAP];
  UI0S32 prior_right_row_count;
  UI0ID prior_popup_item_ids[READER_VIEW_POPUP_ITEM_SNAPSHOT_CAP];
  UI0S32 prior_popup_item_count;
  ReaderViewPopupKind prior_popup_kind;
  /* Deferred until a newly opened left-panel layout is published. */
  ReaderViewLeftPanelMode pending_left_panel_focus;
} ReaderViewState;

typedef enum ReaderViewLayoutMode
{
  ReaderViewLayout_WideDocked,
  ReaderViewLayout_SingleDocked,
  ReaderViewLayout_Overlay,
} ReaderViewLayoutMode;

typedef enum ReaderViewToolbarDensity
{
  ReaderViewToolbar_Full,
  ReaderViewToolbar_Compact,
  ReaderViewToolbar_Overflow,
} ReaderViewToolbarDensity;

typedef struct ReaderViewLayoutInput
{
  UI0Rect bounds;
  ReaderViewFeatureFlags features;
  ReaderViewDocumentFlags document_flags;
  UI0S32 host_toolbar_leading_width;
  UI0S32 host_toolbar_trailing_width;
} ReaderViewLayoutInput;

typedef struct ReaderViewLayout
{
  ReaderViewLayoutMode mode;
  ReaderViewToolbarDensity toolbar_density;
  UI0B32 toolbar_visible;
  UI0B32 progress_visible;
  UI0B32 left_panel_visible;
  UI0B32 right_panel_visible;
  UI0B32 left_panel_overlay;
  UI0B32 right_panel_overlay;
  UI0B32 previous_gutter_visible;
  UI0B32 next_gutter_visible;

  UI0Rect bounds;
  UI0Rect toolbar_rect;
  UI0Rect shared_toolbar_rect;
  UI0Rect host_toolbar_leading_rect;
  UI0Rect host_toolbar_trailing_rect;
  UI0Rect body_rect;
  UI0Rect left_panel_rect;
  UI0Rect right_panel_rect;
  /* One atomic rendering contract resolved with this layout's panel state. */
  UI0Rect viewport_rect;
  UI0Rect page_surface_rect;
  UI0Rect content_rect;
  UI0Rect previous_gutter_rect;
  UI0Rect next_gutter_rect;
  UI0Rect previous_gutter_visual_rect;
  UI0Rect next_gutter_visual_rect;
  UI0Rect progress_rect;
} ReaderViewLayout;

typedef struct ReaderViewContentGeometryStyle
{
  UI0S32 page_horizontal_inset;
  UI0S32 page_max_width;
  UI0S32 page_min_width;
  UI0S32 content_inset_x;
  UI0S32 content_inset_y;
  UI0S32 content_min_width;
  UI0S32 content_min_height;
} ReaderViewContentGeometryStyle;

typedef struct ReaderViewContentGeometry
{
  UI0Rect viewport_rect;
  UI0Rect page_surface_rect;
  UI0Rect content_rect;
} ReaderViewContentGeometry;

typedef enum ReaderViewRangeMove
{
  ReaderViewRangeMove_None,
  ReaderViewRangeMove_First,
  ReaderViewRangeMove_Last,
  ReaderViewRangeMove_PreviousPage,
  ReaderViewRangeMove_NextPage,
} ReaderViewRangeMove;

typedef struct ReaderViewInput
{
  UI0InputState ui;
  UI0TextInputFrameInput find_text;
  UI0TextAreaFrameInput note_text;
  UI0S32 move_vertical_delta;
  UI0S32 move_horizontal_delta;
  ReaderViewRangeMove range_move;
  UI0B32 escape_pressed;
} ReaderViewInput;

typedef enum ReaderViewActionKind
{
  ReaderViewAction_None,
  ReaderViewAction_Open,
  ReaderViewAction_PreviousPage,
  ReaderViewAction_NextPage,
  ReaderViewAction_HistoryBack,
  ReaderViewAction_HistoryForward,
  ReaderViewAction_SeekLocation,
  ReaderViewAction_SelectSetting,
  ReaderViewAction_ToggleBookmark,
  ReaderViewAction_FindChanged,
  ReaderViewAction_FindCommitted,
  ReaderViewAction_FindPrevious,
  ReaderViewAction_FindNext,
  ReaderViewAction_ActivateTocRow,
  ReaderViewAction_ActivateFindRow,
  ReaderViewAction_RightFilterChanged,
  ReaderViewAction_ActivateRightRow,
  ReaderViewAction_ToggleRightRowStar,
  ReaderViewAction_EditRightRowNote,
  ReaderViewAction_DeleteRightRow,
  ReaderViewAction_ExportRightRows,
  ReaderViewAction_SetHighlightColor,
  ReaderViewAction_RemoveHighlight,
  ReaderViewAction_CopySelection,
  ReaderViewAction_DictionarySelection,
  ReaderViewAction_WebLookupSelection,
  ReaderViewAction_TranslateSelection,
  ReaderViewAction_SaveNote,
  ReaderViewAction_DeleteNote,
  ReaderViewAction_ToggleFullscreen,
  ReaderViewAction_ToggleDistractionFree,
  /* Explicit Cancel button: host clears its committed reader selection. */
  ReaderViewAction_CancelNote,
} ReaderViewActionKind;

typedef struct ReaderViewAction
{
  ReaderViewActionKind kind;
  ReaderViewKey key;
  ReaderViewKey auxiliary_key;
  ReaderViewSettingKind setting_kind;
  ReaderViewRightRowKind right_row_kind;
  ReaderViewRightFilter right_filter;
  UI0U64 value;
  ReaderViewText text;
} ReaderViewAction;

typedef enum ReaderViewTextStyle
{
  ReaderViewTextStyle_Default,
  ReaderViewTextStyle_ChromeTitle,
  ReaderViewTextStyle_ChromeMetadata,
  ReaderViewTextStyle_MenuItem,
  ReaderViewTextStyle_NoteEditor,
} ReaderViewTextStyle;

typedef struct ReaderViewTextBinding
{
  UI0ID source_id;
  ReaderViewText text;
  ReaderViewTextStyle style;
  /* Optional byte range for host-measured in-line match highlighting. */
  UI0U32 match_start;
  UI0U32 match_size;
} ReaderViewTextBinding;

typedef enum ReaderViewSemanticRole
{
  ReaderViewSemantic_Toolbar,
  ReaderViewSemantic_Panel,
  ReaderViewSemantic_Dialog,
  ReaderViewSemantic_Group,
  ReaderViewSemantic_Button,
  ReaderViewSemantic_ToggleButton,
  ReaderViewSemantic_SearchBox,
  ReaderViewSemantic_TextArea,
  ReaderViewSemantic_Slider,
  ReaderViewSemantic_Tab,
  ReaderViewSemantic_List,
  ReaderViewSemantic_ListItem,
  ReaderViewSemantic_Menu,
  ReaderViewSemantic_MenuItem,
  ReaderViewSemantic_Status,
} ReaderViewSemanticRole;

typedef UI0U32 ReaderViewSemanticFlags;
enum
{
  ReaderViewSemantic_None      = 0,
  ReaderViewSemantic_Enabled   = 1u << 0,
  ReaderViewSemantic_Focusable = 1u << 1,
  ReaderViewSemantic_Focused   = 1u << 2,
  ReaderViewSemantic_Selected  = 1u << 3,
  ReaderViewSemantic_Checked   = 1u << 4,
  ReaderViewSemantic_Expanded  = 1u << 5,
  ReaderViewSemantic_Current   = 1u << 6,
  ReaderViewSemantic_Busy      = 1u << 7,
  ReaderViewSemantic_Offscreen = 1u << 8,
  ReaderViewSemantic_ReadOnly  = 1u << 9,
};

typedef enum ReaderViewSemanticControl
{
  ReaderViewSemanticControl_None,
  ReaderViewSemanticControl_Open,
  ReaderViewSemanticControl_Contents,
  ReaderViewSemanticControl_Find,
  ReaderViewSemanticControl_HistoryBack,
  ReaderViewSemanticControl_HistoryForward,
  ReaderViewSemanticControl_Fullscreen,
  ReaderViewSemanticControl_DistractionFree,
  ReaderViewSemanticControl_FontSize,
  ReaderViewSemanticControl_LineSpacing,
  ReaderViewSemanticControl_FontFamily,
  ReaderViewSemanticControl_Theme,
  ReaderViewSemanticControl_Annotations,
  ReaderViewSemanticControl_Bookmark,
  ReaderViewSemanticControl_PreviousPage,
  ReaderViewSemanticControl_NextPage,
  ReaderViewSemanticControl_Progress,
  ReaderViewSemanticControl_LeftContentsTab,
  ReaderViewSemanticControl_LeftFindTab,
  ReaderViewSemanticControl_LeftPanelClose,
  ReaderViewSemanticControl_TocRow,
  ReaderViewSemanticControl_FindInput,
  ReaderViewSemanticControl_FindClear,
  ReaderViewSemanticControl_FindRow,
  ReaderViewSemanticControl_RightFilter,
  ReaderViewSemanticControl_RightExport,
  ReaderViewSemanticControl_RightPanelClose,
  ReaderViewSemanticControl_RightFilterOption,
  ReaderViewSemanticControl_RightRow,
  ReaderViewSemanticControl_RightRowStar,
  ReaderViewSemanticControl_RightRowMenu,
  ReaderViewSemanticControl_RightActionGoTo,
  ReaderViewSemanticControl_RightActionToggleStar,
  ReaderViewSemanticControl_RightActionEditNote,
  ReaderViewSemanticControl_RightActionDelete,
} ReaderViewSemanticControl;

typedef struct ReaderViewSemanticNode
{
  UI0ID id;
  UI0ID parent_id;
  ReaderViewSemanticRole role;
  ReaderViewSemanticFlags flags;
  UI0Rect rect;
  ReaderViewText name;
  ReaderViewText value;
  ReaderViewSemanticControl control;
  ReaderViewKey source_key;
  UI0U64 range_value;
  UI0U64 range_min;
  UI0U64 range_max;
} ReaderViewSemanticNode;

typedef UI0U32 ReaderViewFrameErrorFlags;
enum
{
  ReaderViewFrameError_None               = 0,
  ReaderViewFrameError_BadInput           = 1u << 0,
  ReaderViewFrameError_StaleDocumentState = 1u << 1,
  ReaderViewFrameError_StaleNoteRevision  = 1u << 2,
  ReaderViewFrameError_ProjectionCap      = 1u << 3,
  ReaderViewFrameError_RecordCap          = 1u << 4,
  ReaderViewFrameError_ActionCap          = 1u << 5,
  ReaderViewFrameError_DuplicateKey       = 1u << 6,
  ReaderViewFrameError_InvalidText        = 1u << 7,
  ReaderViewFrameError_InvalidMatch       = 1u << 8,
  ReaderViewFrameError_InvalidFilter      = 1u << 9,
  ReaderViewFrameError_InvalidChoice      = 1u << 10,
  ReaderViewFrameError_InvalidProgress    = 1u << 11,
  ReaderViewFrameError_InvalidAttachment  = 1u << 12,
};

typedef UI0U32 ReaderViewFrameChangeFlags;
enum
{
  ReaderViewFrameChange_None              = 0,
  ReaderViewFrameChange_StateChanged      = 1u << 0,
  ReaderViewFrameChange_LayoutChanged     = 1u << 1,
  ReaderViewFrameChange_ProjectionInvalid = 1u << 2,
  ReaderViewFrameChange_FocusChanged      = 1u << 3,
};

typedef struct ReaderViewFrameStorage
{
  UI0LayoutBox layout_boxes[READER_VIEW_LAYOUT_BOX_CAP];
  UI0SignalRecord signal_records[READER_VIEW_SIGNAL_CAP];
  UI0ControlRecord control_records[READER_VIEW_CONTROL_CAP];
  UI0DrawCommand draw_commands[READER_VIEW_DRAW_COMMAND_CAP];
  UI0PopupRootRecord popup_records[READER_VIEW_POPUP_CAP];
  UI0PanelRecord panel_records[READER_VIEW_PANEL_CAP];
  UI0ToolbarSlotRecord toolbar_slots[READER_VIEW_TOOLBAR_SLOT_CAP];
  UI0ListRowRecord list_records[READER_VIEW_LIST_RECORD_CAP];
  UI0SidenavRecord sidenav_records[READER_VIEW_SIDENAV_RECORD_CAP];
  UI0TextInputRecord text_input_records[READER_VIEW_TEXT_INPUT_CAP];
  UI0TextAreaRecord text_area_records[READER_VIEW_TEXT_AREA_CAP];
  UI0SliderRecord slider_records[READER_VIEW_SLIDER_CAP];
  UI0ScrollRecord scroll_records[READER_VIEW_SCROLL_CAP];
  ReaderViewTextBinding text_bindings[READER_VIEW_TEXT_BINDING_CAP];
  ReaderViewSemanticNode semantic_nodes[READER_VIEW_SEMANTIC_NODE_CAP];
  ReaderViewAction actions[READER_VIEW_ACTION_CAP];
  /* API 3 recovery storage is append-only after every earlier field. */
  char right_filter_labels[4][READER_VIEW_RIGHT_FILTER_LABEL_CAP];
  UI0SidenavRecord
    reference_sidenav_records[READER_VIEW_REFERENCE_SIDENAV_RECORD_CAP];
  /* API 3 frozen inline-note TextArea evidence remains caller-storage owned. */
  UI0TextAreaRowRecord
    note_text_area_row_records[READER_VIEW_TEXT_AREA_ROW_CAP];
  UI0TextAreaSelectionRecord
    note_text_area_selection_records[READER_VIEW_TEXT_AREA_SELECTION_CAP];
  UI0TextAreaRow
    note_text_area_layout_rows[READER_VIEW_TEXT_AREA_ROW_CAP];
} ReaderViewFrameStorage;

typedef struct ReaderViewBuildInput
{
  UI0U64 frame_index;
  ReaderViewState *state;
  const ReaderViewLayout *layout;
  const ReaderViewProjection *projection;
  const ReaderViewInput *input;
  const UI0ResolvedTheme *theme;
  ReaderViewFindTextMetrics find_text_metrics;
  ReaderViewNoteTextMetrics note_text_metrics;
} ReaderViewBuildInput;

typedef struct ReaderViewFrame
{
  ReaderViewLayout layout;
  const UI0DrawCommand *draw_commands;
  UI0S32 draw_command_count;
  const ReaderViewTextBinding *text_bindings;
  UI0S32 text_binding_count;
  const ReaderViewSemanticNode *semantic_nodes;
  UI0S32 semantic_node_count;
  const ReaderViewAction *actions;
  UI0S32 action_count;
  ReaderViewFrameErrorFlags error_flags;
  ReaderViewFrameChangeFlags change_flags;
} ReaderViewFrame;

ReaderViewLabels reader_view_default_english_labels(void);

void reader_view_state_init(ReaderViewState *state);
/*
 * A matching document key, including zero, is the same document identity and
 * preserves transient interaction state across builds. The first nonzero
 * projection may bind a zero-key state automatically. Callers must explicitly
 * reset before every transition away from a nonzero document key.
 */
void reader_view_state_reset_document(ReaderViewState *state,
                                      UI0U64 document_key);
ReaderViewText reader_view_find_query(const ReaderViewState *state);
ReaderViewText reader_view_note_draft(const ReaderViewState *state);

UI0B32 reader_view_resolve_layout(const ReaderViewState *state,
                                  const ReaderViewLayoutInput *input,
                                  ReaderViewLayout *out_layout);
ReaderViewContentGeometryStyle reader_view_default_content_geometry_style(void);
/*
 * Resolves caller-owned value geometry only. A null style selects the public
 * defaults. On failure, non-null out_geometry storage is zeroed.
 */
UI0B32 reader_view_resolve_content_geometry(
  UI0Rect viewport_rect,
  const ReaderViewContentGeometryStyle *style,
  ReaderViewContentGeometry *out_geometry);

void reader_view_frame_storage_init(ReaderViewFrameStorage *storage);

UI0B32 reader_view_build(const ReaderViewBuildInput *input,
                         ReaderViewFrameStorage *storage,
                         ReaderViewFrame *out_frame);

UI0B32 reader_view_accessibility_focus(ReaderViewState *state,
                                       UI0ID semantic_id);

UI0B32 reader_view_accessibility_invoke(ReaderViewState *state,
                                        UI0ID semantic_id);

UI0B32 reader_view_open_note_editor(
  ReaderViewState *state,
  const ReaderViewSelectionProjection *selection);

/* Host acknowledgement after a successful Save/Delete note action. */
UI0B32 reader_view_close_note_editor(ReaderViewState *state);

#endif /* READERVIEW0_READER_VIEW_H */

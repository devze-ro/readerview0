#ifndef READERVIEW0_READER_VIEW_H
#define READERVIEW0_READER_VIEW_H

enum
{
  READER_VIEW_FIND_QUERY_CAP = 128,
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
  READER_VIEW_TEXT_INPUT_CAP = 1,
  READER_VIEW_TEXT_AREA_CAP = 1,
  READER_VIEW_SLIDER_CAP = 2,
  READER_VIEW_SCROLL_CAP = 4,
  READER_VIEW_TEXT_BINDING_CAP = 640,
  READER_VIEW_SEMANTIC_NODE_CAP = 384,
  READER_VIEW_DEFAULT_PAGE_HORIZONTAL_INSET = 24,
  READER_VIEW_DEFAULT_PAGE_MAX_WIDTH = 660,
  READER_VIEW_DEFAULT_PAGE_MIN_WIDTH = 160,
  READER_VIEW_DEFAULT_CONTENT_INSET_X = 52,
  READER_VIEW_DEFAULT_CONTENT_INSET_Y = 68,
  READER_VIEW_DEFAULT_CONTENT_MIN_WIDTH = 80,
  READER_VIEW_DEFAULT_CONTENT_MIN_HEIGHT = 48,
};

typedef struct ReaderViewText
{
  const char *data;
  UI0S32 size;
} ReaderViewText;

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
  ReaderViewRightAction_ToggleStar = 1u << 1,
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
} ReaderViewLabels;

typedef struct ReaderViewProjection
{
  UI0U64 document_key;
  ReaderViewFeatureFlags features;
  ReaderViewDocumentFlags document_flags;
  ReaderViewSurfaceStatus content;
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
  UI0Rect viewport_rect;
  UI0Rect previous_gutter_rect;
  UI0Rect next_gutter_rect;
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

typedef struct ReaderViewTextBinding
{
  UI0ID source_id;
  ReaderViewText text;
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

typedef struct ReaderViewSemanticNode
{
  UI0ID id;
  UI0ID parent_id;
  ReaderViewSemanticRole role;
  ReaderViewSemanticFlags flags;
  UI0Rect rect;
  ReaderViewText name;
  ReaderViewText value;
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
} ReaderViewFrameStorage;

typedef struct ReaderViewBuildInput
{
  UI0U64 frame_index;
  ReaderViewState *state;
  const ReaderViewLayout *layout;
  const ReaderViewProjection *projection;
  const ReaderViewInput *input;
  const UI0ResolvedTheme *theme;
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

#endif /* READERVIEW0_READER_VIEW_H */

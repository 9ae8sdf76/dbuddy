#include <stdio.h>
#include <time.h>
#include "ui.h"
#include "styles.h"

void create_display(void);

void create_obj_left_top_container_day(void);
void create_obj_left_top_container_month(void);
void create_obj_left_top_container_year(void);
void create_obj_top_container_actions(void);
void create_obj_top_container_time(void);

void create_page_left_bottom_itinerary(void);
void create_page_left_bottom_main(void);
void create_page_left_main(void);
void create_page_left_top_container(void);
void create_page_left_top_main(void);
void create_page_main(void);
void create_page_right_calendar(void);
void create_page_right_main(void);
void create_page_settings(void);
void create_page_top_container(void);
void create_page_top_main(void);

void create_screen(void);

lv_coord_t get_next_row_pos(lv_obj_t * obj, int padding);

void cb_action_settings_event_handler(lv_obj_t *obj, lv_event_t event);
void cb_calendar_event_handler(lv_obj_t *obj, lv_event_t event);
void cb_list_btn_general(lv_obj_t * obj, lv_event_t event);
void cb_list_btn_calendars(lv_obj_t * obj, lv_event_t event);
void cb_settings_win_close(lv_obj_t * obj, lv_event_t event);
void cb_time_task_handler(lv_task_t * task);
void update_calendar(ui_t ui);
void update_time(ui_t ui);

static ui_t * p_Ui = NULL;

ui_t *ui_init(void) {
    static bool ui_initialized = false;

    if (ui_initialized) {
        LV_LOG_WARN("ui_init: already inited");
        return p_Ui;
    }

    p_Ui = lv_mem_alloc(sizeof(ui_t));

    styles_init();

    create_display();
    create_screen();
    create_page_main();
    create_page_top_main();
    create_page_right_main();
    create_page_top_container();
    create_obj_top_container_time();
    create_obj_top_container_actions();
    create_page_left_main();
    create_page_left_top_main();
    create_page_left_top_container();
    create_obj_left_top_container_month();
    create_obj_left_top_container_day();
    create_obj_left_top_container_year();
    create_page_left_bottom_main();
    create_page_left_bottom_itinerary();
    create_page_right_calendar();

    ui_initialized = true;
    LV_LOG_INFO("ui_init ready");

    return p_Ui;
}

void cb_action_settings_event_handler(lv_obj_t *obj, lv_event_t event) {
    if (event == LV_EVENT_PRESSED) {
        create_page_settings();

        if (p_Ui->settings.main) {
            lv_win_ext_t *win_ext = lv_obj_get_ext_attr(p_Ui->settings.main);
            p_Ui->settings.header_offset = lv_obj_get_height_fit(win_ext->header);

            p_Ui->settings.list = lv_list_create(p_Ui->settings.main, NULL);
            lv_obj_set_size(p_Ui->settings.list, DEFAULT_LIST_WIDTH, lv_obj_get_height_fit(p_Ui->settings.main) - p_Ui->settings.header_offset);
            lv_obj_add_style(p_Ui->settings.list, LV_LIST_PART_BG, &style_default_border_none);
            lv_obj_add_style(p_Ui->settings.list, LV_LIST_PART_BG, &style_default_background_transparent_full);
            lv_obj_add_style(p_Ui->settings.list, LV_LIST_PART_BG, &style_default_border_sides_only_right);
            lv_obj_add_style(p_Ui->settings.list, LV_LIST_PART_BG, &style_default_border_color_white);
            lv_obj_add_style(p_Ui->settings.list, LV_LIST_PART_BG, &style_default_border_thin);
            lv_obj_add_style(p_Ui->settings.list, LV_LIST_PART_BG, &style_focused_border_color_white);

            lv_obj_t * btn_general = lv_list_add_btn(p_Ui->settings.list, NULL, "General");
            lv_obj_add_style(btn_general, LV_BTN_PART_MAIN, &style_default_background_transparent_full);
            lv_obj_set_event_cb(btn_general, cb_list_btn_general);

            lv_obj_t * btn_calendars = lv_list_add_btn(p_Ui->settings.list, NULL, "Calendars");
            lv_obj_add_style(btn_calendars, LV_BTN_PART_MAIN, &style_default_border_none);
            lv_obj_add_style(btn_calendars, LV_BTN_PART_MAIN, &style_default_background_transparent_full);
            lv_obj_set_event_cb(btn_calendars, cb_list_btn_calendars);
        }
    }
}

void cb_calendar_event_handler(lv_obj_t *obj, lv_event_t event) {
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_calendar_date_t *date = lv_calendar_get_pressed_date(obj);

        if (date) {
            p_Ui->date.month = date->month;
            p_Ui->date.day = date->day;
            p_Ui->date.year = date->year;

            update_calendar(*p_Ui);
        }
    }
}

void cb_list_btn_general(lv_obj_t * obj, lv_event_t event) {
    if (event == LV_EVENT_PRESSED) {
        if (p_Ui->settings.calendars && lv_debug_check_obj_valid(p_Ui->settings.calendars)) {
            lv_obj_del(p_Ui->settings.calendars);
            p_Ui->settings.calendars = NULL;
        }

        p_Ui->settings.general = lv_page_create(p_Ui->settings.main, NULL);
        lv_obj_set_size(p_Ui->settings.general, lv_obj_get_width_fit(p_Ui->settings.main) - DEFAULT_LIST_WIDTH, lv_obj_get_height_fit(p_Ui->settings.main) - p_Ui->settings.header_offset);
        lv_obj_set_pos(p_Ui->settings.general, DEFAULT_LIST_WIDTH, 0);
        lv_obj_add_style(p_Ui->settings.general, LV_STATE_DEFAULT, &style_default_border_none);
        lv_obj_add_style(p_Ui->settings.general, LV_STATE_DEFAULT, &style_default_background_transparent_full);
        lv_obj_add_style(p_Ui->settings.general, LV_STATE_DEFAULT, &style_default_padding_default);

        lv_obj_t * heading_time = lv_obj_create(p_Ui->settings.general, NULL);
        lv_obj_set_width(heading_time, lv_obj_get_width_fit(p_Ui->settings.general));
        lv_obj_add_style(heading_time, LV_OBJ_PART_MAIN, &style_default_border_sides_only_bottom);
        lv_obj_add_style(heading_time, LV_OBJ_PART_MAIN, &style_default_border_thin);
        lv_obj_add_style(heading_time, LV_OBJ_PART_MAIN, &style_default_radius_none);
        lv_obj_add_style(heading_time, LV_OBJ_PART_MAIN, &style_default_background_transparent_full);

        lv_obj_t * heading_label = lv_label_create(heading_time, NULL);
        lv_label_set_text(heading_label, "Time");

        lv_obj_set_height(heading_time, lv_obj_get_height(heading_label));

        lv_obj_t * row_format = lv_label_create(p_Ui->settings.general, NULL);
        lv_label_set_long_mode(row_format, LV_LABEL_LONG_BREAK);
        lv_obj_set_width(row_format, lv_obj_get_width_fit(p_Ui->settings.general));
        lv_label_set_text(row_format, "24-hour Formatting (e.g., 16:00)");
        lv_obj_set_pos(row_format, 0, get_next_row_pos(heading_time, DEFAULT_PADDING));

        lv_obj_t * format_toggle = lv_switch_create(p_Ui->settings.general, NULL);
        lv_obj_align(format_toggle, row_format, LV_ALIGN_IN_RIGHT_MID, 0, 0);

        lv_obj_t * row_meridiem = lv_label_create(p_Ui->settings.general, NULL);
        lv_label_set_long_mode(row_meridiem, LV_LABEL_LONG_BREAK);
        lv_obj_set_width(row_meridiem, lv_obj_get_width_fit(p_Ui->settings.general));
        lv_label_set_text(row_meridiem, "Use Meridiem (e.g., am/pm)");
        lv_obj_set_pos(row_meridiem, 0, get_next_row_pos(row_format, DEFAULT_PADDING + DEFAULT_PADDING));

        lv_obj_t * meridiem_toggle = lv_switch_create(p_Ui->settings.general, NULL);
        lv_obj_align(meridiem_toggle, row_meridiem, LV_ALIGN_IN_RIGHT_MID, 0, 0);

        lv_obj_t * row_flash_colon = lv_label_create(p_Ui->settings.general, NULL);
        lv_label_set_long_mode(row_flash_colon, LV_LABEL_LONG_BREAK);
        lv_obj_set_width(row_flash_colon, lv_obj_get_width_fit(p_Ui->settings.general));
        lv_label_set_text(row_flash_colon, "Flash the Time Separators");
        lv_obj_set_pos(row_flash_colon, 0, get_next_row_pos(row_meridiem, DEFAULT_PADDING + DEFAULT_PADDING));

        lv_obj_t * flash_colon_toggle = lv_switch_create(p_Ui->settings.general, NULL);
        lv_obj_align(flash_colon_toggle, row_flash_colon, LV_ALIGN_IN_RIGHT_MID, 0, 0);

        lv_obj_t * row_screensaver = lv_label_create(p_Ui->settings.general, NULL);
        lv_label_set_long_mode(row_screensaver, LV_LABEL_LONG_BREAK);
        lv_obj_set_width(row_screensaver, lv_obj_get_width_fit(p_Ui->settings.general));
        lv_label_set_text(row_screensaver, "Time as Screensaver");
        lv_obj_set_pos(row_screensaver, 0, get_next_row_pos(row_flash_colon, DEFAULT_PADDING + DEFAULT_PADDING));

        lv_obj_t * screensaver_toggle = lv_switch_create(p_Ui->settings.general, NULL);
        lv_obj_align(screensaver_toggle, row_screensaver, LV_ALIGN_IN_RIGHT_MID, 0, 0);
    }
}

void cb_list_btn_calendars(lv_obj_t * obj, lv_event_t event) {
    if (event == LV_EVENT_PRESSED) {
        if (p_Ui->settings.general && lv_debug_check_obj_valid(p_Ui->settings.general)) {
            lv_obj_del(p_Ui->settings.general);
            p_Ui->settings.general = NULL;
        }

        p_Ui->settings.calendars = lv_page_create(p_Ui->settings.main, NULL);
        lv_obj_set_size(p_Ui->settings.calendars, lv_obj_get_width_fit(p_Ui->settings.main) - DEFAULT_LIST_WIDTH, lv_obj_get_height_fit(p_Ui->settings.main) - p_Ui->settings.header_offset);
        lv_obj_set_pos(p_Ui->settings.calendars, DEFAULT_LIST_WIDTH, 0);
        lv_obj_add_style(p_Ui->settings.calendars, LV_STATE_DEFAULT, &style_default_border_none);
        lv_obj_add_style(p_Ui->settings.calendars, LV_STATE_DEFAULT, &style_default_background_transparent_full);
    }
}

void cb_settings_win_close(lv_obj_t * obj, lv_event_t event) {
    if (event == LV_EVENT_RELEASED) {
        if (p_Ui->settings.general && lv_debug_check_obj_valid(p_Ui->settings.general)) {
            lv_obj_del(p_Ui->settings.general);
        }

        if (p_Ui->settings.calendars && lv_debug_check_obj_valid(p_Ui->settings.calendars)) {
            lv_obj_del(p_Ui->settings.calendars);
        }

        p_Ui->settings.general = NULL;
        p_Ui->settings.calendars = NULL;
    }

    lv_win_close_event_cb(obj, event);
}

void cb_time_task_handler(lv_task_t * task) {
    ui_t ui = *(ui_t *)(task->user_data);
    update_time(ui);
}

void create_display(void) {
    p_Ui->display = lv_disp_get_default();

    LV_IMG_DECLARE(tft_background_art)
    lv_disp_set_bg_image(p_Ui->display, &tft_background_art);
}

void create_obj_left_top_container_day(void) {
    p_Ui->page.left.top.container_day = lv_obj_create(p_Ui->page.left.top.container, p_Ui->page.main);
    lv_obj_set_size(p_Ui->page.left.top.container_day, lv_obj_get_width_fit(p_Ui->page.left.top.container) / 3, lv_obj_get_height_fit(p_Ui->page.left.top.container));
    lv_obj_set_pos(p_Ui->page.left.top.container_day, lv_obj_get_width_fit(p_Ui->page.left.top.container_month), 0);

    p_Ui->page.left.top.label_day = lv_label_create(p_Ui->page.left.top.container_day, NULL);
    lv_label_set_text(p_Ui->page.left.top.label_day, "");
    lv_obj_set_auto_realign(p_Ui->page.left.top.label_day, true);
    lv_obj_align(p_Ui->page.left.top.label_day, p_Ui->page.left.top.container_day, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style(p_Ui->page.left.top.label_day, LV_LABEL_PART_MAIN, &style_default_font_black_large_72);
}

void create_obj_left_top_container_month(void) {
    p_Ui->page.left.top.container_month = lv_obj_create(p_Ui->page.left.top.container, p_Ui->page.main);
    lv_obj_set_size(p_Ui->page.left.top.container_month, lv_obj_get_width_fit(p_Ui->page.left.top.container) / 3, lv_obj_get_height_fit(p_Ui->page.left.top.container));

    p_Ui->page.left.top.label_month = lv_label_create(p_Ui->page.left.top.container_month, NULL);
    lv_label_set_text(p_Ui->page.left.top.label_month, "");
    lv_obj_set_auto_realign(p_Ui->page.left.top.label_month, true);
    lv_obj_align(p_Ui->page.left.top.label_month, p_Ui->page.left.top.container_month, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style(p_Ui->page.left.top.label_month, LV_LABEL_PART_MAIN, &style_default_font_black_medium);
}

void create_obj_left_top_container_year(void) {
    p_Ui->page.left.top.container_year = lv_obj_create(p_Ui->page.left.top.container, p_Ui->page.main);
    lv_obj_set_size(p_Ui->page.left.top.container_year, lv_obj_get_width_fit(p_Ui->page.left.top.container) / 3, lv_obj_get_height_fit(p_Ui->page.left.top.container));
    lv_obj_set_pos(p_Ui->page.left.top.container_year, lv_obj_get_width_fit(p_Ui->page.left.top.container_month) + lv_obj_get_width_fit(p_Ui->page.left.top.container_day), 0);

    p_Ui->page.left.top.label_year = lv_label_create(p_Ui->page.left.top.container_year, NULL);
    lv_label_set_text(p_Ui->page.left.top.label_year, "");
    lv_obj_set_auto_realign(p_Ui->page.left.top.label_year, true);
    lv_label_set_long_mode(p_Ui->page.left.top.label_year, LV_LABEL_LONG_BREAK);
    lv_obj_set_size(p_Ui->page.left.top.label_year, lv_obj_get_width_fit(p_Ui->page.left.top.container_year), lv_obj_get_height(p_Ui->page.left.top.container_year));
    lv_label_set_align(p_Ui->page.left.top.label_year, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(p_Ui->page.left.top.label_year, p_Ui->page.left.top.container_year, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style(p_Ui->page.left.top.label_year, LV_LABEL_PART_MAIN, &style_default_font_black_medium);
}

void create_obj_top_container_actions(void) {
    p_Ui->page.top.container_actions = lv_obj_create(p_Ui->page.top.container, p_Ui->page.top.main);

    lv_obj_set_size(p_Ui->page.top.container_actions, lv_obj_get_width_fit(p_Ui->page.top.container) / 2, lv_obj_get_height_fit(p_Ui->page.top.main));
    lv_obj_set_pos(p_Ui->page.top.container_actions, lv_obj_get_width(p_Ui->page.top.container_time), 0);

    p_Ui->page.top.btn_action_settings = lv_btn_create(p_Ui->page.top.container_actions, NULL);
    lv_obj_set_size(p_Ui->page.top.btn_action_settings, lv_obj_get_width_fit(p_Ui->page.top.container_actions) / 12, lv_obj_get_height_fit(p_Ui->page.top.container_actions));
    lv_obj_align(p_Ui->page.top.btn_action_settings, p_Ui->page.top.container_actions, LV_ALIGN_IN_RIGHT_MID, 0, 0);
    lv_obj_reset_style_list(p_Ui->page.top.btn_action_settings, LV_BTN_PART_MAIN);

    lv_obj_t * label = lv_label_create(p_Ui->page.top.btn_action_settings, NULL);
    lv_label_set_text(label, LV_SYMBOL_SETTINGS);
    lv_obj_set_auto_realign(label, true);
    lv_obj_align(label, p_Ui->page.top.container_actions, LV_ALIGN_IN_RIGHT_MID, -DEFAULT_PADDING / 2, 0);
    lv_obj_add_style(label, LV_LABEL_PART_MAIN, &style_default_font_regular_small);

    lv_obj_set_style_local_text_color(p_Ui->page.top.btn_action_settings, LV_LABEL_PART_MAIN, LV_STATE_PRESSED, LV_COLOR_AQUA);
    lv_obj_set_event_cb(p_Ui->page.top.btn_action_settings, cb_action_settings_event_handler);
}

void create_obj_top_container_time(void) {
    p_Ui->page.top.container_time = lv_obj_create(p_Ui->page.top.container, p_Ui->page.top.main);
    lv_obj_set_size(p_Ui->page.top.container_time, lv_obj_get_width_fit(p_Ui->page.top.main) / 2, lv_obj_get_height_fit(p_Ui->page.top.main));

    p_Ui->page.top.label_time = lv_label_create(p_Ui->page.top.container_time, NULL);
    lv_label_set_text(p_Ui->page.top.label_time, "");
    lv_obj_set_auto_realign(p_Ui->page.top.label_time, true);
    lv_obj_align(p_Ui->page.top.label_time, p_Ui->page.top.container_time, LV_ALIGN_IN_LEFT_MID, DEFAULT_PADDING / 2, 0);
    lv_obj_add_style(p_Ui->page.top.label_time, LV_LABEL_PART_MAIN, &style_default_font_regular_small);

    lv_task_create(cb_time_task_handler, 500, LV_TASK_PRIO_MID, p_Ui);
}

void create_page_left_bottom_itinerary(void) {
    p_Ui->page.left.bottom.itinerary = lv_obj_create(p_Ui->page.left.bottom.main, p_Ui->page.main);

    lv_obj_set_size(p_Ui->page.left.bottom.itinerary, lv_page_get_width_fit(p_Ui->page.left.bottom.main), lv_page_get_height_fit(p_Ui->page.left.bottom.main));
    lv_obj_add_style(p_Ui->page.left.bottom.itinerary, LV_PAGE_PART_BG, &style_default_background_overlay_color_black);
}

void create_page_left_bottom_main(void) {
    p_Ui->page.left.bottom.main = lv_page_create(p_Ui->page.left.main, p_Ui->page.main);

    lv_obj_set_pos(p_Ui->page.left.bottom.main, 0, lv_obj_get_height(p_Ui->page.left.top.main));
    lv_obj_set_size(p_Ui->page.left.bottom.main, lv_obj_get_width(p_Ui->page.left.main), lv_obj_get_height(p_Ui->page.left.main) - lv_obj_get_height(p_Ui->page.left.top.main));
    lv_obj_set_style_local_pad_top(p_Ui->page.left.bottom.main, LV_PAGE_PART_BG, LV_STATE_DEFAULT, 0);
    lv_obj_add_style(p_Ui->page.left.bottom.main, LV_PAGE_PART_BG, &style_default_padding_default);
}

void create_page_left_main(void) {
    p_Ui->page.left.main = lv_obj_create(p_Ui->page.main, p_Ui->page.main);

    lv_obj_set_size(p_Ui->page.left.main, (LV_HOR_RES * 1 / 3), LV_VER_RES - lv_obj_get_height(p_Ui->page.top.main));
    lv_obj_align(p_Ui->page.left.main, p_Ui->page.main, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
}

void create_page_left_top_container(void) {
    p_Ui->page.left.top.container = lv_obj_create(p_Ui->page.left.top.main, p_Ui->page.main);

    lv_obj_set_size(p_Ui->page.left.top.container, lv_page_get_width_fit(p_Ui->page.left.top.main), lv_page_get_height_fit(p_Ui->page.left.top.main));
    lv_obj_add_style(p_Ui->page.left.top.container, LV_PAGE_PART_BG, &style_default_background_overlay_color_black);
}

void create_page_left_top_main(void) {
    p_Ui->page.left.top.main = lv_page_create(p_Ui->page.left.main, p_Ui->page.main);

    lv_obj_set_size(p_Ui->page.left.top.main, lv_obj_get_width(p_Ui->page.left.main), (lv_obj_get_width(p_Ui->page.left.main) * 2 / 3) - DEFAULT_PADDING * 2);
    lv_obj_add_style(p_Ui->page.left.top.main, LV_PAGE_PART_BG, &style_default_padding_default);
}

void create_page_main(void) {
    p_Ui->page.main = lv_page_create(p_Ui->screen, NULL);

    lv_obj_set_size(p_Ui->page.main, LV_HOR_RES, LV_VER_RES);
    lv_obj_add_style(p_Ui->page.main, LV_PAGE_PART_BG, &style_default_border_none);
    lv_obj_add_style(p_Ui->page.main, LV_PAGE_PART_BG, &style_default_padding_none);
    lv_obj_add_style(p_Ui->page.main, LV_PAGE_PART_BG, &style_default_background_transparent_full);
}

void create_page_right_calendar(void) {
    p_Ui->page.right.container = lv_obj_create(p_Ui->page.right.main, p_Ui->page.main);
    lv_obj_set_size(p_Ui->page.right.container, lv_obj_get_width_fit(p_Ui->page.right.main), lv_obj_get_height_fit(p_Ui->page.right.main));
    lv_obj_add_style(p_Ui->page.right.container, LV_PAGE_PART_BG, &style_default_padding_default);
    lv_obj_add_style(p_Ui->page.right.container, LV_PAGE_PART_BG, &style_default_padding_left_none);

    p_Ui->page.right.calendar = lv_calendar_create(p_Ui->page.right.container, NULL);
    lv_obj_set_size(p_Ui->page.right.calendar, lv_obj_get_width_fit(p_Ui->page.right.container), lv_obj_get_height_fit(p_Ui->page.right.container));
    lv_obj_align(p_Ui->page.right.calendar, p_Ui->page.right.main, LV_ALIGN_IN_LEFT_MID, 0, 0);
    lv_obj_add_style(p_Ui->page.right.calendar, LV_CALENDAR_PART_BG, &style_default_background_overlay_color_black);
    lv_obj_add_style(p_Ui->page.right.calendar, LV_CALENDAR_PART_BG, &style_default_border_none);
    lv_obj_add_style(p_Ui->page.right.calendar, LV_CALENDAR_PART_DATE, &style_focused_background_blend_mode_additive);
    lv_obj_add_style(p_Ui->page.right.calendar, LV_CALENDAR_PART_DATE, &style_focused_background_color_purple);
    lv_obj_add_style(p_Ui->page.right.calendar, LV_CALENDAR_PART_DATE, &style_focused_text_color_black);

    lv_obj_set_style_local_text_font(p_Ui->page.right.calendar, LV_CALENDAR_PART_HEADER, LV_STATE_DEFAULT, &roboto_black_24);
    lv_obj_set_style_local_text_font(p_Ui->page.right.calendar, LV_CALENDAR_PART_DAY_NAMES, LV_STATE_DEFAULT, &roboto_black_16);
    lv_obj_set_style_local_text_font(p_Ui->page.right.calendar, LV_CALENDAR_PART_BG, LV_STATE_DEFAULT, &roboto_regular_12);

    lv_obj_set_event_cb(p_Ui->page.right.calendar, cb_calendar_event_handler);

    update_calendar(*p_Ui);
}

void create_page_right_main(void) {
    p_Ui->page.right.main = lv_obj_create(p_Ui->page.main, p_Ui->page.main);

    lv_obj_set_size(p_Ui->page.right.main, (LV_HOR_RES * 2 / 3), LV_VER_RES - lv_obj_get_height(p_Ui->page.top.main));
    lv_obj_align(p_Ui->page.right.main, p_Ui->page.main, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
}

void create_page_settings(void) {
        p_Ui->settings.main = lv_win_create(p_Ui->screen, NULL);
        lv_obj_set_size(p_Ui->settings.main, lv_page_get_width_fit(p_Ui->page.main) - DEFAULT_MENU_HEIGHT - DEFAULT_PADDING, lv_page_get_height_fit(p_Ui->page.main) - DEFAULT_MENU_HEIGHT - DEFAULT_PADDING);
        lv_obj_align(p_Ui->settings.main, p_Ui->screen, LV_ALIGN_CENTER, 0, 0);

        lv_obj_add_style(p_Ui->settings.main, LV_WIN_PART_HEADER, &style_default_background_color_black);
        lv_obj_add_style(p_Ui->settings.main, LV_WIN_PART_HEADER, &style_default_border_color_white);
        lv_obj_add_style(p_Ui->settings.main, LV_WIN_PART_HEADER, &style_default_border_sides_only_bottom);
        lv_obj_add_style(p_Ui->settings.main, LV_WIN_PART_HEADER, &style_default_border_thin);
        lv_obj_add_style(p_Ui->settings.main, LV_WIN_PART_HEADER, &style_default_font_regular_small);
        lv_obj_add_style(p_Ui->settings.main, LV_WIN_PART_HEADER, &style_default_padding_top_none);
        lv_obj_add_style(p_Ui->settings.main, LV_WIN_PART_HEADER, &style_default_padding_right_none);
        lv_obj_add_style(p_Ui->settings.main, LV_WIN_PART_HEADER, &style_default_padding_bottom_none);
        lv_obj_add_style(p_Ui->settings.main, LV_WIN_PART_BG, &style_default_background_color_black);
        lv_obj_add_style(p_Ui->settings.main, LV_WIN_PART_BG, &style_default_radius_normal);
        lv_obj_add_style(p_Ui->settings.main, LV_WIN_PART_CONTENT_SCROLLABLE, &style_default_padding_none);

        lv_win_set_title(p_Ui->settings.main, "Settings");

        lv_obj_t * btn_close = lv_win_add_btn(p_Ui->settings.main, LV_SYMBOL_CLOSE);

        lv_obj_set_event_cb(btn_close, cb_settings_win_close);
}

void create_page_top_container(void) {
    p_Ui->page.top.container = lv_obj_create(p_Ui->page.top.main, p_Ui->page.main);

    lv_obj_set_size(p_Ui->page.top.container, lv_obj_get_width_fit(p_Ui->page.top.main), lv_obj_get_height_fit(p_Ui->page.top.main));
}

void create_page_top_main(void) {
    p_Ui->page.top.main = lv_obj_create(p_Ui->page.main, p_Ui->page.main);

    lv_obj_set_size(p_Ui->page.top.main, LV_HOR_RES, DEFAULT_MENU_HEIGHT);
    lv_obj_add_style(p_Ui->page.top.main, LV_PAGE_PART_BG, &style_default_background_overlay_color_black);
    lv_obj_add_style(p_Ui->page.top.main, LV_PAGE_PART_BG, &style_default_radius_none);
}

void create_screen(void) {
    p_Ui->screen = lv_scr_act();

    lv_obj_add_style(p_Ui->screen, LV_OBJMASK_PART_MAIN, &style_default_background_transparent_full);
}

lv_coord_t get_next_row_pos(lv_obj_t * obj, int padding) {
    return lv_obj_get_y(obj) + lv_obj_get_height(obj) + padding;
}

void update_calendar(ui_t ui) {
    lv_calendar_date_t date = p_Ui->date;
    time_t t = time(NULL);
    struct tm local = *localtime(&t);

    if (date.year == 0 && date.month == 0 && date.day == 0) {
        date.year = local.tm_year + 1900;
        date.day = local.tm_mday;
        date.month = local.tm_mon + 1;
    }

    lv_calendar_set_today_date(ui.page.right.calendar, &date);
    lv_calendar_set_showed_date(ui.page.right.calendar, &date);

    char bufDay[4], bufMonth[4], bufYear[3], bufCentury[3], selected[11];

    sprintf(selected, "%d-%d-%d", date.year, date.month, date.day);
    strptime(selected, "%Y-%m-%d", &local);

    strftime(bufDay, sizeof(bufDay), "%a", &local);
    strftime(bufMonth, sizeof(bufMonth), "%b", &local);
    strftime(bufYear, sizeof(bufYear), "%y", &local);
    strftime(bufCentury, sizeof(bufCentury), "%C", &local);

    lv_label_set_text_fmt(ui.page.left.top.label_month, "%s\n%s", bufDay, bufMonth);
    lv_label_set_text_fmt(ui.page.left.top.label_day, "%02u", local.tm_mday);
    lv_label_set_text_fmt(ui.page.left.top.label_year, "%s\n%s", bufCentury, bufYear);
}

void update_time(ui_t ui) {
    time_t t = time(NULL);
    struct tm local = *localtime(&t);
    char bufMeridiem[3];
    int tmHour = local.tm_hour;

    if (local.tm_hour >= 12) {
        sprintf(bufMeridiem, "%s", "pm");
    } else {
        sprintf(bufMeridiem, "%s", "am");
    }

    if (tmHour > 12) {
        tmHour = tmHour - 12;
    }

    static int count_on = 0, count_off = 0;
    char * colon = strstr(lv_label_get_text(ui.page.top.label_time), ":");

    if ((count_on % 2) == 0 && colon == NULL) {
        lv_label_set_text_fmt(ui.page.top.label_time, "%02u:%02u %s", tmHour, local.tm_min, bufMeridiem);

        count_on = 0;
    } else if ((count_off % 2) == 0) {
        lv_label_set_text_fmt(ui.page.top.label_time, "%02u %02u %s", tmHour, local.tm_min, bufMeridiem);

        count_off = 0;
    }

    count_off++;
    count_on++;
}

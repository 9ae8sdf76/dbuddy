#include <lvgl.h>
#include <unistd.h>
#include <cstdio>

#include <Arduino.h>
#include <WiFi.h>

#include "dbuddy.h"
#include "esp32dev.h"
#include "widgets.h"

using namespace dbuddy;

extern "C" {
    void cb_time_task_handler(lv_task_t * task);
    lv_calendar_date_t * get_calendar_date(Hal *);

#if LV_MEM_CUSTOM == 0
    void cb_memory_monitor_task_handler(lv_task_t * param);
#endif

    void cb_wifi_task_handler(lv_task_t * param);
}

DBuddy * DBuddy::instance = nullptr;

void DBuddy::setup(Hal * hal, Ui * ui, Config * config, bool use_dbl_buff, lv_indev_type_t input_type) {
    instance = new DBuddy(hal, ui);
    instance->config = config;
    instance->init(use_dbl_buff, input_type);
    instance->config->begin();
}

void DBuddy::loop() {
    lv_task_handler();

    Config * config = instance->get_config();
    config->loop();

    usleep(5000);
}

void DBuddy::init(bool use_dbl_buff, lv_indev_type_t input_type) {
    hal->init();
    hal->run(use_dbl_buff, input_type);

    ui->add_widget(WIDGET_SCREEN, new Screen(ui));
    ui->add_widget(WIDGET_PAGE, new Page(ui));
    ui->add_widget(WIDGET_MENU, new Menu(ui));
    ui->add_widget(WIDGET_TIME_CONTAINER, new TimeContainer(ui));
    ui->add_widget(WIDGET_TIME_LABEL, new TimeLabel(ui));
    ui->add_widget(WIDGET_ACTIONS_CONTAINER, new ActionsContainer(ui));
    ui->add_widget(WIDGET_ACTION_SETTINGS, new ActionSettings(ui));
    ui->add_widget(WIDGET_CALENDAR_DATE, new CalendarDate(ui));
    ui->add_widget(WIDGET_CALENDAR_DETAILS, new CalendarDetails(ui));
    ui->add_widget(WIDGET_CALENDAR, new Calendar(ui));
    ui->add_widget(WIDGET_WIFI_SIGNAL, new WifiSignal(ui));

    ui->create_task(cb_time_task_handler, 500, LV_TASK_PRIO_MID, this);
#if LV_MEM_CUSTOM == 0
    ui->create_task(cb_memory_monitor_task_handler, 5000);
#endif
    ui->create_task(cb_wifi_task_handler, 500);

    initialize_calendar();
}

void DBuddy::initialize_calendar() {
    lv_calendar_date_t * today = get_calendar_date(hal);

    auto * calendar = (Calendar *) ui->get_widget(WIDGET_CALENDAR);
    calendar->set_today(today);
    calendar->set_showed(today);
}

lv_calendar_date_t * get_calendar_date(Hal * hal) {
    auto * today = new lv_calendar_date_t;
    today->year = hal->get_year();
    today->month = hal->get_month();
    today->day = hal->get_day();

    return today;
}

void cb_time_task_handler(lv_task_t * task) {
    auto * db = (DBuddy *) task->user_data;
    Hal * hal = db->get_hal();
    Ui * ui = db->get_ui();

    lv_obj_t * time_label = ui->get_widget(WIDGET_TIME_LABEL)->get_self();

    if (hal->get_hours() == 0 && hal->get_minutes() == 0 && hal->get_seconds() == 0) {
        lv_obj_t * calendar = ui->get_widget(WIDGET_CALENDAR)->get_self();
        lv_calendar_date_t * today = get_calendar_date(hal);
        lv_calendar_set_today_date(calendar, today);
    }

    char bufMeridiem[3] = "";
    int tmHour = hal->get_hours();

    if (tmHour >= 12) {
        sprintf(bufMeridiem, "%s", "pm");
    } else {
        sprintf(bufMeridiem, "%s", "am");
    }

    if (tmHour > 12) {
        tmHour -= 12;
    }

    if (tmHour == 0) {
        tmHour += 12;
    }

    static int count = 0;
    char * colon = strstr(lv_label_get_text(time_label), ":");

    if (((count % 2) == 0 && colon == nullptr)/* || (p_config->time.flash == 0)*/) {
        lv_label_set_text_fmt(time_label, "%02u:%02u %s", tmHour, hal->get_minutes(), bufMeridiem);
        count = 0;
    } else if ((count % 2) == 0) {
        lv_label_set_text_fmt(time_label, "%02u %02u %s", tmHour, hal->get_minutes(), bufMeridiem);
        count = 0;
    }

    ++count;
}

void cb_wifi_task_handler(lv_task_t * param) {
    auto * ui = (Ui *)param->user_data;
    Widget * wifi_signal = ui->get_widget(WIDGET_WIFI_SIGNAL);

    switch (WiFiClass::status()) {
        case WL_CONNECTED:
            wifi_signal->remove_style(LV_OBJ_PART_MAIN, ui->get_styles()->get_text_color_gray(LV_STATE_DEFAULT));
            wifi_signal->add_style(LV_OBJ_PART_MAIN, ui->get_styles()->get_text_color_white(LV_STATE_DEFAULT));
            break;
        default: // Disconnected, never connected, etc
            wifi_signal->remove_style(LV_OBJ_PART_MAIN, ui->get_styles()->get_text_color_white(LV_STATE_DEFAULT));
            wifi_signal->add_style(LV_OBJ_PART_MAIN, ui->get_styles()->get_text_color_gray(LV_STATE_DEFAULT));
            break;
    }
}

#if LV_MEM_CUSTOM == 0
/**
 * Print the memory usage periodically
 * @param param
 */
void cb_memory_monitor_task_handler(lv_task_t * param) {
    (void) param; /*Unused*/

    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);

    Serial.printf("used: %6d (%3d %%), frag: %3d %%, biggest free: %6d\n",
                  (int) mon.total_size - mon.free_size, mon.used_pct, mon.frag_pct,
                  (int) mon.free_biggest_size);
}
#endif

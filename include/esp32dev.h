#pragma once

#include <Adafruit_RA8875.h>
#include <RTClib.h>
#include <TouchScreen.h>

#include "config.h"
#include "hal.h"

#define DBUDDY_ESP32DEV_CORE_0 (0)
#define DBUDDY_ESP32DEV_CORE_1 (1)

namespace dbuddy {
    class ESP32Dev : public Hal {
    public:
        ESP32Dev(Adafruit_RA8875 * tft, TouchScreen * ts, Config * config, uint8_t interrupt) : Hal(), tft(tft), ts(ts), config(config), interrupt(interrupt) {}

        void init() final;

        void display_flush(lv_disp_drv_t *, const lv_area_t *, lv_color_t *) final;
        bool input_read(lv_indev_drv_t *, lv_indev_data_t *) final;

        int get_year() const final;
        int get_month() const final;
        int get_day() const final;
        int get_hours() const final;
        int get_minutes() const final;
        int get_seconds() const final;

        template<typename Func>
        TaskHandle_t * execute_on_core(Func const &, const char *, uint32_t, void *, BaseType_t);

        RTC_DS3231 * get_rtc() const { return rtc; }
    private:
        Adafruit_RA8875 * tft;
        TouchScreen * ts;
        Config * config;

        uint8_t interrupt;

        RTC_DS3231 * rtc = new RTC_DS3231();
    };
}

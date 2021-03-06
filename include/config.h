#pragma once

#include <ConfigManager.h>

#define CONFIG_DEFAULT_TIMESERVER ("time.nist.gov")
#define CONFIG_DEFAULT_TIMEZONE ("America/New_York")

// The property length is used by ArduinoJson when calculating the capacity.
#define CONFIG_PROPERTY_LENGTH (5)
#define CONFIG_CALENDARS_PROPERTY_LENGTH (3)

namespace dbuddy {
    const uint8_t CALENDARS = 8;

    typedef struct configuration_calendar_t {
        char name[100];
        char color[10];
        char url[200];
    } config_cal_t;

    typedef struct configuration_t {
        char timeserver[200];
        char timezone[32];
        unsigned int timeinterval;

        uint8_t calendars;
        config_cal_t calendar[CALENDARS];
    } config_t;

    typedef struct metadata_t {
        uint8_t version;
    } meta_t;

    class Config {
    public:
        explicit Config(const char *, uint16_t);

        char * timeserver() const;
        uint32_t timeinterval() const;
        char * timezone() const;

        uint8_t version() const;

        void timeserver(const char *);
        void timeinterval(uint32_t);
        void timezone(const char *);

        uint8_t add_calendar(config_cal_t &);

        void clear_calendars();
        config_cal_t get_calendar(uint8_t) const;
        const config_cal_t * get_calendars() const;
        bool has_calendar(uint8_t) const;
        uint8_t length_calendars() const;

        void begin();
        bool isStation();
        bool isAccessPoint();
        void loop();
        void save();

        void streamFile(const char* file, const char mime[]);
    private:
        config_t config{};
        meta_t meta{};

        ConfigManager manager{};
    };
}

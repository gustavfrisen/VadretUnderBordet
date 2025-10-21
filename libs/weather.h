#ifndef WEATHER_H
#define WEATHER_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    double latitude;
    double longitude;
    double generationtime_ms;
    int utc_offset_seconds;
    char* timezone;
    char* timezone_abbreviation;
    double elevation;

    char* unit_time;
    char* unit_interval;
    char* unit_windspeed;
    char* unit_winddirection;
    char* unit_is_day;
    char* unit_weathercode;

    char* time;
    int interval;
    double temperature;
    double windspeed;
    int winddirection;
    int is_day;
    int weathercode;
} weather_t;

// Memory management
weather_t* weather_create(void);
int weather_free(weather_t* w);
int weather_reset(weather_t* w);  // sets all fields to 0 or NULL

// JSON
int weather_from_json(const char* json, weather_t* w);
int weather_to_json(const weather_t* w, char* buffer, size_t size);

// Validation
int weather_is_valid(const weather_t* w);

// String conversion
int string_to_weather(const weather_t* w, char* buffer, size_t bufferSize);
int weather_to_string(const char* data, weather_t* w);

// Utilities
int weather_update(weather_t* dest, const weather_t* src);
int weather_compare(const weather_t* a, const weather_t* b);
int weather_to_imperial(weather_t* w);  // °C → °F, m/s → mph
int weather_to_metric(weather_t* w);    // °F → °C, mph → m/s
int weather_print(const weather_t* w);
int weather_print_pretty(const weather_t* w);

#endif // WEATHER_H
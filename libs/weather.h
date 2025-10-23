#ifndef WEATHER_H
#define WEATHER_H

#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>

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

// Interface functions
int weather_get(const char* city_name, weather_t* w);

// String conversion
int string_to_weather(weather_t* w, const char* buffer, size_t bufferSize);
int weather_to_string(const weather_t* w, char* buffer, size_t bufferSize);

// JSON conversion
int weather_from_json(json_t* json, weather_t* w);
int weather_to_json(const char* cityName, const weather_t* w);

// Utilities
int weather_is_stale(const weather_t* w, int max_age_seconds);

//int weather_to_imperial(weather_t* w);  // °C → °F, m/s → mph
//int weather_to_metric(weather_t* w);    // °F → °C, mph → m/s

int weather_print(const weather_t* w);
int weather_print_pretty(const weather_t* w);

#endif // WEATHER_H
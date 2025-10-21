#ifndef WEATHER_H
#define WEATHER_H

#include <stdio.h>
#include <stdlib.h>

struct weather_t {
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
}

int write_weather_to_file(const weather_t* w, const char* filename);
int load_weather_from_file(weather_t* w, const char* filename);
int weather_to_string(const char* data, weather_t* w);
int parse_weather_string(const weather_t* w, char* buffer, size_t bufferSize);
int weather_print(const weather_t* w);

#endif // WEATHER_H
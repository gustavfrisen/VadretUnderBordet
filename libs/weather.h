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

    // Units for current_units
    char* unit_time;
    char* unit_interval;
    char* unit_temperature_2m;
    char* unit_relative_humidity_2m;
    char* unit_apparent_temperature;
    char* unit_is_day;
    char* unit_precipitation;
    char* unit_rain;
    char* unit_showers;
    char* unit_snowfall;
    char* unit_weather_code;
    char* unit_cloud_cover;
    char* unit_pressure_msl;
    char* unit_surface_pressure;
    char* unit_wind_speed_10m;
    char* unit_wind_direction_10m;
    char* unit_wind_gusts_10m;

    // Current weather data
    char* time;
    int interval;
    double temperature_2m;
    int relative_humidity_2m;
    double apparent_temperature;
    int is_day;
    double precipitation;
    double rain;
    double showers;
    double snowfall;
    int weather_code;
    int cloud_cover;
    double pressure_msl;
    double surface_pressure;
    double wind_speed_10m;
    int wind_direction_10m;
    double wind_gusts_10m;
} weather_t;

// ========== Cache Management Functions ==========
int does_weather_cache_exist(const char* city_name);
int is_weather_cache_stale(const char* city_name, int max_age_seconds);
int load_weather_from_cache(const char* city_name, char** jsonStr);
int save_weather_to_cache(const char* city_name, const char* jsonStr);

// ========== Serialization Functions ==========
int process_openmeteo_response(const char* api_response, char** client_response); // Used by server to process API response
int deserialize_weather_response(const char* client_response, weather_t* weather); // Used by client to parse response

void free_weather(weather_t* weather);

#endif // WEATHER_H
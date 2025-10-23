#include <time.h>

#include "weather.h"
#include "utils.h"

int weather_get(const char *city_name, weather_t *w)
{
    create_folder("weather_cache");
    
    if (!city_name || !w) return -1;
    
    char filename[256];
    snprintf(filename, sizeof(filename), "weather_cache/%s.json", city_name);
    
    json_error_t error;
    json_t *city_json = json_load_file(filename, 0, &error);
    
    if (!city_json) {
        fprintf(stderr, "Error loading JSON from file %s: %s\n", filename, error.text);
        return -1;
    }
    else
    {
        weather_from_json(city_json, w);
        if (weather_is_stale(w, w->interval) == 0) {
            return 0;
        }
        else {
            return -1;
        }

        json_decref(city_json);
    }
    
    return 0;
}

int string_to_weather(weather_t *w, const char *buffer, size_t bufferSize)
{
    if (!w || !buffer || bufferSize == 0) return -1;

    json_error_t error;
    json_t* json = json_loads(buffer, 0, &error);
    
    if (!json) {
        fprintf(stderr, "Error parsing JSON string: %s\n", error.text);
        return -1;
    }

    int result = weather_from_json(json, w);
    json_decref(json);
    
    return result;
}

int weather_to_string(const weather_t *w, char *buffer, size_t bufferSize)
{
    if (!w || !buffer || bufferSize == 0) return -1;

    // Build the JSON structure
    json_t* root = json_object();
    
    // Top-level fields
    json_object_set_new(root, "latitude", json_real(w->latitude));
    json_object_set_new(root, "longitude", json_real(w->longitude));
    json_object_set_new(root, "generationtime_ms", json_real(w->generationtime_ms));
    json_object_set_new(root, "utc_offset_seconds", json_integer(w->utc_offset_seconds));
    json_object_set_new(root, "timezone", json_string(w->timezone ? w->timezone : ""));
    json_object_set_new(root, "timezone_abbreviation", json_string(w->timezone_abbreviation ? w->timezone_abbreviation : ""));
    json_object_set_new(root, "elevation", json_real(w->elevation));

    // current_weather_units object
    json_t* units = json_object();
    json_object_set_new(units, "time", json_string(w->unit_time ? w->unit_time : ""));
    json_object_set_new(units, "interval", json_string(w->unit_interval ? w->unit_interval : ""));
    json_object_set_new(units, "temperature", json_string(w->unit_windspeed ? w->unit_windspeed : ""));
    json_object_set_new(units, "windspeed", json_string(w->unit_windspeed ? w->unit_windspeed : ""));
    json_object_set_new(units, "winddirection", json_string(w->unit_winddirection ? w->unit_winddirection : ""));
    json_object_set_new(units, "is_day", json_string(w->unit_is_day ? w->unit_is_day : ""));
    json_object_set_new(units, "weathercode", json_string(w->unit_weathercode ? w->unit_weathercode : ""));
    json_object_set_new(root, "current_weather_units", units);

    // current_weather object
    json_t* current = json_object();
    json_object_set_new(current, "time", json_string(w->time ? w->time : ""));
    json_object_set_new(current, "interval", json_integer(w->interval));
    json_object_set_new(current, "temperature", json_real(w->temperature));
    json_object_set_new(current, "windspeed", json_real(w->windspeed));
    json_object_set_new(current, "winddirection", json_integer(w->winddirection));
    json_object_set_new(current, "is_day", json_integer(w->is_day));
    json_object_set_new(current, "weathercode", json_integer(w->weathercode));
    json_object_set_new(root, "current_weather", current);

    // Convert to string
    char* json_str = json_dumps(root, JSON_INDENT(4));
    json_decref(root);
    
    if (!json_str) return -1;
    
    // Check if buffer is large enough
    size_t json_len = strlen(json_str);
    if (json_len >= bufferSize) {
        free(json_str);
        return -1;
    }
    
    // Copy to buffer
    strncpy(buffer, json_str, bufferSize - 1);
    buffer[bufferSize - 1] = '\0';
    free(json_str);
    
    return 0;
}

int weather_from_json(json_t* json, weather_t* w)
{
    if (!json || !w) return -1;

    // Parse top-level fields
    json_t* lat_json = json_object_get(json, "latitude");
    if (json_is_number(lat_json)) {
        w->latitude = json_number_value(lat_json);
    }

    json_t* lon_json = json_object_get(json, "longitude");
    if (json_is_number(lon_json)) {
        w->longitude = json_number_value(lon_json);
    }

    json_t* gen_json = json_object_get(json, "generationtime_ms");
    if (json_is_number(gen_json)) {
        w->generationtime_ms = json_number_value(gen_json);
    }

    json_t* utc_json = json_object_get(json, "utc_offset_seconds");
    if (json_is_integer(utc_json)) {
        w->utc_offset_seconds = json_integer_value(utc_json);
    }

    json_t* tz_json = json_object_get(json, "timezone");
    if (json_is_string(tz_json)) {
        const char* tz = json_string_value(tz_json);
        w->timezone = strdup(tz);
    }

    json_t* tz_abbr_json = json_object_get(json, "timezone_abbreviation");
    if (json_is_string(tz_abbr_json)) {
        const char* tz_abbr = json_string_value(tz_abbr_json);
        w->timezone_abbreviation = strdup(tz_abbr);
    }

    json_t* elev_json = json_object_get(json, "elevation");
    if (json_is_number(elev_json)) {
        w->elevation = json_number_value(elev_json);
    }

    // Parse current_weather_units object
    json_t* units = json_object_get(json, "current_weather_units");
    if (json_is_object(units)) {
        json_t* unit_time = json_object_get(units, "time");
        if (json_is_string(unit_time)) {
            w->unit_time = strdup(json_string_value(unit_time));
        }

        json_t* unit_interval = json_object_get(units, "interval");
        if (json_is_string(unit_interval)) {
            w->unit_interval = strdup(json_string_value(unit_interval));
        }

        json_t* unit_windspeed = json_object_get(units, "windspeed");
        if (json_is_string(unit_windspeed)) {
            w->unit_windspeed = strdup(json_string_value(unit_windspeed));
        }

        json_t* unit_winddirection = json_object_get(units, "winddirection");
        if (json_is_string(unit_winddirection)) {
            w->unit_winddirection = strdup(json_string_value(unit_winddirection));
        }

        json_t* unit_is_day = json_object_get(units, "is_day");
        if (json_is_string(unit_is_day)) {
            w->unit_is_day = strdup(json_string_value(unit_is_day));
        }

        json_t* unit_weathercode = json_object_get(units, "weathercode");
        if (json_is_string(unit_weathercode)) {
            w->unit_weathercode = strdup(json_string_value(unit_weathercode));
        }
    }

    // Parse current_weather object
    json_t* current = json_object_get(json, "current_weather");
    if (json_is_object(current)) {
        json_t* time_json = json_object_get(current, "time");
        if (json_is_string(time_json)) {
            w->time = strdup(json_string_value(time_json));
        }

        json_t* interval_json = json_object_get(current, "interval");
        if (json_is_integer(interval_json)) {
            w->interval = json_integer_value(interval_json);
        }

        json_t* temp_json = json_object_get(current, "temperature");
        if (json_is_number(temp_json)) {
            w->temperature = json_number_value(temp_json);
        }

        json_t* windspeed_json = json_object_get(current, "windspeed");
        if (json_is_number(windspeed_json)) {
            w->windspeed = json_number_value(windspeed_json);
        }

        json_t* winddir_json = json_object_get(current, "winddirection");
        if (json_is_integer(winddir_json)) {
            w->winddirection = json_integer_value(winddir_json);
        }

        json_t* is_day_json = json_object_get(current, "is_day");
        if (json_is_integer(is_day_json)) {
            w->is_day = json_integer_value(is_day_json);
        }

        json_t* weathercode_json = json_object_get(current, "weathercode");
        if (json_is_integer(weathercode_json)) {
            w->weathercode = json_integer_value(weathercode_json);
        }
    }

    return 0;
}

int weather_to_json(const char* cityName, const weather_t* w)
{
    if (!cityName || !w) return -1;

    create_folder("weather_cache");

    // Build the JSON structure
    json_t* root = json_object();
    
    // Top-level fields
    json_object_set_new(root, "latitude", json_real(w->latitude));
    json_object_set_new(root, "longitude", json_real(w->longitude));
    json_object_set_new(root, "generationtime_ms", json_real(w->generationtime_ms));
    json_object_set_new(root, "utc_offset_seconds", json_integer(w->utc_offset_seconds));
    json_object_set_new(root, "timezone", json_string(w->timezone ? w->timezone : ""));
    json_object_set_new(root, "timezone_abbreviation", json_string(w->timezone_abbreviation ? w->timezone_abbreviation : ""));
    json_object_set_new(root, "elevation", json_real(w->elevation));

    // current_weather_units object
    json_t* units = json_object();
    json_object_set_new(units, "time", json_string(w->unit_time ? w->unit_time : ""));
    json_object_set_new(units, "interval", json_string(w->unit_interval ? w->unit_interval : ""));
    json_object_set_new(units, "temperature", json_string(w->unit_windspeed ? w->unit_windspeed : ""));
    json_object_set_new(units, "windspeed", json_string(w->unit_windspeed ? w->unit_windspeed : ""));
    json_object_set_new(units, "winddirection", json_string(w->unit_winddirection ? w->unit_winddirection : ""));
    json_object_set_new(units, "is_day", json_string(w->unit_is_day ? w->unit_is_day : ""));
    json_object_set_new(units, "weathercode", json_string(w->unit_weathercode ? w->unit_weathercode : ""));
    json_object_set_new(root, "current_weather_units", units);

    // current_weather object
    json_t* current = json_object();
    json_object_set_new(current, "time", json_string(w->time ? w->time : ""));
    json_object_set_new(current, "interval", json_integer(w->interval));
    json_object_set_new(current, "temperature", json_real(w->temperature));
    json_object_set_new(current, "windspeed", json_real(w->windspeed));
    json_object_set_new(current, "winddirection", json_integer(w->winddirection));
    json_object_set_new(current, "is_day", json_integer(w->is_day));
    json_object_set_new(current, "weathercode", json_integer(w->weathercode));
    json_object_set_new(root, "current_weather", current);

    // Save to file
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "weather_cache/%s.json", cityName);
    
    if (json_dump_file(root, filepath, JSON_INDENT(4)) != 0) {
        json_decref(root);
        return -1;
    }

    json_decref(root);
    return 0;
}

int weather_is_stale(const weather_t* w, int max_age_seconds)
{
    if (!w || !w->time) return -1;

    // Parse ISO8601 timestamp: "2025-10-07T18:15"
    struct tm weather_time = {0};
    if (sscanf(w->time, "%d-%d-%dT%d:%d",
               &weather_time.tm_year,
               &weather_time.tm_mon,
               &weather_time.tm_mday,
               &weather_time.tm_hour,
               &weather_time.tm_min) != 5) {
        return -1; // Parse error
    }
    
    // Adjust tm_year (years since 1900) and tm_mon (0-11)
    weather_time.tm_year -= 1900;
    weather_time.tm_mon -= 1;
    weather_time.tm_isdst = -1;
    
    // Convert to time_t
    time_t weather_timestamp = mktime(&weather_time);
    if (weather_timestamp == -1) {
        return -1; // Conversion error
    }
    
    // Get current time
    time_t current_time = time(NULL);
    
    // Calculate age in seconds
    double age_seconds = difftime(current_time, weather_timestamp);
    
    // Return 1 if stale (age > max_age), 0 if fresh
    return (age_seconds > max_age_seconds) ? 1 : 0;
}

int weather_print(const weather_t *w)
{
    if (!w) return -1;
    printf("Weather for (%.4f, %.4f):\n", w->latitude, w->longitude);
    printf("Temperature: %.2f °C\n", w->temperature);
    printf("Windspeed: %.2f m/s\n", w->windspeed);
    printf("Wind Direction: %d°\n", w->winddirection);
    printf("Weather Code: %d\n", w->weathercode);
    return 0;
}

int weather_print_pretty(const weather_t *w)
{
    if (!w) return -1;
    printf("Weather Information:\n");
    printf("Location: (%.4f, %.4f)\n", w->latitude, w->longitude);
    printf("Temperature: %.2f °C\n", w->temperature);
    printf("Windspeed: %.2f m/s\n", w->windspeed);
    printf("Wind Direction: %d°\n", w->winddirection);
    printf("Weather Code: %d\n", w->weathercode);
    return 0;
}
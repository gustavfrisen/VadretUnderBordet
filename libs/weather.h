#ifndef WEATHER_H
#define WEATHER_H

#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>

/**
 * @brief Structure representing weather data for a location
 * 
 * Contains all weather information including location, units, and current conditions.
 */
typedef struct {
    double latitude;                  /**< Latitude coordinate */
    double longitude;                 /**< Longitude coordinate */
    double generationtime_ms;         /**< Time taken to generate the data (milliseconds) */
    int utc_offset_seconds;           /**< UTC offset in seconds */
    char* timezone;                   /**< Timezone string (e.g., "GMT") */
    char* timezone_abbreviation;      /**< Timezone abbreviation */
    double elevation;                 /**< Elevation in meters */

    char* unit_time;                  /**< Unit for time field */
    char* unit_interval;              /**< Unit for interval field */
    char* unit_windspeed;             /**< Unit for wind speed */
    char* unit_winddirection;         /**< Unit for wind direction */
    char* unit_is_day;                /**< Unit for is_day field */
    char* unit_weathercode;           /**< Unit for weather code */

    char* time;                       /**< ISO8601 timestamp (e.g., "2025-10-07T18:15") */
    int interval;                     /**< Update interval in seconds */
    double temperature;               /**< Current temperature */
    double windspeed;                 /**< Current wind speed */
    int winddirection;                /**< Wind direction in degrees */
    int is_day;                       /**< Day indicator (1 = day, 0 = night) */
    int weathercode;                  /**< WMO weather code */
} weather_t;

/**
 * @brief Load weather data from disk cache for a city
 * 
 * Attempts to load cached weather data from weather_cache/<city_name>.json.
 * Checks if the data is stale based on the interval field.
 * 
 * @param city_name Name of the city
 * @param w Pointer to weather_t struct to populate
 * @return 0 if fresh data loaded successfully, -1 if file not found or data is stale
 */
int weather_get(const char* city_name, weather_t* w);

/**
 * @brief Save weather struct to JSON file in cache directory
 * 
 * Creates a JSON file in weather_cache/<cityName>.json with all weather data.
 * The file is formatted with indentation for readability.
 * Creates the weather_cache directory if it doesn't exist.
 * 
 * @param cityName Name of the city (used as filename)
 * @param w Pointer to weather_t struct to save
 * @return 0 on success, -1 on error
 */
int weather_to_json(const char* cityName, const weather_t* w);

/**
 * @brief Parse JSON string and populate weather struct
 * 
 * Takes a JSON string buffer and parses it into the weather_t structure.
 * Uses jansson library to parse the JSON format.
 * 
 * @param w Pointer to weather_t struct to populate
 * @param buffer JSON string to parse
 * @param bufferSize Size of the buffer
 * @return 0 on success, -1 on error
 */
int string_to_weather(weather_t* w, const char* buffer, size_t bufferSize);

/**
 * @brief Convert weather struct to JSON string
 * 
 * Serializes the weather_t structure into a formatted JSON string.
 * The output includes proper nesting of units and current weather objects.
 * 
 * @param w Pointer to weather_t struct to convert
 * @param buffer Output buffer for JSON string
 * @param bufferSize Size of the output buffer
 * @return 0 on success, -1 if buffer too small or on error
 */
int weather_to_string(const weather_t* w, char* buffer, size_t bufferSize);

/**
 * @brief Check if weather data is stale
 * 
 * Compares the timestamp in the weather data with the current time.
 * Uses ISO8601 timestamp parsing to determine age.
 * 
 * @param w Pointer to weather_t struct to check
 * @param max_age_seconds Maximum age in seconds before data is considered stale
 * @return 1 if stale, 0 if fresh, -1 on error
 */
int weather_is_stale(const weather_t* w, int max_age_seconds);

/**
 * @brief Print weather data to stdout
 * 
 * Prints basic weather information including location, temperature,
 * wind speed, wind direction, and weather code.
 * 
 * @param w Pointer to weather_t struct to print
 * @return 0 on success, -1 on error
 */
int weather_print(const weather_t* w);

/**
 * @brief Print weather data in a formatted, readable way
 * 
 * Prints weather information with labels and better formatting
 * for improved readability.
 * 
 * @param w Pointer to weather_t struct to print
 * @return 0 on success, -1 on error
 */
int weather_print_pretty(const weather_t* w);

#endif // WEATHER_H
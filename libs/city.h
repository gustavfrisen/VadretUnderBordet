#ifndef CITY_H
#define CITY_H

/**
 * @brief Represents a city with a name and geographic coordinates.
 */
typedef struct city_t
{
	char* name;
	float latitude;
	float longitude;
} city_t;

/**
 * @brief Create and initialize a city object.
 *
 * Parses latitude and longitude from strings and allocates a new city_t.
 * On success, stores the allocated pointer in _CityPtr.
 *
 * @param _Name       City name (UTF-8). Must be non-NULL.
 * @param _Latitude   Latitude as string (e.g., "59.3293"). Must be non-NULL.
 * @param _Longitude  Longitude as string (e.g., "18.0686"). Must be non-NULL.
 * @param _CityPtr    Output pointer receiving the allocated city_t* on success.
 * @return 0 on success, -1 on error (e.g., allocation/parse failure). On error, *_CityPtr is set to NULL.
 */
int city_init(const char* _Name, const char* _Latitude, const char* _Longitude, city_t** _CityPtr);

/**
 * @brief Dispose a city object and set its pointer to NULL.
 *
 * Frees the memory associated with the given city (including name) and
 * sets *_cityPtr to NULL to avoid dangling pointers. Safe to call with
 * NULL or with *_cityPtr == NULL (no-op).
 *
 * @param _cityPtr Pointer to the city_t* to dispose and nullify.
 */
void city_dispose(city_t** _cityPtr);

#endif
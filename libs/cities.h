#ifndef CITIES_H
#define CITIES_H

#include "linked_list.h"
#include "city.h"

/**
 * @brief Structure representing a collection of cities
 */
typedef struct cities_t
{
	LinkedList list; /**< Linked list containing city_t items */
	
} cities_t;

/**
 * @brief Initialize a cities collection
 * 
 * Allocates memory for a new cities_t structure, loads cities from disk,
 * loads any missing cities from the hardcoded list, and saves to disk.
 * 
 * @param cities_ptr Pointer to a cities_t pointer that will be initialized
 * @return 0 on success, -1 on failure
 */
int cities_init(cities_t** cities_ptr);

/**
 * @brief Find a city by its name
 * 
 * @param cities Pointer to the cities collection
 * @param name Name of the city to find
 * @param city_ptr Pointer to store the found city (can be NULL)
 * @return 0 if city found, -1 if not found or on error
 */
int cities_get_city_by_name(cities_t* cities, const char* name, city_t** city_ptr);

/**
 * @brief Get a city by its index in the list
 * 
 * @param cities Pointer to the cities collection
 * @param index Zero-based index of the city
 * @param city_ptr Pointer to store the found city (can be NULL)
 * @return 0 on success, -1 on error
 */
int cities_get_city_by_index(cities_t* cities, int index, city_t** city_ptr);

/**
 * @brief Sort cities alphabetically by name
 * 
 * Uses bubble sort to arrange cities in alphabetical order.
 * 
 * @param cities Pointer to the cities collection
 * @return 0 on success, -1 on error
 */
int cities_sort_by_name(cities_t* cities);

/**
 * @brief Sort cities by distance from a reference city
 * 
 * Sorts cities based on their distance from the specified city.
 * Uses squared Euclidean distance for comparison.
 * 
 * @param cities Pointer to the cities collection
 * @param city_name Name of the reference city to measure distances from
 * @return 0 on success, -1 if city not found or on error
 */
int cities_sort_by_distance(cities_t *cities, const char *city_name);

/**
 * @brief Print all cities with their coordinates
 * 
 * Prints each city's name, latitude, and longitude to stdout.
 * 
 * @param cities Pointer to the cities collection
 * @return 0 on success, -1 on error
 */
int cities_print(cities_t* cities);

/**
 * @brief Print all cities in a formatted, readable way
 * 
 * Prints each city with labeled fields and separators for better readability.
 * 
 * @param cities Pointer to the cities collection
 * @return 0 on success, -1 on error
 */
int cities_print_pretty(cities_t* cities);

/**
 * @brief Dispose of a cities collection and free memory
 * 
 * Frees all memory associated with the cities collection, including
 * all city objects in the list.
 * 
 * @param cities_ptr Pointer to the cities_t pointer to dispose
 * @return 0 on success, -1 on error
 */
int cities_dispose(cities_t** cities_ptr);

#endif
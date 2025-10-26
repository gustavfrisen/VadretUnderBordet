#ifndef CITIES_H
#define CITIES_H

#include "linked_list.h"
#include "city.h"

typedef struct cities_t
{
	LinkedList list;
	
} cities_t;

// ========== Initialization ==========
int cities_init(cities_t** cities_ptr);

// ========== City Management ==========
int cities_add_city(cities_t* cities, city_t* city);
int cities_remove_city(cities_t* cities, const char* name);

// ========== Retrieval & Sorting ==========
int cities_get_city_by_name(cities_t* cities, const char* name, city_t** city_ptr);
int cities_get_city_by_index(cities_t* cities, int index, city_t** city_ptr);

int cities_sort_by_name(cities_t* cities);
int cities_sort_by_distance(cities_t *cities, const char *city_name);

// ========== Printing ==========
int cities_print(cities_t* cities);
int cities_print_pretty(cities_t* cities);

// ========== Memory Management ==========
int cities_dispose(cities_t** cities_ptr);

#endif
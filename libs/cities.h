#ifndef CITIES_H
#define CITIES_H

#include "linked_list.h"
#include "city.h"

typedef struct cities_t
{
	LinkedList list;
	
} cities_t;

int cities_init(cities_t** cities_ptr);

int cities_load_from_disk(cities_t* cities);
int cities_save_to_disk(cities_t* cities);
int cities_load_from_string_list(cities_t* cities, const char* string_list);

int cities_get_city_by_name(cities_t* cities, const char* name, city_t** city_ptr);
int cities_get_city_by_index(cities_t* cities, int index, city_t** city_ptr);

int cities_print(cities_t* cities);
int cities_print_pretty(cities_t* cities);

int cities_reset(cities_t* cities);
int cities_dispose(cities_t** cities_ptr);
int cities_destroy(cities_t* cities, city_t** city_ptr);

#endif
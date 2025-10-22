#ifndef CITY_H
#define CITY_H

typedef struct city_t
{
	char* name;
	float latitude;
	float longitude;
} city_t;

int city_init(const char* _name, float _latitude, float _longitude, city_t** _cityPtr);
void city_dispose(city_t** _cityPtr);

#endif
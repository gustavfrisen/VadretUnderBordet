#include <stdio.h>
#include <stdlib.h>
#include "cities.h"

int main()
{
    printf("\nMain started...\n");

    cities_t* cities = NULL;
    cities_init(&cities);
    //cities_print(&cities);

    printf("elements in cities: %ld\n", cities->list.size);
    //printf("city name of first element: %s\n", ((city_t*)cities->list.head->item)->name);

    printf("Main ended...\n\n");
    return 0;
}
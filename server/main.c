#include <stdio.h>
#include <stdlib.h>
#include "cities.h"

int main()
{
    printf("\nMain started...\n");

    cities_t* cities = NULL;
    cities_init(&cities);
    cities_print(cities);

    printf("Main ended...\n\n");
    return 0;
}
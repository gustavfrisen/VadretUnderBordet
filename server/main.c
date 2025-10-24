#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cities.h"
#include "weather.h"

char* get_input()
{
    static char buffer[256];
    if (fgets(buffer, sizeof(buffer), stdin)) {
        buffer[strcspn(buffer, "\n")] = '\0';
        return buffer;
    }
    return NULL;
}

int menu_main()
{
    printf("--Main-Menu--\n");
    printf("1: Print all cities\n");
    printf("2: Print weather for city\n");
    printf("0: Exit\n");

    char* input = get_input();
    long value = strtol(input, NULL, 10);
    if (value < 0 || value > 2) {
        printf("Invalid choice.\n");
        return -1;
    }
    else {
        return (int)value;
    }

    return 0;
}

int main()
{
    cities_t* cities = NULL;
    cities_init(&cities);
    if (!cities) {
        printf("Failed to initialize cities.\n");
        return -1;
    }

    printf("\n");

    int choice = menu_main();
    if (choice == 0) {
        printf("Exiting program.\n");
        return 0;
    }

    if (choice == 1) {
        cities_print(cities);
    }
    else if (choice == 2) {
        printf("Enter city name: ");
        char* input = get_input();
        weather_t* weather = malloc(sizeof(weather_t));
        if (!weather) {
            printf("Memory allocation failed.\n");
            return -1;
        }

        if (weather_get(input, weather) == 0) {
            weather_print(weather);
        }
        else {
            printf("Failed to get weather for city: %s\n", input);
        }
    }
    else {
        printf("Invalid choice.\n");
    }

    return 0;
}
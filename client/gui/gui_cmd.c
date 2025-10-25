#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gui_cmd.h"

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
    system("clear");
    printf("--Main-Menu--\n");
    printf("1: Cities manager\n");
    printf("2: Weather manager\n");
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

int menu_cities()
{
    system("clear");
    printf("--Cities-Menu--\n");
    printf("1: Add city\n");
    printf("2: Remove city\n");
    printf("3: List cities\n");
    printf("0: Back to main menu\n");

    char* input = get_input();
    long value = strtol(input, NULL, 10);
    if (value < 0 || value > 3) {
        printf("Invalid choice.\n");
        return -1;
    }
    else {
        return (int)value;
    }

    return 0;
}

int menu_weather()
{
    system("clear");
    printf("--Weather-Menu--\n");
    printf("1: Get weather for city\n");
    printf("2: Update weather for all cities\n");
    printf("0: Back to main menu\n");

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
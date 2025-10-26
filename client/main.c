#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // For sleep() function

#include "cities.h"
#include "city.h"
#include "weather.h"

#include "HTTP.h"

char* get_input();
void main_menu();
void cities_menu();
void weather_menu();

int main() {
    
    return 0;
}

char* get_input()
{
    static char buffer[256];
    if (fgets(buffer, sizeof(buffer), stdin)) {
        buffer[strcspn(buffer, "\n")] = '\0';
        return buffer;
    }
    return NULL;
}

void main_menu() {
    system("clear");
    printf("--Main-Menu--\n");
    printf("1: Cities manager\n");
    printf("2: Weather manager\n");
    printf("0: Exit\n");
}

void cities_menu() {
    system("clear");
    printf("--Cities-Menu--\n");
    printf("1: Add city\n");
    printf("2: Remove city\n");
    printf("3: List cities\n");
    printf("0: Back to main menu\n");
}

void weather_menu() {
    system("clear");
    printf("--Weather-Menu--\n");
    printf("1: Does weather exist?\n");
    printf("2: Is weather stale?\n");
    printf("3: Load weather from cache\n");
    printf("4: Save weather to cache\n");
    printf("0: Back to main menu\n");
}
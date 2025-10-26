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
    cities_t* cities;
    cities_init(&cities);

    city_t* city;
    cities_get_city_by_name(cities, "Stockholm", &city);
    if (city) {
        char url[512];
        snprintf(url, sizeof(url), METEO_FORECAST_HTTP_URL, city->latitude, city->longitude);
        
        http_response_t* response;
        http_get(url, &response);
        if (response->status_code == 200) {
            char* client_response;
            process_openmeteo_http_response(response->body, &client_response);
            weather_t weather;
            deserialize_weather_http_response(client_response, &weather);
            weather_http_print_pretty(&weather);
            free(client_response);
        } else {
            printf("Failed to get weather data. HTTP Status: %d\n", response->status_code);
        }
        http_response_free(response);
    } else {
        printf("City not found.\n");
    }
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
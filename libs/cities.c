#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tinydir.h>
#include <jansson.h>

#include "cities.h"
#include "utils.h"

const char* cities_list = 	"Stockholm:59.3293:18.0686\n"
							"Göteborg:57.7089:11.9746\n"
							"Malmö:55.6050:13.0038\n"
							"Uppsala:59.8586:17.6389\n"
							"Västerås:59.6099:16.5448\n"
							"Örebro:59.2741:15.2066\n"
							"Linköping:58.4109:15.6216\n"
							"Helsingborg:56.0465:12.6945\n"
							"Jönköping:57.7815:14.1562\n"
							"Norrköping:58.5877:16.1924\n"
							"Lund:55.7047:13.1910\n"
							"Gävle:60.6749:17.1413\n"
							"Sundsvall:62.3908:17.3069\n"
							"Umeå:63.8258:20.2630\n"
							"Luleå:65.5848:22.1567\n"
							"Kiruna:67.8558:20.2253\n";

int cities_init(cities_t **cities_ptr)
{
    *cities_ptr = malloc(sizeof(cities_t));
    if (*cities_ptr == NULL) return -1;

    cities_t *cities = *cities_ptr;

    memset(&cities->list, 0, sizeof(LinkedList));

    create_folder("cities"); // ensure the directory exists

    //cities_load_from_disk(cities);
    cities_load_from_string_list(cities);
    cities_save_to_disk(cities);

    return 0;
}

int cities_load_from_disk(cities_t *cities)
{
    tinydir_dir dir;
    const char* folder_path = "cities";

    if (tinydir_open(&dir, folder_path) == -1)
    {
        perror("Error opening cities directory");
        return -1;
    }

    while (dir.has_next)
    {
        tinydir_file file;
        if (tinydir_readfile(&dir, &file) == -1)
        {
            perror("Error reading file in cities directory");
            tinydir_close(&dir);
            return -1;
        }

        if (!file.is_dir)
        {
            printf("Found city file: %s\n", file.name);
            // Here you would load the city data from the file
        }

        if (tinydir_next(&dir) == -1)
        {
            perror("Error moving to next file in cities directory");
            tinydir_close(&dir);
            return -1;
        }
    }

    tinydir_close(&dir);

    return 0;
}

int cities_load_from_string_list(cities_t *cities)
{
    if (!cities) return -1;

    char* list_copy = strdup(cities_list);
    if (!list_copy) return -1;

    char* ptr = list_copy;

    char* name = NULL;
    char* lat_str = NULL;
    char* lon_str = NULL;

    do {
        name = ptr;
        lat_str = strchr(ptr, ':');
        if (!lat_str) break;
        *lat_str = '\0';
        lat_str++;

        lon_str = strchr(lat_str, ':');
        if (!lon_str) break;
        *lon_str = '\0';
        lon_str++;

        char* end_ptr = strchr(lon_str, '\n');
        if (end_ptr) {
            *end_ptr = '\0';
            ptr = end_ptr + 1;
        } else {
            ptr = NULL;
        }

        city_t* city = NULL;

        city_init(name, lat_str, lon_str, &city);

        if (city) {
            cities_add_city(cities, city);
        }

    } while (ptr);

    return 0;
}

int cities_save_to_disk(cities_t *cities)
{
    if (!cities) return -1;

    Node *node = cities->list.head;
    while (node) {
        city_t *city = (city_t *)node->item;
        if (city && city->name) {
            char filepath[256];
            snprintf(filepath, sizeof(filepath), "cities/%s.json", city->name);
            FILE *file = fopen(filepath, "w");
            if (file) {
                json_t *city_json = json_object();
                json_object_set_new(city_json, "name", json_string(city->name));
                json_object_set_new(city_json, "latitude", json_real(city->latitude));
                json_object_set_new(city_json, "longitude", json_real(city->longitude));
                char *json_str = json_dumps(city_json, JSON_INDENT(4));
                fprintf(file, "%s", json_str);
                free(json_str);
                json_decref(city_json);
                fclose(file);
            }
        }
        node = node->front;
    }

    return 0;
}

int cities_add_city(cities_t *cities, city_t *city)
{
    LinkedList_append(&cities->list, city);

    return 0;
}

int cities_print(cities_t *cities)
{
    if (!cities) return -1;

    Node *node = cities->list.head;
    while (node) {
        city_t *city = (city_t *)node->item;
        if (city && city->name) {
            printf("%s\n", city->name);
        }
        node = node->front;
    }

    return 0;
}

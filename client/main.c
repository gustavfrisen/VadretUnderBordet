#include <stdio.h>
#include <stdlib.h>
#include "gui/gui_cmd.h"

int main()
{
restart:
    int choice_main = menu_main();
    switch (choice_main) {
        case 1:
            int choice_cm = menu_cities();
            switch (choice_cm)
            {
                case 1:
                    // Add city logic here
                    break;
                case 2:
                    // Remove city logic here
                    break;
                case 3:
                    // List cities logic here
                    break;
                case 0:
                    // Back to main menu
                    goto restart;
                    break;
                default:
                    break;
            }
            break;
        case 2:
            int choice_wm = menu_weather();
            switch (choice_wm)
            {
                case 1:
                    // Show weather logic here
                    break;
                case 2:
                    // Update weather logic here
                    break;
                case 0:
                    // Back to main menu
                    goto restart;
                    break;
                default:
                    break;
            }
            break;
        case 0:
            break;
        default:
            break;
    }
    return 0;
}
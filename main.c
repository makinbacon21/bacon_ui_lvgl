#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>

#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"

#include "demo_app.h"

void configure_simulator(int argc, char **argv);

int main(int argc, char **argv)
{
    configure_simulator(argc, argv);

    /* Initialize LVGL. */
    lv_init();

    /* Initialize the configured backend SDL2, FBDEV, libDRM or wayland */
    lv_linux_disp_init();

    /*Create a Demo*/
    lv_demo_benchmark();

    lv_demo_run_loop();

    return 0;
}

/*
 * Process command line arguments and environment
 * variables to configure the simulator
 */
void configure_simulator(int argc, char **argv)
{

    int opt = 0;
    bool err = false;

    /* Default values */
    config.fullscreen = config.maximize = false;
    config.window_width = atoi(getenv("LV_SIM_WINDOW_WIDTH") ?: "1920");
    config.window_height = atoi(getenv("LV_SIM_WINDOW_HEIGHT") ?: "1080");

    /* Parse the command-line options. */
    while ((opt = getopt(argc, argv, "fmw:h:")) != -1)
    {
        switch (opt)
        {
        case 'f':
            config.fullscreen = true;
            if (LV_USE_SDL)
            {
                fprintf(stderr, "The SDL driver doesn't support fullscreen mode on start\n");
                exit(1);
            }
            break;
        case 'm':
            config.maximize = true;
            if (LV_USE_SDL)
            {
                fprintf(stderr, "The SDL driver doesn't support maximized mode on start\n");
                exit(1);
            }
            break;
        case 'w':
            config.window_width = atoi(optarg);
            break;
        case 'h':
            config.window_height = atoi(optarg);
            break;
        case ':':
            fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            exit(1);
        case '?':
            fprintf(stderr, "Unknown option -%c.\n", optopt);
            exit(1);
        }
    }
}

#include <stdio.h>
#include <stdlib.h>

#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"

#include "demo_app.h"

int main(int argc, char **argv)
{
    /* Initialize LVGL. */
    lv_init();

    /* Initialize the configured backend SDL2, FBDEV, libDRM or wayland */
    lv_linux_disp_init();

    /*Create a Demo*/
    lv_demo_benchmark();

    lv_demo_run_loop();

    return 0;
}
 
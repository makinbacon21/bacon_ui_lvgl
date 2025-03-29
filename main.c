#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "lvgl/lvgl.h"

#if USE_SDL
#define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain"       \
                            issue*/
#include "lv_drivers/sdl/sdl.h"
#include <SDL2/SDL.h>
#elif USE_X11
#include "lv_drivers/x11/x11.h"
#else // sim
#include "lv_drivers/display/fbdev.h"
#endif

// #include "btlib.h"
#include "bacon.h"

#define DISP_BUF_SIZE (128 * 1024)

int main(int argc, char **argv) {
    /* Initialize LVGL. */
    lv_init();

    // if(init_blue("devices.txt") == 0)
    //     return(0);

    // le_scan();

    /* mouse input device */
    static lv_indev_drv_t indev_drv_1;
    lv_indev_drv_init(&indev_drv_1);
    indev_drv_1.type = LV_INDEV_TYPE_POINTER;

    /* keyboard input device */
    static lv_indev_drv_t indev_drv_2;
    lv_indev_drv_init(&indev_drv_2);
    indev_drv_2.type = LV_INDEV_TYPE_KEYPAD;

    /* mouse scroll wheel input device */
    static lv_indev_drv_t indev_drv_3;
    lv_indev_drv_init(&indev_drv_3);
    indev_drv_3.type = LV_INDEV_TYPE_ENCODER;

    lv_group_t *g = lv_group_create();
    lv_group_set_default(g);

    lv_disp_t *disp = NULL;

#if USE_SDL
    /* Use the 'monitor' driver which creates window on PC's monitor to simulate
     * a display*/
    sdl_init();

    /*Create a display buffer*/
    static lv_disp_draw_buf_t disp_buf1;
    static lv_color_t buf1_1[MONITOR_HOR_RES * 100];
    static lv_color_t buf1_2[MONITOR_HOR_RES * 100];
    lv_disp_draw_buf_init(&disp_buf1, buf1_1, buf1_2, MONITOR_HOR_RES * 100);

    /*Create a display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv); /*Basic initialization*/
    disp_drv.draw_buf = &disp_buf1;
    disp_drv.flush_cb = sdl_display_flush;
    disp_drv.hor_res = MONITOR_HOR_RES;
    disp_drv.ver_res = MONITOR_VER_RES;
    disp_drv.antialiasing = 1;

    disp = lv_disp_drv_register(&disp_drv);

    /* Add the input device driver */
    // mouse_init();
    indev_drv_1.read_cb = sdl_mouse_read;

    // keyboard_init();
    indev_drv_2.read_cb = sdl_keyboard_read;

    // mousewheel_init();
    indev_drv_3.read_cb = sdl_mousewheel_read;

#elif USE_X11
    lv_x11_init("LVGL Simulator Demo", DISP_HOR_RES, DISP_VER_RES);

    /*Create a display buffer*/
    static lv_disp_draw_buf_t disp_buf1;
    static lv_color_t buf1_1[DISP_HOR_RES * 100];
    static lv_color_t buf1_2[DISP_HOR_RES * 100];
    lv_disp_draw_buf_init(&disp_buf1, buf1_1, buf1_2, DISP_HOR_RES * 100);

    /*Create a display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &disp_buf1;
    disp_drv.flush_cb = lv_x11_flush;
    disp_drv.hor_res = DISP_HOR_RES;
    disp_drv.ver_res = DISP_VER_RES;
    disp_drv.antialiasing = 1;

    disp = lv_disp_drv_register(&disp_drv);

    /* Add the input device driver */
    indev_drv_1.read_cb = lv_x11_get_pointer;
    indev_drv_2.read_cb = lv_x11_get_keyboard;
    indev_drv_3.read_cb = lv_x11_get_mousewheel;
#else
    fbdev_init();

    /*A small buffer for LittlevGL to draw the screen's content*/
    static lv_color_t buf[DISP_BUF_SIZE];

    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

    /*Initialize and register a display driver*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &disp_buf;
    disp_drv.flush_cb = fbdev_flush;
    disp_drv.hor_res = 368; // changeme
    disp_drv.ver_res = 552; // changeme
    disp_drv.sw_rotate = 1;
    disp_drv.rotated = LV_DISP_ROT_90;
    lv_disp_drv_register(&disp_drv);
#endif

    lv_start_bacon();

    lv_bacon_run_loop();

    return 0;
}

/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
uint32_t custom_tick_get(void) {
    static uint64_t start_ms = 0;
    if (start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}

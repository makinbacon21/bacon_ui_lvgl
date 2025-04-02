#include <stdio.h>
#include <time.h>

#include "bacon_pub.h"

#define CLOCK_WIDTH 300
#define CLOCK_HEIGHT 100

lv_opa_t mask_map[CLOCK_WIDTH * CLOCK_HEIGHT];

lv_obj_t *clock_grad;
lv_obj_t *date_lbl;

int32_t entry();
int32_t suspend();
int32_t resume();

bacon_app_t appspec = {
    .entry = &entry,
    .suspend = &suspend,
    .resume = &resume,
    .id = "bacon_clock_app",
    .priority = 0,
    .destroy_on_exit = true,
    .clock = false,
    .update_period = 5,
    .surface = NULL,
};

static void update_date_time() {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char dayOfWeek[10];
    char time[10];

    switch (tm->tm_wday) {
    case 0:
        strcpy(dayOfWeek, "Sunday");
        break;
    case 1:
        strcpy(dayOfWeek, "Monday");
        break;
    case 2:
        strcpy(dayOfWeek, "Tuesday");
        break;
    case 3:
        strcpy(dayOfWeek, "Wednesday");
        break;
    case 4:
        strcpy(dayOfWeek, "Thursday");
        break;
    case 5:
        strcpy(dayOfWeek, "Friday");
        break;
    case 6:
        strcpy(dayOfWeek, "Saturday");
        break;
    default:
        strcpy(dayOfWeek, "ERROR");
    }

    snprintf(time, 10, "%d:%02d %s\n", tm->tm_hour % 12, tm->tm_min,
             (tm->tm_hour > 12) ? "PM" : "AM");
    update_gradient_text(mask_map, time, CLOCK_WIDTH, CLOCK_HEIGHT);
    lv_label_set_text_fmt(date_lbl, "%s, %d/%d/%d\n", dayOfWeek, tm->tm_mon + 1,
                          tm->tm_mday, tm->tm_year - 100);
}

int32_t entry() {
    clock_grad =
        create_gradient_text(mask_map, TEAL, "", CLOCK_WIDTH, CLOCK_HEIGHT);

    date_lbl = lv_label_create(appspec.surface);
    lv_obj_add_style(date_lbl, style_body, 0);
    lv_obj_align(date_lbl, LV_ALIGN_CENTER, 0, 64);

    update_date_time();
    lv_timer_create(update_date_time, 1000, NULL);

    return 0;
}

int32_t suspend() {
    lv_anim_t clock_vanish;
    lv_anim_t date_vanish;
    lv_anim_init(&clock_vanish);
    lv_anim_init(&date_vanish);

    lv_anim_set_exec_cb(&clock_vanish, (lv_anim_exec_xcb_t)fade);
    lv_anim_set_var(&clock_vanish, clock_grad);
    lv_anim_set_time(&clock_vanish, 500);
    lv_anim_set_values(&clock_vanish, 100, 0);

    lv_anim_set_exec_cb(&date_vanish, (lv_anim_exec_xcb_t)fade);
    lv_anim_set_var(&date_vanish, date_lbl);
    lv_anim_set_time(&date_vanish, 500);
    lv_anim_set_values(&date_vanish, 100, 0);

    lv_anim_start(&clock_vanish);
    lv_anim_start(&date_vanish);

    return 0;
}

int32_t resume() {
    lv_anim_t clock_vanish;
    lv_anim_t date_vanish;
    lv_anim_init(&clock_vanish);
    lv_anim_init(&date_vanish);

    lv_anim_set_exec_cb(&clock_vanish, (lv_anim_exec_xcb_t)fade);
    lv_anim_set_var(&clock_vanish, clock_grad);
    lv_anim_set_time(&clock_vanish, 500);
    lv_anim_set_values(&clock_vanish, 0, 100);

    lv_anim_set_exec_cb(&date_vanish, (lv_anim_exec_xcb_t)fade);
    lv_anim_set_var(&date_vanish, date_lbl);
    lv_anim_set_time(&date_vanish, 500);
    lv_anim_set_values(&date_vanish, 0, 100);

    lv_anim_start(&clock_vanish);
    lv_anim_start(&date_vanish);

    return 0;
}

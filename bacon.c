#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "bacon.h"

#define CLOCK_WIDTH 300
#define CLOCK_HEIGHT 100

static lv_style_t style_title;
static lv_style_t style_heading;
static lv_style_t style_body;

lv_opa_t mask_map[CLOCK_WIDTH * CLOCK_HEIGHT];

lv_obj_t *date_lbl;
lv_obj_t *event_lbl;
lv_obj_t *event_sub_lbl;
lv_obj_t *status_lbl;
lv_obj_t *clock_grad;

bacon_app_t clock_app = NULL;
bacon_app_t current_app = clock_app;

pthread_mutex_t clock_mutex;

static bool clockstate = false;

void fade(lv_obj_t *var, lv_opa_t value) {
    lv_obj_set_style_opa(var, value, 0);
}

static void timeout_cb(lv_timer_t *timer) {
    lv_anim_t clock_vanish;
    lv_anim_init(&clock_vanish);

    lv_anim_set_exec_cb(&clock_vanish, (lv_anim_exec_xcb_t)fade);
    lv_anim_set_var(&clock_vanish, clock_grad);
    lv_anim_set_time(&clock_vanish, 500);
    lv_anim_set_values(&clock_vanish, 100, 0);

    pthread_mutex_lock(&clock_mutex);
    lv_anim_start(&clock_vanish);
    lv_obj_del(date_lbl);
    clockstate = true;
    pthread_mutex_unlock(&clock_mutex);
}

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

static void update_status() {
    lv_label_set_text(event_lbl, "EWR → ATX");
    lv_label_set_text(event_sub_lbl, "In 4 hours, leave soon");

    if (clockstate) {
        time_t t = time(NULL);
        struct tm *tm = localtime(&t);
        lv_label_set_text_fmt(status_lbl, "%d:%02d %s // 72%%\n",
                              tm->tm_hour % 12, tm->tm_min,
                              (tm->tm_hour > 12) ? "PM" : "AM");
    } else {
        lv_label_set_text(status_lbl, "72%");
    }
}

static void update_status_cb(lv_timer_t *timer) {
    pthread_mutex_lock(&clock_mutex);
    update_status();
    if (!clockstate)
        update_date_time();
    pthread_mutex_unlock(&clock_mutex);
}

void create_statusbar(lv_obj_t *screen) {
    event_lbl = lv_label_create(screen);
    lv_obj_add_style(event_lbl, &style_heading, 0);
    lv_obj_align(event_lbl, BACON_TOP_LEFT);

    event_sub_lbl = lv_label_create(screen);
    lv_obj_add_style(event_sub_lbl, &style_body, 0);
    lv_obj_align(event_sub_lbl, BACON_TOP_LEFT_SUB);

    status_lbl = lv_label_create(screen);
    lv_obj_add_style(status_lbl, &style_heading, 0);
    lv_obj_align(status_lbl, BACON_TOP_RIGHT);

    update_status();
}

void create_clock(lv_obj_t *screen) {
    clock_grad =
        create_gradient_text(mask_map, TEAL, "", CLOCK_WIDTH, CLOCK_HEIGHT);

    date_lbl = lv_label_create(screen);
    lv_obj_add_style(date_lbl, &style_body, 0);
    lv_obj_align(date_lbl, LV_ALIGN_CENTER, 0, 64);

    update_date_time();
}

void lv_start_bacon(void) {
    lv_obj_t *screen = lv_scr_act();

    if (pthread_mutex_init(&clock_mutex, NULL)) {
        printf("ERROR: failed to init mutex\n");
        return;
    }

    lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000), LV_PART_MAIN);

    bacon_init_styles();

    // create styles
    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, &lv_font_fira_sans_bold_64);
    lv_style_set_text_color(&style_title, lv_color_white());

    // create styles
    lv_style_init(&style_heading);
    lv_style_set_text_font(&style_heading, &lv_font_fira_sans_bold_24);
    lv_style_set_text_color(&style_heading, lv_color_white());

    // create styles
    lv_style_init(&style_body);
    lv_style_set_text_font(&style_body, &lv_font_fira_sans_24);
    lv_style_set_text_color(&style_body, lv_color_white());

    create_statusbar(screen);
    create_clock(screen);

    lv_timer_create(update_status_cb, 1000, NULL);

    lv_timer_t *timer = lv_timer_create(timeout_cb, 5000, NULL);
    lv_timer_set_repeat_count(timer, 1);
}

void launch_app(bacon_app_t *app) {
    current_app.suspend();
    app.entry();
    current_app = app;
}

void close_app(bacon_app_t *app) {
    app.suspend();
    current_app = clock_app;
}

void lv_bacon_run_loop(void) {
    /*Handle LVGL tasks*/
    while (1) {
        lv_timer_handler();
        usleep(5000);
    }
}

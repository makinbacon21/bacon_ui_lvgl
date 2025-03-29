
#ifndef LV_BACON_H
#define LV_BACON_H

#include "lvgl/lvgl.h"
#include "bacon_theme.h"

typedef prio_t uint8_t;
typedef freq_t uint16_t;

typedef struct {
    int32_t (*entry)();
    int32_t (*suspend)();
    int32_t (*resume)();
    prio_t priority;
    bool destroy_on_exit;
    freq_t update_freq;
    lv_obj_t surface;
} bacon_app_t;

void lv_start_bacon(void);
void lv_bacon_run_loop(void);

void launch_app(bacon_app_t *app);
void close_app(bacon_app_t *app);

#endif /*LV_BACON_H*/

#include "bacon.h"

int32_t entry();
int32_t suspend();
int32_t resume();

bacon_app_t appspec = {
    .entry = &entry,
    .suspend = &suspend,
    .resume = &resume,
    .id = "bacon_test_app",
    .priority = 0,
    .destroy_on_exit = true,
    .clock = true,
    .update_period = 5,
    .surface = NULL,
};

int32_t entry() {
    lv_obj_t *test_lbl = lv_label_create(appspec.surface);
    lv_obj_add_style(test_lbl, style_title, 0);
    lv_obj_align(test_lbl, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(test_lbl, "TEST APP");

    return 0;
}

int32_t suspend() {
    return 0;
}

int32_t resume() {
    return 0;
}

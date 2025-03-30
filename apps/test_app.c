#include "bacon_pub.h"

int32_t entry() {
    return 0;
}

int32_t suspend() {
    return 0;
}

int32_t resume() {
    return 0;
}

bacon_app_t appspec = {
    .entry = &entry,
    .suspend = &suspend,
    .resume = &resume,
    .id = "bacon_test_app",
    .priority = 0,
    .destroy_on_exit = true,
    .update_period = 5,
    .surface = NULL,
};

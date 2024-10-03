#ifndef DEMO_APP_H
#define DEMO_APP_H

#include <stdbool.h>
#include <stdint.h> // For using uint16_t

typedef struct {
    uint16_t window_width;
    uint16_t window_height;
    bool fullscreen;
    bool maximize;
} WindowConfig;

extern WindowConfig config;

void lv_demo_run_loop(void);
void lv_linux_disp_init(void);

#endif // DEMO_APP_H
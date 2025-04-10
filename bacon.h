
#ifndef LV_BACON_H
#define LV_BACON_H

#include "lvgl/lvgl.h"
#include "bacon_theme.h"

#ifdef SIMULATOR
#define APP_DIR "./bin/apps/"
#else
#define APP_DIR "/usr/bin/bacon/"
#endif

typedef uint8_t prio_t;
typedef uint16_t period_t;

typedef struct {
    int32_t (*entry)();
    int32_t (*suspend)();
    int32_t (*resume)();
    char id[272];
    prio_t priority;
    bool destroy_on_exit;
    bool clock;
    period_t update_period;
    lv_obj_t *surface;
} bacon_app_t;

/**
 * BACON APP MANIFEST
 * Parsed from JSON
 * Example:
 * {
 *     "appmanifest": {
 *         "name": "Clock",
 *         "publisher": "Thomas Makin",
 *         "version": "v1.0.0"
 *     },
 * }
 */
typedef struct {
    char name[256];
    char publisher[256];
    char version[16];
} bacon_app_manifest_t;

void lv_start_bacon(void);
void lv_bacon_run_loop(void);

void launch_app(bacon_app_t *app);
void suspend_app(bacon_app_t *app);

uint16_t request_wakelock();
uint16_t release_wakelock();

#endif /*LV_BACON_H*/

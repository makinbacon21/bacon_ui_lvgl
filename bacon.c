#include <dirent.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include <dlfcn.h>

#include <json-c/json_tokener.h>
#include <json-c/json_types.h>

#include "bacon.h"

static bacon_app_t **registered_apps;
static uint16_t app_cnt = 0;

lv_obj_t *event_lbl;
lv_obj_t *event_sub_lbl;
lv_obj_t *status_lbl;

bacon_app_t clock_app;
bacon_app_t current_app;

pthread_mutex_t clock_mutex;

static bool clockstate = false;

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
    pthread_mutex_unlock(&clock_mutex);
}

void create_statusbar(lv_obj_t *screen) {
    event_lbl = lv_label_create(screen);
    lv_obj_add_style(event_lbl, style_heading, 0);
    lv_obj_align(event_lbl, BACON_TOP_LEFT);

    event_sub_lbl = lv_label_create(screen);
    lv_obj_add_style(event_sub_lbl, style_body, 0);
    lv_obj_align(event_sub_lbl, BACON_TOP_LEFT_SUB);

    status_lbl = lv_label_create(screen);
    lv_obj_add_style(status_lbl, style_heading, 0);
    lv_obj_align(status_lbl, BACON_TOP_RIGHT);

    update_status();
}

int parse_manifest(FILE *fd, bacon_app_manifest_t *manifest) {
    fseek(fd, 0, SEEK_END);
    long fsize = ftell(fd);
    rewind(fd);

    char *json_str = malloc(fsize + 1);
    if (!json_str) {
        LV_LOG_ERROR("Failed to alloc json buf\n");
        return -ENOMEM;
    }

    fread(json_str, 1, fsize, fd);
    json_str[fsize] = '\0';

    struct json_object *parsed_json = json_tokener_parse(json_str);
    free(json_str);

    if (!parsed_json) {
        LV_LOG_ERROR("Failed to parse JSON\n");
        return -EINVAL;
    }

    struct json_object *device, *metrics;
    if (json_object_object_get_ex(parsed_json, "appmanifest", &device)) {
        struct json_object *name, *publisher, *version;
        if (json_object_object_get_ex(device, "name", &name) &&
            json_object_object_get_ex(device, "publisher", &publisher) &&
            json_object_object_get_ex(device, "version", &version)) {
            snprintf(manifest->name, 256, "%s", json_object_get_string(name));
            snprintf(manifest->publisher, 256, "%s",
                     json_object_get_string(publisher));
            snprintf(manifest->version, 16, "%s",
                     json_object_get_string(version));
        } else {
            LV_LOG_ERROR("Missing required manifest element(s)\n");
            return -EINVAL;
        }
    } else {
        LV_LOG_ERROR("Missing top level manifest element\n");
        return -EINVAL;
    }

    json_object_put(parsed_json);
    return 0;
}

int discover_applications() {
    int ret;
    char *dlerr;
    struct dirent *de;

    DIR *dr = opendir(APP_DIR);

    if (dr == NULL) {
        LV_LOG_ERROR("App directory does not exist\n");
        return -EBADF;
    }

    while ((de = readdir(dr)) != NULL) {
        struct stat st;
        bacon_app_manifest_t *manifest;
        char filename[512] = "";

        // is named right?
        size_t len = strlen(de->d_name);
        if ((len < 6) || strcmp(de->d_name + len - 6, ".bcpkg") != 0) {
            LV_LOG_INFO("Skipping non-matching dir %s", de->d_name);
            continue;
        }

        // get info
        if (fstatat(dirfd(dr), de->d_name, &st, 0) < 0) {
            LV_LOG_ERROR("Failed to get file info for %s", de->d_name);
            continue;
        }

        // is folder?
        if (!S_ISDIR(st.st_mode))
            continue;

        LV_LOG_INFO("Got app: %s\n", de->d_name);

        strcat(filename, APP_DIR);
        strncat(filename, de->d_name, 481);
        strcat(filename, "/manifest.json");

        FILE *fd = fopen(filename, "r");
        if (!fd) {
            LV_LOG_ERROR("Failed to open manifest!\n");
            continue;
        }

        // verify application manifest
        manifest = malloc(sizeof(bacon_app_manifest_t));
        if (!manifest) {
            LV_LOG_ERROR("Failed to alloc space for manifest\n");
            fclose(fd);
            closedir(dr);
            return -ENOMEM;
        }

        ret = parse_manifest(fd, manifest);
        fclose(fd);

        if (ret < 0) {
            LV_LOG_ERROR("Malformed manifest for app %s, errno %d\n",
                         de->d_name, ret);
            free(manifest);
            continue;
        }

        LV_LOG_INFO("MANIFEST REGISTERED: %s\n", manifest->name);
        LV_LOG_INFO("Publisher: %s, Version: %s\n", manifest->publisher,
                    manifest->version);

        free(manifest);

        // register application
        filename[0] = '\0';

        strcat(filename, APP_DIR);
        strncat(filename, de->d_name, 478);
        strcat(filename, "/libapp.so");

        void *applib;
        applib = dlopen(filename, RTLD_NOW);
        if (!applib) {
            LV_LOG_ERROR("Failed to open app %s! %s\n", de->d_name, dlerror());
            continue;
        }

        bacon_app_t *app;

        app = (bacon_app_t *) dlsym(applib, "appspec");
        if (!app) {
            LV_LOG_ERROR("Failed to find appspec symbol!");
            closedir(dr);
            return -EINVAL;
        }

        app->surface =
            lv_scr_act(); // CHANGEME: apps should render to interior block

        app_cnt++;

        if (app_cnt == 1) {
            registered_apps = malloc(sizeof(uintptr_t));
        } else {
            registered_apps = (bacon_app_t **)realloc(registered_apps, sizeof(uintptr_t) * app_cnt);
        }

        registered_apps[app_cnt - 1] = app;

        LV_LOG_INFO("Registered app id %s\n", app->id);
    }

    closedir(dr);
}

void lv_start_bacon(void) {
    lv_obj_t *screen = lv_scr_act();

    discover_applications();

    if (pthread_mutex_init(&clock_mutex, NULL)) {
        printf("ERROR: failed to init mutex\n");
        return;
    }

    lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000), LV_PART_MAIN);

    bacon_init_styles();

    create_statusbar(screen);

    registered_apps[0]->entry();

    lv_timer_create(update_status_cb, 1000, NULL);
}

void launch_app(bacon_app_t *app) {
    current_app.suspend();
    app->entry();
    current_app = *app;
}

void close_app(bacon_app_t *app) {
    app->suspend();
    current_app = clock_app;
}

void lv_bacon_run_loop(void) {
    /*Handle LVGL tasks*/
    while (1) {
        lv_timer_handler();
        usleep(5000);
    }
}

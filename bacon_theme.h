
#ifndef LV_BACON_THEME_H
#define LV_BACON_THEME_H

#define DISPLAY_PADDING 30
# define BACON_TOP_RIGHT LV_ALIGN_TOP_RIGHT, -1 * DISPLAY_PADDING, DISPLAY_PADDING
# define BACON_TOP_LEFT LV_ALIGN_TOP_LEFT, DISPLAY_PADDING, DISPLAY_PADDING
# define BACON_TOP_LEFT_SUB LV_ALIGN_TOP_LEFT, DISPLAY_PADDING, DISPLAY_PADDING+32

#include "lvgl/lvgl.h"

typedef enum
{
    TEAL,
    PINK,
    BLUE,
} bacon_grad_t;

extern lv_style_t *style_title;
extern lv_style_t *style_heading;
extern lv_style_t *style_body;

extern lv_style_t *teal_grad;
extern lv_style_t *pink_grad;
extern lv_style_t *blue_grad;

void bacon_init_styles(void);

void update_gradient_text(lv_opa_t *mask_map, char *txt, const uint32_t width,
    const uint32_t height);

lv_obj_t * create_gradient_text(lv_opa_t *mask_map, bacon_grad_t type, char *txt,
    const uint32_t width, const uint32_t height);

void fade(lv_obj_t *var, lv_opa_t value);

#endif /*LV_BACON_THEME_H*/

#include <unistd.h>
#include <stdio.h>

#include "bacon_theme.h"

static lv_style_t teal_grad;
static lv_style_t pink_grad;
static lv_style_t blue_grad;

void bacon_init_styles(void) {
    /* teal gradient */
    lv_style_init(&teal_grad);
    lv_style_set_border_width(&teal_grad, 0);
    lv_style_set_bg_color(&teal_grad, lv_color_hex(0x3DC358));
    lv_style_set_bg_grad_color(&teal_grad, lv_color_hex(0x34B1A5));
    lv_style_set_bg_grad_dir(&teal_grad, LV_GRAD_DIR_HOR);

    /* pink gradient */
    lv_style_init(&pink_grad);
    lv_style_set_border_width(&pink_grad, 0);
    lv_style_set_bg_color(&pink_grad, lv_color_hex(0xC33D94));
    lv_style_set_bg_grad_color(&pink_grad, lv_color_hex(0x8911A4));
    lv_style_set_bg_grad_dir(&pink_grad, LV_GRAD_DIR_HOR);

    /* blue gradient */
    lv_style_init(&blue_grad);
    lv_style_set_border_width(&blue_grad, 0);
    lv_style_set_bg_color(&blue_grad, lv_color_hex(0x5ABDE4));
    lv_style_set_bg_grad_color(&blue_grad, lv_color_hex(0x0C3F7A));
    lv_style_set_bg_grad_dir(&blue_grad, LV_GRAD_DIR_HOR);

}

static void add_mask_event_cb(lv_event_t * e)
{
    static lv_draw_mask_map_param_t m;
    static int16_t mask_id;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    lv_opa_t * mask_map = lv_event_get_user_data(e);
    if(code == LV_EVENT_COVER_CHECK) {
        lv_event_set_cover_res(e, LV_COVER_RES_MASKED);
    }
    else if(code == LV_EVENT_DRAW_MAIN_BEGIN) {
        lv_draw_mask_map_init(&m, &obj->coords, mask_map);
        mask_id = lv_draw_mask_add(&m, NULL);

    }
    else if(code == LV_EVENT_DRAW_MAIN_END) {
        lv_draw_mask_free_param(&m);
        lv_draw_mask_remove_id(mask_id);
    }
}

void canvas_gradient_text(lv_opa_t *mask_map, char *txt, const uint32_t width,
    const uint32_t height)
{
    lv_obj_t * canvas = lv_canvas_create(lv_scr_act());

    lv_canvas_set_buffer(canvas, mask_map, width, height, LV_IMG_CF_ALPHA_8BIT);
    lv_canvas_fill_bg(canvas, lv_color_black(), LV_OPA_TRANSP);

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.color = lv_color_white();
    label_dsc.align = LV_TEXT_ALIGN_CENTER;
    label_dsc.font = &lv_font_fira_sans_bold_64;

    lv_canvas_draw_text(canvas, 0, 16, width, &label_dsc, txt);

    lv_obj_del(canvas);
}

lv_obj_t * create_gradient_text(lv_opa_t *mask_map, bacon_grad_t type, char *txt,
    const uint32_t width, const uint32_t height)
{
    canvas_gradient_text(mask_map, txt, width, height);

    lv_obj_t * grad = lv_obj_create(lv_scr_act());
    lv_obj_set_size(grad, width, height);
    lv_obj_center(grad);

    switch(type) {
        case TEAL:
            lv_obj_add_style(grad, &teal_grad, 0);
            break;
        case PINK:
            lv_obj_add_style(grad, &pink_grad, 0);
            break;
        case BLUE:
            lv_obj_add_style(grad, &blue_grad, 0);
            break;
        default:
            printf("wtf\n");
    }

    lv_obj_add_event_cb(grad, add_mask_event_cb, LV_EVENT_ALL, mask_map);

    return grad;
}

void update_gradient_text(lv_opa_t *mask_map, char *txt, const uint32_t width,
    const uint32_t height)
{
    canvas_gradient_text(mask_map, txt, width, height);
    lv_obj_invalidate(lv_scr_act());
}

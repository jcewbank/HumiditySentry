#include "ui_window.h"
#include "ui_common.h"

#include "LCD2_Driver.h"

#include "ui_window_tex.c"

#define UI_CNVS_Y(Y, CNVS) \
    ((Y) * (CNVS)->width)
#define UI_CNVS_XY(X, Y, CNVS)      \
    ((X) + ((Y) * (CNVS)->width))

static void draw_border(
    uint16_t x,
    uint16_t y,
    uint16_t w,
    uint16_t h,
    ui_color_t16 outer_color,
    ui_color_t16 inner_color,
    ui_cnvs_t* cnvs,
    uint8_t scale)
    {
    uint8_t real_border_width = scale * UI_WND_TEX_BRDR_SZ;
    uint8_t scale_shift;
    // Validate the strategy
    if((w < (real_border_width * 2) + 1) || (h < (real_border_width * 2) + 1)){
        return;
    }

    switch(scale){
        case 1:
            scale_shift = 0;
            break;

        case 2:
            scale_shift = 1;
            break;

        case 4:
            scale_shift = 2;
            break;

        case 8:
            scale_shift = 4;
            break;

        case 16:
            scale_shift = 5;
            break;

        default:
            assert(false);
            break;
    }

    uint16_t x_pos;
    uint16_t y_pos;
    uint16_t xw_dist;   // Distance between x and right side
    uint16_t yh_dist;   // Distance between y and bottom side
    uint16_t glyph_x;
    uint16_t glyph_y;
    uint16_t glyph_idx;
    uint32_t cnvs_y = UI_CNVS_Y(y, cnvs);   // Get the origin pixel offset to get to this Y-row
    const uint8_t* glyph;
    for(y_pos = 0; y_pos < h; y_pos++){
        yh_dist = h - y_pos - 1;
        for(x_pos = 0; x_pos < w; x_pos++){
            xw_dist = w - x_pos - 1;
            if(x_pos < real_border_width){                // Left
                if(y_pos < real_border_width){                // Top left corner
                    glyph = UI_WND_TEX_TOPLEFT;
                    glyph_x = x_pos >> scale_shift;
                    glyph_y = y_pos >> scale_shift;
                }
                else if(yh_dist < real_border_width){         // Bottom left corner
                    glyph = UI_WND_TEX_BOTLEFT;
                    glyph_x = x_pos >> scale_shift;
                    glyph_y = (real_border_width - yh_dist - 1) >> scale_shift;
                }
                else{                                           // Left border
                    glyph = UI_WND_TEX_LEFT;
                    glyph_x = x_pos >> scale_shift;
                    glyph_y = 0;
                }
                glyph_idx = glyph_x + (UI_WND_TEX_BRDR_SZ * glyph_y);
            }
            else if(xw_dist < real_border_width){     // Right
                if(y_pos < real_border_width){                // Top right corner
                    glyph = UI_WND_TEX_TOPRIGHT;
                    glyph_x = (real_border_width - xw_dist - 1) >> scale_shift;
                    glyph_y = y_pos >> scale_shift;
                }
                else if(yh_dist < real_border_width){         // Bottom right corner
                    glyph = UI_WND_TEX_BOTRIGHT;
                    glyph_x = (real_border_width - xw_dist - 1) >> scale_shift;
                    glyph_y = (real_border_width - yh_dist - 1) >> scale_shift;
                }
                else{                                           // Right border
                    glyph = UI_WND_TEX_RIGHT;
                    glyph_x = (real_border_width - xw_dist - 1) >> scale_shift;
                    glyph_y = 0;
                }
                glyph_idx = glyph_x + (UI_WND_TEX_BRDR_SZ * glyph_y);
            }
            else if((yh_dist < real_border_width)){        // Center bottom
                glyph = UI_WND_TEX_BOT;
                glyph_x = 0;
                glyph_y = (real_border_width - yh_dist - 1) >> scale_shift;
                glyph_idx = glyph_y;
            }
            else if(y_pos < real_border_width){           // Center top
                glyph = UI_WND_TEX_TOP;
                glyph_x = 0;
                glyph_y = y_pos >> scale_shift;
                glyph_idx = glyph_y;
            }
            // TODO JCE change this back to == 
            else if(x_pos >= real_border_width){
                x_pos = w - real_border_width - 1;
                continue;
            }
            else{
                assert(false);
            }

            // Transpose the glyph color to screenspace
            if(glyph[glyph_idx] & 0x1){
                cnvs->buf[x + x_pos + cnvs_y] = outer_color;
            }
            else if(glyph[glyph_idx] & 0x2){
                cnvs->buf[x + x_pos + cnvs_y] = inner_color;
            }
        }
        cnvs_y += cnvs->width;  // bump the y-offset into the canvas buffer
    }
}

void ui_window_draw(ui_ctx_t *ctx, void *data){
    #define BORDER_FUDGE_FACTOR (2) // draw interior 2px smaller when drawing border
    ui_window_t *wnd;
    wnd = (ui_window_t *)data;
    ui_widget_t* children;
    uint16_t x = wnd->base.container.x + wnd->base.container.x_ofst;
    uint16_t y = wnd->base.container.y + wnd->base.container.y_ofst;

    ui_common_updt((ui_widget_t *)data);

    if(wnd->border_color_inner != wnd->background_color 
    || wnd->border_color_outer != wnd->background_color){
        LCD2_SetColor_Rect(
            wnd->background_color,
            x + BORDER_FUDGE_FACTOR,
            y + BORDER_FUDGE_FACTOR,
            wnd->base.container.w - (2 * BORDER_FUDGE_FACTOR),
            wnd->base.container.h - (2 * BORDER_FUDGE_FACTOR),
            wnd->base.cnvs->width,
            wnd->base.cnvs->buf,
            wnd->base.cnvs->buf_sz
            );
        draw_border(
            x, 
            y,
            wnd->base.container.w,
            wnd->base.container.h,
            wnd->border_color_outer,
            wnd->border_color_inner,
            wnd->base.cnvs,
            2
            );
    }
    else{
        LCD2_SetColor_Rect(
            wnd->background_color,
            x,
            y,
            wnd->base.container.w,
            wnd->base.container.h,
            wnd->base.cnvs->width,
            wnd->base.cnvs->buf,
            wnd->base.cnvs->buf_sz
            );
    }

    if(wnd->base.children){
        children = (ui_widget_t *)wnd->base.children;
        children->draw(ctx, children);
    }

    wnd->base.dirty = false;
    #undef BORDER_FUDGE_FACTOR
}

void ui_window_init(ui_window_t* wnd, ui_draw_function_t draw_fn){
    if(draw_fn == NULL){
        wnd->base.draw = ui_window_draw;
    }
    else{
        wnd->base.draw = draw_fn;
    }
}
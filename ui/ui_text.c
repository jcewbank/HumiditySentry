#include "LCD_GUI.h"

#include "ui_colors.h"
#include "ui_text.h"
#include "ui_common.h"

// static __force_inline void plot_16bit_pixel(){
//     uint16_t XDir_Num;
//     uint16_t YDir_Num;

//     for(XDir_Num = 0; XDir_Num < 2 - 1; XDir_Num++) {
//         for(YDir_Num = 0; YDir_Num < 2 - 1; YDir_Num++) {
//             // LCD_SetPointlColor(Xpoint + XDir_Num, Ypoint + YDir_Num, Color);
//         }
//     }
// }

// Max supported font size is 24pt (17px wide)
static uint8_t char_buf[24 * 17 * sizeof(uint8_t)];

static void draw_char(
    POINT Xpoint,
    POINT Ypoint,
    const char Acsii_Char,
    sFONT* Font,
    COLOR Color_Foreground,
    COLOR Color_Background,
    ui_cnvs_t *cnvs
    )
{
    POINT Page, Column;

    uint32_t Char_Offset = (Acsii_Char - ' ') * Font->Height * (Font->Width / 8 + (Font->Width % 8 ? 1 : 0));
    const unsigned char *ptr = &Font->table[Char_Offset];
    uint32_t write_ofst;

    if(Color_Background == FONT_BACKGROUND){
        // Translate the 8-bit glyph to 16-bit canvas space pixel-by-pixel :(
        for(Page = 0; Page < Font->Height; Page ++ ) {
            for(Column = 0; Column < Font->Width; Column ++ ) {
                write_ofst = (Xpoint + Column)
                           + ((Ypoint + Page) * cnvs->width);
                if(*ptr & (0x80 >> (Column % 8))){
                    cnvs->buf[write_ofst] = Color_Foreground;
                }
                
                //One pixel is 8 bits
                if(Column % 8 == 7)
                    ptr++;
            }/* Write a line */

            if(Font->Width % 8 != 0){
                ptr++;
            }
        }/* Write all */
    }
    else{
        // Translate the 8-bit glyph to 16-bit canvas space pixel-by-pixel :(
        for(Page = 0; Page < Font->Height; Page ++ ) {
            for(Column = 0; Column < Font->Width; Column ++ ) {
                write_ofst = (Xpoint + Column)
                           + ((Ypoint + Page) * cnvs->width);
                if(*ptr & (0x80 >> (Column % 8))){
                    cnvs->buf[write_ofst] = Color_Foreground;
                }
                else{
                    cnvs->buf[write_ofst] = Color_Background;
                }
                
                //One pixel is 8 bits
                if(Column % 8 == 7)
                    ptr++;
            }/* Write a line */

            if(Font->Width % 8 != 0){
                ptr++;
            }
        }/* Write all */
    }
}

static void draw_string(ui_text_t *widget){
    const char *s = widget->txt;
    POINT x = widget->base.container.x + widget->base.container.x_ofst;
    POINT y = widget->base.container.y + widget->base.container.y_ofst;
    POINT Xpoint = x;
    POINT Ypoint = y;
    POINT parent_right = PARENT(widget)->container.x + PARENT(widget)->container.w;
    POINT parent_bottom = PARENT(widget)->container.y + PARENT(widget)->container.h;

    if(x > parent_right || y > parent_bottom) {
        // TODO JCE this is a bomb eventually improve error report
        assert(false);
        return;
    }

    while(*s != '\0') {
        //if X direction filled , reposition to(Xstart,Ypoint),Ypoint is Y direction plus the height of the character
        if((Xpoint + widget->font->Width ) > parent_right) {
            Xpoint = x;
            Ypoint += widget->font->Height;
        }

        // If the Y direction is full, reposition to(Xstart, Ystart)
        if((Ypoint  + widget->font->Height ) > parent_bottom) {
            Xpoint = x;
            Ypoint = y;
        }
        
        draw_char(
            Xpoint,
            Ypoint,
            *s,
            widget->font,
            widget->fg_color.raw,
            widget->bg_color.raw,
            widget->base.cnvs
            );

        //The next character of the address
        s++;

        //The next word of the abscissa increases the font of the broadband
        Xpoint += widget->font->Width;
    }
}

static void ui_text_draw(ui_ctx_t *ctx, void *data){
    ui_text_t *widget;
    widget = (ui_text_t *)data;

    ui_common_updt((ui_widget_t *)data);

    draw_string(widget);

    widget->base.dirty = false;
}

void ui_text_init(ui_text_t* ui_text, char* str, uint8_t sz){
    memset(ui_text, 0, sizeof(ui_text_t));
    ui_text->txt = str;
    ui_text->txt[sz-1] = '\0';
    ui_text->sz = sz;
    ui_text->base.draw = ui_text_draw;
}
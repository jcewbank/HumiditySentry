#include "ui.h"
#include "ui_mainwindow.h"
#include "LCD_Driver.h"
#include "LCD_Touch.h"
#include "LCD_GUI.h"
#include "LCD_Bmp.h"
#include "LCD2_Driver.h"
#include "DEV_Config.h"
#include <stdio.h>
#include <stdalign.h>
#include "pico/stdlib.h"
#include "pico/malloc.h"
#include "hardware/watchdog.h"
#include "hardware/timer.h"
#include "ui.h"
#include "ui_colors.h"
#include "ui_text.h"
#include "ui_window.h"
#include "ui_util.h"
#include "../tch/tch_main.h"
#include "../i2c_prj/sht40/sht40.h"
#include "../i2c_prj/seesaw/seesaw.h"
#include "fonts.h"

#define UI_DEBUG 0

#define MSG_TXT_SZ 128
static char msg[MSG_TXT_SZ];    // TODO JCE probably remove

static ui_ctx_t     main_ctx;
static ui_ctx_t*    ctx;
ui_window_t*        main_wnd_ptr;
static ui_window_t    main_wnd;
static ui_cnvs_t     main_cnvs;

// Sensor data
extern volatile sht40_trh_data_t
                        trh_data;
extern volatile sht40_err_t8 
                        sht40_err;
extern volatile seesaw_soil_data_t
                        soil_data;

// LCD and touch panel states
extern volatile tch_state_t touch_state;

// TODO JCE temporary until some kind of design scheme
static ui_text_t    main_text;

// Framebuffer
//     320x240x16bpp = 153,600 bytes
//     Pi Pico has 264KB SRAM
//     Pi Pico 2 has 520KB SRAM
#define FRAMEBUFFER_SIZE ((uint32_t)(240 * 320 * sizeof(uint16_t)))
uint16_t *fb;

int ui_init(void){
    uint8_t counter = 0;
       
    System_Init();
    SD_Init();
    LCD_SCAN_DIR  lcd_scan_dir = D2U_L2R;
    LCD_Init(lcd_scan_dir, 255);
    TP_Init(lcd_scan_dir);

    // Use default touchscreen calibration
    // Without this we'll get maxval for any touch
    TP_GetAdFac();

    //LCD_SetWindow(0, 0, 320, 240);
    GUI_Clear(UI_COLOR_BLACK);

    // UI setup
    ctx = &main_ctx;
    main_wnd_ptr = &main_wnd;
    ui_window_init(&main_wnd, NULL);
    main_wnd.base.cnvs = &main_cnvs;
    // Allocate the framebuffer and guarantee
    // 16-bit alignment for DMA transfers
    if(fb != NULL){
        free(fb);
    }
    fb = malloc(FRAMEBUFFER_SIZE + 1);    // extra byte ensure 16-bit alignment
    memset(fb, 0, FRAMEBUFFER_SIZE);
    assert(fb != NULL);
    if(((uint32_t)fb & 0x1) != 0){
        main_wnd.base.cnvs->buf = fb + 1;
        assert((uint32_t)main_wnd.base.cnvs->buf & 0x1 == 0);
    }
    else{
        main_wnd.base.cnvs->buf = fb;
    }
    main_wnd.base.cnvs->buf = fb;
    main_wnd.base.cnvs->buf_sz = FRAMEBUFFER_SIZE;
    main_wnd.base.cnvs->width = 320;
    main_wnd.base.cnvs->height = 240;
    main_wnd.base.container.w = 220;
    main_wnd.base.container.h = 100;
    main_wnd.base.container.x = 10;
    main_wnd.base.container.y = 20;
    main_wnd.base.dirty = true;
    main_wnd.background_color = UI_COLOR_WHITE;
    main_wnd.border_color_outer = UI_COLOR_BLACK;
    main_wnd.border_color_inner = UI_COLOR_WHITE;
    #define UI_REL_ADD_CHILD(PARENT, CHILD)         \
        (PARENT)->base.children = (ui_window_t*)(CHILD); \
        (CHILD)->base.parent = (ui_window_t*)(PARENT)

    // Add a text box to the window
    ui_text_init(&main_text, msg, MSG_TXT_SZ);
    main_text.base.container.rel_ofst = true;
    main_text.base.container.x = 20;
    main_text.base.container.y = 20;
    main_text.font = &Font24;
    main_text.fg_color.raw = UI_COLOR_BLACK;
    main_text.bg_color.raw = UI_COLOR_WHITE;
    main_text.base.cnvs = &main_cnvs;

    // Link main window to the child text
    UI_REL_ADD_CHILD(main_wnd_ptr, &main_text);

    // LCD init
    LCD_SetBackLight(0xFFFF);

    return 0;
}

void get_sht40_data_string(char *s, uint16_t sz){
    switch(sht40_err){
        case SHT40_ERR_OK:
            snprintf(s, sz, "%.1fF %.1f%%", (trh_data.temp * 1.8f) + 32.0f, trh_data.rh);
            break;

        case SHT40_ERR_CRC_TEMP:
            snprintf(s, sz, "Temperature CRC mismatch\n");
            break;

        case SHT40_ERR_CRC_RH:
            snprintf(s, sz, "RH CRC mismatch\n");
            break;
    }
}

void ui_main(){
    #if UI_DEBUG
    absolute_time_t start_time, end_time;
    uint64_t time_diff;
    start_time = get_absolute_time();
    #endif

    ui_color_t bg_color = {0};

    get_sht40_data_string(msg, MSG_TXT_SZ);    
    printf("Seesaw: %.1fF, %d\n", 
        (soil_data.temp * 1.8f) + 32.0f, 
        soil_data.moisture);
    if(touch_state.pressed){
        uint16_t clamped_x = touch_state.coords.x;
        uint16_t clamped_y = touch_state.coords.y;
        if(touch_state.coords.x + main_wnd.base.container.w + 1 > main_wnd.base.cnvs->width){
            clamped_x = main_wnd.base.cnvs->width - main_wnd.base.container.w - 1;
        }
        if(touch_state.coords.y + main_wnd.base.container.h + 1 > main_wnd.base.cnvs->height){
            clamped_y = main_wnd.base.cnvs->height - main_wnd.base.container.h - 1;
        }
        main_wnd.base.container.x = clamped_x;
        main_wnd.base.container.y = clamped_y;
    }

    bg_color = generate_rainbow();
    LCD2_SetColor(bg_color.raw, main_wnd.base.cnvs->buf, main_wnd.base.cnvs->buf_sz / 2);
    main_wnd.base.draw(ctx, &main_wnd);
    
    LCD2_Flush(
        main_wnd_ptr->base.cnvs->buf,
        main_wnd_ptr->base.cnvs->buf_sz);

    #if UI_DEBUG
    end_time = get_absolute_time();
    time_diff = absolute_time_diff_us(start_time, end_time);

    // Only notify when main() took longer than 200ms
    // if(time_diff > 200000){
    if(true){
        sprintf(msg, "Main: %llums", time_diff / 1000);
        ui_dbg_print(msg);
    }
    #endif
}


#include "tch_main.h"
#include "pico/time.h"
#include "../lib/lcd/LCD_Touch.h"

volatile tch_state_t touch_state;

void tch_init(){
    memset(&touch_state, 0, sizeof(touch_state));
}

void tch_main(uint64_t tick){
    bool pressed = TP_GetCoords(
        &touch_state.coords.x, 
        &touch_state.coords.y);
    
    // Not pressed -> Pressed
    if(!touch_state.pressed){
        touch_state.last_press.down = tick;
        touch_state.last_press.up = 0;
        touch_state.pressed = true;
    }

    // Pressed -> Not pressed
    if(!pressed && touch_state.pressed){
        touch_state.last_press.up = tick;
        touch_state.pressed = false;
    }
}
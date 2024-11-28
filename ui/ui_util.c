#include "ui_util.h"
#include "ui_colors.h"

const ui_color_t16 color_wheel[UI_COLOR_CNT] = {
	UI_COLOR_WHITE,
	UI_COLOR_BLACK,
	UI_COLOR_BLUE,
	UI_COLOR_BRED,
	UI_COLOR_GRED,
	UI_COLOR_GBLUE,
	UI_COLOR_RED,
	UI_COLOR_MAGENTA,
	UI_COLOR_GREEN,
	UI_COLOR_CYAN,
	UI_COLOR_YELLOW,
	UI_COLOR_BROWN,
	UI_COLOR_BRRED,
	UI_COLOR_GRAY,
	};

static uint32_t tick;

#define UI_COLOR_WHEEL_STEPS (500)
ui_color_t generate_rainbow() {
    ui_color_t color = {0};

	// Clamp tick
	tick = tick % UI_COLOR_WHEEL_STEPS;

	// Map the current step to a hue value (0 to 360 degrees)
	float hue = (float)tick / (float)UI_COLOR_WHEEL_STEPS * 360.0f;

	// Convert hue to RGB
	float r, g, b;
	int h = (int)(hue / 60.0f) % 6;
	float f = (hue / 60.0f) - h;
	float q = 1.0f - f;

	switch (h) {
		case 0: r = 1.0f; g = f; b = 0.0f; break;
		case 1: r = q; g = 1.0f; b = 0.0f; break;
		case 2: r = 0.0f; g = 1.0f; b = f; break;
		case 3: r = 0.0f; g = q; b = 1.0f; break;
		case 4: r = f; g = 0.0f; b = 1.0f; break;
		case 5: r = 1.0f; g = 0.0f; b = q; break;
		default: r = g = b = 0.0f; break;
	}

	// For 8-bit RGB would scale to 0–255
	// Because using 5-6-5 RGB, scale to 5- or 6-bit range
	color.rgb.red = (uint8_t)(r * 31.0f);
	color.rgb.green = (uint8_t)(g * 63.0f);
	color.rgb.blue = (uint8_t)(b * 31.0f);

    // Swap bytes for the 16-bit framebuffer writes
    color.raw = (color.raw >> 8) | (color.raw << 8);

    tick++;
    return(color);
}
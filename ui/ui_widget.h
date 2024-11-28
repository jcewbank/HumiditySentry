#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include "ui.h"
#include "ui_cnvs.h"
#include "ui_container.h"

typedef uint8_t ui_widget_id_t8;
enum {
    UI_WIDGET_WINDOW,
    UI_TEXT,
};

typedef void (*ui_draw_function_t)
    (
    ui_ctx_t *ctx,
    void *data
    );

typedef struct {
    // Common widget params
    ui_widget_id_t8     id;
    ui_cnvs_t*          cnvs;
    ui_draw_function_t  draw;
    ui_container_t      container;
    bool                dirty;
    void*               parent;
    void*               prev;
    void*               next;
    void*               children;
} ui_widget_t;

#endif
#include "ui_common.h"

static void updt_container(ui_widget_t* widget){
    if(PARENT(widget) != NULL){
        widget->container.x_ofst = PARENT(widget)->container.x + PARENT(widget)->container.x_ofst;
        widget->container.y_ofst = PARENT(widget)->container.y + PARENT(widget)->container.y_ofst;
    }
}

void ui_common_updt(ui_widget_t* widget){
    updt_container(widget);
}
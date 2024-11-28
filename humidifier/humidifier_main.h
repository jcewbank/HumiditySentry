#ifndef HUMIDIFIER_MAIN_H
#define HUMIDIFIER_MAIN_H

typedef struct{
    bool on;
} humidifier_state_t;

void humidifier_init();
void humidifier_main();

#endif
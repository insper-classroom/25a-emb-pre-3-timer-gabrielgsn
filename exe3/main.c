#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int flag_f_r = 0;

volatile bool alarme_ativo = false;
volatile bool alarme_disparou = false;
alarm_id_t lp_alarm_id = -1;

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) { // fall edge
        if (gpio == BTN_PIN_R)
            flag_f_r = 1;
    } else if (events == 0x8) { // rise edge
        if (gpio == BTN_PIN_R)
            flag_f_r = 0;
    }
}

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    alarme_ativo = false;
    alarme_disparou = true;
    // Can return a value here in us to fire in the future
    return 0;
}


int main() {
    stdio_init_all();

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(
        BTN_PIN_R, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);

    int pressionado = 0;
    int ligado = 0;
    
    while (true) {
        if(flag_f_r){
            if(!pressionado){
                if(!alarme_ativo){
                    alarme_ativo = true;
                    lp_alarm_id = add_alarm_in_ms(500, &alarm_callback, NULL, true);
                    pressionado = 1;
                }
            }
        }
        if(!flag_f_r){
            if(pressionado){
                if(alarme_ativo){
                    cancel_alarm(lp_alarm_id);
                    alarme_ativo = false;
                }
                if(alarme_disparou){
                    ligado = !ligado;
                    gpio_put(LED_PIN_R, ligado);
                    alarme_disparou = false;
                }
                pressionado = 0;
            }
        }
    }
}

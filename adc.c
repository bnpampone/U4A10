#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"


#define VRX_PIN 26  // Eixo X (LED vermelho)
#define VRY_PIN 27  // Eixo Y (LED azul)
#define SW_PIN 22   // Botão do joystick
#define BTN_A_PIN 5 // Botão A (Para desligar o PWM)
#define LED_RED_PIN 12 // LED vermelho (X)
#define LED_BLUE_PIN 13 // LED azul (Y)
#define LED_GREEN_PIN 11 // LED verde (Botão)

#define PWM_WRAP 4095
#define DEAD_ZONE 100

bool pwm_enabled = true; // Variavel controla PWM esta ativo ou nao

// Inicializa o PWM no pino especificado
uint pwm_init_gpio(uint gpio, uint wrap) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true);
    return slice_num;
}

// Função para converter leitura do ADC em valor de PWM com zona morta
uint16_t adc_to_pwm(uint16_t adc_value) {
    if (adc_value > (2048 - DEAD_ZONE) && adc_value < (2048 + DEAD_ZONE)) {
        return 0; // Apaga o LED na região central
    }
    // Calcula a intensidade de brilho, maior nos extremos
    return (uint16_t)((abs((int)adc_value - 2048) * PWM_WRAP) / 2048);
}

// Função de interrupção para o botão A (GPIO 5)
void button_A_isr() {
    static uint32_t last_interrupt_time = 0;
    uint32_t current_time = time_us_32();

    // Debounce: só processa a interrupção se passou tempo suficiente
    if (current_time - last_interrupt_time > 200000) { // 200ms debounce
        pwm_enabled = !pwm_enabled; // Alterna o estado do PWM
        if (!pwm_enabled) {
            pwm_set_gpio_level(LED_RED_PIN, 0);
            pwm_set_gpio_level(LED_BLUE_PIN, 0);
            pwm_set_gpio_level(LED_GREEN_PIN, 0);
        }
    }
    last_interrupt_time = current_time;
}

int main() {
    stdio_init_all();
    adc_init();

    adc_gpio_init(VRX_PIN); // Eixo X
    adc_gpio_init(VRY_PIN); // Eixo Y

    gpio_init(SW_PIN);
    gpio_set_dir(SW_PIN, GPIO_IN);
    gpio_pull_up(SW_PIN); // Puxa o botão para "HIGH"

    gpio_init(BTN_A_PIN);  // Inicializa o botão A
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_pull_up(BTN_A_PIN); // Puxa o botão A para "HIGH"
    gpio_set_irq_enabled_with_callback(BTN_A_PIN, GPIO_IRQ_EDGE_FALL, true, &button_A_isr); // Habilita interrupção para o botão A

    // Inicializa os LEDs com PWM
    uint red_slice = pwm_init_gpio(LED_RED_PIN, PWM_WRAP);
    uint blue_slice = pwm_init_gpio(LED_BLUE_PIN, PWM_WRAP);
    uint green_slice = pwm_init_gpio(LED_GREEN_PIN, PWM_WRAP);

    while (true) {
        adc_select_input(0);
        uint16_t vrx_value = adc_read(); // Lê o eixo X (LED vermelho)

        adc_select_input(1);
        uint16_t vry_value = adc_read(); // Lê o eixo Y (LED azul)

        bool sw_value = gpio_get(SW_PIN) == 0; // Lê o botão (0 = pressionado)

        // Se PWM estiver habilitado, ajusta o brilho dos LEDs com base no joystick
        if (pwm_enabled) {
            pwm_set_gpio_level(LED_RED_PIN, adc_to_pwm(vrx_value));
            pwm_set_gpio_level(LED_BLUE_PIN, adc_to_pwm(vry_value));
        }

        if (sw_value) {
            pwm_set_gpio_level(LED_GREEN_PIN, PWM_WRAP); // Brilho máximo com o botão
        } else {
            pwm_set_gpio_level(LED_GREEN_PIN, 0); // Apaga o LED se o botão não estiver pressionado
        }

        printf("VRX: %u, VRY: %u, SW: %d, PWM: %d\n", vrx_value, vry_value, sw_value, pwm_enabled);

        sleep_ms(100);
    }

    return 0;
}

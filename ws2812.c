#include <stdio.h> // Biblioteca padrão de E/S
#include <stdlib.h> // Biblioteca padrão
#include "pico/stdlib.h" // Biblioteca Pico Standard Library
#include "hardware/pio.h" // Funções para configuração do PIO
#include "hardware/clocks.h" // Funções para configuração dos clocks
#include "ws2812.pio.h" // Programa para a matriz de LEDs WS2812

#define IS_RGBW false // Define se o LED é RGB (false) ou RGBW (true)
#define NUM_PIXELS 25 // Número de LEDs na matriz (5x5)
#define WS2812_PIN 7 // Pino de dados da matriz de LEDs
#define BUTTON_A_PIN 5 // Pino do botão A
#define BUTTON_B_PIN 6 // Pino do botão B
#define LED_RGB_RED_PIN 11 // Pino do LED RGB vermelho
#define LED_RGB_GREEN_PIN 12 // Pino do LED RGB verde
#define LED_RGB_BLUE_PIN 13 // Pino do LED RGB azul

// Variáveis globais para controle do LED e cor
uint8_t numero_exibido = 0; // Número exibido na matriz (0-9)

// Variáveis de debouncing para os botões
uint32_t ultimo_tempo_botao_a = 0;
uint32_t ultimo_tempo_botao_b = 0;
#define DEBOUNCE_DELAY 200 // tempo de debounce em milissegundos

// Função para enviar um pixel para a matriz de LEDs
static inline void enviar_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u); // Envia o pixel para a matriz
}

// Função para converter valores RGB para um único valor de 32 bits
static inline uint32_t converter_rgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b); // Formato GRB (WS2812)
}

uint32_t buffer_leds[NUM_PIXELS] = {0}; // Buffer para armazenar as cores de todos os LEDs

// Função para apagar todos os LEDs
void atualizar_buffer_leds() {
    for (int i = 0; i < NUM_PIXELS; i++) {
        buffer_leds[i] = 0; // Desliga todos os LEDs
    }
}

// Função para enviar o estado do buffer de LEDs para a matriz
void definir_leds_do_buffer() {
    for (int i = 0; i < NUM_PIXELS; i++) {
        enviar_pixel(buffer_leds[i]); // Envia o estado de cada LED
    }
}

// Callback para tratar interrupções dos botões
void botao_callback(uint gpio, uint32_t eventos) {
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());

    // Verifica qual botão foi pressionado e atualiza o número exibido
    // Botão A incrementa o número exibido
    if (gpio == BUTTON_A_PIN && (tempo_atual - ultimo_tempo_botao_a > DEBOUNCE_DELAY)) {
        numero_exibido = (numero_exibido + 1) % 10; // Incrementa (mantém entre 0 e 9)
        ultimo_tempo_botao_a = tempo_atual;
        printf("Botão A - Número exibido: %d\n", numero_exibido);
    }
    // Botão B decrementa o número exibido
    else if (gpio == BUTTON_B_PIN && (tempo_atual - ultimo_tempo_botao_b > DEBOUNCE_DELAY)) {
        numero_exibido = (numero_exibido + 9) % 10; // Decrementa (mantém entre 0 e 9)
        ultimo_tempo_botao_b = tempo_atual;
        printf("Botão B - Número exibido: %d\n", numero_exibido);
    }
}

// Função para exibir um número na matriz (0-9)
void exibir_numero(uint8_t num) {
    // Padrões para cada número (0-9) em uma matriz 5x5
    uint32_t padroes_numeros[10][25] = {
        {1,1,1,1,1, 1,0,0,0,1, 1,0,0,0,1, 1,0,0,0,1, 1,1,1,1,1}, // 0
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 1,1,1,1,1}, // 1
        {1,1,1,0,1, 1,0,1,0,1, 1,0,1,0,1, 1,0,1,0,1, 1,0,1,1,1}, // 2
        {1,0,1,0,1, 1,0,1,0,1, 1,0,1,0,1, 1,0,1,0,1, 1,1,1,1,1}, // 3
        {0,0,1,1,1, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 1,1,1,1,1}, // 4
        {1,0,1,1,1, 1,0,1,0,1, 1,0,1,0,1, 1,0,1,0,1, 1,1,1,0,1}, // 5
        {1,1,1,1,1, 1,0,1,0,1, 1,0,1,0,1, 1,0,1,0,1, 1,1,1,0,1}, // 6
        {0,0,0,0,1, 1,0,0,0,0, 0,0,0,0,1, 1,0,0,0,0, 1,1,1,1,1}, // 7
        {1,1,1,1,1, 1,0,1,0,1, 1,0,1,0,1, 1,0,1,0,1, 1,1,1,1,1}, // 8
        {1,0,1,1,1, 1,0,1,0,1, 1,0,1,0,1, 1,0,1,0,1, 1,1,1,1,1}  // 9
    };

    // Atualiza o buffer com o padrão do número selecionado
    for (int i = 0; i < NUM_PIXELS; i++) {
        if (padroes_numeros[num][i]) {
            buffer_leds[i] = converter_rgb_u32(255, 0, 0); // Vermelho
        } else {
            buffer_leds[i] = 0; // Desliga o LED
        }
    }
}

int main() {
    stdio_init_all(); // Inicializa a comunicação serial
    printf("WS2812 5x5 Matrix - Controlando LEDs com Botões A e B\n");

    PIO pio = pio0; // Seleciona o PIO 0
    int sm = 0; // Seleciona o State Machine 0
    uint offset = pio_add_program(pio, &ws2812_program); // Carrega o programa para o PIO

    // Inicializa o programa para a matriz de LEDs
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    // Configuração dos botões
    gpio_init(BUTTON_A_PIN); // Botão A
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN); // Define como entrada
    gpio_pull_up(BUTTON_A_PIN); // Habilita resistor de pull-up
    gpio_set_irq_enabled(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true); // Habilita interrupção de borda de descida

    gpio_init(BUTTON_B_PIN); // Botão B
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN); // Define como entrada
    gpio_pull_up(BUTTON_B_PIN); // Habilita resistor de pull-up
    gpio_set_irq_enabled(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true); // Habilita interrupção de borda de descida

    // Apenas um callback para ambos os botões
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &botao_callback); 
    gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true, &botao_callback);

    // Configuração do LED RGB
    gpio_init(LED_RGB_RED_PIN); // LED RGB vermelho
    gpio_set_dir(LED_RGB_RED_PIN, GPIO_OUT); // Define como saída
    gpio_init(LED_RGB_GREEN_PIN); // LED RGB verde
    gpio_set_dir(LED_RGB_GREEN_PIN, GPIO_OUT); // Define como saída
    gpio_init(LED_RGB_BLUE_PIN); // LED RGB azul
    gpio_set_dir(LED_RGB_BLUE_PIN, GPIO_OUT); // Define como saída

    bool estado_led_rgb = false; // Estado do LED RGB
    uint32_t ultimo_tempo_troca_led_rgb = 0; // Último tempo de troca do LED RGB

    // Loop principal para exibir o número na matriz de LEDs
    while (1) {
        // Atualiza o buffer com o número a ser exibido
        exibir_numero(numero_exibido);

        // Envia o estado do buffer para a matriz de LEDs
        definir_leds_do_buffer();

        // Controle do LED RGB piscando 5 vezes por segundo
        uint32_t tempo_atual = to_ms_since_boot(get_absolute_time()); // Tempo atual em milissegundos
        if (tempo_atual - ultimo_tempo_troca_led_rgb >= 100) {
            estado_led_rgb = !estado_led_rgb; // Inverte o estado do LED RGB
            gpio_put(LED_RGB_RED_PIN, estado_led_rgb); // Define o estado do LED RGB vermelho
            gpio_put(LED_RGB_GREEN_PIN, 0); // Desliga o LED RGB verde
            gpio_put(LED_RGB_BLUE_PIN, 0); // Desliga o LED RGB azul
            ultimo_tempo_troca_led_rgb = tempo_atual; // Atualiza o tempo da última troca
        }

        // Aguarda um pouco para evitar uso excessivo da CPU (10 ms) e continuar o loop
        sleep_ms(10);
    }

    return 0;
}
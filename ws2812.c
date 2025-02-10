// Bibliotecas necessárias para Raspberry Pi Pico W
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "ws2812.pio.h"
#include "inc/font.h"
#include "inc/ssd1306.h"
#include "pico/stdio_uart.h"
#include "pico/stdio_usb.h"

// Definições de hardware
#define PINO_MATRIZ 7      // Pino de dados da matriz de LEDs
#define NUM_PIXELS 25      // Número de LEDs na matriz
#define LED_VERMELHO 13    // Pino do LED vermelho
#define LED_VERDE 11       // Pino do LED verde
#define LED_AZUL 12        // Pino do LED azul
#define BOTAO_A 5          // Pino do botão A
#define BOTAO_B 6          // Pino do botão B
#define PORTA_I2C i2c1     // Porta I2C
#define I2C_SDA 14         // Pino SDA
#define I2C_SCL 15         // Pino SCL
#define DEBOUNCE_DELAY 200 // Define para o atraso de debounce

// Variáveis globais
bool estadoLedVerde = false;
bool estadoLedAzul = false;

// Funções
uint32_t ultimo_tempo_botao_a = 0;
uint32_t ultimo_tempo_botao_b = 0;

// Programa para controlar a matriz de LEDs WS2812
PIO pio = pio0; // Pino de saída
uint sm;        // Máquina de estado
uint offset;    // Offset do programa
ssd1306_t ssd;  // Display

// Programa para controlar a matriz de LEDs WS2812
void exibir_numero_na_matriz(uint8_t num)
{
    uint32_t padroesNumeros[10][NUM_PIXELS] = {
        {1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1}, // 0
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1}, // 1
        {1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1}, // 2
        {1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1}, // 3
        {0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1}, // 4
        {1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1}, // 5
        {1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1}, // 6
        {0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1}, // 7
        {1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1}, // 8
        {1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1}  // 9
    };

    // Exibe o número na matriz de LEDs
    if (num > 9)
        return;
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        // Define a cor do LED de acordo com o padrão
        uint32_t cor = padroesNumeros[num][i] ? 0x00FF00 : 0x000000;
        pio_sm_put_blocking(pio, sm, cor << 8); // Envia a cor para o LED
    }
}

void atualizar_display()
{
    // Array de mensagens baseadas no estado dos LEDs
    const char *mensagens[4][2] = {
        {"Nenhum LED", "Ligado"}, // Estado 0: Ambos desligados
        {"LED Azul", "Ligado"},  // Estado 1: Apenas o LED azul ligado
        {"LED Verde", "Ligado"},   // Estado 2: Apenas o LED verde ligado
        {"Ambos LEDs", "Ligados"} // Estado 3: Ambos ligados
    };

    // Calcula o índice do estado atual
    uint8_t estado = (estadoLedVerde << 1) | estadoLedAzul;

    // Limpa o display
    ssd1306_fill(&ssd, false);

    // Exibe a mensagem correspondente ao estado atual
    ssd1306_draw_string(&ssd, mensagens[estado][0], 0, 0); // Linha superior
    ssd1306_draw_string(&ssd, mensagens[estado][1], 0, 8); // Linha inferior

    // Envia os dados para o display
    ssd1306_send_data(&ssd);
}

// Função de callback para tratar os botões A e B
void botao_callback(uint gpio, uint32_t eventos)
{
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());

    // Verifica se ambos os botões estão pressionados
    if (gpio_get(BOTAO_A) == 0 && gpio_get(BOTAO_B) == 0)
    {
        // Verifica se o tempo de debounce foi atingido
        if (tempo_atual - ultimo_tempo_botao_a > DEBOUNCE_DELAY && tempo_atual - ultimo_tempo_botao_b > DEBOUNCE_DELAY)
        {
            estadoLedVerde = true;               // Liga o LED verde
            estadoLedAzul = true;                // Liga o LED azul
            gpio_put(LED_VERDE, estadoLedVerde); // Atualiza o estado dos LEDs
            gpio_put(LED_AZUL, estadoLedAzul);   // Atualiza o estado dos LEDs

            // Exibe a mensagem no console
            printf("Ambos botoes pressionados. LEDs Verde e Azul ligados.\n");
            atualizar_display(); // Atualiza o display

            ultimo_tempo_botao_a = tempo_atual; // Atualiza o tempo do botão A
            ultimo_tempo_botao_b = tempo_atual; // Atualiza o tempo do botão B
            return;                             // Sai da função após tratar ambos os botões
        }
    }

    // Trata o botão A
    if (gpio == BOTAO_A)
    {
        // Verifica se o tempo de debounce foi atingido
        if (tempo_atual - ultimo_tempo_botao_a > DEBOUNCE_DELAY)
        {
            if (gpio_get(BOTAO_A) == 0)
            {
                estadoLedVerde = !estadoLedVerde;                                                        // Alterna o estado do LED verde
                gpio_put(LED_VERDE, estadoLedVerde);                                                     // Atualiza o estado do LED verde
                printf("Botao A pressionado. LED Verde %s.\n", estadoLedVerde ? "ligado" : "desligado"); // Exibe a mensagem no console
            }
            ultimo_tempo_botao_a = tempo_atual; // Atualiza o tempo do botão A
        }
    }
    // Trata o botão B
    else if (gpio == BOTAO_B)
    {
        // Verifica se o tempo de debounce foi atingido
        if (tempo_atual - ultimo_tempo_botao_b > DEBOUNCE_DELAY)
        {
            if (gpio_get(BOTAO_B) == 0)
            {
                estadoLedAzul = !estadoLedAzul;                                                        // Alterna o estado do LED azul
                gpio_put(LED_AZUL, estadoLedAzul);                                                     // Atualiza o estado do LED azul
                printf("Botao B pressionado. LED Azul %s.\n", estadoLedAzul ? "ligado" : "desligado"); // Exibe a mensagem no console
            }
            ultimo_tempo_botao_b = tempo_atual; // Atualiza o tempo do botão B
        }
    }

    // Atualiza o display e a matriz
    uint8_t numero_exibido = (estadoLedVerde || estadoLedAzul) ? 1 : 0; // Exibe 1 se algum LED estiver ligado
    exibir_numero_na_matriz(numero_exibido);                            // Exibe o número na matriz
    atualizar_display();                                                // Atualiza o display
}

// Função para processar caracteres recebidos pela porta serial USB
void processar_caractere_serial()
{
    // Verifica se há caracteres disponíveis na porta serial
    if (stdio_usb_connected())
    {
        int caractere = getchar_timeout_us(0); // Lê um caractere da porta serial
        // Verifica se o caractere é válido
        if (caractere != PICO_ERROR_TIMEOUT)
        {
            ssd1306_fill(&ssd, false);                 // Limpa o display
            char buffer[2] = {(char)caractere, '\0'};  // Converte o caractere para uma string
            ssd1306_draw_string(&ssd, buffer, 40, 20); // Exibe o caractere no display
            ssd1306_send_data(&ssd);                   // Envia os dados para o display

            // Verifica se o caractere é um dígito
            if (caractere >= '0' && caractere <= '9')
            {
                uint8_t numero = caractere - '0'; // Converte o caractere para um número
                exibir_numero_na_matriz(numero);  // Exibe o número na matriz
            }
        }
    }
}

int main()
{
    stdio_init_all(); // Inicializa a porta serial USB

    i2c_init(PORTA_I2C, 400 * 1000);           // Inicializa a porta I2C
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Define o pino SDA como pino I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Define o pino SCL como pino I2C
    gpio_pull_up(I2C_SDA);                     // Habilita o resistor de pull-up no pino SDA
    gpio_pull_up(I2C_SCL);                     // Habilita o resistor de pull-up no pino SCL

    gpio_init(LED_VERDE);              // Inicializa o pino do LED verde
    gpio_set_dir(LED_VERDE, GPIO_OUT); // Define o pino do LED verde como saída
    gpio_put(LED_VERDE, 0);            // Desliga o LED verde

    gpio_init(LED_AZUL);              // Inicializa o pino do LED azul
    gpio_set_dir(LED_AZUL, GPIO_OUT); // Define o pino do LED azul como saída
    gpio_put(LED_AZUL, 0);            // Desliga o LED azul

    gpio_init(BOTAO_A);                                      // Inicializa o pino do botão A
    gpio_set_dir(BOTAO_A, GPIO_IN);                          // Define o pino do botão A como entrada
    gpio_pull_up(BOTAO_A);                                   // Habilita o resistor de pull-up no pino do botão A
    gpio_set_irq_enabled(BOTAO_A, GPIO_IRQ_EDGE_FALL, true); // Habilita a interrupção por borda de descida no botão A

    gpio_init(BOTAO_B);                                      // Inicializa o pino do botão B
    gpio_set_dir(BOTAO_B, GPIO_IN);                          // Define o pino do botão B como entrada
    gpio_pull_up(BOTAO_B);                                   // Habilita o resistor de pull-up no pino do botão B
    gpio_set_irq_enabled(BOTAO_B, GPIO_IRQ_EDGE_FALL, true); // Habilita a interrupção por borda de descida no botão B

    ssd1306_init(&ssd, WIDTH, HEIGHT, false, 0x3C, PORTA_I2C); // Inicializa o display
    ssd1306_config(&ssd);                                      // Configura o display
    ssd1306_send_data(&ssd);                                   // Envia os dados para o display
    ssd1306_fill(&ssd, false);                                 // Limpa o display
    ssd1306_send_data(&ssd);                                   // Envia os dados para o display

    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &botao_callback); // Habilita a interrupção por borda de descida no botão A
    gpio_set_irq_enabled_with_callback(BOTAO_B, GPIO_IRQ_EDGE_FALL, true, &botao_callback); // Habilita a interrupção por borda de descida no botão B

    offset = pio_add_program(pio, &ws2812_program);                   // Adiciona o programa ao PIO
    sm = pio_claim_unused_sm(pio, true);                              // Reivindica uma máquina de estado
    ws2812_program_init(pio, sm, offset, PINO_MATRIZ, 800000, false); // Inicializa o programa WS2812 no PIO e no pino da matriz de LEDs

    // Loop principal do programa
    while (true)
    {
        processar_caractere_serial(); // Processa os caracteres recebidos pela porta serial
        sleep_ms(100);                // Aguarda 100 ms
    }

    return 0; // Retorna 0
}

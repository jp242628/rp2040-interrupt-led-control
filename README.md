# Projeto WS2812_Display com Raspberry Pi Pico W

## Descrição
Este projeto demonstra o controle de uma matriz de LEDs WS2812, LEDs comuns e um display OLED 128x64 utilizando um Raspberry Pi Pico W. O projeto inclui funcionalidades de controle de LEDs via botões com tratamento de debounce, exibição de mensagens no display OLED e comunicação serial via UART.

## Funcionalidades
- Controle de LEDs comuns (verde e azul) utilizando botões com tratamento de debounce.
- Controle de uma matriz de LEDs WS2812 para exibir números.
- Exibição de mensagens no display OLED 128x64 via I2C.
- Envio de informações pela UART.
- Processamento de caracteres recebidos pela porta serial USB.

## Hardware e Conexões
- **Raspberry Pi Pico W**
- **Matriz de LEDs WS2812** conectada ao pino GPIO 7.
- **LEDs comuns**:
  - LED verde no pino GPIO 11.
  - LED azul no pino GPIO 12.
  - LED vermelho no pino GPIO 13.
- **Botões**:
  - Botão A no pino GPIO 5.
  - Botão B no pino GPIO 6.
- **Display OLED 128x64** conectado via I2C:
  - SDA no pino GPIO 14.
  - SCL no pino GPIO 15.

## Compilação e Execução
1. Clone o repositório:
   ```sh
   git clone <URL_DO_REPOSITORIO>
   cd <NOME_DO_REPOSITORIO>
   ```
   
2. Configure o ambiente, compile e carregue o código no Raspberry Pi Pico utilizando o ambiente de desenvolvimento de sua preferência.   

## Código Principal

O código principal está localizado no arquivo `ws2812.c`. Ele inclui a inicialização do hardware, configuração dos periféricos e o loop principal do programa. As principais funções são:

- `exibir_numero_na_matriz(uint8_t num)`: Exibe um número na matriz de LEDs WS2812.
- `atualizar_display()`: Atualiza o display OLED com mensagens baseadas no estado dos LEDs.
- `botao_callback(uint gpio, uint32_t eventos)`: Função de callback para tratar interrupções dos botões.
- `processar_caractere_serial()`: Processa caracteres recebidos pela porta serial USB.
- `main()`: Função principal que inicializa o hardware e entra no loop principal.

Para mais detalhes, consulte o código-fonte no arquivo `ws2812.c`.
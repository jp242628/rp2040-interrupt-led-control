# rp2040-interrupt-led-control
Repositório do projeto da EmbarcaTech - Unidade 4 (Microcontroladores): controle de LEDs com interrupções no RP2040 usando a placa BitDogLab. Inclui debouncing de botões, LEDs RGB e matriz WS2812, integrando hardware e software.

# WS2812 5x5 Matrix Control

Este projeto demonstra o controle de uma matriz de LEDs WS2812 (5x5) utilizando um Raspberry Pi Pico. O controle é feito através de dois botões que incrementam e decrementam o número exibido na matriz. Além disso, um LED RGB comum é utilizado para piscar a uma taxa de 5 vezes por segundo.

### Vídeo de Demonstração
O vídeo de demonstração deste projeto foi entregue no Moodle, conforme solicitado. Nele, são mostradas as seguintes funcionalidades:
- Controle do LED RGB piscando em vermelho.
- Incremento e decremento dos números na matriz de LEDs WS2812.
- Efeitos visuais dos números de 0 a 9 na matriz.

## Funcionalidades

1. **Uso de interrupções**: Todas as funcionalidades relacionadas aos botões são implementadas utilizando rotinas de interrupção (IRQ).
2. **Debouncing**: Implementação do tratamento de bouncing dos botões via software.
3. **Controle de LEDs**: O projeto inclui o uso de LEDs comuns e LEDs WS2812, demonstrando o domínio de diferentes tipos de controle.

## Hardware Utilizado

- Raspberry Pi Pico
- Matriz de LEDs WS2812 (5x5)
- 2 Botões
- LED RGB comum

## Conexões

- **Matriz WS2812**: Pino de dados conectado ao GPIO 7 do Pico.
- **Botão A**: Conectado ao GPIO 5 do Pico.
- **Botão B**: Conectado ao GPIO 6 do Pico.
- **LED RGB**:
  - Vermelho: Conectado ao GPIO 11 do Pico.
  - Verde: Conectado ao GPIO 12 do Pico.
  - Azul: Conectado ao GPIO 13 do Pico.

## Código

O código está bem estruturado e comentado para facilitar o entendimento. As principais funções são:

- `enviar_pixel(uint32_t pixel_grb)`: Envia um pixel para a matriz de LEDs.
- `converter_rgb_u32(uint8_t r, uint8_t g, uint8_t b)`: Converte valores RGB para um único valor de 32 bits.
- `atualizar_buffer_leds()`: Apaga todos os LEDs.
- `definir_leds_do_buffer()`: Envia o estado do buffer de LEDs para a matriz.
- `botao_callback(uint gpio, uint32_t eventos)`: Callback para tratar interrupções dos botões.
- `exibir_numero(uint8_t num)`: Exibe um número na matriz (0-9).

## Como Executar

1. Clone o repositório:
    ```sh
    git clone https://github.com/jp242628/rp2040-interrupt-led-control.git
    ```
2. Navegue até o diretório do projeto:
    ```sh
    cd rp2040-interrupt-led-control
    ```
3. Compile e carregue o código no Raspberry Pi Pico utilizando o ambiente de desenvolvimento de sua preferência.

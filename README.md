# U4A10 ADC JOYSTICK
Este projeto usa a Raspberry Pi Pico com a placa BitDogLab para controlar LEDs RGB e interagir com um joystick. Os eixos X e Y do joystick ajustam o brilho dos LEDs Azul e Vermelho via PWM, apagando-os na posição central (valor 2048) e aumentando o brilho nos extremos (0 e 4095). O LED Verde liga e desliga ao pressionar o botão do joystick.

O botão do joystick também deve alterar a borda do display SSD1306, mas a comunicação I2C ainda não foi implementada. O botão A ativa ou desativa os LEDs controlados por PWM. As funções dos botões usam interrupções (IRQ) com debounce em software para evitar leituras incorretas.

Os principais componentes são: joystick (GPIOs 26 e 27), botão do joystick (GPIO 22), botão A (GPIO 5), LEDs RGB (GPIOs 11, 12 e 13) e o display SSD1306 via I2C (GPIOs 14 e 15).

O código-fonte está no repositório e há dois vídeos demonstrando o projeto:

Explicação do Código https://youtu.be/5m87WQF7bvU

Execução na Placa Com Execução no Terminal https://youtube.com/shorts/oy3Iuqm9fc8?feature=share 

Observação: O projeto está incompleto, a comunicação I2C com o display SSD1306 ainda não foi implementada.

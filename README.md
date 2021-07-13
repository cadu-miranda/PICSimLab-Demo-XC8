# PICSimLab-Demo-XC8

## Aplicação feita no Ubuntu 21.10 e no PICSimLab 0.8.7

Código feito em linguagem C com o compilador XC8 para demonstração da placa PICGenios do simulador PICSimLab. 

# No simulador -> Teclado Matricial...

-> Pressione (1) para alternar entre ligado/desligado os LEDS do PORTB<7:4> por 5 vezes.

<img src="/imagens/botao-1.png" width="700" height="400">

-> Pressione (2) para ativar o display de 7 segmentos e mostrar o padrão "0 1 2 3" por 3 vezes.

<img src="/imagens/botao-2.png" width="700" height="400">

-> Pressione (3) para ativar o TRIMPOT (P1-AN0) e fazer a conversão A/D. [Note no LCD o valor do TRIMPOT - (AN0 = 0349)].

<img src="/imagens/botao-3.png" width="700" height="400">

-> Pressione (4) para alternar entre ligado/desligado os relês por 3 vezes.

<img src="/imagens/botao-4.png" width="700" height="400">

-> Pressione (5) para modular a largura de pulso (PWM) da ventoinha entre 0-100% / 0-1023 (resolução de 10 bits).

<img src="/imagens/botao-5.png" width="700" height="400">

-> Pressione (6) para gravar o valor (0x61 - ASCII 'a') na memória EEPROM interna do PIC.

<img src="/imagens/botao-6.png" width="700" height="400">

-> Pressione (7) para gravar o valor (0x62 - ASCII 'b') em memória EEPROM externa via protocolo I2C. [Note o valor gravado no simulador - 62].

<img src="/imagens/botao-7.png" width="700" height="400">

# USART

O acompanhamento do código pode ser feito através do canal USART. Para isso, foi feita a instalação e a configuração do tty0tty.

<img src="/imagens/usart.png" width="700" height="400">

# * Código sujeito a mudanças.

## OBS: Os botões (8) (9) (*) (0) e (#) NÃO executam nenhuma ação, mas podem ser programados para tal.

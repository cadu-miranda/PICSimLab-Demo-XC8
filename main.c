// Código de demonstração para o PIC18F4520 usando o compilador XC8 e o simulador PICSimLab.

/* 
 * DIP1 -> Ligar: 1, 2, 3, 4, 5 e 10
 * DIP2 -> Ligar: 1, 2, 3, 4, 8 e 9
 */

#include <xc.h>
#include <stdio.h>
#include <stdbool.h>
#include "../../libraries/fuses4520.h"
#include "../../libraries/USART.h"

#define _XTAL_FREQ 8000000UL

// Protótipos das funções

// LCD
void lcd_init(void);
void lcd_cmd(char cmd);
void lcd_envia_byte(char nivel, char dado);
char set_bit(char cmd, char comp);
void lcd_escreve_bf(char lin, char col, char *str);

// Teclado Matricial
char varreTeclas(void);
char trataTeclas(void);

// EEPROM Interna
unsigned char eepromREAD(unsigned char addr);
void eepromWRITE(unsigned char addr, unsigned char val);

// Principais
void c1_leds(void);
void c2_7segs(void);
void c3_convAD(void);
void c4_reles(void);
void c5_pwm(void);
void dutyCycle(int valor);
void c6_eeprom_int(void);
void c_outros(char caractere);

// Definição da matriz do teclado (3 x 4)

#define LIN1 LATDbits.LD0
#define LIN2 LATDbits.LD1
#define LIN3 LATDbits.LD2
#define LIN4 LATDbits.LD3

#define COL1 PORTBbits.RB0
#define COL2 PORTBbits.RB1
#define COL3 PORTBbits.RB2

int leituraPotenciometro = 0x00;
_Bool lePotenciometro = 0x00, teclaPressionada = 0x00;
char bufferLCD[16], bufferUSART[50], tecla = 0x00, i = 0x00;

const unsigned char digitos[] = {

    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01100111 // 9  
};

int main(void) {

    TRISAbits.RA2 = 0x00;
    TRISAbits.RA3 = 0x00;
    TRISAbits.RA4 = 0x00;
    TRISAbits.RA5 = 0x00;

    LATAbits.LA2 = 0x00;
    LATAbits.LA3 = 0x00;
    LATAbits.LA4 = 0x00;
    LATAbits.LA5 = 0x00;

    TRISB = 0b00000111;

    PORTB = 0b00000000;

    TRISCbits.RC0 = 0x00; // Relê 1 
    TRISCbits.RC1 = 0x00; // Buzzer
    TRISCbits.RC2 = 0x00; // Ventoinha

    LATCbits.LC0 = 0x00;
    LATCbits.LC1 = 0x00;
    LATCbits.LC2 = 0x00;

    TRISD = 0b00000000;

    LATD = 0b00000000;

    TRISEbits.RE0 = 0x00; // Relê 2

    LATEbits.LE0 = 0x00;

    // Begin - Configuração dos registradores de conversão A/D

    ADCON0bits.ADON = 0x01; // conversor A/D habilitado

    ADCON1 = 0b00001110; /* Bits 7 e 6 = 0 - Não utilizados
                          * VCFG <1:0> = 00 - Define as tensões de referência em GND e VDD
                          * PCFG<3:0>  = 1110 - Habilita canal 0 para leitura
                          */

    ADCON2 = 0b10000111; /* ADFM      = 1 - Resultado da covnersão justificado à direita
                          * Bit 6     = 0 - Não utilizado
                          * ACQT<2:0> = 0 - 0 TAD
                          * ADCS<2:0> = 111 - Oscilador interno para A/D
                           */

    // End - Configuração dos registradores de conversão A/D


    // Begin - Configuração dos registradores do PWM

    T2CON = 0b00000100; /*Prescaler = 1:1
                                 * Postscaler = 1:1
                                 * TMR2ON = 1*/

    PR2 = 249; // Período de PWM = 125us - Freq. 8KHz

    TMR2 = 0x00;

    // End - Configuração dos registradores do PWM

    RBPU = 0x00;

    lcd_init();
    USART_init();

    USART_print("Demonstracao - PICSimLab - PICGenios\n\r");
    USART_print("=======================================================\n\r");
    USART_print("\nMicrocontrolador PIC18F4520\r\n");
    USART_print("Baudrate  = 9600\r\n");
    USART_print("Data Bits = 8\r\n");
    USART_print("(N)o Parity Bit\r\n");
    USART_print("(1) Stop Bit\r\n");
    USART_print("\r\n");

    lcd_escreve_bf(1, 1, "   PIC18F4520   ");
    lcd_escreve_bf(2, 1, "  Demonstracao  ");

    __delay_ms(1000);

    lcd_escreve_bf(1, 1, "Tecla:          ");
    lcd_escreve_bf(2, 1, "                ");

    USART_print("Digite uma tecla:\r\n\n");

    USART_print("1      - LEDS\r\n");
    USART_print("2      - Display de 7 segmentos\r\n");
    USART_print("3      - Conversor A/D\r\n");
    USART_print("4      - RELES\r\n");
    USART_print("5      - VNT PWM\r\n");
    USART_print("6      - EEPROM interna\r\n");
    USART_print("OUTROS - SEM ACAO\r\n");

    for (;;) {

        tecla = trataTeclas();

        if (teclaPressionada) {

            LATCbits.LC1 = 0x01;

            __delay_ms(50);

            LATCbits.LC1 = 0x00;

            sprintf(bufferLCD, "%c", tecla);
            lcd_escreve_bf(1, 8, bufferLCD);

            switch (tecla) {

                case '1':

                    c1_leds();
                    break;

                case '2':

                    c2_7segs();
                    break;

                case '3':

                    c3_convAD();
                    break;

                case '4':

                    c4_reles();
                    break;

                case '5':

                    c5_pwm();
                    break;

                case '6':

                    c6_eeprom_int();
                    break;

                case '7':

                    c_outros('7');
                    break;

                case '8':

                    c_outros('8');
                    break;

                case '9':

                    c_outros('9');
                    break;

                case '*':

                    c_outros('*');
                    break;

                case '0':

                    c_outros('0');
                    break;

                case '#':

                    c_outros('#');
                    break;
            }

            if (!lePotenciometro) {

                lcd_escreve_bf(1, 1, "Tecla:          ");
                lcd_escreve_bf(2, 1, "                ");

                USART_print("\r\nDigite uma tecla:\r\n");

                teclaPressionada = 0x00;

                __delay_ms(50);
            }
        }

        if (lePotenciometro) {

            ADCON0 = 0b00000001; // seleciona canal AN0 para leitura A/D - TRIMPOT

            __delay_us(100);

            ADCON0bits.GO = 0x01; // inicia a conversão A/D

            while (ADCON0bits.GO); // aguarda a finalização da conversão

            leituraPotenciometro = ADRESH;

            leituraPotenciometro = (leituraPotenciometro << 8) + ADRESL;

            lcd_escreve_bf(1, 1, "Atue no Pot. AN0");

            sprintf(bufferLCD, "   AN0 = %04d   ", leituraPotenciometro);
            lcd_escreve_bf(2, 1, bufferLCD);

            sprintf(bufferUSART, "AN0 = %04d \r\n", leituraPotenciometro);
            USART_print(bufferUSART);

            __delay_ms(100);
        }
    }

    return 0;
}

void lcd_init(void) {

    char i; // Variável de controle para envio de comandos

    TRISD = 0b00000000; //  Define todos os bits da porta D como saída
    TRISEbits.RE2 = 0; //  Define o bit 2 da porta E como saída
    TRISEbits.RE1 = 0; //  Define o bit 1 da porta E como saída

    LATD = 0b00000000; // Desliga todos os bits da porta D
    LATEbits.LE2 = 0; // Desliga o bit 2 da porta E
    LATEbits.LE1 = 0; //  Desliga o bit 1 da porta E

    __delay_ms(15);

    for (i = 0; i < 3; i++) {

        lcd_cmd(0x30);
        __delay_ms(5);
    }

    lcd_cmd(0x02); // informa que o lcd deve trabalhar com 4 bits de dados
    __delay_us(40); // atraso de 40 us

    lcd_cmd(0x28); // Comunicação em 4 bits com 2 linhas p/ matrix 7x5
    __delay_us(40); // Atraso de 40 us

    lcd_cmd(0x01); // limpa a memória do LCD
    __delay_ms(2); // Atraso de 2 ms

    lcd_cmd(0x0C); // liga o display sem cursor
    __delay_us(40); // Atraso de 40 us

    lcd_cmd(0x06); // desloca o cursor à direita após um novo caractere
    __delay_us(40); // atraso de 40 us
}

char set_bit(char cmd, char comp) {

    if ((cmd & comp) >= 1)

        return 1;

    else

        return 0;
}

// 0x30 = 0b00110000 & 0b00010000 -> > 0
// 0x0C = 0b00001100 & 0b00010000 -> = 0

void lcd_cmd(char cmd) {

    LATDbits.LD4 = set_bit(cmd, 0b00010000);
    LATDbits.LD5 = set_bit(cmd, 0b00100000);
    LATDbits.LD6 = set_bit(cmd, 0b01000000);
    LATDbits.LD7 = set_bit(cmd, 0b10000000);

    LATEbits.LE1 = 1; // Atribui o valor alto ao bit 1 da porta E (EN = 1)
    LATEbits.LE1 = 0; // Atribui o valor baixo ao bit 1 da porta E (EN = 0)

    __delay_us(100); // gera um atraso de 100 us

    LATDbits.LD4 = set_bit(cmd, 0b00000001);
    LATDbits.LD5 = set_bit(cmd, 0b00000010);
    LATDbits.LD6 = set_bit(cmd, 0b00000100);
    LATDbits.LD7 = set_bit(cmd, 0b00001000);

    LATEbits.LE1 = 1; // Atribui o valor alto ao bit 1 da porta E (EN = 1)
    LATEbits.LE1 = 0; // Atribui o valor baixo ao bit 2 da porta E (EN = 0)
}

void lcd_envia_byte(char nivel, char dado) {

    LATEbits.LE2 = nivel; // habilita comando ou leitura para o lcd
    __delay_us(100); // atraso de 100 us
    LATEbits.LE1 = 0; // atribui  nível lógico 0 ao bit 1 da porta E  (EN = 0)

    lcd_cmd(dado);

}

void lcd_escreve_bf(char lin, char col, char *str) {

    char end; // variável de controle para posicionamento do cursor

    if (lin == 1) // caso a variável lin seja 1, posiciona o cursor na coluna informada

        end = col + 0x80 - 1;

    else if (lin == 2)

        end = col + 0xC0 - 1; // caso a variável lin seja 2, posiciona o cursor na coluna informada

    lcd_envia_byte(0, end);

    while (*str) {

        lcd_envia_byte(1, *str);
        str++;
    }
}

char varreTeclas(void) {

    char key = ' ';

    LIN1 = 0;
    LIN2 = 1;
    LIN3 = 1;
    LIN4 = 1;

    if (!COL1)

        key = '1';

    if (!COL2)

        key = '2';

    if (!COL3)

        key = '3';

    __delay_ms(1);

    LIN1 = 1;
    LIN2 = 0;
    LIN3 = 1;
    LIN4 = 1;

    if (!COL1)

        key = '*';

    if (!COL2)

        key = '0';

    if (!COL3)

        key = '#';

    __delay_ms(1);

    LIN1 = 1;
    LIN2 = 1;
    LIN3 = 0;
    LIN4 = 1;

    if (!COL1)

        key = '7';

    if (!COL2)

        key = '8';

    if (!COL3)

        key = '9';

    __delay_ms(1);

    LIN1 = 1;
    LIN2 = 1;
    LIN3 = 1;
    LIN4 = 0;

    if (!COL1)

        key = '4';

    if (!COL2)

        key = '5';

    if (!COL3)

        key = '6';

    __delay_ms(1);

    return (key);
}

char trataTeclas(void) {

    int t;

    t = varreTeclas();

    if ((t != ' ') && (!teclaPressionada)) {

        teclaPressionada = 1;

        if (t != tecla) {

            __delay_ms(10);
            if (varreTeclas() == t) return (t);
        }
    }

    teclaPressionada = 0;
}

unsigned char eepromREAD(unsigned char addr) {

    EEADR = addr;
    EECON1bits.RD = 1;

    return EEDATA;
}

void eepromWRITE(unsigned char addr, unsigned char val) {

    EEADR = addr;
    EEDATA = val;

    EECON1bits.WREN = 1;

    EECON2 = 0x55;
    EECON2 = 0xAA;

    EECON1bits.WR = 1;
    while (EECON1bits.WR);

    EECON1bits.WREN = 0;

    return;
}

void c1_leds(void) {

    lcd_escreve_bf(2, 1, "      LEDS      ");
    USART_print("\r\n1 - LEDS\r\n\n");

    __delay_ms(10);

    for (int i = 0; i < 5; i++) {

        LATBbits.LB4 = 0;
        LATBbits.LB5 = 0;
        LATBbits.LB6 = 0;
        LATBbits.LB7 = 0;

        __delay_ms(500);

        USART_print("LEDS ON\r\n");

        LATBbits.LB4 = 1;
        LATBbits.LB5 = 1;
        LATBbits.LB6 = 1;
        LATBbits.LB7 = 1;

        __delay_ms(500);

        USART_print("LEDS OFF\r\n\n");
    }

    LATBbits.LB4 = 0;
    LATBbits.LB5 = 0;
    LATBbits.LB6 = 0;
    LATBbits.LB7 = 0;

    lcd_init();

    lcd_escreve_bf(1, 1, "Tecla:          ");
    lcd_escreve_bf(2, 1, "                ");
}

void c2_7segs(void) {

    lcd_escreve_bf(2, 1, "  7 segmentos   ");
    USART_print("\r\n2 - Display de 7 segmentos\r\n\n");

    __delay_ms(10);

    for (int i = 0; i < 3; i++) {

        LATAbits.LA2 = 1;
        LATD = digitos[0]; // 0 no DISP1
        USART_print("0 ");
        __delay_ms(500);
        LATAbits.LA2 = 0;

        LATAbits.LA3 = 1;
        LATD = digitos[1]; // 1 no DISP2
        USART_print("1 ");
        __delay_ms(500);
        LATAbits.LA3 = 0;

        LATAbits.LA4 = 1;
        LATD = digitos[2]; // 2 no DISP3
        USART_print("2 ");
        __delay_ms(500);
        LATAbits.LA4 = 0;

        LATAbits.LA5 = 1;
        LATD = digitos[3]; // 3 no DISP4
        USART_print("3\r\n");
        __delay_ms(500);
        LATAbits.LA5 = 0;
    }

    LATAbits.LA2 = 0;
    LATAbits.LA3 = 0;
    LATAbits.LA4 = 0;
    LATAbits.LA5 = 0;

    lcd_init();

    lcd_escreve_bf(1, 1, "Tecla:          ");
    lcd_escreve_bf(2, 1, "                ");
}

void c3_convAD(void) {

    lcd_escreve_bf(2, 1, "  Conversor A/D   ");
    USART_print("\r\n3 - Conversor A/D\r\n\n");

    __delay_ms(1000);

    lePotenciometro = ~lePotenciometro;

    if (!lePotenciometro);

    else {

        lcd_escreve_bf(1, 1, "Digite 3 p/ SAIR");
        USART_print("Digite 3 p/ SAIR\r\n\n");

        lcd_escreve_bf(2, 1, "Atue no Pot. AN0");

        __delay_ms(2000);
    }
}

void c4_reles(void) {

    USART_print("\r\n4 - RELES\r\n\n");

    __delay_ms(10);

    for (i = 0; i < 3; i++) {

        LATCbits.LC0 = 1;
        LATEbits.LE0 = 0;

        lcd_escreve_bf(2, 1, "     RELE 1     ");
        USART_print("RELE 1 \r\n");

        __delay_ms(1000);

        LATCbits.LC0 = 0;
        LATEbits.LE0 = 1;

        lcd_escreve_bf(2, 1, "     RELE 2     ");
        USART_print("RELE 2 \r\n\n");

        __delay_ms(1000);
    }

    LATCbits.LC0 = 0;
    LATEbits.LE0 = 0;

    lcd_escreve_bf(1, 1, "Tecla:          ");
    lcd_escreve_bf(2, 1, "                ");
}

void c5_pwm(void) {

    USART_print("\r\n5 - VNT PWM\r\n\n");

    __delay_ms(10);

    dutyCycle(0);
    lcd_escreve_bf(2, 1, " VNT PWM - 0 %  ");
    USART_print("VNT PWM - 0 %\r\n");

    __delay_ms(1000);

    dutyCycle(256);
    lcd_escreve_bf(2, 1, " VNT PWM - 25 % ");
    USART_print("VNT PWM - 25 %\r\n");

    __delay_ms(2500);

    dutyCycle(512);
    lcd_escreve_bf(2, 1, " VNT PWM - 50 % ");
    USART_print("VNT PWM - 50 %\r\n");

    __delay_ms(2500);

    dutyCycle(768);
    lcd_escreve_bf(2, 1, " VNT PWM - 75 % ");
    USART_print("VNT PWM - 75 %\r\n");

    __delay_ms(2500);

    dutyCycle(1023);
    lcd_escreve_bf(2, 1, " VNT PWM - 100 %");
    USART_print("VNT PWM - 100 %\r\n");

    __delay_ms(3000);

    dutyCycle(0);

    lcd_escreve_bf(1, 1, "Tecla:          ");
    lcd_escreve_bf(2, 1, "                ");
}

void c6_eeprom_int(void) {

    lcd_escreve_bf(2, 1, " EEPROM interna ");
    USART_print("\r\n6 - EEPROM interna\r\n");

    __delay_ms(1000);

    lcd_escreve_bf(1, 1, "Escrevendo: 0x61");
    lcd_escreve_bf(2, 1, "   Aguarde...   ");

    eepromWRITE(0x10, 0x61);

    __delay_ms(1500);

    lcd_escreve_bf(1, 1, "Leitura em 0x10:");
    sprintf(bufferLCD, "%x - ASCII: %c", eepromREAD(0x10), eepromREAD(0x10));
    lcd_escreve_bf(2, 2, bufferLCD);

    __delay_ms(1500);
}

void c_outros(char caractere) {

    lcd_escreve_bf(2, 1, "    SEM ACAO    ");

    sprintf(bufferUSART, "\r\n%c - SEM ACAO\r\n", caractere);
    USART_print(bufferUSART);

    __delay_ms(500);

    lcd_escreve_bf(1, 1, "Tecla:          ");
    lcd_escreve_bf(2, 1, "                ");
}

void dutyCycle(int valor) {

    int baixo = (valor << 4) | 0X0C; //assim mantem os 4 bits mais baixo como 1100 - modo PWM

    CCPR1L = valor >> 2;
    CCP1CON = baixo & 0b00111111;

    //Duty Cycle (Período) = CCP1CON + CCPR1L <5:4> * Tosc * (Prescaler TIMER2) 
    //Para converter em Freq. fazer 1/ Período de Duty Cycle
}

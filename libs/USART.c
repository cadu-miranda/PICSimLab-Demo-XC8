#include "USART.h"

void USART_init(void) {

    // inicializa a comunicação serial

    TXSTAbits.TXEN = 1; // habilita a transmissão pela USART
    RCSTAbits.CREN = 1; // habilita a recepção pela serial (adição)
    TXSTAbits.SYNC = 0; // habilita o modo assíncrono
    RCSTAbits.SPEN = 1; // habilita USART e condigura os pinos RC6 e RC7 como TX e RX

    TXSTAbits.BRGH = 0; // configura os controle de baudrate
    BAUDCTLbits.BRG16 = 0;

    SPBRG = 12; // baudrate = 9600bps
}

void USART_print_char(char c) {

    // envia caracteres pela serial

    while (!TRMT); // aguarda a finalização da transmissão do caractere

    TXREG = c; // carrega em TXREG e transmite o caractere via USART
}

void USART_print(char *str) {

    // envia string pela serial

    while (*str != '\0') {

        USART_print_char(*str);
        str++;
    }
}

char USART_read(void) {

    while (!RCIF); // enquanto !RCIF, aguarda a chegada de um caractere

    return RCREG; // retorna caractere recebido 
}

#include "EEPROM.h"

void I2C_Master_Init(const unsigned long c) {

    SSPCON1 = 0b00101000; //SSP Module as Master
    SSPCON2 = 0;
    SSPADD = (_XTAL_FREQ / (4 * c)) - 1; //Setting Clock Speed
    SSPSTAT = 0;
    TRISCbits.RC3 = 1;
    TRISCbits.RC4 = 1;
}

void I2C_Master_Wait(void) {

    while ((SSPSTAT & 0x04) || (SSPCON2 & 0x1F)); //Transmit is in progress
}

void I2C_Master_Start(void) {

    I2C_Master_Wait();
    SEN = 1; //Initiate start condition
}

void I2C_Master_RepeatedStart(void) {

    I2C_Master_Wait();
    RSEN = 1; //Initiate repeated start condition
}

void I2C_Master_Stop(void) {

    I2C_Master_Wait();
    PEN = 1; //Initiate stop condition
}

void I2C_Master_Write(unsigned d) {

    I2C_Master_Wait();
    SSPBUF = d; //Write data to SSPBUF
}

unsigned short I2C_Master_Read(unsigned short a) {

    unsigned short temp;

    I2C_Master_Wait();
    RCEN = 1;
    I2C_Master_Wait();
    temp = SSPBUF; //Read data from SSPBUF
    I2C_Master_Wait();
    ACKDT = (a) ? 0 : 1; //Acknowledge bit
    ACKEN = 1; //Acknowledge sequence

    return temp;
}

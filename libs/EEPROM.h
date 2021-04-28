#include <xc.h>
#define _XTAL_FREQ 8000000UL

void I2C_Master_Init(const unsigned long c);
void I2C_Master_Wait(void);
void I2C_Master_Start(void);
void I2C_Master_RepeatedStart(void);
void I2C_Master_Stop(void);
void I2C_Master_Write(unsigned d);
unsigned short I2C_Master_Read(unsigned short a);

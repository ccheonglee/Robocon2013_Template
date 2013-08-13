#ifndef __24LC512_H__
#define __24LC512_H__

#include "stm32f4xx.h"

#define I2C_ADDRESS7_MAIN      0xAA
#define I2C_ADDRESS7_EEPROM    0xA0

#define RELAY_TICK_ADD   0x1000
#define WIRE_REST_ADD   0x2000


//#define EEROM_WRITE_ENABLE   GPIO_ResetBits(GPIOB, GPIO_Pin_5)
//#define EEROM_WRITE_DISABLE   GPIO_ResetBits(GPIOB, GPIO_Pin_5)
//#define EEROM_WRITE_DISABLE   GPIO_SetBits(GPIOB, GPIO_Pin_5)


void eepromInit (void);
void eepromWriteByte (uint16_t address, uint8_t data);
uint8_t eepromReadByte (uint16_t address);
void eepromWriteData (uint16_t address, uint8_t* data, uint16_t length);
void eepromReadData (uint16_t address, uint8_t* data, uint16_t length);
void eepromWaitReady (void);

#endif

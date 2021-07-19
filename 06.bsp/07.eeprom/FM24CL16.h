
#ifndef   FM24CL16_H
#define   FM24CL16_H

#include <stm32f2xx.h>
#include "string.h"
#include "type.h"

uint8  FMWrite(uint8 *writedata, uint32 addr, uint16 num);
uint8  FMRead(uint8 *readdata, uint32 addr, uint16 num);

void FM24_Init(void);

#define Eeprom_Write(a,b,c)	FMWrite(b,a,c)
#define Eeprom_Read(a,b,c) FMRead(b,a,c)
#define Eeprom_Init() FM24_Init()

#endif


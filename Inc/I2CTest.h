#ifndef __I2CTEST_H__
#define __I2CTEST_H__

#include "fm33le0xx_fl.h"
#include "main.h"
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
#define DELAY_T         1       //(us)????,??????????

/*----------- I2C?? -----------*/
#define MY_DEV_ADDR     0x50    //EEPROM????(7?,??????)

/*----------- API -----------*/
//?????I2C                                    
void SoftI2C_Init(void);
uint8_t I2CTest(void);

//???
//void    SoftI2C_Start(void);
//void    SoftI2C_Stop(void);
//uint8_t SoftI2C_In(bool ack);
//bool    SoftI2C_Out(uint8_t data);

//??I2C???
bool SoftI2C_Write(uint16_t addr, bool addr16b, uint8_t *buf, uint32_t len);

//??I2C???
bool SoftI2C_Read(uint16_t addr, bool addr16b, uint8_t *buf, uint32_t len);
uint8_t Test_I2C(void);
#ifdef __cplusplus
}// extern "C"
#endif
#endif

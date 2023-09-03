/**
  ****************************************************************************************************
  * @file    main.c
  * @author  FMSH Application Team
  * @brief   Header file of FL Module
  ****************************************************************************************************
  * @attention
  *
  * Copyright (c) [2021] [Fudan Microelectronics]
  * THIS SOFTWARE is licensed under Mulan PSL v2.
  * can use this software according to the terms and conditions of the Mulan PSL v2.
  * You may obtain a copy of Mulan PSL v2 at:
  *          http://license.coscl.org.cn/MulanPSL2
  * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
  * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
  * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
  * See the Mulan PSL v2 for more details.
  *
  ****************************************************************************************************
  */

#include "main.h"
#include "I2C.h"
#include "I2CTest.h"

#include "Arduino.h"
#include "Wire.h"

#define ZT7548_SLAVE_ADDR 0x20

#define LED PC1

#define RESET_Pin PC0
#define TP_INT_Pin PA0
#define TE_IN_Pin PA1

#define Soft_i2C

uint8_t read_buf[40];
uint8_t cmd_buf[10];
uint8_t staus = 0;
TwoWire ZT7548(SCL_Pin, SDA_Pin, SOFT_FAST);
bool busy = 0;
/**    
* Chip Series: FM33LE0xx;
* FL Version: v1.2;
* Project Name: I2C_Test;
* Project Version: v1.2.1.0;
* Project ID: 1692899958052622337;
* Create Date: 2023-08-19;
*/
void ZT7548_INT()
{
			  #ifdef HW_i2C			
			  I2C_Read_nByte(0x20, 0x8000,read_buf, 40);
	      I2C_Write_nByte(0x20, 0x0300, 0, 0);	
			  #endif
			
        #ifdef Soft_i2C			
				ZT7548.beginTransmission(ZT7548_SLAVE_ADDR); 		
				ZT7548.write(0x80);        		
				ZT7548.write(0x00);        		    		
				ZT7548.endTransmission(); 
				delay_us(50);
			
				ZT7548.requestFrom(ZT7548_SLAVE_ADDR, 40);    
			
				while (ZT7548.available()) 
				{ 
						for(uint8_t i = 0; i < 40; i++)
					{
					  read_buf[i] = ZT7548.read(); 
					}
				}	
				
				ZT7548.beginTransmission(ZT7548_SLAVE_ADDR); 		
				ZT7548.write(0x03);        		
				ZT7548.write(0x00);        		    		
				ZT7548.endTransmission(); 

			  #endif		
				digitalWrite(TP_INT_Pin, LOW);
        delay(10);
				digitalWrite(TP_INT_Pin, HIGH);

}
  
int main(void)
{
    /* Initialize FL Driver Library */
    FL_Init();
    Delay_Init();
    /* Configure the system clock */
    MF_Clock_Init();
    
   
    /* Initialize all configured peripherals */
    MF_Config_Init();
	  pinMode(RESET_Pin, OUTPUT);


		pinMode(LED, OUTPUT);
    pinMode(TE_IN_Pin,OUTPUT);
			digitalWrite(TE_IN_Pin, 1);

		pinMode(TP_INT_Pin, OUTPUT);
		digitalWrite(TP_INT_Pin, HIGH);

//    pinMode(TE_IN_Pin, INPUT_PULLUP);
//    attachInterrupt(TE_IN_Pin, LED_Toogle, FALLING);
	
	  digitalWrite(RESET_Pin, HIGH);
		delay(10);
		digitalWrite(RESET_Pin, LOW);
		delay(100);
		digitalWrite(RESET_Pin, HIGH);
		delay(10);
  #ifdef Soft_i2C
	  ZT7548.begin();
	
		cmd_buf[0] = 0x01;
		cmd_buf[1] = 0x00;
		ZT7548.beginTransmission(ZT7548_SLAVE_ADDR); 		
		ZT7548.write(0x00);        		
		ZT7548.write(0xC0);        		
		ZT7548.write(cmd_buf, 2);        		
		ZT7548.endTransmission(); 
		delay(10);

		cmd_buf[0] = 0x01;
		cmd_buf[1] = 0x00;
		ZT7548.beginTransmission(ZT7548_SLAVE_ADDR); 		
		ZT7548.write(0x02);        		
		ZT7548.write(0xC0);        		
		ZT7548.write(cmd_buf, 2);        		
		ZT7548.endTransmission(); 
		delay(10);
		

		ZT7548.beginTransmission(ZT7548_SLAVE_ADDR); 		
		ZT7548.write(0x04);        		
		ZT7548.write(0xC0);        		
		ZT7548.endTransmission();
		delay(10);
		
		cmd_buf[0] = 0x01;
		cmd_buf[1] = 0x00;
		ZT7548.beginTransmission(ZT7548_SLAVE_ADDR); 		
		ZT7548.write(0x01);        		
		ZT7548.write(0xC0);        		
		ZT7548.write(cmd_buf, 2);        		
		ZT7548.endTransmission(); 
		delay(10);
		#endif
    while(1)
    {     
      if(busy == 0)
			{
				
			ZT7548_INT();
			delay(100);
			}
    }

}



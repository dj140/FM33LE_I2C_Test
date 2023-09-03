#include "I2C.h"
#include "main.h"

#define I2CREAD     1   //I2C主机读操作
#define I2CWRITE    0   //I2C主机写操作

uint8_t readdata[5];                       //接收的数据
uint8_t writedata[4] = {0x04, 0x82, 0x07, 0x00}; //要发送的数据
uint8_t  Buffer_Rx_IIC1[40];
uint8_t Response_Message[40];
uint8_t  Rx_Idx_IIC1=0;
uint8_t  Tx_Idx_IIC1=0;
uint32_t temp = 0;
uint8_t flag = 1;

uint8_t TX_NUM = 0;

void I2C_IRQHandler(void)
{
//	  uint32_t status = I2C->SSPISR;
//	  uint32_t status2 = I2C->SSPSR;

//		    if(status & I2C_SSPISR_ADM)
//    {   // I2C1 Address match event occurs
//        FL_I2C_Slave_ClearFlag_AddressMatch(I2C);
//			temp = FL_I2C_Slave_GetDataDirection(I2C); 
////        Address_Matching_Callback();
//    }
//    if(temp == FL_I2C_SSP_DATA_DIRECTION_MASTER_TO_SLAVE)
//    {
//        FL_I2C_Slave_ClearFlag_RXComplete(I2C);   
//			readdata[Rx_Idx_IIC1++] = FL_I2C_Slave_ReadRXBuff(I2C);

////        if((readdata[0]) == 0x90)                     //主机写
////					{
////			}
////        else if(readdata[0] == 0x91)             //主机读
////			{
////							        FL_I2C_Slave_ClearFlag_TXComplete(I2C);

////			        FL_I2C_Slave_WriteTXBuff(I2C, 0x34);

////			}
//    }
//    else 
//    {
//			        FL_I2C_Slave_WriteTXBuff(I2C, 0x34);

//			        FL_I2C_Slave_ClearFlag_RXComplete(I2C);

//    }
   if((FL_ENABLE == FL_I2C_Slave_IsEnabledIT_AddressMatch(I2C))
            && (FL_SET == FL_I2C_Slave_IsActiveFlag_AddressMatch(I2C)))
    {
		    Rx_Idx_IIC1=0;
        Tx_Idx_IIC1=0;
        temp = FL_I2C_Slave_ReadRXBuff(I2C);                            //读地址
        FL_I2C_Slave_ClearFlag_AddressMatch(I2C);
        FL_I2C_Slave_ClearFlag_RXComplete(I2C);
        busy = 1;
        if((temp & 0x01) == I2CWRITE)                     //主机写
        {
            while(!FL_I2C_Slave_IsActiveFlag_RXComplete(I2C));

            Buffer_Rx_IIC1[Rx_Idx_IIC1++] = FL_I2C_Slave_ReadRXBuff(I2C);
            FL_I2C_Slave_ClearFlag_RXComplete(I2C);
switch(Buffer_Rx_IIC1[0])
	{
			case 0xAE:  
					TX_NUM = 1;
					Response_Message[0] = 0x26;				
					break;
			case 0x0F:  
					TX_NUM = 1;				
					Response_Message[0] = 0x08;				
					break;
			case 0xE1:  
				  TX_NUM = 4;
					Response_Message[0] = 0x04;			
					Response_Message[1] = 0x82;				
					Response_Message[2] = 0x07;				
					Response_Message[3] = 0x00;				
					break;	
			case 0xF6: 
					TX_NUM = 10;				
					Response_Message[0] = 0x55;
					Response_Message[1] = 0x56;
					Response_Message[2] = 0x32;
					Response_Message[3] = 0x00;
					Response_Message[4] = 0x00;
					Response_Message[5] = 0x00;
					Response_Message[6] = 0x00;
					Response_Message[7] = 0x00;
					Response_Message[8] = 0x11;
					Response_Message[9] = 0x08;			
					break;	
			case 0x02:  
				  TX_NUM = 3;
					Response_Message[0] = 0x54;
					Response_Message[1] = 0x00;
					Response_Message[2] = 0x00;
					break;		
			case 0x0B:  
				  TX_NUM = 3;
					Response_Message[0] = 0x13;
					Response_Message[1] = 0x0C;
					Response_Message[2] = 0x00;		
					break;		
			case 0xE2:
				  TX_NUM = 1;				
					Response_Message[0] = 0x82;
					break;		
			case 0xE3: 
				  TX_NUM = 1;				
					Response_Message[0] = 0x07;
					break;				
			case 0x10:  // Get Point status
					for(int i = 0; i < 40; i++)
					{
						
							Response_Message[i] = 0x00;
					}
					TX_NUM = 8;
					if(read_buf[2] == 0x02)		// if detet 2 finger
					{
						flag++;
						if(flag == 0x01)
						{
							//Finger 0 event info (touch / event type / hover / palm / event id[0~3])
							Response_Message[0] = 0xA2;
							
							//Finger 0 xy coordinate (high)  y coordinate (bit 11 ~ bit 8) x coordinate (bit 11 ~ bit 8)
							Response_Message[1] = (read_buf[13] << 4) | (read_buf[11] & 0x0F);
						
							//Finger 0 x coordinate (bit 7 ~ bit 0)
							Response_Message[2] = read_buf[10] ;
						
							//Finger 0 y coordinate (bit 7 ~ bit 0)
							Response_Message[3] = read_buf[12] ;

							//Finger 0 z (strength)
							Response_Message[4] = read_buf[14] ;
						}
						else
						{
							//Finger 0 event info (touch / event type / hover / palm / event id[0~3])
							Response_Message[0] =  0xA1;									
										
							//Finger 0 xy coordinate (high)  y coordinate (bit 11 ~ bit 8) x coordinate (bit 11 ~ bit 8)
							Response_Message[1] = (read_buf[7] << 4) | (read_buf[5] & 0x0F);
						
							//Finger 0 x coordinate (bit 7 ~ bit 0)
							Response_Message[2] = read_buf[4] ;
						
							//Finger 0 y coordinate (bit 7 ~ bit 0)
							Response_Message[3] = read_buf[6] ;

							//Finger 0 z (strength)
							Response_Message[4] = read_buf[8] ;
							
							flag = 0;
							}
						}
						else
						{
							//Finger 0 event info (touch / event type / hover / palm / event id[0~3])
							Response_Message[0] = (read_buf[1]<<4 | 0x7F) & 0xA1;									
										
							//Finger 0 xy coordinate (high)  y coordinate (bit 11 ~ bit 8) x coordinate (bit 11 ~ bit 8)
							Response_Message[1] = (read_buf[7] << 4) | (read_buf[5] & 0x0F);
						
							//Finger 0 x coordinate (bit 7 ~ bit 0)
							Response_Message[2] = read_buf[4] ;
						
							//Finger 0 y coordinate (bit 7 ~ bit 0)
							Response_Message[3] = read_buf[6] ;

							//Finger 0 z (strength)
							Response_Message[4] = read_buf[8] ;
						}
						//Finger 1 event info (touch / event type / hover / palm / event id[0~3])		
						if(read_buf[2] == 0x02)		
						{							
							Response_Message[8] = 0xA2;
						}
						else
						{
							Response_Message[8] = 0x22;						
						}
						//Finger 1 xy coordinate (high)  y coordinate (bit 11 ~ bit 8) x coordinate (bit 11 ~ bit 8)
						Response_Message[9] = (read_buf[13] << 4) | (read_buf[11] & 0x0F);					

						//Finger 1 x coordinate (bit 7 ~ bit 0)
						Response_Message[10] = read_buf[10] ;
					
						//Finger 1 y coordinate (bit 7 ~ bit 0)
						Response_Message[11] = read_buf[12] ;

						//Finger 1 z (strength)
						Response_Message[12] = read_buf[14] ;
					break;
					default: break;
	}
        }

        else
            if((temp & 0x01) == I2CREAD)              //主机读
            {

							for(int i = 0; i < TX_NUM; i++)
							{
								
										 FL_I2C_Slave_WriteTXBuff(I2C, Response_Message[i]);

										while(!FL_I2C_Slave_IsActiveFlag_TXComplete(I2C));

										FL_I2C_Slave_ClearFlag_TXComplete(I2C);
							}

							
            }
    }

busy = 0;
		
		
		
}


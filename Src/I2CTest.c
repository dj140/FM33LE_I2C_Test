#include "I2CTest.h"


/*----------- IO引脚操作 -----------*/
//PD9--SCL,PD10--SDA

#define SDA_IN()                    GPIOD->FCR &= ~(0x3 << 20); \
                                    GPIOD->INEN |= (0x1 << 10);  //PD10功能选择输入，且输入使能
                
#define SDA_OUT()                   GPIOD->FCR = GPIOD->FCR & ~(0x3 << 20) | (0x1 << 20); \
                                    GPIOD->INEN &= ~(0x1 << 10);   //PD10功能选择输出，且输入关闭
//	#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))
//																	
//    MODIFY_REG(GPIOx->FCR, ((pin * pin) * GPIO_FCR), ((pin * pin) * mode));

#define SCL_OUT()                   GPIOD->FCR = GPIOD->FCR & ~(0x3 << 18) | (0x1 << 18);\
                                    GPIOD->INEN &= ~(0x1 << 9);   //PD9功能选择输出，且输入关闭

#define SCL_IN()                    GPIOD->FCR &=  ~(0x3 << 18);\
                                    GPIOD->INEN |= (0x1 << 9);  //PD9功能选择输入，且输入使能

    
#define SCL_WRITE1                  (GPIOD->DSET = (0x1 << 9))
#define SCL_WRITE0                  (GPIOD->DRST = (0x1 << 9))
                            
#define SDA_WRITE1                  (GPIOD->DSET = (0x1 << 10))
#define SDA_WRITE0                  (GPIOD->DRST = (0x1 << 10))
  
#define SDA_READ()                  ((GPIOD->DIN & (0x1 << 10)) >> 10)
#define SCL_READ()                  ((GPIOD->DIN & (0x1 << 9)) >> 9)

/*----------- 私有函数 -----------*/ 
//发送起始位
void SoftI2C_Start(void)
{
    SDA_WRITE1; 
    SDA_OUT(); 
    
    FL_DelayUs(DELAY_T);

    SCL_WRITE1;
    FL_DelayUs(DELAY_T);

    SDA_WRITE0;   //SCL=1时，SDA=1->0,为起始位
    FL_DelayUs(DELAY_T);

    SCL_WRITE0;
    FL_DelayUs(DELAY_T);  
}

//发送停止位
void SoftI2C_Stop(void)
{
    SDA_OUT();
    SDA_WRITE0;
    FL_DelayUs(DELAY_T);

    SCL_WRITE1;
    FL_DelayUs(DELAY_T);

    SDA_WRITE1;   //SCL=1时，SDA=0->1，为停止位
    FL_DelayUs(DELAY_T);

    SCL_WRITE0;
    SDA_IN();       //释放SDA  
}

//发送8位数据位并等待ACK
bool SoftI2C_Out(uint8_t data)
{
    uint8_t i;
    uint16_t cnt = 0;

    SDA_OUT();
    
    for(i=0; i<8; i++)
    {
        (data & 0x80) ? SDA_WRITE1 : SDA_WRITE0;
        FL_DelayUs(DELAY_T);
        SCL_WRITE1;
        FL_DelayUs(DELAY_T);
        
        
        SCL_WRITE0;
        FL_DelayUs(DELAY_T);


        
        data <<= 1;
    }
     
    SDA_IN();    //第九个bit，释放SDA，悬空SDA，等待从机将SDA拉低 
    FL_DelayUs(DELAY_T);
    
    
    SCL_WRITE1;
    SCL_IN();
    while(SCL_READ() ==0);     //从机如果有SCL延展，等待从机延展结束
    SCL_OUT();
    
    SCL_WRITE1;      //第九个bit，主机为从机提供时钟，高电平
   // FL_DelayUs(DELAY_T);     //调节第九个CLK宽度
    
    //等待ack
    while(SDA_READ() != 0 && cnt < 100)
    {
        cnt++;
    }
    
    SCL_WRITE0;
    
    if(cnt == 100)
    {
        return false;   // No ACK
    }
    else
    {
        return true;    // ACK
    }
}

//接收8位数据位并发送ACK
uint8_t SoftI2C_In(bool ack)
{
    uint8_t i;
    uint8_t tmp = 0;
    
    SCL_WRITE0;
    SDA_IN();
    FL_DelayUs(DELAY_T);
    
    for(i=0; i<8; i++)
    {
        SCL_WRITE1;
        FL_DelayUs(DELAY_T);

        tmp = (SDA_READ() == 0) ? (tmp << 1) : (tmp << 1 | 1);
        SCL_WRITE0;
        FL_DelayUs(DELAY_T);
    }
  
    //发送ack
    SDA_OUT();
    
    if(ack)
    {
        SDA_WRITE0;
        FL_DelayUs(DELAY_T);

        SCL_WRITE1;
        FL_DelayUs(DELAY_T);

        SCL_WRITE0;
    }
    else
    {
        SDA_WRITE1;
        FL_DelayUs(DELAY_T);

        SCL_WRITE1;
        FL_DelayUs(DELAY_T);

        SCL_WRITE0;
    }

    return tmp;
}                             

/*----------- API -----------*/
//初始化软件I2C                                    
void SoftI2C_Init(void)
{
    /* 使能GPIO时钟总线 */
    FL_RCC_EnableGroup1BusClock(FL_RCC_GROUP1_BUSCLK_PAD);
    
    //开始时SCL拉低，SDA释放
    SCL_OUT();
    SCL_WRITE0;

    FL_GPIO_EnablePinOpenDrain(GPIOD, FL_GPIO_PIN_10);   //开漏输出模式，需外部接上拉电阻
    FL_GPIO_EnablePinOpenDrain(GPIOD, FL_GPIO_PIN_9);


    SDA_IN();
}

/** 
  * @brief 软件I2C写数据
  *
  * @param  addr      要写入的数据地址
  * @param  buf       数据缓存指针
  * @param  len       要写入的长度
  */
bool SoftI2C_Write(uint16_t addr, bool addr16b, uint8_t *buf, uint32_t len)
{
    int i;
    
    //发送起始位
    SoftI2C_Start();
    
    //发送器件地址和写命令
    SoftI2C_Out((uint8_t)(MY_DEV_ADDR << 1 | 0));
    
    //发送要写的地址
    if(addr16b)
    {
        if(!SoftI2C_Out(addr>>8)) return false;
    }
    if(!SoftI2C_Out(addr)) return false;
    
    //发送数据
    for(i=0; i<len; i++)
    {
        if(!SoftI2C_Out(buf[i])) return false;
    }
    
    //发送结束位
    SoftI2C_Stop();
    
    return true;
}

/** 
  * @brief 软件I2C读数据
  *
  * @param  addr    要读取的寄存器地址
  * @param  buf     数据缓存指针
  * @param  len     要读取的长度
  */
bool SoftI2C_Read(uint16_t addr, bool addr16b, uint8_t *buf, uint32_t len)
{
    int i;
    
    //发送起始位
    SoftI2C_Start();
    
    //发送器件地址和写命令
    if(!SoftI2C_Out((uint8_t)(MY_DEV_ADDR << 1 | 0))) return false;
    
    //发送要读取的地址
    if(addr16b)
    {
        if(!SoftI2C_Out(addr>>8)) return false;
    }
    if(!SoftI2C_Out(addr)) return false;
    
    //重新发送起始位
    SoftI2C_Start();
    
    //发送器件地址和读命令
    if(!SoftI2C_Out((uint8_t)(MY_DEV_ADDR << 1 | 1))) return false;
    
    //读取数据
    for(i=0; i<len; i++)
    {
        buf[i] = SoftI2C_In(i < len-1 ? true : false);
    }
    
    //发送结束位
    SoftI2C_Stop();
    
    return true;
}


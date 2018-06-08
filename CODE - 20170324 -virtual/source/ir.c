
#include "includes.h"

const uint8_t ACCtrIRCode[16][8] = {
  {0x0a,0x50,0x20,0x20,0xf5,0xaf,0xdf,0xdf},//关机
  {0x0a,0x50,0xa0,0x20,0xf5,0xaf,0x5f,0xdf},//开机 制冷 16
  {0x0a,0x50,0xa1,0x20,0xf5,0xaf,0x5e,0xdf},//开机 制冷 17
  {0x0a,0x50,0xa2,0x20,0xf5,0xaf,0x5d,0xdf},//开机 制冷 18
  {0x0a,0x50,0xa3,0x20,0xf5,0xaf,0x5c,0xdf},//开机 制冷 19
  {0x0a,0x50,0xa4,0x20,0xf5,0xaf,0x5b,0xdf},//开机 制冷 20
  {0x0a,0x50,0xa5,0x20,0xf5,0xaf,0x5a,0xdf},//开机 制冷 21
  {0x0a,0x50,0xa6,0x20,0xf5,0xaf,0x59,0xdf},//开机 制冷 22 
  {0x0a,0x50,0xa7,0x20,0xf5,0xaf,0x58,0xdf},//开机 制冷 23
  {0x0a,0x50,0xa8,0x20,0xf5,0xaf,0x57,0xdf},//开机 制冷 24
  {0x0a,0x50,0xa9,0x20,0xf5,0xaf,0x56,0xdf},//开机 制冷 25
  {0x0a,0x50,0xaa,0x20,0xf5,0xaf,0x55,0xdf},//开机 制冷 26
  {0x0a,0x50,0xab,0x20,0xf5,0xaf,0x54,0xdf},//开机 制冷 27
  {0x0a,0x50,0xac,0x20,0xf5,0xaf,0x53,0xdf},//开机 制冷 28
  {0x0a,0x50,0xad,0x20,0xf5,0xaf,0x52,0xdf},//开机 制冷 29
  {0x0a,0x50,0xae,0x20,0xf5,0xaf,0x51,0xdf},//开机 制冷 30
};

g_irMsg irMsg;


static void IR_Random_bit(int bit_h,int bit_l)
{
  OUTPUT_38K_PWM();
  delay_us(bit_h); 
  CLOUSE_38K_PWM();
  delay_us(bit_l); 
}

static void IR_Random_start(void)
{
  IR_Random_bit(6100,7600);
}

static void IR_Random_bit0(void)
{
  IR_Random_bit(500,1500);
}

static void IR_Random_bit1(void)
{
  IR_Random_bit(500,3500);
}

static void IR_Random_stop(void)
{
  IR_Random_bit(500,7600);
  IR_Random_bit(500,0);
}
//IR 发送红外脉冲
void IR_SendPulse(void)
{
  BLUE_LED_ON();
  for(int i=0;i<sizeof(irMsg.IRconvert16)/2;)
  {
    IR_Random_bit(irMsg.IRconvert16[i],irMsg.IRconvert16[i+1]); 
    i+=2;
  }
  BLUE_LED_OFF();
  ClearEvent(IR_SEND_PULSE_EVENT);
}

void IR_Study(void)
{
  if(irMsg.IrStatus == IR_TX_RX_NON)
  {
     GREEN_LED_ON();
     memset(irMsg.IRPulsebuf, 0, sizeof(irMsg.IRPulsebuf));
     irMsg.IrStatus = IR_RX_START; 
     SetEventTimeOut(IR_CODE_EVENT,105000);
  }
}
//读取IR flah脉冲函数
void IR_ReadFlashEvent(void)
{
  BLUE_LED_ON();
  while(flash_read(IR_ADDRESS_START,irMsg.IRreadflash32,sizeof(irMsg.IRreadflash32)/4));  
  IR_U32toU16();
  BLUE_LED_OFF();
  ClearEvent(IR_READ_FLASH_EVENT); 
}

//将接收到的脉冲数组写入 flash中
void  IR_WriteFlashPage(void)
{    
  if(irMsg.IrStatus==IR_RX_END)
  { 
    GREEN_LED_OFF();
    BLUE_LED_ON();
    //等待写完成 如果写错误 则返回1 程序停下来
    //写一页
    IR_U16toU32();//将数组进行转换
    while((flash_write(IR_ADDRESS_START,irMsg.IRconvert32,sizeof(irMsg.IRconvert32)/4)));
    irMsg.RxPulseNum = 0;
    irMsg.IrStatus = IR_TX_RX_NON;
    BLUE_LED_OFF();
    ClearEvent(IR_CODE_EVENT);     
    flash_read(IR_ADDRESS_START,buf,sizeof(buf)/4);
    SetEvent(IR_READ_FLASH_EVENT);
  }
}
//写入flash中时候类型转换 将IRPulsebuf 转换成IRconvert32  数组
 void IR_U16toU32(void )
{
  int i,j=0;
  memset(irMsg.IRconvert32, 0, sizeof(irMsg.IRconvert32));  
  for(i=0;i<sizeof(irMsg.IRPulsebuf)/2;)
  {
    irMsg.IRconvert32[j++]=((irMsg.IRPulsebuf[i]<<16)|irMsg.IRPulsebuf[i+1]);
    i+=2;
  }  
}
//将读取到的 IRreadflash32 转换成16位
 void IR_U32toU16(void)
{
  int i,j=0;
  memset(irMsg.IRconvert16, 0, sizeof(irMsg.IRconvert16));
  for(i=0;i<sizeof(irMsg.IRreadflash32)/4;i++)
  {
    irMsg.IRconvert16[j]=irMsg.IRreadflash32[i]>>16;
    irMsg.IRconvert16[j+1]=irMsg.IRreadflash32[i]&0xffff;
    j+=2;
  }  
  memset(irMsg.IRreadflash32, 0, sizeof(irMsg.IRreadflash32));
}

static uint8_t CodingTest[8]={0};
void IR_Coding(void)
{
  if(irMsg.IrStatus == IR_RX_END)
  {  
    if(irMsg.IRPulsebuf[0] > 5900 && irMsg.IRPulsebuf[0] < 6300 && irMsg.IRPulsebuf[1] > 7400 && irMsg.IRPulsebuf[1] < 7800)
    {
      int i,j = 0,n = 0;
      for(i=2;i<irMsg.RxPulseNum;i++)
      {
        if(irMsg.IRPulsebuf[i] > 400 && irMsg.IRPulsebuf[i] < 700)
        {
          i++;
          if(irMsg.IRPulsebuf[i] > 1300 && irMsg.IRPulsebuf[i] < 1700)
          {
            CodingTest[j] &= ~(0x01 << n);
          }
          else if(irMsg.IRPulsebuf[i] > 3300 && irMsg.IRPulsebuf[i] < 3700)
          {
            CodingTest[j] |= (0x01 << n);
          }
          else
          {
            break;
          }
          if(++n == 8)
          {         
            n = 0;
            if(++j == 8)break;
          }
        }
      }
    }
  }
  irMsg.RxPulseNum = 0;
  irMsg.IrStatus = IR_TX_RX_NON;
  GREEN_LED_OFF();
  ClearEvent(IR_CODE_EVENT); 
}

void IR_TEST_SEND(void)
{
   for(int i=0;i<32;i++)
   {
     IR_Random_bit1();
     IR_Random_bit0();
   }
   IR_Random_stop();
}

void IR_Send(int code)
{
  int i=0,j=0;
  
  if(code < 0 || code >= 16)return;
    
  IR_Random_start();
  for(i=0;i<8;i++)
  {
    for(j=0;j<8;j++)
    {
      if(ACCtrIRCode[code][i]&(0x01 << j))
      {
        IR_Random_bit1();
      }
      else
      {
        IR_Random_bit0();
      }
    }
  }
  IR_Random_stop();
  
}
/*
void IR_Send(unsigned char *ircode)
{
  uint8_t checksum[2];
  checksum[0] = ircode[0] + ircode[2];
  checksum[1] = ircode[1] + ircode[3];
  
  IR_Random_bit(9000,2500); //开始码
  
  for(uint8_t i=0;i<4;i++)  //First Ram,Second Ram,Third Ram,Fourth Ram.
  {
    for(uint8_t j=0;j<4;j++)
    {
      if(ircode[i] & (0x01 << j))
      {
        IR_Random_bit1();
      }
      else
      {
        IR_Random_bit0();
      }
    }
  }
   
  for(uint8_t i=0;i<48;i++)  //Fifth Ram -- Sixteenth Ram(12*4,all 0)
  {
    IR_Random_bit0();
  }
  
  for(uint8_t i=0;i<2;i++)  //Seventeen Ram,Eighteen Ram.
  {
    for(uint8_t j=0;j<4;j++)
    {
      if(checksum[i] & (0x01 << j))
      {
        IR_Random_bit1();
      }
      else
      {
        IR_Random_bit0();
      }
    }
  }
  
  IR_Random_bit0();
}
*/
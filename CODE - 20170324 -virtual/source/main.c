
#include "includes.h"

static volatile uint32_t EventTimeOut[EVENT_MAX_COUNT] = {0,0,0,0,0,0,0,0};
static volatile uint8_t SysEvent = 0;
//测试使用
unsigned long buf[128] ={0};
uint8_t STUDY_CMD[4]    = {0x01,0x00,0x00,0x00};
uint8_t SEND_CMD[4]     ={0x02,0x00,0x00,0x00};

volatile int flag=99;

void SetEventTimeOut(uint8_t event,uint32_t TimeOut)
{
  SysEvent &= ~event;
  if(TimeOut)
  {
   switch(event)
   {
     case 0x01:EventTimeOut[0] = TimeOut;break;
     case 0x02:EventTimeOut[1] = TimeOut;break;
     case 0x04:EventTimeOut[2] = TimeOut;break;
     case 0x08:EventTimeOut[3] = TimeOut;break;
     case 0x10:EventTimeOut[4] = TimeOut;break;
     case 0x20:EventTimeOut[5] = TimeOut;break;
     case 0x40:EventTimeOut[6] = TimeOut;break;
     case 0x80:EventTimeOut[7] = TimeOut;break;
     default:break;
   }
  }
}

void SetEvent(uint8_t event)
{
  for(uint8_t i=0;i<EVENT_MAX_COUNT;i++)
  {
    if(event&(0x01<<i))
    {
      EventTimeOut[i] = 0;
    }
  }
  SysEvent |= event;
}

void ClearEvent(uint8_t event)
{
  for(uint8_t i=0;i<EVENT_MAX_COUNT;i++)
  {
    if(event&(0x01<<i))
    {
      EventTimeOut[i] = 0;
      break;
    }
  }
  SysEvent &= ~event;
}

void GetEvent(void)
{
  for(uint8_t i=0;i<EVENT_MAX_COUNT;i++)
  {
    if(EventTimeOut[i])
    {
     if(--EventTimeOut[i]==0)
     {
       SysEvent |= 0x01<<i;
     }
    }
  }
}

void Var_Init(void)
{
  irMsg.IrStatus = IR_TX_RX_NON;
  irMsg.RxPulseNum = 0;
  memset(irMsg.IRPulsebuf, 0, sizeof(irMsg.IRPulsebuf));
}
void BlueChage(uint8_t i)
{
  for(int time=0;time<i;time++)
  {
    IWDG_ReloadCounter();
    BLUE_LED_ON();
    delay_ms(500);
    BLUE_LED_OFF() ;
    IWDG_ReloadCounter();
    delay_ms(500);
  } 
  flag=99;
  
}
void main(void)
{ 
  HW_Init(); 
  
  Var_Init();
  
  while(InitTCM310F());
  
  delay_ms(500);
  while(EnOcean_Poll_handler1());//写一个等待接收完发过来的Enocean信息
  BLUE_LED_OFF();
  GREEN_LED_OFF();
  //给ir回复一个信息
  EO_SendMsg(FUNCTION_NO_WPS_RSP_MSG,DestinationID1,NULL,0);
  
  for(;;)
  {
    EnOcean_Poll_handler();
    
//    if(SysEvent)
//    {
//      if(SysEvent & WPS_SEND_MSG_EVENT)
//      {
//        Send_WPS_Msg();
//      }
      if(SysEvent & KEY_EVENT)
      {
        Key_Event_Process(); //按键功时间有改动  需要调整     
      }    
      if(flag!=99)
      {
       BlueChage(flag);
      }  

//      if(SysEvent & IR_CODE_EVENT)
//      {
//        //IR_Coding(); 
//        IR_WriteFlashPage();
//      } 
//      if(SysEvent & IR_READ_FLASH_EVENT)
//      {//读取flash中脉冲
//        IR_ReadFlashEvent();
//      }
//      if(SysEvent & IR_SEND_PULSE_EVENT)
//      {
//       IR_SendPulse();
//      }
//    }    
   IWDG_ReloadCounter();
  }
}
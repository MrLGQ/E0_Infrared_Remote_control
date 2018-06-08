
#include "includes.h"

static volatile uint32_t EventTimeOut[EVENT_MAX_COUNT] = {0,0,0,0,0,0,0,0};
static volatile uint8_t SysEvent = 0;
//表示多长时间后自动执行该事件
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
//获取事件
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
//清除事件
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
//获取多长时间溢出的事件  而不是一般事件 
//针对的是SetEventTimeOut()函数
//在滴答定时器中做判断 高明  好
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
//变量初始化
void Var_Init(void)
{
  irMsg.IrStatus = IR_TX_RX_NON;
  irMsg.RxPulseNum = 0;
  memset(irMsg.IRPulsebuf, 0, sizeof(irMsg.IRPulsebuf));
}

void main(void)
{ 
  HW_Init(); 
  
  Var_Init();
  
  while(InitTCM310F());
  
  delay_ms(500);
  
  BLUE_LED_OFF();
  GREEN_LED_OFF();
  
  for(;;)
  {
    EnOcean_Poll_handler();
    
    if(SysEvent)
    {
      if(SysEvent & WPS_SEND_MSG_EVENT)
      {// 发送模式时候 广播自己的ChipID 
        Send_WPS_Msg();
      }
      if(SysEvent & KEY_EVENT)
      {// 设置模式和清除地址用
        Key_Event_Process(); //按键功时间有改动  需要调整     
      }       
      if(SysEvent & IR_READ_FLASH_EVENT)
      {//读取flash中脉冲 并发送
        IR_ReadFlashEvent();
      }
      //发送ir的response
      if(SysEvent & IR_SEND_RESPONSE)
      {//给GW 发送一个回复消息
        IR_SendResponse(g_sEOmsg.SourceID,ir_data[data_response]);
      }
      if(SysEvent & IR_STUDY_EVENT)
      {//开始学习  
        IR_Study();        
      }
      if(SysEvent & IR_STUDY_NO_DATA)
      {//发送一个 没有ir的回复信息  response=1 nodata 
        //学习模式时候，10s没有收到红外信号
        IR_SendResponse_Nodata();     
      }
    }    
    IWDG_ReloadCounter();
  }
}
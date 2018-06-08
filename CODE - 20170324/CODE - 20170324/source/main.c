
#include "includes.h"

static volatile uint32_t EventTimeOut[EVENT_MAX_COUNT] = {0,0,0,0,0,0,0,0};
static volatile uint8_t SysEvent = 0;
//��ʾ�೤ʱ����Զ�ִ�и��¼�
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
//��ȡ�¼�
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
//����¼�
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
//��ȡ�೤ʱ��������¼�  ������һ���¼� 
//��Ե���SetEventTimeOut()����
//�ڵδ�ʱ�������ж� ����  ��
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
//������ʼ��
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
      {// ����ģʽʱ�� �㲥�Լ���ChipID 
        Send_WPS_Msg();
      }
      if(SysEvent & KEY_EVENT)
      {// ����ģʽ�������ַ��
        Key_Event_Process(); //������ʱ���иĶ�  ��Ҫ����     
      }       
      if(SysEvent & IR_READ_FLASH_EVENT)
      {//��ȡflash������ ������
        IR_ReadFlashEvent();
      }
      //����ir��response
      if(SysEvent & IR_SEND_RESPONSE)
      {//��GW ����һ���ظ���Ϣ
        IR_SendResponse(g_sEOmsg.SourceID,ir_data[data_response]);
      }
      if(SysEvent & IR_STUDY_EVENT)
      {//��ʼѧϰ  
        IR_Study();        
      }
      if(SysEvent & IR_STUDY_NO_DATA)
      {//����һ�� û��ir�Ļظ���Ϣ  response=1 nodata 
        //ѧϰģʽʱ��10sû���յ������ź�
        IR_SendResponse_Nodata();     
      }
    }    
    IWDG_ReloadCounter();
  }
}
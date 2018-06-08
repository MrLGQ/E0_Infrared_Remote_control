
#include "includes.h"

const uint8_t ACCtrIRCode[16][8] = {
  {0x0a,0x50,0x20,0x20,0xf5,0xaf,0xdf,0xdf},//�ػ�
  {0x0a,0x50,0xa0,0x20,0xf5,0xaf,0x5f,0xdf},//���� ���� 16
  {0x0a,0x50,0xa1,0x20,0xf5,0xaf,0x5e,0xdf},//���� ���� 17
  {0x0a,0x50,0xa2,0x20,0xf5,0xaf,0x5d,0xdf},//���� ���� 18
  {0x0a,0x50,0xa3,0x20,0xf5,0xaf,0x5c,0xdf},//���� ���� 19
  {0x0a,0x50,0xa4,0x20,0xf5,0xaf,0x5b,0xdf},//���� ���� 20
  {0x0a,0x50,0xa5,0x20,0xf5,0xaf,0x5a,0xdf},//���� ���� 21
  {0x0a,0x50,0xa6,0x20,0xf5,0xaf,0x59,0xdf},//���� ���� 22 
  {0x0a,0x50,0xa7,0x20,0xf5,0xaf,0x58,0xdf},//���� ���� 23
  {0x0a,0x50,0xa8,0x20,0xf5,0xaf,0x57,0xdf},//���� ���� 24
  {0x0a,0x50,0xa9,0x20,0xf5,0xaf,0x56,0xdf},//���� ���� 25
  {0x0a,0x50,0xaa,0x20,0xf5,0xaf,0x55,0xdf},//���� ���� 26
  {0x0a,0x50,0xab,0x20,0xf5,0xaf,0x54,0xdf},//���� ���� 27
  {0x0a,0x50,0xac,0x20,0xf5,0xaf,0x53,0xdf},//���� ���� 28
  {0x0a,0x50,0xad,0x20,0xf5,0xaf,0x52,0xdf},//���� ���� 29
  {0x0a,0x50,0xae,0x20,0xf5,0xaf,0x51,0xdf},//���� ���� 30
};

g_irMsg irMsg;
//���ⲿflash��д����  һҳд4k 1kдһ�������ź� �ܹ�һҳд4������    
//�޸Ĺ� ���ڵĳ�����1kд�������� һҳд8������  �ܹ���д1024������
static int  Writeflash_page(uint8_t page)
{ 
 int  volatile result=1;
  uint32_t EraseAddr=(unsigned long)((page/8)*4096);//ҳ ����ַ����ҳ
  uint32_t Loction=(unsigned long)((page%8)*512) ;//λ�� (unsigned long)((page%4)*1024) 
  memset(irMsg.AllArray,0,sizeof(irMsg.AllArray));//�������
  memset(irMsg.CheckArray,0,sizeof(irMsg.CheckArray));//

  sFLASH_ReadBuffer(irMsg.AllArray,EraseAddr,sizeof(irMsg.AllArray));//�����е���ȫ��ȡ����
  delay_ms(10);
  memcpy((uint8_t *)(&irMsg.AllArray[Loction]),irMsg.IRconvert8,sizeof(irMsg.IRconvert8));//����������ȥ
  sFLASH_EraseSector(EraseAddr);//���ջ���ַ����
  delay_ms(10);
  //����дʱ��ָ��+2 ���������д���� ������һ��������2��������
  sFLASH_WriteBuffer(irMsg.AllArray,EraseAddr,4094);//����д��ȥsizeof(Allbuf)
  delay_ms(10);
  sFLASH_ReadBuffer(irMsg.CheckArray,EraseAddr,sizeof(irMsg.CheckArray));
  for(int i=Loction;i<4094;i++)
  {
    if(irMsg.CheckArray[i]!=irMsg.AllArray[i]) break;
    else {result=0;break;}
  }
   return result;

//  memset(irMsg.Text,0,sizeof(irMsg.Text));
//  int result =1;
//  unsigned long PAGE=(uint32_t)(page*4096);
//  sFLASH_EraseSector(PAGE);
//  delay_ms(15);//��ʱ�ȴ�һ��
//  sFLASH_WriteBuffer(irMsg.IRconvert8,PAGE, sizeof(irMsg.IRconvert8));
//  delay_ms(15);
//  sFLASH_ReadBuffer(irMsg.Text,PAGE,sizeof(irMsg.Text));
//  for(int i=0;i<sizeof(irMsg.IRconvert8);i++)
//  {
//    if(irMsg.Text[i]!=irMsg.IRconvert8[i])  
//    {
//      result=1;break;      
//    }
//    else{
//      result=0;break;
//    }
//  }
//  return result;  
}
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
//IR ���ͺ�������
void IR_SendPulse(void)
{
  int j=0;
  BLUE_LED_ON();
  delay_ms(500);//�۲�Ч�� ��ʱ
  for(int i=0;i<sizeof(irMsg.IRconvert16)/4;i++)
  {
    IR_Random_bit(irMsg.IRconvert16[j],irMsg.IRconvert16[j+1]); 
    j+=2;
  }
  BLUE_LED_OFF();
  //ClearEvent(IR_SEND_PULSE_EVENT);
}
//IR �����ܷ���
void IR_SendResponse(uint32_t addr, uint8_t Response)
{ 
  ir_data[data_response]=Response;
  EO_SendMsg(FUNCTION_NO_CTR_MSG,addr,ir_data,4);
  ClearEvent(IR_SEND_RESPONSE);
}
void IR_SendResponse_Nodata(void)
{
    GREEN_LED_OFF();
    ClearEvent(IR_STUDY_EVENT);//��û�յ�������� Ҫ�����¼������
    if(irMsg.IrStatus==IR_RX_START)
    {//������  û�н��յ������ź�
      ir_data[data_response]=1;
      ClearEvent(IR_STUDY_NO_DATA);
      SetEvent(IR_SEND_RESPONSE);
      irMsg.IrStatus=IR_TX_RX_NON;//ʹ����֮��Ҫ����־λ������
    }
}

//ѧϰ��ʼ  ��handle���濪ʼִ��

void IR_Study(void)
{ 
   if(irMsg.IrStatus == IR_TX_RX_NON)
    {    
       GREEN_LED_ON();
       memset(irMsg.IRPulsebuf, 0, sizeof(irMsg.IRPulsebuf));
       irMsg.IrStatus = IR_RX_START;  
       SetEventTimeOut(IR_STUDY_NO_DATA,10000);//10s���ж��Ƿ���յ�IR��Ϣ
    }  
     else if(irMsg.IrStatus==IR_RX_END)
    { 
      GREEN_LED_OFF();
      BLUE_LED_ON();      
      //if() �ж�
      //�ȴ�д��� ���д���� �򷵻�1 ����ͣ����
      //дһҳ
      IR_U16toU8();//����ת��Ϊ8λ��д��     
      while(Writeflash_page(ir_data[data_data]));
      irMsg.RxPulseNum = 0;
      irMsg.IrStatus = IR_TX_RX_NON;         
      ClearEvent(IR_STUDY_EVENT);      
      BLUE_LED_OFF();     
      ir_data[data_response]=response_ok;
      SetEvent(IR_SEND_RESPONSE);
    }  
}

//��ȡIR flah���庯��
void IR_ReadFlashEvent(void)
{
  BLUE_LED_ON();
  //while(flash_read(IR_ADDRESS_START,irMsg.IRreadflash32,sizeof(irMsg.IRreadflash32)/4)); 
  sFLASH_ReadBuffer(irMsg.IRreadflash8,(uint32_t)(ir_data[data_data]*512),sizeof(irMsg.IRreadflash8));
  //��ȡ��ȷ   ���Ҷ����������������� 
  //3������ģʽʱ��flashֵΪ0 ����û�ж���
  //if((irMsg.IRreadflash8[0]!=0)||(irMsg.IRreadflash8[1]!=0))
  if(irMsg.IRreadflash8[0]!=0xFF)
  {//�������ֵ�ƽ��ң����  0��1 �ܻᷢ��һ��������  �����ж��Ƿ񰴼��������
    IR_U8toU16();
    BLUE_LED_OFF();
    IR_SendPulse();//��������  
    ClearEvent(IR_READ_FLASH_EVENT);  
    ir_data[data_response]=response_ok;//
    SetEvent(IR_SEND_RESPONSE);
  }
  else
  {//���û�ж�ȡ��ֵ ˵������û�ж���   �̵���500ms��Ϩ��
    BLUE_LED_OFF();
    GREEN_LED_ON();
    delay_ms(500);
    GREEN_LED_OFF();
    ClearEvent(IR_READ_FLASH_EVENT);
    ir_data[data_response]=response_nodefine;
    SetEvent(IR_SEND_RESPONSE);
  }
}

//��flash�ж�ȡ����8λ����ת����16λ��
void IR_U8toU16(void)
{
  int i,j=0,k=0,z=0;
  memset(irMsg.IRconvert16,0,sizeof(irMsg.IRconvert16));
  k=sizeof(irMsg.IRPulsebuf);
  if(k%2==1) k=(sizeof(irMsg.IRreadflash8)/2+1);
  else k=(sizeof(irMsg.IRreadflash8)/2);
  for(i=0;i<k;i++)
  {//i��k����ѭ��  z����ת�� 
    irMsg.IRconvert16[j++]=((irMsg.IRreadflash8[z]<<8)|irMsg.IRreadflash8[z+1]);
      //(unsigned short)(irMsg.IRreadflash8[i]&0xff)<<8+(irMsg.IRreadflash8[i+1]&0xff);  
    z+=2;
  }
  //*****�������ε�
  //memset(irMsg.IRreadflash8,0,sizeof(irMsg.IRreadflash8));
}
//��16λ������ת����8byte����  ����д�뵽flash
void IR_U16toU8(void)
{
  int i,j=0,k=0;
  memset(irMsg.IRconvert8,0,sizeof(irMsg.IRconvert8));
  k=sizeof(irMsg.IRPulsebuf);
  if(k%2==1) k=(sizeof(irMsg.IRPulsebuf)/2+1);
  else k=(sizeof(irMsg.IRPulsebuf)/2);
  for(i=0;i<k;i++)
  {//ע������ط�д��ʱ����ֱ�ӡ� Ҫ����ת�� ��Ȼ���ݲ���ȷ
//    irMsg.IRconvert8[j]=(unsigned char)((irMsg.IRPulsebuf[i]&0xff00)>>8);
//    irMsg.IRconvert8[j+1]=(unsigned char)(irMsg.IRPulsebuf[i]&0xff);
//    j+=2;    
    irMsg.IRconvert8[j++]=irMsg.IRPulsebuf[i]>>8;
    irMsg.IRconvert8[j++]=irMsg.IRPulsebuf[i]&0xff;
  } 
}
//static uint8_t CodingTest[8]={0};
//void IR_Coding(void)
//{
//  if(irMsg.IrStatus == IR_RX_END)
//  {  
//    if(irMsg.IRPulsebuf[0] > 5900 && irMsg.IRPulsebuf[0] < 6300 && irMsg.IRPulsebuf[1] > 7400 && irMsg.IRPulsebuf[1] < 7800)
//    {
//      int i,j = 0,n = 0;
//      for(i=2;i<irMsg.RxPulseNum;i++)
//      {
//        if(irMsg.IRPulsebuf[i] > 400 && irMsg.IRPulsebuf[i] < 700)
//        {
//          i++;
//          if(irMsg.IRPulsebuf[i] > 1300 && irMsg.IRPulsebuf[i] < 1700)
//          {
//            CodingTest[j] &= ~(0x01 << n);
//          }
//          else if(irMsg.IRPulsebuf[i] > 3300 && irMsg.IRPulsebuf[i] < 3700)
//          {
//            CodingTest[j] |= (0x01 << n);
//          }
//          else
//          {
//            break;
//          }
//          if(++n == 8)
//          {         
//            n = 0;
//            if(++j == 8)break;
//          }
//        }
//      }
//    }
//  }
//  irMsg.RxPulseNum = 0;
//  irMsg.IrStatus = IR_TX_RX_NON;
//  GREEN_LED_OFF();
//  ClearEvent(IR_CODE_EVENT); 
//}

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
  
  IR_Random_bit(9000,2500); //��ʼ��
  
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

#include "includes.h"

/*******************************************************************************
flash��д������
���룺	
		u32 StartAddr	flash��ʼ��ַ
		u32 *p_data	��д������ָ��
		u32 size	д�����ݵ�����
�����	
		0����ȷִ��		
		��0������
ע�⣺��������һ����u32 ��ָ�룬������һ���ǰ���4�ֽڶ���д��ġ�
���ԣ�sizeҲ��u32�ĸ������ֽ�����4��֮һ��
*******************************************************************************/
int flash_write(unsigned long StartAddr,unsigned long *p_data,unsigned long size)
{	
  volatile FLASH_Status FLASHStatus;	
  unsigned long EndAddr=StartAddr+size*4;	
  vu32 NbrOfPage = 0;	
  unsigned long EraseCounter = 0x0, Address = 0x0;
  int i;
  int MemoryProgramStatus=1;
  //Ϊһ��ͨ��
  FLASH_Unlock();          //��������
  NbrOfPage=((EndAddr-StartAddr)>>10)+1;	//�ж��ٸ�ҳ������	
  FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); //����������б�־
  //��ҳ
  FLASHStatus=FLASH_COMPLETE;
  for(EraseCounter=0;(EraseCounter<NbrOfPage)&&(FLASHStatus==FLASH_COMPLETE);EraseCounter++)
  {		
    //FLASHStatus=FLASH_ErasePage(StartAddr+(PageSize*EraseCounter));
    FLASHStatus=FLASH_ErasePage(StartAddr+(1024*EraseCounter));
  }
  //��ʼд����
  Address = StartAddr;
  i=0;
  while((Address<EndAddr)&&(FLASHStatus==FLASH_COMPLETE))
  {	
    FLASHStatus=FLASH_ProgramWord(Address,p_data[i++]);
    Address=Address+4;
  }
  //���������Ƿ����
  Address = StartAddr;
  i=0;
  while((Address < EndAddr) && (MemoryProgramStatus != 0))
  {	
    if((*(vu32*) Address) != p_data[i++])
    {		
      MemoryProgramStatus = 0;
      return 1;
    }
    Address += 4;
  }
  return 0;
}

//��flash��д��һ��uint16 λ������
int flash_write_u16(uint32_t StartAddr,unsigned short *p_data, unsigned long size)
{  
  volatile FLASH_Status  FLASHStatus;
  FLASHStatus=FLASH_COMPLETE; 
  unsigned long EndAddr=StartAddr+size*2;//�ֽ���*2��ʾ��ַ  sizeof������Ƕ��ٸ��ֽ� 
  unsigned long Address = 0x0;
  int MemoryProgramStatus=1;
  int i;  
  FLASH_Unlock();          //��������
  FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); //����������б�־
  while(FLASH_ErasePage(StartAddr)!=FLASH_COMPLETE); //��ҳ
  
  Address=StartAddr;//�ô��ǲ���ı�StartAddr��ֵ �����Ժ�ʹ��
  i=0;
  while((Address<EndAddr)&&(FLASHStatus==FLASH_COMPLETE))//д����
  {
    FLASH_ProgramHalfWord(Address,p_data[i++]);
    Address+=2;//2���ֽ� 
  }
  //����������ȷ��
  Address=StartAddr;
  i=0;
  while((Address < EndAddr) && (MemoryProgramStatus != 0))
  {	
    if((*(vu16*) Address) != p_data[i++])
    {		
      MemoryProgramStatus = 0;
      return 1;
    }
    Address += 2;
  }
  return 0;
}


int flash_read(unsigned long StartAddr,unsigned long *p_data,unsigned long size)
{
  unsigned long EndAddr=StartAddr+size*4;
  int MemoryProgramStatus=1;
  unsigned long Address = 0x0;
  int i=0;
  Address = StartAddr;
  while((Address < EndAddr) && (MemoryProgramStatus != 0))
  {
    p_data[i++]=(*(vu32*) Address);
    Address += 4;	
  }
  return 0;	
}
//��ȡflash�е����� �浽IRReadFlashbuf��
int flash_read_u16(unsigned long StartAddr,unsigned short *p_data,unsigned long size)
{
  unsigned short EndAddr=StartAddr+size*2;
  int MemoryProgramStatus=1;
  unsigned long Address = 0x0;
  int i=0;
  Address = StartAddr;
  while((Address < EndAddr) && (MemoryProgramStatus != 0))
  {
    p_data[i++]=(*(vu16*) Address);
    Address += 2;	
  }
  return 0;	
}
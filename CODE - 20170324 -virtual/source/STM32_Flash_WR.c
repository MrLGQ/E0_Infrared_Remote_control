
#include "includes.h"

/*******************************************************************************
flash的写函数：
输入：	
		u32 StartAddr	flash起始地址
		u32 *p_data	待写入数据指针
		u32 size	写入数据的数量
输出：	
		0：正确执行		
		非0：出错
注意：输入数据一定是u32 的指针，即数据一定是按照4字节对齐写入的。
所以：size也是u32的个数（字节数的4分之一）
*******************************************************************************/
int flash_write(unsigned long StartAddr,unsigned long *p_data,unsigned long size)
{	
  volatile FLASH_Status FLASHStatus;	
  unsigned long EndAddr=StartAddr+size*4;	
  vu32 NbrOfPage = 0;	
  unsigned long EraseCounter = 0x0, Address = 0x0;
  int i;
  int MemoryProgramStatus=1;
  //为一是通过
  FLASH_Unlock();          //解锁函数
  NbrOfPage=((EndAddr-StartAddr)>>10)+1;	//有多少个页被擦除	
  FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); //清除所有已有标志
  //擦页
  FLASHStatus=FLASH_COMPLETE;
  for(EraseCounter=0;(EraseCounter<NbrOfPage)&&(FLASHStatus==FLASH_COMPLETE);EraseCounter++)
  {		
    //FLASHStatus=FLASH_ErasePage(StartAddr+(PageSize*EraseCounter));
    FLASHStatus=FLASH_ErasePage(StartAddr+(1024*EraseCounter));
  }
  //开始写数据
  Address = StartAddr;
  i=0;
  while((Address<EndAddr)&&(FLASHStatus==FLASH_COMPLETE))
  {	
    FLASHStatus=FLASH_ProgramWord(Address,p_data[i++]);
    Address=Address+4;
  }
  //检验数据是否出错
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

//向flash中写入一个uint16 位的数据
int flash_write_u16(uint32_t StartAddr,unsigned short *p_data, unsigned long size)
{  
  volatile FLASH_Status  FLASHStatus;
  FLASHStatus=FLASH_COMPLETE; 
  unsigned long EndAddr=StartAddr+size*2;//字节数*2表示地址  sizeof算出的是多少个字节 
  unsigned long Address = 0x0;
  int MemoryProgramStatus=1;
  int i;  
  FLASH_Unlock();          //解锁函数
  FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); //清除所有已有标志
  while(FLASH_ErasePage(StartAddr)!=FLASH_COMPLETE); //擦页
  
  Address=StartAddr;//好处是不会改变StartAddr的值 方便以后使用
  i=0;
  while((Address<EndAddr)&&(FLASHStatus==FLASH_COMPLETE))//写数据
  {
    FLASH_ProgramHalfWord(Address,p_data[i++]);
    Address+=2;//2个字节 
  }
  //检验数据正确性
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
//读取flash中的数据 存到IRReadFlashbuf中
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
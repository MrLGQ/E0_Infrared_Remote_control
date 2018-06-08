#ifndef STM32_FLASH_WR_H 
#define STM32_FLASH_WR_H 

#ifdef __cplusplus
extern "C" {
#endif

#define    CTR_DEVICEID_START     0x0800fc00          //page 63
#define    IR_ADDRESS_START       0x08000000       //pege 0
int flash_write(unsigned long StartAddr,unsigned long *p_data,unsigned long size);
int flash_read(unsigned long StartAddr,unsigned long *p_data,unsigned long size);

//红外脉冲存在flash中的读取
int flash_write_u16(uint32_t StartAddr,unsigned short *p_data, unsigned long size);
int flash_read_u16(unsigned long StartAddr,unsigned short *p_data,unsigned long size);
  
#ifdef __cplusplus
    }
#endif

#endif
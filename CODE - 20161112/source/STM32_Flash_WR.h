#ifndef STM32_FLASH_WR_H 
#define STM32_FLASH_WR_H 

#ifdef __cplusplus
extern "C" {
#endif

#define    CTR_DEVICEID_START              0x0800fc00   //page 63
  
int flash_write(unsigned long StartAddr,unsigned long *p_data,unsigned long size);
int flash_read(unsigned long StartAddr,unsigned long *p_data,unsigned long size);


  
#ifdef __cplusplus
    }
#endif

#endif
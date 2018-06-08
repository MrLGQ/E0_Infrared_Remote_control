
#ifndef ENOCEAN_H 
#define ENOCEAN_H 

#ifdef __cplusplus
extern "C" {
#endif
  
#define  ENOCEAN_RX_MAX          128
  
#define FILTER_KIND_APPLY        0x80
#define FILTER_KIND_BLOCKS       0x00
  
#define FUNCTION_NO_WPS_MSG      0x0001     
#define FUNCTION_NO_WPS_RSP_MSG  0x0002  
#define FUNCTION_NO_CTR_MSG      0x0003 
  
  
typedef struct{
  uint8_t EnOcean_RX_buf[ENOCEAN_RX_MAX];
  uint8_t EnOcean_RX_len;
  uint8_t EnOceanTelegramRxflags;
  uint32_t TCM310FBaseID;
  uint32_t TCM310FChipID;
}g_sEOmsg_t;  


typedef enum{
 Mode_Normal,
 Mode_WPS,
 Mode_Clear
}Work_Mode_t;


extern g_sEOmsg_t g_sEOmsg;

uint8_t InitTCM310F(void); 
Work_Mode_t GetWorkMode(void);
void SetWorkMode(Work_Mode_t Work_Mode);
void Send_WPS_Msg(void);
uint8_t EnOcean_Poll_handler(void);
  
  
#ifdef __cplusplus
    }
#endif

#endif
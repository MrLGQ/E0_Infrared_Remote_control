
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
  uint32_t SourceID;
}g_sEOmsg_t;  
//ir 交互模块
extern  uint8_t ir_data[4];
#define data_cmd                0
#define data_data               1
#define data_response           2
#define data_null               3

#define cmd_study               1
#define cmd_send                2

#define response_ok             0
#define response_nodata         1
#define response_cmderror       2
#define response_nodefine       3

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

extern uint8_t EO_SendMsg(uint16_t FunctionNo,uint32_t DestinationID,uint8_t *data,uint8_t len);
  
//读取flash中的数据到IRREADFLASHbuf中
 int  ReadFlashPulse(uint32_t  startaddr );  
#ifdef __cplusplus
    }
#endif

#endif
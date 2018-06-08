
#ifndef MAIN_H 
#define MAIN_H 

#ifdef __cplusplus
extern "C" {
#endif

#define  EVENT_MAX_COUNT             8  
  
#define  WPS_SEND_MSG_EVENT          0x01
#define  KEY_EVENT                   0x02  
#define  IR_CODE_EVENT               0x04

void GetEvent(void);  
void SetEventTimeOut(uint8_t event,uint32_t TimeOut);
void SetEvent(uint8_t event);
void ClearEvent(uint8_t event);  
  
  
#ifdef __cplusplus
    }
#endif

#endif
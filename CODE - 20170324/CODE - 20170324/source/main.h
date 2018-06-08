
#ifndef MAIN_H 
#define MAIN_H 

#ifdef __cplusplus
extern "C" {
#endif

#define  EVENT_MAX_COUNT             8  
  
#define  WPS_SEND_MSG_EVENT          0x01
#define  KEY_EVENT                   0x02  
//#define   
#define  IR_READ_FLASH_EVENT         0x08
#define  IR_SEND_PULSE_EVENT         0x10//10000
#define  IR_SEND_RESPONSE            0x20
#define  IR_STUDY_EVENT              0x40
#define  IR_STUDY_NO_DATA            0x80

void GetEvent(void);  
void SetEventTimeOut(uint8_t event,uint32_t TimeOut);
void SetEvent(uint8_t event);
void ClearEvent(uint8_t event);  

  
  
#ifdef __cplusplus
    }
#endif

#endif

#ifndef IR_H 
#define IR_H 

#ifdef __cplusplus
extern "C" {
#endif

#define   IR_TX_RX_NON                0
#define   IR_TX_START                 1
#define   IR_TX_END                   2
#define   IR_RX_START                 3
#define   IR_RX_END                   4 
  
typedef struct{
    unsigned char IrStatus;
    long PulseTimeStart;
    long PulseTimeEnd;
    unsigned short RxPulseNum;
    unsigned short IRPulsebuf[256];
    

    unsigned short IRconvert16[256];//转换成16位数组       
    unsigned char  IRconvert8[512];//转换成8byte 数组
    unsigned char  IRreadflash8[512];//读取到flash中的8位数组
    unsigned char  Text[512];
    unsigned char  AllArray[4096];
    unsigned char  CheckArray[4096];
    //unsigned short 
}g_irMsg;

extern g_irMsg irMsg;
    
void IR_Study(void);
void IR_Coding(void);
//void IR_Send(unsigned char *ircode);
void IR_Send(int code);
void IR_TEST_SEND(void);


void IR_ReadFlashEvent(void);
//IR 发送红外脉冲
void IR_SendPulse(void);
void IR_SendResponse(uint32_t addr,uint8_t Response);
void IR_SendResponse_Nodata(void);

void IR_U16toU8(void);
void IR_U8toU16(void);
  
#ifdef __cplusplus
    }
#endif

#endif
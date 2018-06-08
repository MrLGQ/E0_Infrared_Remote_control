
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
    
    unsigned long  IRconvert32[128];//转换成32位数组
    unsigned long  IRreadflash32[128];//读取到的flash中32位数组
    unsigned short IRconvert16[256];//转换成16位数组
    //unsigned short 
}g_irMsg;

extern g_irMsg irMsg;
    
void IR_Study(void);
void IR_Coding(void);
//void IR_Send(unsigned char *ircode);
void IR_Send(int code);
void IR_TEST_SEND(void);

void  IR_WriteFlashPage(void);
void IR_ReadFlashEvent(void);
//IR 发送红外脉冲
void IR_SendPulse(void);

void IR_U16toU32(void );
void IR_U32toU16(void);
  
#ifdef __cplusplus
    }
#endif

#endif
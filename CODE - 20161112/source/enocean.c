
#include "includes.h"
#include "ESP3_COM.h"
#include "enocean.h"


const uint8_t CRC8Table[256]   =   {       
  0x00,0x07,0x0E,0x09,0x1C,0x1B,0x12,0x15,0x38,0x3F,0x36,0x31,0x24,0x23,0x2A,0x2D,     
  0x70,0x77,0x7E,0x79,0x6C,0x6B,0x62,0x65,0x48,0x4F,0x46,0x41,0x54,0x53,0x5A,0x5D,     
  0xE0,0xE7,0xEE,0xE9,0xFC,0xFB,0xF2,0xF5,0xD8,0xDF,0xD6,0xD1,0xC4,0xC3,0xCA,0xCD,     
  0x90,0x97,0x9E,0x99,0x8C,0x8B,0x82,0x85,0xA8,0xAF,0xA6,0xA1,0xB4,0xB3,0xBA,0xBD,     
  0xC7,0xC0,0xC9,0xCE,0xDB,0xDC,0xD5,0xD2,0xFF,0xF8,0xF1,0xF6,0xE3,0xE4,0xED,0xEA,     
  0xB7,0xB0,0xB9,0xBE,0xAB,0xAC,0xA5,0xA2,0x8F,0x88,0x81,0x86,0x93,0x94,0x9D,0x9A,     
  0x27,0x20,0x29,0x2E,0x3B,0x3C,0x35,0x32,0x1F,0x18,0x11,0x16,0x03,0x04,0x0D,0x0A,     
  0x57,0x50,0x59,0x5E,0x4B,0x4C,0x45,0x42,0x6F,0x68,0x61,0x66,0x73,0x74,0x7D,0x7A,     
  0x89,0x8E,0x87,0x80,0x95,0x92,0x9B,0x9C,0xB1,0xB6,0xBF,0xB8,0xAD,0xAA,0xA3,0xA4,     
  0xF9,0xFE,0xF7,0xF0,0xE5,0xE2,0xEB,0xEC,0xC1,0xC6,0xCF,0xC8,0xDD,0xDA,0xD3,0xD4,     
  0x69,0x6E,0x67,0x60,0x75,0x72,0x7B,0x7C,0x51,0x56,0x5F,0x58,0x4D,0x4A,0x43,0x44,     
  0x19,0x1E,0x17,0x10,0x05,0x02,0x0B,0x0C,0x21,0x26,0x2F,0x28,0x3D,0x3A,0x33,0x34,     
  0x4E,0x49,0x40,0x47,0x52,0x55,0x5C,0x5B,0x76,0x71,0x78,0x7F,0x6A,0x6D,0x64,0x63,     
  0x3E,0x39,0x30,0x37,0x22,0x25,0x2C,0x2B,0x06,0x01,0x08,0x0F,0x1A,0x1D,0x14,0x13,     
  0xAE,0xA9,0xA0,0xA7,0xB2,0xB5,0xBC,0xBB,0x96,0x91,0x98,0x9F,0x8A,0x8D,0x84,0x83,     
  0xDE,0xD9,0xD0,0xD7,0xC2,0xC5,0xCC,0xCB,0xE6,0xE1,0xE8,0xEF,0xFA,0xFD,0xF4,0xF3     
};

g_sEOmsg_t g_sEOmsg;

static Work_Mode_t g_Work_Mode;

#define CTR_DEVICE_MAX                  30
#define CTR_ID_INVALID                  0xffffffff
#define FLASH_START                     0xaaaa5555
#define FLASH_END                       0x5555aaaa
static uint32_t Device_GW_ID[CTR_DEVICE_MAX];

//CRC-8               x8+x2+x+1
// ²é±í·¨  
static uint8_t GetCheckSum(uint8_t *crcData, uint32_t crcDataLen)  
{    
    uint8_t crc8 = 0;  
    for(; crcDataLen > 0; crcDataLen--){  
        crc8 = CRC8Table[crc8^*crcData];  
        crcData++;  
    }  
  
    return(crc8);  
}

void USART1_IRQHandler(void)
{
  static uint8_t PacketHeader = 0;
  static uint8_t PacketLen = 0;
  if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
  {	      
    char Rxdata = USART_ReceiveData(USART1);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) != SET);
 
    if(g_sEOmsg.EnOcean_RX_len < ENOCEAN_RX_MAX && (!g_sEOmsg.EnOceanTelegramRxflags)){
      g_sEOmsg.EnOcean_RX_buf[g_sEOmsg.EnOcean_RX_len++] = Rxdata;
      if(!PacketHeader){
        if(g_sEOmsg.EnOcean_RX_buf[SYNC_BYTE_SEQ] != SYNC_BYTE)g_sEOmsg.EnOcean_RX_len = 0;
        else{
          if(g_sEOmsg.EnOcean_RX_len == PACKET_HEADER_LEN+2){
            if(GetCheckSum(&g_sEOmsg.EnOcean_RX_buf[DATA_LENGTH_H_SEQ],PACKET_HEADER_LEN) == g_sEOmsg.EnOcean_RX_buf[HEADER_CRC8_SEQ]){
              PacketLen = BASE_PACKET_LEN + (g_sEOmsg.EnOcean_RX_buf[DATA_LENGTH_H_SEQ]<<8) 
                          + g_sEOmsg.EnOcean_RX_buf[DATA_LENGTH_L_SEQ] + g_sEOmsg.EnOcean_RX_buf[OPTION_LENGTH_SEQ];
              PacketHeader = 1;              
            }else g_sEOmsg.EnOcean_RX_len = 0;
          }
        }
      }else if(g_sEOmsg.EnOcean_RX_len == PacketLen){
        g_sEOmsg.EnOceanTelegramRxflags = 1;
        PacketHeader = 0;
        PacketLen = 0;
        g_sEOmsg.EnOcean_RX_len = 0; 
      }
    }else {   
      g_sEOmsg.EnOcean_RX_len = 0;
      PacketHeader = 0;
      PacketLen = 0;
    }
  }
}

static void EnOceanSendPacket(uint8_t *inBuff, uint16_t usLength)
{
  while(g_sEOmsg.EnOcean_RX_len);
  g_sEOmsg.EnOceanTelegramRxflags = 0;
  UartSendPacket(USART1,inBuff,usLength);
}

static int UpdateCtrDevice(uint32_t CtrDeviceID[])
{
  unsigned long buf[CTR_DEVICE_MAX + 2];
  
  buf[0] = FLASH_START;
  buf[CTR_DEVICE_MAX + 2 -1] = FLASH_END;
  for(uint8_t i=0;i<CTR_DEVICE_MAX;i++){
    buf[i+1] = CtrDeviceID[i];
  }
  return flash_write(CTR_DEVICEID_START,buf,sizeof(buf)/4);
}

static int ReadCtrDevice(uint32_t CtrDeviceID[])
{
  unsigned long buf[CTR_DEVICE_MAX + 2];
  
  flash_read(CTR_DEVICEID_START,buf,sizeof(buf)/4);
  if(buf[0] == FLASH_START && buf[CTR_DEVICE_MAX + 2 -1] == FLASH_END){
    for(uint8_t i=0;i<CTR_DEVICE_MAX;i++){
      CtrDeviceID[i] = buf[i+1];
    }
    return 0;
  }
  return 1;
}

static int PollCtrDevice(uint32_t deviceID)
{
  for(uint8_t i=0;i<CTR_DEVICE_MAX;i++)
  {
    if(deviceID == Device_GW_ID[i])return 0;
  }
  return 1;
}

static void AddDevice(uint32_t deviceID)
{
  for(uint8_t i=0;i<CTR_DEVICE_MAX;i++)
  {
    if(0xffffffff == Device_GW_ID[i])
    {
      Device_GW_ID[i] = deviceID;
      UpdateCtrDevice(Device_GW_ID);   
      break;
    }
  }
}

static uint8_t TCM310FReset(void)
{
  uint8_t result = 1;
  uint8_t buf[8];
  buf[SYNC_BYTE_SEQ] = SYNC_BYTE;
  buf[DATA_LENGTH_H_SEQ] = 0;
  buf[DATA_LENGTH_L_SEQ] = 1;
  buf[OPTION_LENGTH_SEQ] = 0;
  buf[PACKET_TYPE_SEQ] = PACKET_TYPES_COMMON_COMMAND;
  buf[HEADER_CRC8_SEQ] = GetCheckSum(&buf[DATA_LENGTH_H_SEQ],PACKET_HEADER_LEN);
  buf[FIRST_DATA_SEQ] = CO_WR_RESET;
  buf[FIRST_DATA_SEQ+1] = GetCheckSum(&buf[FIRST_DATA_SEQ],CONVERT_TO_INT(buf[DATA_LENGTH_L_SEQ],buf[DATA_LENGTH_H_SEQ]));
  EnOceanSendPacket(buf,BASE_PACKET_LEN + CONVERT_TO_INT(buf[DATA_LENGTH_L_SEQ],buf[DATA_LENGTH_H_SEQ]));
   
  while(!g_sEOmsg.EnOceanTelegramRxflags);
  if(g_sEOmsg.EnOcean_RX_buf[FIRST_DATA_SEQ] == RET_OK)result = 0;
  g_sEOmsg.EnOceanTelegramRxflags = 0;
  
  return result;
}

/*
static uint8_t GetBaseID(void)
{
  uint8_t result = 1;
  uint8_t buf[8];
  buf[SYNC_BYTE_SEQ] = SYNC_BYTE;
  buf[DATA_LENGTH_H_SEQ] = 0;
  buf[DATA_LENGTH_L_SEQ] = 1;
  buf[OPTION_LENGTH_SEQ] = 0;
  buf[PACKET_TYPE_SEQ] = PACKET_TYPES_COMMON_COMMAND;
  buf[HEADER_CRC8_SEQ] = GetCheckSum(&buf[DATA_LENGTH_H_SEQ],PACKET_HEADER_LEN);
  buf[FIRST_DATA_SEQ] = CO_RD_IDBASE;
  buf[FIRST_DATA_SEQ+1] = GetCheckSum(&buf[FIRST_DATA_SEQ],CONVERT_TO_INT(buf[DATA_LENGTH_L_SEQ],buf[DATA_LENGTH_H_SEQ]));
  
  EnOceanSendPacket(buf,BASE_PACKET_LEN + CONVERT_TO_INT(buf[DATA_LENGTH_L_SEQ],buf[DATA_LENGTH_H_SEQ]));

  while(!g_sEOmsg.EnOceanTelegramRxflags);  

  if(g_sEOmsg.EnOcean_RX_buf[FIRST_DATA_SEQ] == RET_OK){
     g_sEOmsg.TCM310FBaseID = (g_sEOmsg.EnOcean_RX_buf[FIRST_DATA_SEQ+1]<<24)+
                              (g_sEOmsg.EnOcean_RX_buf[FIRST_DATA_SEQ+1+1]<<16)+
                              (g_sEOmsg.EnOcean_RX_buf[FIRST_DATA_SEQ+1+2]<<8)+
                              g_sEOmsg.EnOcean_RX_buf[FIRST_DATA_SEQ+1+3];  
    result = 0;
  }
  g_sEOmsg.EnOceanTelegramRxflags = 0;
  return result;
}
*/

static uint8_t GetChipID(void)
{
  uint8_t result = 1;
  uint8_t buf[8];
  buf[SYNC_BYTE_SEQ] = SYNC_BYTE;
  buf[DATA_LENGTH_H_SEQ] = 0;
  buf[DATA_LENGTH_L_SEQ] = 1;
  buf[OPTION_LENGTH_SEQ] = 0;
  buf[PACKET_TYPE_SEQ] = PACKET_TYPES_COMMON_COMMAND;
  buf[HEADER_CRC8_SEQ] = GetCheckSum(&buf[DATA_LENGTH_H_SEQ],PACKET_HEADER_LEN);
  buf[FIRST_DATA_SEQ] = CO_RD_VERSION;
  buf[FIRST_DATA_SEQ+1] = GetCheckSum(&buf[FIRST_DATA_SEQ],CONVERT_TO_INT(buf[DATA_LENGTH_L_SEQ],buf[DATA_LENGTH_H_SEQ]));
  
  EnOceanSendPacket(buf,BASE_PACKET_LEN + CONVERT_TO_INT(buf[DATA_LENGTH_L_SEQ],buf[DATA_LENGTH_H_SEQ]));

  while(!g_sEOmsg.EnOceanTelegramRxflags);  

  if(g_sEOmsg.EnOcean_RX_buf[FIRST_DATA_SEQ] == RET_OK){
     g_sEOmsg.TCM310FChipID = (g_sEOmsg.EnOcean_RX_buf[15]<<24)+
                              (g_sEOmsg.EnOcean_RX_buf[16]<<16)+
                              (g_sEOmsg.EnOcean_RX_buf[17]<<8)+
                              g_sEOmsg.EnOcean_RX_buf[18];  
    result = 0;
  }
  g_sEOmsg.EnOceanTelegramRxflags = 0;
  return result;
}

/*
static uint8_t FilterEnable(uint8_t enable)
{
  uint8_t result = 1;
  uint8_t buf[10];
  buf[SYNC_BYTE_SEQ] = SYNC_BYTE;
  buf[DATA_LENGTH_H_SEQ] = 0;
  buf[DATA_LENGTH_L_SEQ] = 3;
  buf[OPTION_LENGTH_SEQ] = 0;
  buf[PACKET_TYPE_SEQ] = PACKET_TYPES_COMMON_COMMAND;
  buf[HEADER_CRC8_SEQ] = GetCheckSum(&buf[DATA_LENGTH_H_SEQ],PACKET_HEADER_LEN);
  buf[FIRST_DATA_SEQ] = CO_WR_FILTER_ENABLE;
  buf[FIRST_DATA_SEQ+1] = enable;  //enable FILTER
  buf[FIRST_DATA_SEQ+2] = 0;
  buf[FIRST_DATA_SEQ+3] = GetCheckSum(&buf[FIRST_DATA_SEQ],CONVERT_TO_INT(buf[DATA_LENGTH_L_SEQ],buf[DATA_LENGTH_H_SEQ])); 
  
  EnOceanSendPacket(buf,BASE_PACKET_LEN + CONVERT_TO_INT(buf[DATA_LENGTH_L_SEQ],buf[DATA_LENGTH_H_SEQ]));
 
  while(!g_sEOmsg.EnOceanTelegramRxflags);  

  if(g_sEOmsg.EnOcean_RX_buf[FIRST_DATA_SEQ] == RET_OK)result = 0;
  g_sEOmsg.EnOceanTelegramRxflags = 0;
  
  return result;
}

static uint8_t FilterAddDeviceID(uint32_t deviceID,uint8_t kind)
{
  uint8_t result = 1;
  uint8_t buf[14]; 
  
  buf[SYNC_BYTE_SEQ] = SYNC_BYTE;
  buf[DATA_LENGTH_H_SEQ] = 0;
  buf[DATA_LENGTH_L_SEQ] = 7;
  buf[OPTION_LENGTH_SEQ] = 0;
  buf[PACKET_TYPE_SEQ] = PACKET_TYPES_COMMON_COMMAND;
  buf[HEADER_CRC8_SEQ] = GetCheckSum(&buf[DATA_LENGTH_H_SEQ],PACKET_HEADER_LEN);
  buf[FIRST_DATA_SEQ] = CO_WR_FILTER_ADD;
  buf[FIRST_DATA_SEQ+1] = 0;  //Filter type
  buf[FIRST_DATA_SEQ+2] = deviceID>>24;  //Filter value device ID
  buf[FIRST_DATA_SEQ+3] = (deviceID&0xff0000)>>16;
  buf[FIRST_DATA_SEQ+4] = (deviceID&0xff00)>>8;
  buf[FIRST_DATA_SEQ+5] = deviceID&0xff;
  buf[FIRST_DATA_SEQ+6] = kind; //Filter kind 
  buf[FIRST_DATA_SEQ+7] = GetCheckSum(&buf[FIRST_DATA_SEQ],CONVERT_TO_INT(buf[DATA_LENGTH_L_SEQ],buf[DATA_LENGTH_H_SEQ])); 
  
  EnOceanSendPacket(buf,BASE_PACKET_LEN + CONVERT_TO_INT(buf[DATA_LENGTH_L_SEQ],buf[DATA_LENGTH_H_SEQ]));
 
  while(!g_sEOmsg.EnOceanTelegramRxflags);   
  
  if(g_sEOmsg.EnOcean_RX_buf[FIRST_DATA_SEQ] == RET_OK)result = 0;
  g_sEOmsg.EnOceanTelegramRxflags = 0;
  
  return result;
}

static uint8_t FilterDelDeviceID(uint32_t deviceID)
{
  uint8_t result = 1;
  uint8_t buf[13]; 
  
  buf[SYNC_BYTE_SEQ] = SYNC_BYTE;
  buf[DATA_LENGTH_H_SEQ] = 0;
  buf[DATA_LENGTH_L_SEQ] = 6;
  buf[OPTION_LENGTH_SEQ] = 0;
  buf[PACKET_TYPE_SEQ] = PACKET_TYPES_COMMON_COMMAND;
  buf[HEADER_CRC8_SEQ] = GetCheckSum(&buf[DATA_LENGTH_H_SEQ],PACKET_HEADER_LEN);
  buf[FIRST_DATA_SEQ] = CO_WR_FILTER_DEL;
  buf[FIRST_DATA_SEQ+1] = 0;  //Filter type
  buf[FIRST_DATA_SEQ+2] = deviceID>>24;  //Filter value device ID
  buf[FIRST_DATA_SEQ+3] = (deviceID&0xff0000)>>16;
  buf[FIRST_DATA_SEQ+4] = (deviceID&0xff00)>>8;
  buf[FIRST_DATA_SEQ+5] = deviceID&0xff;
  buf[FIRST_DATA_SEQ+6] = GetCheckSum(&buf[FIRST_DATA_SEQ],CONVERT_TO_INT(buf[DATA_LENGTH_L_SEQ],buf[DATA_LENGTH_H_SEQ])); 
  
  EnOceanSendPacket(buf,BASE_PACKET_LEN + CONVERT_TO_INT(buf[DATA_LENGTH_L_SEQ],buf[DATA_LENGTH_H_SEQ]));
 
  while(!g_sEOmsg.EnOceanTelegramRxflags);   
  
  if(g_sEOmsg.EnOcean_RX_buf[FIRST_DATA_SEQ] == RET_OK)result = 0;
  g_sEOmsg.EnOceanTelegramRxflags = 0;
  
  return result;
}

static uint8_t FilterDelAll(void)
{
  uint8_t result = 1;
  uint8_t buf[8]; 
  
  buf[SYNC_BYTE_SEQ] = SYNC_BYTE;
  buf[DATA_LENGTH_H_SEQ] = 0;
  buf[DATA_LENGTH_L_SEQ] = 1;
  buf[OPTION_LENGTH_SEQ] = 0;
  buf[PACKET_TYPE_SEQ] = PACKET_TYPES_COMMON_COMMAND;
  buf[HEADER_CRC8_SEQ] = GetCheckSum(&buf[DATA_LENGTH_H_SEQ],PACKET_HEADER_LEN);
  buf[FIRST_DATA_SEQ] = CO_WR_FILTER_DEL_ALL;
  buf[FIRST_DATA_SEQ+1] = GetCheckSum(&buf[FIRST_DATA_SEQ],CONVERT_TO_INT(buf[DATA_LENGTH_L_SEQ],buf[DATA_LENGTH_H_SEQ])); 
  
  EnOceanSendPacket(buf,BASE_PACKET_LEN + CONVERT_TO_INT(buf[DATA_LENGTH_L_SEQ],buf[DATA_LENGTH_H_SEQ]));
 
  while(!g_sEOmsg.EnOceanTelegramRxflags);   
  
  if(g_sEOmsg.EnOcean_RX_buf[FIRST_DATA_SEQ] == RET_OK)result = 0;
  g_sEOmsg.EnOceanTelegramRxflags = 0;
  
  return result;
}
*/

static uint8_t EO_SendMsg(uint16_t FunctionNo,uint32_t DestinationID,uint8_t *data,uint8_t len)
{
  uint8_t buf[128];
  uint8_t i=0;
  uint8_t result = 1;
  
  if(len >100)return result;
  if(g_sEOmsg.TCM310FChipID == 0)return result;
  
  uint32_t ChipID = CONVERT_TO_LONG_LH(g_sEOmsg.TCM310FChipID);
  uint32_t DID = CONVERT_TO_LONG_LH(DestinationID);
  
  buf[i++] = SYNC_BYTE;          //SYNC_BYTE_SEQ
  buf[i++] = 0;                  //DATA_LENGTH_H_SEQ
  buf[i++] = 4+len;              //DATA_LENGTH_L_SEQ
  buf[i++] = 10;                 //OPTION_LENGTH_SEQ
  buf[i++] = PACKET_TYPES_REMOTE_MAN_COMMAND;//PACKET_TYPE_SEQ
  buf[i++] = GetCheckSum(&buf[DATA_LENGTH_H_SEQ],PACKET_HEADER_LEN);//HEADER_CRC8_SEQ
  buf[i++] = MSB(FunctionNo);     
  buf[i++] = LSB(FunctionNo); 
  buf[i++] = 0;                 //Manufacturer ID 0x0001
  buf[i++] = 1;
  if(len){
    memcpy(&buf[i],data,len);     //User Data
    i += len;
  }
  memcpy(&buf[i],(uint8_t *)&DID,4); //Destination ID
  i += 4;
  memcpy(&buf[i],(uint8_t *)&ChipID,4);    //Sender ID
  i += 4;
  buf[i++] = 0xff; //dBm  Send case: FF
  buf[i++] = 0;    //Send With Delay
  buf[i++] = GetCheckSum(&buf[FIRST_DATA_SEQ], CONVERT_TO_INT(buf[DATA_LENGTH_L_SEQ],buf[DATA_LENGTH_H_SEQ])+buf[OPTION_LENGTH_SEQ]);
  
  EnOceanSendPacket(buf,i);
  
  while(!g_sEOmsg.EnOceanTelegramRxflags);   
  
  if(g_sEOmsg.EnOcean_RX_buf[FIRST_DATA_SEQ] == RET_OK)result = 0;
  g_sEOmsg.EnOceanTelegramRxflags = 0;
  
  return result;
}

uint8_t InitTCM310F(void)
{
  memset(&g_sEOmsg, 0, sizeof(g_sEOmsg));
  
  memset(Device_GW_ID, 0xff, sizeof(Device_GW_ID));
  
  if(TCM310FReset())return 1;
  delay_ms(50);  
  if(GetChipID())return 1;
  //if(FilterEnable(1))return 1;
  ReadCtrDevice(Device_GW_ID);

  return 0;
}

Work_Mode_t GetWorkMode(void)
{
  return g_Work_Mode;
}

void SetWorkMode(Work_Mode_t Work_Mode)
{
  if(Work_Mode != g_Work_Mode)
  {
    switch(Work_Mode)
    {
      case Mode_Normal:{
       BLUE_LED_OFF();
       g_Work_Mode = Mode_Normal;
      }break;
      case Mode_WPS:{
       BLUE_LED_ON(); 
       g_Work_Mode = Mode_WPS;
       SetEvent(WPS_SEND_MSG_EVENT); 
      }break;
      case Mode_Clear:{
        for(uint8_t i=0;i<CTR_DEVICE_MAX;i++){
          Device_GW_ID[i] = 0xffffffff;
        }
        UpdateCtrDevice(Device_GW_ID);
        
        for(int i=0;i<20;i++){
          BLUE_LED_TOGGLE();
          delay_ms(50);  
        }
        BLUE_LED_OFF();
       g_Work_Mode = Mode_Normal;
      }break;
      default: break;
    }  
  }
}

void Send_WPS_Msg(void)
{
  static uint8_t Send_WPS_Msg_Count = 0;
  if(GetWorkMode() == Mode_WPS){
    if(++Send_WPS_Msg_Count <= 10){
      EO_SendMsg(FUNCTION_NO_WPS_MSG,0xffffffff,NULL,0);
      SetEventTimeOut(WPS_SEND_MSG_EVENT,2000);  
    }else{     
      Send_WPS_Msg_Count = 0;
      SetWorkMode(Mode_Normal);
      ClearEvent(WPS_SEND_MSG_EVENT);
    }
  }else{
    Send_WPS_Msg_Count = 0;
    ClearEvent(WPS_SEND_MSG_EVENT);
  }
}

uint8_t EnOcean_Poll_handler(void)
{
  if(g_sEOmsg.EnOceanTelegramRxflags)
  {
    if(g_sEOmsg.EnOcean_RX_buf[PACKET_TYPE_SEQ] == PACKET_TYPES_REMOTE_MAN_COMMAND)
    {
      GREEN_LED_ON();
      
      uint16_t datalen = g_sEOmsg.EnOcean_RX_buf[DATA_LENGTH_L_SEQ];
      uint16_t Fun = CONVERT_TO_INT(g_sEOmsg.EnOcean_RX_buf[REMOTE_MAN_FUN_NO_L_SEQ],g_sEOmsg.EnOcean_RX_buf[REMOTE_MAN_FUN_NO_H_SEQ]);
      
      switch(Fun)
      {
      case FUNCTION_NO_WPS_MSG: break;
      case FUNCTION_NO_WPS_RSP_MSG:{
        if(datalen == 4 && GetWorkMode() == Mode_WPS){
          uint32_t DestinationID = CONVERT_TO_LONG(g_sEOmsg.EnOcean_RX_buf[13],g_sEOmsg.EnOcean_RX_buf[12],
                                                   g_sEOmsg.EnOcean_RX_buf[11],g_sEOmsg.EnOcean_RX_buf[10]);
          uint32_t SourceID = CONVERT_TO_LONG(g_sEOmsg.EnOcean_RX_buf[17],g_sEOmsg.EnOcean_RX_buf[16],
                                              g_sEOmsg.EnOcean_RX_buf[15],g_sEOmsg.EnOcean_RX_buf[14]);
          if(DestinationID == g_sEOmsg.TCM310FChipID && PollCtrDevice(SourceID))
          {
            AddDevice(SourceID);
            SetWorkMode(Mode_Normal);
          }
        }
      }break;
      case FUNCTION_NO_CTR_MSG:{
        if(datalen == 8){
          uint32_t DestinationID = CONVERT_TO_LONG(g_sEOmsg.EnOcean_RX_buf[17],g_sEOmsg.EnOcean_RX_buf[16],
                                                   g_sEOmsg.EnOcean_RX_buf[15],g_sEOmsg.EnOcean_RX_buf[14]);
          //uint32_t SourceID = CONVERT_TO_LONG(g_sEOmsg.EnOcean_RX_buf[21],g_sEOmsg.EnOcean_RX_buf[20],
          //                                    g_sEOmsg.EnOcean_RX_buf[19],g_sEOmsg.EnOcean_RX_buf[18]);
          if(DestinationID == g_sEOmsg.TCM310FChipID/* && !PollCtrDevice(SourceID)*/)
          {
            int code;
            memcpy((uint8_t *)&code,&g_sEOmsg.EnOcean_RX_buf[REMOTE_MAN_USER_DATA_SEQ],4);
            //if(g_sEOmsg.EnOcean_RX_buf[REMOTE_MAN_USER_DATA_SEQ] & 0x04){
            //  g_sEOmsg.EnOcean_RX_buf[REMOTE_MAN_USER_DATA_SEQ] &= 0x08;
            //}
            //IR_Send(&g_sEOmsg.EnOcean_RX_buf[REMOTE_MAN_USER_DATA_SEQ]);
            IR_Send(code);
          }
        }
      }break;
      default:break;
      }     
      GREEN_LED_OFF();
    }
    
    g_sEOmsg.EnOceanTelegramRxflags = 0; 
  }
  return 0;
}
    


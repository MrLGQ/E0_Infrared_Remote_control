#ifndef __ESP3_COM_H__
#define __ESP3_COM_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif
  
#define  SYNC_BYTE                           0x55  
  
//Packet types
#define  PACKET_TYPES_RADIO                  0x01
#define  PACKET_TYPES_RESPONSE               0x02  
#define  PACKET_TYPES_RADIO_SUB_TEL          0x03  
#define  PACKET_TYPES_EVENT                  0x04 
#define  PACKET_TYPES_COMMON_COMMAND         0x05 
#define  PACKET_TYPES_SMART_ACK_COMMAND      0x06 
#define  PACKET_TYPES_REMOTE_MAN_COMMAND     0x07
#define  PACKET_TYPES_RADIO_MESSAGE          0x09
#define  PACKET_TYPES_RADIO_ADVANCED         0x0A

//Return Codes  
#define  RET_OK                              0x00
#define  RET_ERROR                           0x01
#define  RET_NOT_SUPPORTED                   0x02
#define  RET_WRONG_PARAM                     0x03
#define  RET_OPERATION_DENIED                0x04
  
//EVENT Codes
#define  SA_RECLAIM_NOT_SUCCESSFUL           0x01
#define  SA_CONFIRM_LEARN                    0x02
#define  SA_LEARN_ACK                        0x03
#define  CO_READY                            0x04
#define  CO_EVENT_SECUREDEVICES              0x05
  
//COMMON_COMMAND Codes
#define  CO_WR_SLEEP                         0x01
#define  CO_WR_RESET                         0x02
#define  CO_RD_VERSION                       0x03
#define  CO_RD_SYS_LOG                       0x04
#define  CO_WR_SYS_LOG                       0x05
#define  CO_WR_BIST                          0x06
#define  CO_WR_IDBASE                        0x07
#define  CO_RD_IDBASE                        0x08
#define  CO_WR_REPEATER                      0x09
#define  CO_RD_REPEATER                      0x0A
#define  CO_WR_FILTER_ADD                    0x0B
#define  CO_WR_FILTER_DEL                    0x0C
#define  CO_WR_FILTER_DEL_ALL                0x0D
#define  CO_WR_FILTER_ENABLE                 0x0E
#define  CO_RD_FILTER                        0x0F
#define  CO_WR_WAIT_MATURITY                 0x10
#define  CO_WR_SUBTEL                        0x11
#define  CO_WR_MEM                           0x12
#define  CO_RD_MEM                           0x13
#define  CO_RD_MEM_ADDRESS                   0x14
#define  CO_RD_SECURITY                      0x15
#define  CO_WR_SECURITY                      0x16
#define  CO_WR_LEARNMODE                     0x17
#define  CO_RD_LEARNMODE                     0x18
#define  CO_WR_SECUREDEVICE_ADD              0x19
#define  CO_WR_SECUREDEVICE_DEL              0x1A
#define  CO_RD_SECUREDEVICES                 0x1B
#define  CO_WR_MODE                          0x1C
#define  CO_RD_NUMSECUREDEVICES              0x1D
#define  CO_RD_SECUREDEVICE_BY_ID            0x1E
#define  CO_WR_SECUREDEVICE_ADD_PSK          0x1F
#define  CO_WR_SECUREDEVICE_SENDTEACHIN      0x20
#define  CO_WR_TEMPORARY_RLC_WINDOW          0x21
#define  CO_RD_SECUREDEVICE_PSK              0x22
  
//SMART ACK Codes
#define  SA_WR_LEARNMODE                     0x01
#define  SA_RD_LEARNMODE                     0x02
#define  SA_WR_LEARNCONFIRM                  0x03
#define  SA_WR_CLIENTLEARNRQ                 0x04
#define  SA_WR_RESET                         0x05
#define  SA_RD_LEARNEDCLIENTS                0x06
#define  SA_WR_RECLAIMS                      0x07
#define  SA_WR_POSTMASTER                    0x08
  
#define  TELE_RSP_RORG                       0xF6
#define  TELE_1BS_RORG                       0xD5
#define  TELE_4BS_RORG                       0xA5
#define  TELE_VLD_RORG                       0xD2
#define  TELE_ADT_RORG                       0xA6
  
#define  FILTER_ENABLE                       0x01
#define  FILTER_DISABLE                      0x00
#define  FILTER_TYPE_DEVICEID                0
#define  FILTER_TYPE_RORG                    1
#define  FILTER_TYPE_DBM                     2
#define  FILTER_KIND_BLOCKS                  0x00
#define  FILTER_KIND_APPLE                   0x80
  
#define  SYNC_BYTE_SEQ                       0
#define  DATA_LENGTH_H_SEQ                   1
#define  DATA_LENGTH_L_SEQ                   2
#define  OPTION_LENGTH_SEQ                   3
#define  PACKET_TYPE_SEQ                     4
#define  HEADER_CRC8_SEQ                     5
#define  FIRST_DATA_SEQ                      6
#define  REMOTE_MAN_FUN_NO_H_SEQ             6
#define  REMOTE_MAN_FUN_NO_L_SEQ             7
#define  REMOTE_MAN_USER_DATA_SEQ            10

  
#define  PACKET_HEADER_LEN                   4 //(u16DataLength+u8OptionLength+u8PackeType)
#define  BASE_PACKET_LEN                     (1 + 4 + 1 + 1)//(Sync_Byte+PACKET_HEADER_LEN+CRC8H+CRC8D)
  

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __ESP3_COM_H__

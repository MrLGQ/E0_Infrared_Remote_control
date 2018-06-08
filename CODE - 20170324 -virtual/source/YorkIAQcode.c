


typedef enum{
  Switch_OFF,
  Switch_ON
}YorkIAQSwitch;

typedef enum{
  Mode_AUTO,
  Mode_CFT,
  Mode_COOL,
  Mode_DRY,
  Mode_FAN,
  Mode_HEAT
}YorkIAQMode;

typedef enum{
  WindSpeed_AUTO,
  WindSpeed_LOW,
  WindSpeed_MID,
  WindSpeed_HIGH
}YorkIAQWindSpeed;

/*
Function    : GetYorkIAQCode
Description : 根据设定的参数，获得 YorkIAQ 第三代空调遥控器 First Ram,Second Ram,Third Ram,Fourth Ram.
Input       : Switch，Mode，WindSpeed，Temperature，
Output      : retCode[4]， YorkIAQ 第三代空调遥控器 First Ram,Second Ram,Third Ram,Fourth Ram（低4Bit有效）
Return      : 0-成功，其他-失败
*/
int GetYorkIAQCode(YorkIAQSwitch Switch,YorkIAQMode Mode,YorkIAQWindSpeed WindSpeed,uint8_t Temperature,uint8_t *retCode)
{
  uint8_t code[4] = {0,0,0,0};
  
  switch(Mode)
  {
    case Mode_AUTO: code[0] = 0x04;break;
    case Mode_CFT: code[0] = 0x05;break;
    case Mode_COOL: code[0] = 0x00;break;
    case Mode_DRY: code[0] = 0x01;break;
    case Mode_FAN: code[0] = 0x02;break;
    case Mode_HEAT: code[0] = 0x03;break;
    default: return -1;
  }
  if(Switch == Switch_OFF){
    code[0] &= 0xf7;
  }else{
    code[0] |= 0x08;
  }
  
  switch(WindSpeed)
  {
    case WindSpeed_AUTO: code[1] = 0x08;break;
    case WindSpeed_LOW: code[1] = 0x01;break;
    case WindSpeed_MID: code[1] = 0x02;break;
    case WindSpeed_HIGH: code[1] = 0x04;break;
    default: return -1;
  }
  
  if(Temperature >= 16 && Temperature <= 30){
    code[2] = Temperature - 15;
  }else{
    return -1;
  }
  
  for(uint8_t i=0;i<4;i++){
    *(retCode + i) = code[i];
  }
  
  return 0;
}
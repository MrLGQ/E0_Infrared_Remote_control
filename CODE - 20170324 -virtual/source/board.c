
#include "includes.h"


volatile unsigned int system_time_ms = 0;

volatile unsigned int delayCount_ms = 0;

volatile unsigned char TIM2_Int_Cnt = 0;
extern uint8_t SEND_CMD[4];

#ifdef USE_FULL_ASSERT
/*
Function    : assert_failed
Description : Reports the name of the source file and the source
line number where the assert_param error has occurred.
Input       : - file: pointer to the source file name
- line: assert_param error line source number
Output      : None
Return      : None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  /*
  User can add his own implementation to report the file name
  and line number, ex:
  printf("Wrong parameters value: file %s on line %d\r\n", file, line)
  */
  /* Infinite loop */
  while (1)
  {
  }
}
#endif


void delay_ms(unsigned int ms)
{
  delayCount_ms = ms;
  while (delayCount_ms > 0);
}

void delay_us(unsigned int us)
{ 
  //if(IrStatus == IR_RX_START)return;
  if(us == 0)return;
  if(us>32000)us=32000;
  TIM_Cmd(TIM2, ENABLE);
  while (us > TIM2->CNT);
  TIM_Cmd(TIM2, DISABLE);
}

void UartSendPacket(USART_TypeDef* USARTx, unsigned char *inBuff, unsigned short usLength)
{
  if(USARTx != USART1 && USARTx != USART2 && USARTx != USART3)return;
  
  unsigned long ulIndex = 0;
  for (ulIndex = 0; ulIndex < usLength; ulIndex++)
  {
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) != SET);
    
    USART_SendData(USARTx, *inBuff++);
  }
}

void OUTPUT_38K_PWM(void)
{
  GPIO_InitTypeDef gpio;
  //PWM_38K_OUTPUT
  gpio.GPIO_Mode = GPIO_Mode_AF_OD;
  gpio.GPIO_Pin = PWM_38K_PIN;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(PWM_38K_BASE, &gpio);  
  TIM_Cmd(TIM3, ENABLE);
}

void CLOUSE_38K_PWM(void)
{
  TIM_Cmd(TIM3, DISABLE);
  
  GPIO_InitTypeDef gpio;
  //PWM_38K_OUTPUT
  gpio.GPIO_Mode = GPIO_Mode_Out_OD;
  gpio.GPIO_Pin = PWM_38K_PIN;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(PWM_38K_BASE, &gpio); 
  GPIO_SetBits(PWM_38K_BASE, PWM_38K_PIN); 
}

void Key_Event_Process(void)
{
  static int Key_event_timeout = 0;
  
  if(GPIO_ReadInputDataBit(KEY_BASE, KEY_PIN))
  {
    Key_event_timeout++;
    if(Key_event_timeout < 20)
    {
      BLUE_LED_ON(); 
    }
    else if(Key_event_timeout > 20 && Key_event_timeout < 40)
    {
     BLUE_LED_OFF(); 
     GREEN_LED_ON();     
    }
    else if(Key_event_timeout>40)
    {
     GREEN_LED_TOGGLE();
    }
    SetEventTimeOut(KEY_EVENT,100);
  }
  else
  {
      if(Key_event_timeout < 20)
      {
       while( EO_SendMsg(FUNCTION_NO_CTR_MSG ,DestinationID1,STUDY_CMD,4)); //发送成功发回0
       
       BLUE_LED_OFF();        
       }
      else if(Key_event_timeout > 20 && Key_event_timeout < 40)
      {
       while(EO_SendMsg(FUNCTION_NO_CTR_MSG ,DestinationID1,SEND_CMD,4));//uint8_t SEND_CMD[4]
       GREEN_LED_OFF(); 
      } 
      else if(Key_event_timeout>40)
      {
        STUDY_CMD[1]++;
        SEND_CMD[1]++;
        GREEN_LED_OFF();   
      }

    Key_event_timeout = 0;
    ClearEvent(KEY_EVENT);
  }
}

static void IWDG_Configuration(void)
{
  //写入0x5555,用于允许寄存器写入功能 
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); 
  //时钟分频,40K/256=156HZ(6.4ms)
  IWDG_SetPrescaler(IWDG_Prescaler_256); 
  //喂狗时间 6.4MS*500=3.2S.注意不能大于0xfff
  IWDG_SetReload(500);  
  //手动喂狗
  IWDG_ReloadCounter();  
  IWDG_Enable();  
}

static void SysTick_Init(void)
{
  SysTick_Config(SystemCoreClock / 1000 - 1);
}

static void HW_IO_init(void)
{
  GPIO_InitTypeDef gpio;
  EXTI_InitTypeDef exti;
  NVIC_InitTypeDef nvic;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 
  //Remap JTAG (SWD ONLY)
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  //KEY
  gpio.GPIO_Mode = GPIO_Mode_IPD;
  gpio.GPIO_Pin = KEY_PIN;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(KEY_BASE, &gpio); 
  //KEY 中断
  GPIO_EXTILineConfig(KEY_INT_PORT, KEY_INT_PIN);
  
  exti.EXTI_Line = KEY_INT_LINE;
  exti.EXTI_Mode = EXTI_Mode_Interrupt;
  exti.EXTI_Trigger = EXTI_Trigger_Rising;
  exti.EXTI_LineCmd = ENABLE;
  EXTI_Init(&exti);
  
  nvic.NVIC_IRQChannel = KEY_INT_CHANNEL;
  nvic.NVIC_IRQChannelPreemptionPriority = 0;
  nvic.NVIC_IRQChannelSubPriority = 4;
  nvic.NVIC_IRQChannelCmd = ENABLE;
  NVIC_ClearPendingIRQ(KEY_INT_CHANNEL);
  NVIC_Init(&nvic);
  
  //IR_RX
  gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  gpio.GPIO_Pin = IR_RX_IRQ_PIN;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(IR_RX_IRQ_BASE, &gpio);  

  GPIO_EXTILineConfig(IR_RX_IRQ_INT_PORT, IR_RX_IRQ_INT_PIN);
  
  exti.EXTI_Line = IR_RX_IRQ_INT_LINE;
  exti.EXTI_Mode = EXTI_Mode_Interrupt;
  exti.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  exti.EXTI_LineCmd = ENABLE;
  EXTI_Init(&exti);
  
  nvic.NVIC_IRQChannel = IR_RX_IRQ_INT_CHANNEL;
  nvic.NVIC_IRQChannelPreemptionPriority = 0;
  nvic.NVIC_IRQChannelSubPriority = 0;
  nvic.NVIC_IRQChannelCmd = ENABLE;
   
  NVIC_Init(&nvic);
  NVIC_ClearPendingIRQ(IR_RX_IRQ_INT_CHANNEL);

  
  //GREEN LED
  gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  //gpio.GPIO_Mode = GPIO_Mode_Out_OD;
  gpio.GPIO_Pin = LED_GREEN_PIN;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LED_BASE, &gpio);
  GPIO_ResetBits(LED_BASE, LED_GREEN_PIN);  
  
  //BLUE LED 
  gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  //gpio.GPIO_Mode = GPIO_Mode_Out_OD;
  gpio.GPIO_Pin = LED_BLUE_PIN;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LED_BASE, &gpio); 
  GPIO_ResetBits(LED_BASE, LED_BLUE_PIN);
  
  //PWM_38K_OUTPUT
  gpio.GPIO_Mode = GPIO_Mode_Out_OD;
  gpio.GPIO_Pin = PWM_38K_PIN;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(PWM_38K_BASE, &gpio);  
  GPIO_SetBits(PWM_38K_BASE, PWM_38K_PIN); 
  
}

//ENOCEAN
static void UART1Configure(void)
{
  USART_InitTypeDef usart;
  GPIO_InitTypeDef gpio;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  //Clock
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  //GPIO - TX pin
  gpio.GPIO_Mode = GPIO_Mode_AF_PP;
  gpio.GPIO_Pin = GPIO_Pin_9;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &gpio);
  //GPIO - RX pin
  gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  gpio.GPIO_Pin = GPIO_Pin_10;
  //gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &gpio);
  
  //NVIC
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  //USART
  usart.USART_BaudRate = 57600;
  usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  usart.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  usart.USART_Parity = USART_Parity_No;
  usart.USART_StopBits = USART_StopBits_1;
  usart.USART_WordLength = USART_WordLength_8b;
  USART_Init(USART1, &usart);
  
  USART_ITConfig(USART1,USART_IT_RXNE,ENABLE); 

  USART_Cmd(USART1, ENABLE);
}

static void TIM2_Configuration(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  TIM_DeInit(TIM2);
  TIM_TimeBaseStructure.TIM_Period = 40000;
  TIM_TimeBaseStructure.TIM_Prescaler = 72-1;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  //定时时间T计算公式：
  //T=(TIM_Period+1)*(TIM_Prescaler+1)/TIMxCLK=(35999+1)*(1999+1)/72MHz=1s
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);  
  TIM_Cmd(TIM2,DISABLE);
}

//TIM4_Mode_Config PWM_38K_output
static void TIM3_Configuration(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  
  //部分映射，将TIM3_CH4映射到PB1
  GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);
  //GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
  
  TIM_DeInit(TIM3);
  /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 0x0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 1899;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;
  TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM3, ENABLE);

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 1266;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset; 
  TIM_OC4Init(TIM3,&TIM_OCInitStructure);
  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
   
  /* TIM3 counter enable */
  //TIM_Cmd(TIM3,ENABLE);
  CLOUSE_38K_PWM();
  //OUTPUT_38K_PWM();
}

void HW_Init(void)
{
  SysTick_Init();
  
  HW_IO_init();
  
  UART1Configure();
  
  TIM2_Configuration();
  
  TIM3_Configuration();
  
  IWDG_Configuration();
  
}

void SysTick_Handler(void)
{         
  if(delayCount_ms)delayCount_ms--; 
  
  GetEvent();

  ++system_time_ms;
}

void EXTI15_10_IRQHandler(void)
{ 
  if (EXTI_GetITStatus(KEY_INT_LINE) != RESET)
  {
    EXTI_ClearITPendingBit(KEY_INT_LINE);
    SetEventTimeOut(KEY_EVENT,100);
  }
}

void EXTI9_5_IRQHandler(void)
{
  if(EXTI_GetITStatus(IR_RX_IRQ_INT_LINE) != RESET)
  {
    EXTI_ClearITPendingBit(IR_RX_IRQ_INT_LINE);
    if(irMsg.IrStatus == IR_RX_START)
    {
      if(irMsg.RxPulseNum)
      {
        irMsg.PulseTimeEnd = TIM2->CNT;
        irMsg.IRPulsebuf[irMsg.RxPulseNum-1] = irMsg.PulseTimeEnd - irMsg.PulseTimeStart + TIM2_Int_Cnt*40000;   
        TIM2_Int_Cnt = 0;
        irMsg.PulseTimeStart = irMsg.PulseTimeEnd;
        if(irMsg.IRPulsebuf[irMsg.RxPulseNum-1] > 40000 || ++irMsg.RxPulseNum > 254)
        {
          irMsg.RxPulseNum -= 1;
          if(irMsg.IRPulsebuf[irMsg.RxPulseNum-1] > 40000)irMsg.IRPulsebuf[irMsg.RxPulseNum-1] = 0;
          irMsg.IRPulsebuf[irMsg.RxPulseNum] = 0;
          irMsg.IrStatus = IR_RX_END;
          TIM_Cmd(TIM2, DISABLE);  
          SetEvent(IR_CODE_EVENT);
        }        
      }
      else
      {
        irMsg.RxPulseNum = 1;
        irMsg.PulseTimeStart = TIM2->CNT; 
        TIM2_Int_Cnt = 0;
        TIM_Cmd(TIM2, ENABLE);
      }
    }
  }
}

void TIM2_IRQHandler(void)
{ 
  if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET)
  {
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);    
    ++TIM2_Int_Cnt;
    if(irMsg.IrStatus == IR_RX_START)
    {
      if(TIM2_Int_Cnt > 1)
      {
        irMsg.RxPulseNum -= 1;
        irMsg.IRPulsebuf[irMsg.RxPulseNum] = 0;
        irMsg.IrStatus = IR_RX_END;
        TIM_Cmd(TIM2, DISABLE);
        TIM2_Int_Cnt = 0;
        SetEvent(IR_CODE_EVENT);
      }
    }
  }
}






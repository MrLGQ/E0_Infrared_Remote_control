

#ifndef BOARD_H
#define	BOARD_H

#ifdef __cplusplus
extern "C"
{
#endif

//KEY Pin: PA13
#define         KEY_CLOCK               RCC_APB2Periph_GPIOC
#define         KEY_BASE                GPIOC
#define         KEY_PIN                 GPIO_Pin_13
#define         KEY_INT_PORT            GPIO_PortSourceGPIOC
#define         KEY_INT_PIN             GPIO_PinSource13
#define         KEY_INT_LINE            EXTI_Line13
#define         KEY_INT_CHANNEL         EXTI15_10_IRQn 
  
//IR RX IRQ Pin: PA8
#define         IR_RX_IRQ_CLOCK         RCC_APB2Periph_GPIOA
#define         IR_RX_IRQ_BASE          GPIOA
#define         IR_RX_IRQ_PIN           GPIO_Pin_8
#define         IR_RX_IRQ_INT_PORT      GPIO_PortSourceGPIOA
#define         IR_RX_IRQ_INT_PIN       GPIO_PinSource8
#define         IR_RX_IRQ_INT_LINE      EXTI_Line8
#define         IR_RX_IRQ_INT_CHANNEL   EXTI9_5_IRQn
  
// LED Pin: PC4  PC5
#define LED_CLOCK                   RCC_APB2Periph_GPIOC
#define LED_BASE                    GPIOC
#define LED_GREEN_PIN               GPIO_Pin_5
#define LED_BLUE_PIN                GPIO_Pin_4
  
//PWM_38K PIN PB1
#define PWM_38K_CLOCK               RCC_APB2Periph_GPIOB
#define PWM_38K_BASE                GPIOB
#define PWM_38K_PIN                 GPIO_Pin_1


#define BLUE_LED_ON()               GPIO_ResetBits(LED_BASE, LED_BLUE_PIN)
#define BLUE_LED_OFF()              GPIO_SetBits(LED_BASE, LED_BLUE_PIN)
#define GREEN_LED_ON()              GPIO_ResetBits(LED_BASE, LED_GREEN_PIN)
#define GREEN_LED_OFF()             GPIO_SetBits(LED_BASE, LED_GREEN_PIN)
#define BLUE_LED_TOGGLE()           if(GPIO_ReadOutputDataBit(LED_BASE, LED_BLUE_PIN))\
                                    GPIO_ResetBits(LED_BASE, LED_BLUE_PIN);\
                                    else \
                                    GPIO_SetBits(LED_BASE, LED_BLUE_PIN)
#define GREEN_LED_TOGGLE()          if(GPIO_ReadOutputDataBit(LED_BASE, LED_GREEN_PIN))\
                                    GPIO_ResetBits(LED_BASE, LED_GREEN_PIN);\
                                    else \
                                    GPIO_SetBits(LED_BASE, LED_GREEN_PIN)
                                      

extern volatile unsigned int system_time_ms;
 

void delay_ms(unsigned int ms);
void delay_us(unsigned int us);
void UartSendPacket(USART_TypeDef* USARTx, unsigned char *inBuff, unsigned short usLength);
void OUTPUT_38K_PWM(void);
void CLOUSE_38K_PWM(void);
void Key_Event_Process(void);

void HW_Init(void);


#ifdef __cplusplus
}
#endif

#endif

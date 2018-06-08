#ifndef INCLUDES_H 
#define INCLUDES_H 

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#include "misc.h"
#include "STM32_Flash_WR.h"
#include "board.h"  
#include "enocean.h"
#include "ir.h"
#include "main.h"

  
#include <string.h>            //for memcpy
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <setjmp.h>
  

  
  
#define CONVERT_TO_INT(lsb,msb)      ((lsb) + 0x0100*(msb))
  
#define CONVERT_TO_LONG(b1,b2,b3,b4) ((b1) + 0x0100*(b2) + 0x010000*(b3) + 0x01000000*(b4))
  
#define CONVERT_TO_LONG_LH(long)     (((long&0xff000000)>>24) + ((long&0x00ff0000)>>8) + ((long&0x0000ff00)<<8) + ((long&0x000000ff)<<24))

/** Get the Least Significant Byte (LSB) of an unsigned int*/
#define LSB(num) ((num) & 0xFF)

/** Get the Most Significant Byte (MSB) of an unsigned int*/
#define MSB(num) ((num) >> 8)
  
#define MIN(a,b)  (a>=b?b:a)
  
#define isascii(_Char)   ( (unsigned)(_Char) < 0x80 )
  
#define TRUE			1
#define FALSE			(!TRUE)
  
#ifdef __cplusplus
    }
#endif
#endif
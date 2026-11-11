#ifndef __ADS8860_H
#define __ADS8860_H

#include "main.h"
#include "stm32h7xx_hal.h"
#include "appfun_lib.h"

#define SCLK_Pin GPIO_PIN_3
#define SCLK_Port GPIOB
#define DIN_Pin GPIO_PIN_4
#define DIN_Port GPIOB
#define DOUT_Pin GPIO_PIN_6
#define DOUT_Port GPIOA
#define CONVST_Pin GPIO_PIN_15
#define CONVST_Port GPIOA

#define ADS8860_SPI_Port hspi1
extern SPI_HandleTypeDef ADS8860_SPI_Port;

#define Conversation_Relay 100

void ADS8860_ReadValue(void);

void delay_600ns(void);
void ads8860_Init(void);
void delay_us(uint32_t nus);
 void CONVST_Reset(void) ;
 
 void CONVST_Set(void) ;
#endif

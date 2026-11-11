#ifndef _SPIUtil_H_
#define _SPIUtil_H_

#include "stm32h7xx_hal.h"


extern int32_t CurrentSpecdata[512+256];
extern int32_t CurrentDSpecdata[512+256];
extern int32_t CurrentRDSpecdata[512+256];
extern int32_t CurrentNSpecdata[512+256];
extern uint8_t CurrentIntSpecdata[1024+512];

typedef struct
{
  uint32_t xenLampRefVol;
  uint32_t xenLampFrequence;
  uint32_t speColAveTimes;
  uint32_t speColIntSpan;  
}MEA_PARAM, *PMEA_PARAM;


typedef enum {FALSE = 0, TRUE = !FALSE} bool;
//
//typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;


/* Exported variables --------------------------------------------------------*/

extern MEA_PARAM CurrentMeaParam;




void SysTick_DelayNsBySoft(uint32_t deleyNs);
bool SPIU_CollectionAction(void);
bool SPIU_SetLCtrlVol();
void SPIU_ReadSValuePix(uint32_t curColIndex, uint32_t pixIndex,uint16_t isSpec);

  
#endif
#include "DAC80501.h"
#include "spi_util.h"

extern SPI_HandleTypeDef hspi2;

void WriteToDAC80501(uint8_t count,uint8_t *buf);
void DAC80501_CS_RESET(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void DAC80501_CS_SET(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
//void DAC80501_CS_Channal_SET(uint8_t channal);
//void DAC80501_CS_Channal_RESET();
uint8_t DAC80501_Buf[3] = {0,0,0};  
//extern CemsGlobalData_t gCemsData;
//extern volatile float ADVal[3];
//extern float GAS_Val[6];

 void WriteToDAC80501(uint8_t count,uint8_t *buf)
{


   DAC80501_CS_RESET(GPIOB,GPIO_PIN_12);

   SysTick_DelayNsBySoft(100);
   HAL_SPI_Transmit(&hspi2, buf, count, 0xFF);
   SysTick_DelayNsBySoft(100);

   DAC80501_CS_SET(GPIOB,GPIO_PIN_12);
 

}





void DAC80501_VoltageOut() 
{
  uint32_t Voltage =(uint32_t)(CurrentMeaParam.xenLampRefVol/10.0/5.0*4096)*16;
  //uint32_t Voltage =(uint32_t)(32/10.0/5.0*4096)*16;
 //Voltage =Voltage*16;
  //printf("%d\r\n",Voltage);
  DAC80501_Buf[0] = 0x08;            
  DAC80501_Buf[1] = ((Voltage) & 0xFF00) >> 8;
  
  DAC80501_Buf[2] = ((Voltage) & 0x00FF) >> 0;
  
  WriteToDAC80501(3,DAC80501_Buf);
  SysTick_DelayNsBySoft(50);

}



//Æ¬Ñ¡Òý½Å
void DAC80501_CS_RESET(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{    
   HAL_GPIO_WritePin(GPIOx,GPIO_Pin,GPIO_PIN_RESET);	
}

void DAC80501_CS_SET(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{    
   HAL_GPIO_WritePin(GPIOx,GPIO_Pin,GPIO_PIN_SET);	
}




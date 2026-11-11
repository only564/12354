#include "ads8860.h"


//    ADS8860-Test Project.
//    First using 3-Wire CS Mode Without a Busy Indicator
//    In this interface option, DIN is HIGH and CONVST functions as CS Pin
//    
//    Author: xxxxt
//	  Date: 2019-5-23
//	  Project Platform: CubeMX+Keil MDK5
//	  No further more informations provided.
//
extern OSPI_HandleTypeDef hospi1;

static void DOUT_Reset(void) 
{ 
	HAL_GPIO_WritePin(DOUT_Port, DOUT_Pin, GPIO_PIN_RESET); 
}

static void DOUT_Set(void) 
{ 
	HAL_GPIO_WritePin(DOUT_Port, DOUT_Pin, GPIO_PIN_SET);
}

static void  DIN_Reset(void) 
{
	HAL_GPIO_WritePin(DIN_Port, DIN_Pin, GPIO_PIN_RESET);
}

static void DIN_Set(void) 
{
	HAL_GPIO_WritePin(DIN_Port, DIN_Pin, GPIO_PIN_SET);
}

 void CONVST_Reset(void) 
{ 
	HAL_GPIO_WritePin(CONVST_Port, CONVST_Pin, GPIO_PIN_RESET);
}

 void CONVST_Set(void) 
{ 
	HAL_GPIO_WritePin(CONVST_Port, CONVST_Pin, GPIO_PIN_SET);
}
uint8_t readBuffer[20] = {0};
void ADS8860_ReadValue(void)
{
  HAL_StatusTypeDef ret; 
//  uint8_t count=0; 
//  CONVST_Set();
//  delay_600ns();
//  CONVST_Reset();
  ret=HAL_SPI_Receive_DMA(&ADS8860_SPI_Port, readBuffer,2);
//   OSPI_RegularCmdTypeDef s_command = {0};
//
//  /* Initialize the read command */
//  s_command.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
//  s_command.FlashId            = HAL_OSPI_FLASH_ID_1;
//  s_command.InstructionMode    = 0;
//  s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
//  s_command.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
//  s_command.Instruction        = 0;
//  s_command.AddressMode        = 0;
//  s_command.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
//  s_command.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
//  s_command.Address            = 0;
//  s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
//  s_command.DataMode           = 0;
//  s_command.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
//  s_command.DummyCycles        = 0;
//  s_command.NbData             = 2;
//  s_command.DQSMode            = HAL_OSPI_DQS_DISABLE;
//  s_command.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;
//
//  /* Send the command */
//  if (HAL_OSPI_Command(&hospi1, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
//  {
//    //return LY68L6400_ERROR;
//  }
//
//  /* Reception of the data */
//  if (HAL_OSPI_Receive(&hospi1, readBuffer, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
//  {
//    //return LY68L6400_ERROR;
//  }

//  HAL_OSPI_Receive(&hospi1,readBuffer,0xFF);
//   SPI_1LINE_RX(&hspi1);
//  __HAL_SPI_ENABLE(&hspi1);
//  if(__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_RXP))
//  {
//    readBuffer[count++] = hspi1.Instance->RXDR;
//  }
  
  //ret=HAL_SPI_Receive_IT(&ADS8860_SPI_Port, readBuffer,2);
    if(ret!=HAL_OK)
    {
      printf("ret=%d\r\n" ,ret);
    }
  //HAL_SPI_Receive_IT(&ADS8860_SPI_Port, readBuffer,2);
  // CONVST_Set();
   printf("%d\r\n" ,readBuffer[0]*256+readBuffer[1]);
}

void delay_600ns(void)
{
	for (uint16_t i = 0; i < 10; i++)
        {
          __NOP();
        }
        


}
//延时nus
//nus为要延时的us数.     
//注意:nus的值,不要大于798915us(最大值即2^24/fac_us@fac_us=21)
void delay_us(uint32_t nus)
{            
       uint32_t temp;                  
       SysTick->LOAD=nus; //时间加载，fac_us与时钟频率有关，例如：168Mhz时
                                 //fac_us=168，168/168M=1us               
       SysTick->VAL=0x00;         //清空计数器
       SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ; //开始倒数      
       do
       {
              temp=SysTick->CTRL;    //读取控制及状态寄存器的值
 
       }while((temp&0x01)&&!(temp&(1<<16))); //等待时间到达，使能且位16为0（未计到0）
       SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk; //关闭计数器
       SysTick->VAL =0X00;                      //清空计数器
 
}
void ads8860_Init(void)
{
	DIN_Set();
	CONVST_Reset();
}

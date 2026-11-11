#include "spi_util.h"
#include "string.h"
#include "appfun_lib.h"

int32_t CurrentSpecdata[512+256];
int32_t CurrentDSpecdata[512+256];
int32_t CurrentRDSpecdata[512+256];
int32_t CurrentNSpecdata[512+256];
uint8_t CurrentIntSpecdata[1024+512];
MEA_PARAM CurrentMeaParam;
bool StartSpecolAction; //开始光谱采集动作
bool StartDCcolAction;  //开始暗电流采集动作
extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi1;
extern uint8_t spec_Get_Flag;
extern uint8_t readBuffer[20];
bool AssertXeonVol(float XeonVol);
void SPIU_SpecolAction(uint32_t speColAveTimes, uint32_t xenonFlashHp, uint32_t xenonFlashTimes);
void SPIU_DCcolAction(uint32_t speColAveTimes, uint32_t xenonFlashHp, uint32_t xenonFlashTimes);
void SPIU_SpecolAction_256(uint32_t speColAveTimes, uint32_t xenonFlashHp, uint32_t xenonFlashTimes);
void SPIU_DCcolAction_256(uint32_t speColAveTimes, uint32_t xenonFlashHp, uint32_t xenonFlashTimes);
void SPIU_ReadSValuePix(uint32_t curColIndex, uint32_t pixIndex,uint16_t isSpec);

bool AssertXeonVol(float XeonVol)
{
  if((XeonVol < 3.2)||(XeonVol > 4.8))
  {
    return FALSE;
  }
  else
    return TRUE;
}



// 设置电压
bool SPIU_SetLCtrlVol()
{
  uint16_t value=0;
  uint8_t  RefVol[2]={0};
  
  //判定设定参考电压有效性

  if(!AssertXeonVol((float)CurrentMeaParam.xenLampRefVol))
  {
    return FALSE;
  }
  
  //换算成DA的设定值 

   value = (uint16_t)(CurrentMeaParam.xenLampRefVol * 1024 / 4.9);
   RefVol[0]=value<<10&0xff;
   RefVol[1]=value<<2&0xff;


  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET);

  HAL_SPI_Transmit(&hspi2,RefVol,2,0xff);

  SysTick_DelayNsBySoft(200);  

  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET);
  SysTick_DelayNsBySoft(20);

  return TRUE;
}

bool SPIU_CollectionAction(void)
{
  
  uint32_t speColAveTimes;
  float speColIntTimes,xeonFrenquency;
  uint32_t xenonFlashTimes, xenonFlashHp;

  
  //1.清空光谱数据缓存
  memset(CurrentSpecdata,0x0,sizeof(float)*(512+256) );
  memset(CurrentRDSpecdata,0x0,sizeof(float)*(512+256) );

  
  //2.采集参数 
  speColAveTimes = (uint32_t)CurrentMeaParam.speColAveTimes;
  speColIntTimes = (float)CurrentMeaParam.speColIntSpan;
  xeonFrenquency = (float)CurrentMeaParam.xenLampFrequence;
  
  //设定氙灯工作参考电压
//  if(!SPIU_SetLCtrlVol())
//  {
//    
//    return FALSE;
//  }
  
  //参数验证
//  if(!AssertSpeTime(speColIntTimes))
//  {
//    return FALSE;
//  }  
// 
//  if(!AssertXeonFre(xeonFrenquency))
//  {
//     return FALSE;
//  }
  
//  if(!AssertXeonSpe(xeonFrenquency,speColIntTimes))
//  {
//     return FALSE;
//  }
   
  //3.打灯半周期, 计数次数 ms*10000
  xenonFlashHp = (uint32_t)(10000* CurrentMeaParam.xenLampFrequence/8/CurrentMeaParam.speColIntSpan);
  
  //4.打灯次数,积分时间ms,频率是Hz
  xenonFlashTimes = (uint32_t)(CurrentMeaParam.speColIntSpan);
   
  //光谱采集
  if(StartSpecolAction)
  {
    
    StartSpecolAction = FALSE;
    SPIU_SpecolAction(speColAveTimes, xenonFlashHp, xenonFlashTimes);
    
    //SPIU_SpecolAction_256(speColAveTimes, xenonFlashHp, xenonFlashTimes);
    return TRUE;
  } 
  
  //是否需要执行光谱采集动作
  if(StartDCcolAction)
  {
    StartDCcolAction = FALSE;
    //SPIU_DCcolAction(speColAveTimes, xenonFlashHp, xenonFlashTimes);
    
    //SPIU_DCcolAction_256(speColAveTimes, xenonFlashHp, xenonFlashTimes);
    return TRUE;
  }
  
  return FALSE;  
}







void SPIU_SpecolAction(uint32_t speColAveTimes, uint32_t xenonFlashHp, uint32_t xenonFlashTimes)
{
  uint32_t i=0, j=0;
   
  //关闭中断
 //__disable_irq(); 
  //HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET);
   for(i=0; i<=speColAveTimes; i++)
  {
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET);	

  for(i=0; i<=30; i++)
  {
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_RESET);
    SysTick_DelayNsBySoft(80);
       
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_SET);
    SysTick_DelayNsBySoft(80);
  }
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_RESET);

   for(i=0; i<=88; i++)
 {
    
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_RESET);
    SysTick_DelayNsBySoft(80);
       
   HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_SET);
   SysTick_DelayNsBySoft(80); 
  }

      
  for(j=0; j<1024; j++)
  {
 
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_RESET); 
    SysTick_DelayNsBySoft(2);
    if(j<768)
    {
      SPIU_ReadSValuePix(i,j,0);
    
    }
    else
    {
       HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_SET); 
      SysTick_DelayNsBySoft(10);
    }

   //__NOP();
    SysTick_DelayNsBySoft(2);
    
    
  }
      
    

    SysTick_DelayNsBySoft(10);
    
    //最后一次通过AD读取完毕数据后，停止打灯
    if(i == speColAveTimes)
    {
      break;
    } 
    
    SysTick_DelayNsBySoft(500);
    
    j=xenonFlashTimes;
    while(j-- != 0)
    {
      HAL_GPIO_WritePin(GPIOD,GPIO_PIN_10,GPIO_PIN_SET);
      SysTick_DelayNsBySoft(500);
      HAL_GPIO_WritePin(GPIOD,GPIO_PIN_10,GPIO_PIN_RESET);
      SysTick_DelayNsBySoft(30000);
    }
  }

  for(i=0; i<768; i++)
  {
     CurrentNSpecdata[i]=CurrentRDSpecdata[i]-CurrentSpecdata[i];
     CurrentNSpecdata[i] /= (int32_t)speColAveTimes;
  }
  //打开中断
 // __enable_irq();     
}

void SPIU_DCcolAction(uint32_t speColAveTimes, uint32_t xenonFlashHp, uint32_t xenonFlashTimes)
{
  uint32_t i=0, j=0;
  
  
   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET);
  //关闭中断
 __disable_irq(); 
  
  for(i=0; i<=speColAveTimes; i++)
  {

    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET);		
   for(i=0; i<=10; i++)
  {
   
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET);
     SysTick_DelayNsBySoft(60);
        
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET);
     SysTick_DelayNsBySoft(60);
  }
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET);
   for(i=0; i<=89; i++)
  {
    
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET);
    SysTick_DelayNsBySoft(60);
       
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET);
    SysTick_DelayNsBySoft(60); 
  }

      
    for(j=0; j<2048; j++)
    {
      if(j<768)
      {
       SPIU_ReadSValuePix(i, j,0);
      }
       else
      {
        SysTick_DelayNsBySoft(50);
      }   
       HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET);
       SysTick_DelayNsBySoft(10);
       
      if(j<768)
      {
       SPIU_ReadSValuePix(i, j,1);
      }
      else
      {
        SysTick_DelayNsBySoft(50);
      }
       HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET);
       SysTick_DelayNsBySoft(10);
      }
      
    

    SysTick_DelayNsBySoft(10);
    //最后一次通过AD读取完毕数据后，停止打灯
    if(i == speColAveTimes)
    {
      break;
    } 
    
    //SysTick_DelayNsBySoft(10000);
    SysTick_DelayNsBySoft(500);
    
    j=xenonFlashTimes;
    while(j-- != 0)
    {
      //SysTick_DelayNsBySoft(152300);
      SysTick_DelayNsBySoft(30500);

    }
  }

  //CalAve();
  for(i=0; i<768; i++)
  {
    //CurrentDSpecdata[i]=CurrentSpecdata[i]-CurrentRDSpecdata[i];
    CurrentDSpecdata[i]=CurrentRDSpecdata[i]-CurrentSpecdata[i];
//     if(CurrentRDSpecdata[i]>CurrentSpecdata[i])
//    {
//     CurrentDSpecdata[i]=CurrentRDSpecdata[i]-CurrentSpecdata[i];
//    }
//    else
//    {
//      CurrentDSpecdata[i]=CurrentSpecdata[i]-CurrentRDSpecdata[i];
//    }
    CurrentDSpecdata[i] /=(int32_t)speColAveTimes;
  }
  
  //打开中断
  __enable_irq();     
}



void SPIU_SpecolAction_256(uint32_t speColAveTimes, uint32_t xenonFlashHp, uint32_t xenonFlashTimes)
{
  uint32_t i=0, j=0;
   
  //关闭中断
 __disable_irq(); 
     HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
     HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET);
  //CmosST = 1;	(Pulse = 1000ns)
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET);
  
  //CmosCLK = 1;(Pulse = 1000ns)
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET);

  for(i=0; i<=speColAveTimes; i++)
  {

    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET);	
    SysTick_DelayNsBySoft(100);
	
    
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET);
    SysTick_DelayNsBySoft(5);
        
  
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET);
  
    SysTick_DelayNsBySoft(5);    
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET);
    SysTick_DelayNsBySoft(100); 
    for(j=512; j<768; j++)
    {

       SPIU_ReadSValuePix(i, j,0);
          
       HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET);
       SysTick_DelayNsBySoft(10);
       
       SPIU_ReadSValuePix(i, j,1);
       HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET);
       SysTick_DelayNsBySoft(10);
      
    }
    
    //HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_RESET);
    SysTick_DelayNsBySoft(10);
    
    //最后一次通过AD读取完毕数据后，停止打灯
    if(i == speColAveTimes)
    {
      break;
    } 
    
    SysTick_DelayNsBySoft(500);
    
    j=xenonFlashTimes;
    while(j-- != 0)
    {
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
      SysTick_DelayNsBySoft(500);
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
      SysTick_DelayNsBySoft(30000);
    }
  }

  for(i=512; i<768; i++)
  {
     //CurrentNSpecdata[i]=CurrentSpecdata[i]-CurrentRDSpecdata[i];
     CurrentNSpecdata[i]=CurrentRDSpecdata[i]-CurrentSpecdata[i];
     //CurrentNSpecdata[i]=CurrentSpecdata[i];
//   if(CurrentRDSpecdata[i]>CurrentSpecdata[i])
//    {
//     CurrentNSpecdata[i]=CurrentRDSpecdata[i]-CurrentSpecdata[i];
//    }
//    else
//    {
//      CurrentNSpecdata[i]=CurrentSpecdata[i]-CurrentRDSpecdata[i];
//    }
     CurrentNSpecdata[i] /= (int32_t)speColAveTimes;
  }
   //printf("123");
  //打开中断
  __enable_irq();     
}

void SPIU_DCcolAction_256(uint32_t speColAveTimes, uint32_t xenonFlashHp, uint32_t xenonFlashTimes)
{
  uint32_t i=0, j=0;
  
  //关闭中断
 __disable_irq(); 
     HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
     HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET);
  //CmosST = 1;	(Pulse = 1000ns)
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET);
  
  //CmosCLK = 1;(Pulse = 1000ns)
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET);

  
  
   for(i=0; i<=speColAveTimes; i++)
  {

    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET);	
    SysTick_DelayNsBySoft(100);
	
    
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET);
    SysTick_DelayNsBySoft(5);
        
  
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET);
  
    SysTick_DelayNsBySoft(5);    
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET);
    SysTick_DelayNsBySoft(100);
    
    for(j=512; j<768; j++)
    {
     
       SPIU_ReadSValuePix(i, j,0);
          
       HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET);
       SysTick_DelayNsBySoft(10);
       
       SPIU_ReadSValuePix(i, j,1);
       HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET);
       SysTick_DelayNsBySoft(10);
      
    }
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_RESET);
    SysTick_DelayNsBySoft(10);
    //最后一次通过AD读取完毕数据后，停止打灯
    if(i == speColAveTimes)
    {
      break;
    } 
    
    //SysTick_DelayNsBySoft(10000);
    SysTick_DelayNsBySoft(500);
    
    j=xenonFlashTimes;
    while(j-- != 0)
    {
      //SysTick_DelayNsBySoft(152300);
      SysTick_DelayNsBySoft(30500);

    }
  }

  //CalAve();
  for(i=512; i<768; i++)
  {
    //CurrentDSpecdata[i]=CurrentSpecdata[i]-CurrentRDSpecdata[i];
    CurrentDSpecdata[i]=CurrentRDSpecdata[i]-CurrentSpecdata[i];
//     if(CurrentRDSpecdata[i]>CurrentSpecdata[i])
//    {
//     CurrentDSpecdata[i]=CurrentRDSpecdata[i]-CurrentSpecdata[i];
//    }
//    else
//    {
//      CurrentDSpecdata[i]=CurrentSpecdata[i]-CurrentRDSpecdata[i];
//    }
    CurrentDSpecdata[i] /=(int32_t)speColAveTimes;
  }
  
  //打开中断
  __enable_irq();     
}




void SPIU_ReadSValuePix(uint32_t curColIndex, uint32_t pixIndex,uint16_t isSpec)
{
  ADS8860_ReadValue();
  CurrentRDSpecdata[pixIndex]=(float)(readBuffer[0]*256+readBuffer[1]);
}


void SPIU_ReadSValuePix_256(uint32_t curColIndex, uint32_t pixIndex,uint16_t isSpec)
{
  uint16_t data[3] = {0x0, 0x0, 0x0};
  uint8_t val[2]={0};

  SysTick_DelayNsBySoft(50);

  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);

  HAL_SPI_Receive(&hspi1,val,2,0xff);
  data[0]=val[0]*256+val[1];


  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);

  //HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_RESET);
  //第一次采集的数据扔掉，可能是脏数据
 
if(curColIndex>0)
  {   
  
  if(pixIndex==0||pixIndex==1)
  {
    CurrentSpecdata[pixIndex] += 0;
    CurrentRDSpecdata[pixIndex]+=0;
  }
  else if(isSpec==1)
  {
    CurrentSpecdata[pixIndex] += (float)(data[0]);
  }
  else
  {
    CurrentRDSpecdata[pixIndex]+=(float)(data[0]);
  }
 
//    curColIndex++;
  }

}





void SysTick_DelayNsBySoft(uint32_t deleyNs)
{
  __IO uint32_t Delay = deleyNs ;//(SystemCoreClock / 8U / 1000000U)
  //见stm32f1xx_hal_rcc.c -- static void RCC_Delay(uint32_t mdelay)
  do
  {
    __NOP();
  }
  while (Delay --);
}
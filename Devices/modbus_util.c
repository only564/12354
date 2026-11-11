#include "modbus_util.h"
#include "uart_util.h"
#include "spi_util.h"
#include "string.h"
extern UART_HandleTypeDef huart1;
extern bool StartSpecolAction; //开始光谱采集动作
extern bool StartDCcolAction;  //开始暗电流采集动作
extern uint8_t spec_Get_Flag;
void MDBU_GetReceiveFrm(UART_HandleTypeDef* huart, PDATARECEIVE pDataRev, PFRAMESEND pFrmSend);
void Data_Transaction(uint8_t* pData,uint8_t len);
void MDBU_SendSpecData(UART_HandleTypeDef* huart);
void MDBU_Receiver(void)
{  
  MDBU_GetReceiveFrm(&huart1, &USART1_DataRev, &USART1_FrmSnd);  
}




void MDBU_GetReceiveFrm(UART_HandleTypeDef* huart, PDATARECEIVE pDataRev, PFRAMESEND pFrmSend)
{
  uint8_t data[5]={0};
  uint32_t len;
  uint8_t  i;
  if(pDataRev->isRevFrmFinish==TRUE)
  {
   // printf("1");
    for(i=0;i<pDataRev->haveRevBytLen;i++)
    {
       data[i]=pDataRev->frameBuffer[i];
       len++;
    }
     pDataRev->haveRevBytLen=0;
     spec_Get_Flag=1;
//     Data_Transaction(data,len);
//     MDBU_SendSpecData(huart);
     pDataRev->isRevFrmFinish=FALSE;
     //MDBU_CmdTranHandler();
    // NVIC_RESETPRIMASK();   
  }

 
}



void MDBU_SendSpecData(UART_HandleTypeDef* huart)
{
      HAL_UART_Transmit(huart,CurrentIntSpecdata,1024,0xff);
}


void get_Data()
{
  
}


void Data_Transaction(uint8_t* pData,uint8_t len)
{
    uint8_t data[1536]={0};
    CurrentMeaParam.xenLampRefVol=pData[2];
    CurrentMeaParam.speColAveTimes=pData[0];
    CurrentMeaParam.xenLampFrequence=1000/pData[0];
    CurrentMeaParam.speColIntSpan=pData[3];
   //FLAU_SaveMeaParam();
    memset(CurrentSpecdata,0,2048+1024);
    memset(CurrentNSpecdata,0,2048+1024);
    memset(CurrentDSpecdata,0,2048+1024);
    memset(CurrentIntSpecdata,0,1024+512);
//    if(pData[1]==1)
//    {
//     StartDCcolAction=TRUE;
//     //StartSpecolAction=TRUE;
//     SPIU_CollectionAction();
//    }
    StartSpecolAction=TRUE;
    SPIU_CollectionAction();
    
   for(uint16_t i=0;i<768;i++)
   {
     int32_t middleVar;
      
      middleVar= CurrentNSpecdata[i]-CurrentDSpecdata[i];
      //middleVar= (int)CurrentNSpecdata[i];
      middleVar=middleVar< 0 ? 1 : middleVar;
      
//      CurrentIntSpecdata[i*2]=(uint8_t)(middleVar>>8&0xff);
//      CurrentIntSpecdata[i*2+1]=(uint8_t)(middleVar&0xff);
       data[i*2]=(uint8_t)(middleVar>>8&0xff);
       data[i*2+1]=(uint8_t)(middleVar&0xff);
   }
   
   for(uint16_t i=0;i<70*2;i++)
   {
     CurrentIntSpecdata[i]= data[i+200];
   }
    for(uint16_t i=0;i<30*2;i++)
   {
     CurrentIntSpecdata[i+70*2]= data[i+220*2];
   }
    for(uint16_t i=0;i<156*2;i++)
   {
     CurrentIntSpecdata[i+100*2]= data[i+356*2];
   }
     for(uint16_t i=0;i<256*2;i++)
   {
     CurrentIntSpecdata[i+256*2]= data[i+512*2];
   }
}
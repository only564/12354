#include "uart_util.h"


DATARECEIVE USART1_DataRev;

FRAMESEND  USART1_FrmSnd;
enum states revState=state;
uint8_t  revNum=0;
void USARTx_IRQHandler_RXNE(uint8_t UARTRx_Type,PDATARECEIVE pFrmRev)
{ 
 if(revState==state2)
 {
   if(revNum<4)
   {
     pFrmRev->frameBuffer[revNum++]=UARTRx_Type;
     pFrmRev->haveRevBytLen++;
   }
   else
   {
     if(0<pFrmRev->frameBuffer[0]&&pFrmRev->frameBuffer[0]<12&&pFrmRev->frameBuffer[3]==1&&0x1F<pFrmRev->frameBuffer[2]&&pFrmRev->frameBuffer[2]<0x29)
    {
      pFrmRev->isRevFrmFinish=TRUE;
    }
     revState=state;
     revNum=0;
   }
 }
 else
 {
  switch (UARTRx_Type)
  {
  case 0x0A:
    {
      if( revState==state)
      {
        revState=state1;
      }
      else
      {
        revState=state;
      }
      break;
        
    }
    case 0x01:
    {
      if( revState==state1)
      {
       revState=state2;
      }
      else
      {
        revState=state;
      }

      break;
        
    }
  default:
    {
       revState=state;
       break;
    }
  }
 }
    
}
  
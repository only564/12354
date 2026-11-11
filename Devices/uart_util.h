
#ifndef _UARTUtil_H_
#define _UARTUtil_H_

#include "spi_util.h"


#define FRAME_MAX_LEN 100

typedef struct 
{
    bool	isSendRdy;	  //允许发送标志
    uint16_t		frameBytLen;	  //发送帧长度
    uint16_t		haveSndBytLen;	  //已经发送字节
    uint8_t		frameBuffer[FRAME_MAX_LEN]; //发送缓冲
  	
}FRAMESEND,*PFRAMESEND;


typedef struct 
{
  
    bool  isRevFrmFinish;   //接收帧完成
    uint16_t  haveRevBytLen;    //已经接收的数据长度
    uint8_t	  frameBuffer[5]; //接收缓冲
  	
}DATARECEIVE,*PDATARECEIVE;

enum  states
{
  state,
  state1,
  state2,
  state3,
  
};


extern DATARECEIVE USART1_DataRev;

extern FRAMESEND  USART1_FrmSnd;  //USART1的发送缓存
void USARTx_IRQHandler_RXNE(uint8_t UARTRx_Type,PDATARECEIVE pFrmRev);


  
#endif
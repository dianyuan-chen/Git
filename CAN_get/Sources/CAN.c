#include <hidef.h>
#include "derivative.h"
#include "CAN.h"

void INIT_CAN0(void) {
  if (CAN0CTL0_INITRQ == 0)
    CAN0CTL0_INITRQ = 1;
  while (CAN0CTL1_INITAK == 0);
  CAN0BTR0_SJW = 0;
  CAN0BTR0_BRP = 7;
  CAN0BTR1 = 0x1c;
  
  CAN0IDMR0 = 0xFF;
  CAN0IDMR1 = 0xFF;
  CAN0IDMR2 = 0xFF;
  CAN0IDMR3 = 0xFF;
  CAN0IDMR4 = 0xFF;
  CAN0IDMR5 = 0xFF;
  CAN0IDMR6 = 0xFF;
  CAN0IDMR7 = 0xFF;
  
  CAN0CTL1 = 0xC0;
  CAN0CTL0 = 0x00;
  while(CAN0CTL1_INITAK);
  while(CAN0CTL0_SYNCH == 0);
  CAN0RIER_RXFIE = 1;
}

Bool MSCAN0SendMsg(struct CAN_Msg msg){ //CAN0发送函数      
  unsigned char  send_buf,sp;
  if (msg.len > 8)
    return FALSE;            //检查数据长度
  if (CAN0CTL0_SYNCH==0)       //检查总线时钟是否同步
    return FALSE;
  send_buf = 0;
  do {
    CAN0TBSEL = CAN0TFLG;     //选择缓冲器，最低置1位
    send_buf = CAN0TBSEL;
  } while(!send_buf);          //寻找空间的缓冲器
  
  CAN0TXIDR0 = (unsigned char)(msg.id>>3);
  CAN0TXIDR1 = (unsigned char)(msg.id<<5);  //写入标识符  
  if (msg.RTR)                 //判断是否为远程帧
    CAN0TXIDR1 |= 0x10;
  for (sp = 0; sp < msg.len; sp++)
    *((&CAN0TXDSR0)+sp) = msg.data[sp];   // 写入数据  

  CAN0TXDLR = msg.len;        // 写入数据长度
  //CAN0TXTBPR = msg.prty;   // 写入优先级
  CAN0TFLG = send_buf;     // 清 TXx 标志 (缓冲器准备发送)

  return (TRUE);
}         

Bool MSCAN0GetMsg(struct Can_Msg *msg) {  //CAN0接受函数
  unsigned char sp;
  if (!(CAN0RFLG_RXF))       //检测接收标志 0 没有收到新消息，1 有新消息
    return FALSE;
  if (CAN0RXIDR1_IDE)        //检测CAN协议报文模式（0标准帧/1扩展帧） 
    return FALSE;
  //将相应寄存器数据移位放入id
  msg->id = (unsigned int)(CAN0RXIDR0<<3) | (unsigned char)(CAN0RXIDR1>>5);
  if (CAN0RXIDR1 & 0x10)     //判断是否为远程帧
    msg->RTR = TRUE;
  else
    msg->len = FALSE;       //读取标识符 0数据帧 1远程帧
  msg->len = CAN0RXDLR;     //读取数据长度
  for (sp = 0;sp < msg->len;sp++)
    msg->data[sp] = *((&CAN0RXDSR0)+sp);//读取数据
  CAN0RFLG = 0x01;
  return TRUE;
}

  
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

Bool MSCAN0SendMsg(struct CAN_Msg msg){ //CAN0���ͺ���      
  unsigned char  send_buf,sp;
  if (msg.len > 8)
    return FALSE;            //������ݳ���
  if (CAN0CTL0_SYNCH==0)       //�������ʱ���Ƿ�ͬ��
    return FALSE;
  send_buf = 0;
  do {
    CAN0TBSEL = CAN0TFLG;     //ѡ�񻺳����������1λ
    send_buf = CAN0TBSEL;
  } while(!send_buf);          //Ѱ�ҿռ�Ļ�����
  
  CAN0TXIDR0 = (unsigned char)(msg.id>>3);
  CAN0TXIDR1 = (unsigned char)(msg.id<<5);  //д���ʶ��  
  if (msg.RTR)                 //�ж��Ƿ�ΪԶ��֡
    CAN0TXIDR1 |= 0x10;
  for (sp = 0; sp < msg.len; sp++)
    *((&CAN0TXDSR0)+sp) = msg.data[sp];   // д������  

  CAN0TXDLR = msg.len;        // д�����ݳ���
  //CAN0TXTBPR = msg.prty;   // д�����ȼ�
  CAN0TFLG = send_buf;     // �� TXx ��־ (������׼������)

  return (TRUE);
}         

Bool MSCAN0GetMsg(struct Can_Msg *msg) {  //CAN0���ܺ���
  unsigned char sp;
  if (!(CAN0RFLG_RXF))       //�����ձ�־ 0 û���յ�����Ϣ��1 ������Ϣ
    return FALSE;
  if (CAN0RXIDR1_IDE)        //���CANЭ�鱨��ģʽ��0��׼֡/1��չ֡�� 
    return FALSE;
  //����Ӧ�Ĵ���������λ����id
  msg->id = (unsigned int)(CAN0RXIDR0<<3) | (unsigned char)(CAN0RXIDR1>>5);
  if (CAN0RXIDR1 & 0x10)     //�ж��Ƿ�ΪԶ��֡
    msg->RTR = TRUE;
  else
    msg->len = FALSE;       //��ȡ��ʶ�� 0����֡ 1Զ��֡
  msg->len = CAN0RXDLR;     //��ȡ���ݳ���
  for (sp = 0;sp < msg->len;sp++)
    msg->data[sp] = *((&CAN0RXDSR0)+sp);//��ȡ����
  CAN0RFLG = 0x01;
  return TRUE;
}

  
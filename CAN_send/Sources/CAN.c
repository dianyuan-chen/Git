#include <hidef.h>
#include "derivative.h"
#include "CAN.h"

void INIT_CAN0(void) {
  if(CAN0CTL0_INITRQ == 0)  //��ѯ�Ƿ�����ʼ��
    CAN0CTL0_INITRQ = 1; //�����ʼ��״̬
  while (CAN0CTL1_INITAK == 0); //��ʼ�����ֱ�־����ʼ��ģ��ʹ��(ȷ��)
  
  CAN0BTR0_SJW = 0; //����ͬ����ת���
  CAN0BTR0_BRP = 7; //���ñ�����Ԥ��Ƶ��
  CAN0BTR1 = 0x1c;  //����λʱ��
  
  CAN0IDMR0 = 0xFF;
  CAN0IDMR1 = 0xFF;
  CAN0IDMR2 = 0xFF;
  CAN0IDMR3 = 0xFF;
  CAN0IDMR4 = 0xFF;
  CAN0IDMR5 = 0xFF;
  CAN0IDMR6 = 0xFF;
  CAN0IDMR7 = 0xFF; //�ر��˲���
  
  CAN0CTL1 = 0xC0;  //ʹ��MSCANģ�飬ʹ������ʱ��Դ
  CAN0CTL0 = 0x00;  //����һ������ģʽ
  while (CAN0CTL1_INITAK);  //�ȴ��ص�һ������ģʽ
  while (CAN0CTL0_SYNCH == 0);  //�ȴ�����ʱ��ͬ��
  CAN0RIER_RXFIE = 0; //��ֹ�����ж�
}

Bool MSCAN0SendMsg(struct Can_Msg msg) {  //CAN0���ͺ���
  unsigned char send_buf,sp;
  if (msg.len > 8)
    return FALSE;
  if(CAN0CTL0_SYNCH == 0)
    return FALSE;
  send_buf = 0;  
  do {
    CAN0TBSEL = CAN0TFLG; //ѡ�񻺳����������1λ
    send_buf = CAN0TBSEL; 
  } while (!send_buf);  //Ѱ�ҿ��еĻ�����
  CAN0TXIDR0 = (unsigned char)(msg.id >> 3);
  CAN0TXIDR1 = (unsigned char)(msg.id << 5);  //д���ʶ��
  if(msg.RTR)
    CAN0TXIDR1 |= 0x10; //Զ��֡
  for(sp = 0; sp < msg.len; sp++)
    *((&CAN0TXDSR0)+sp) = msg.data[sp]; //д������
  CAN0TXDLR = msg.len;  //д�����ݳ���
  CAN0TFLG = send_buf;  //��TXx��־��������׼�����ͣ�
  return TRUE;
}

#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "MC9S12XEP100.h"

#define LED PORTC_PC4
#define LED_dir DDRC_DDRC4
#define ID 0x0001
#define data_len 3
unsigned char a;
unsigned char senddata[3] = {'Z','A','C'};

struct CAN_Msg {             //������ձ��ĵĽṹ��
  unsigned int id;
  Bool RTR;                 //Զ�̷������� 0����֡ 1Զ��֡
  unsigned char data[8];
  unsigned char len;
};


struct CAN_Msg msg_send;     //����ṹ�����

void INIT_PLL(void) {        //��ʼ�����໷
  CLKSEL_PLLSEL = 0;        //�ڲ�����ʱ����Դ�ھ���
  //PLLCTL_PLLON = 0;         //�ر�PLL
  //SYNR = 0x47;
  //REFDV = 0x41;
  //POSTDIV = 0x00;           //PLL:64M;BUS:32M
  //PLLCTL_PLLON = 1;         //��PLL
  
  //_asm(nop);
  //_asm(nop);                //�ȴ���������������
  //while (CRGFLG_LOCK == 0);  //�ȴ�PLL�ȶ�
  //CLKSEL_PLLSEL = 1;        //ѡ��PLL��Ϊʱ��Դ
}

void INIT_PIT0(void) {
  PITCFLMT_PITE = 0;  //�ر�PIT PITTF��־λ����
  PITCE_PCE0 = 1; //PCE0ͨ��ʹ��
  PITMUX = 0x00;  //ͨ��0��1��2��3ʹ��΢��ʱ����׼0����
  PITMTLD0 = 100 - 1;
  PITLD0 = 40000 - 1;//T = (PITMTLD+1)*(PITLD + 1)/Fbus = 1000ms
  PITINTE_PINTE0 = 1;//ʹ��PIT
  PITCFLMT_PITE = 1;
}

void INIT_CAN0(void) {
  if (CAN0CTL0_INITRQ == 0)  //��ѯ�Ƿ�����ʼ��
    CAN0CTL0_INITRQ = 1;    //�����ʼ��״̬
  while (CAN0CTL1_INITAK == 0);//��ʼ�����ֱ�־,��ʼ��ģʽʹ��(ȷ�ϣ� 
  CAN0BTR0_SJW = 0;         //ͬ����ת��ȣ�1Tqʱ������
  CAN0BTR0_BRP  =0;         //������Ԥ��Ƶ����0
  CAN0BTR1 = 0x1c;          //TSEG1:13Tq,TSEG2:2Tq 
                            //λʱ��=������Ԥ��Ƶ��*(1+TSEG1+TSEG2)/CANCLK=1
                            //������=1/λʱ�� = 250kb/s
  CAN0IDMR0 = 0xFF;
  CAN0IDMR1 = 0xFF;
  CAN0IDMR2 = 0xFF;
  CAN0IDMR3 = 0xFF;
  CAN0IDMR4 = 0xFF;
  CAN0IDMR5 = 0xFF;
  CAN0IDMR6 = 0xFF;
  CAN0IDMR7 = 0xFF;          // �ر��˲���
  
  CAN0CTL1 = 0xC0;           //ʹ��MSCANģ�飬ʹ������ʱ��Դ
  CAN0CTL0 = 0x00;           //����һ��ģʽ����
  while (CAN0CTL1_INITAK);    //�ȴ��ص�һ������ģʽ
  while (CAN0CTL0_SYNCH == 0);//�ȴ�ӿ����ʱ��ͬ��
  CAN0RIER_RXFIE = 1;        //�򿪽����ж�
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
  CAN0TFLG = send_buf;     // �� TXx ��־ (������׼������)

  return (TRUE);
}

#pragma CODE_SEG __NEAR_SEG NON_BANKED
void interrupt VectorNumber_Vpit0 PIT0(void) {

  MSCAN0SendMsg(msg_send);
  PITTF_PTF0 = 1;  
}

#pragma CODE_SEG DEFAULT

void main(void) {
  DisableInterrupts;
  LED_dir = 1;
  LED = 1;
  INIT_PLL();

  INIT_CAN0();
  
  msg_send.id = ID;
  for(a = 0; a < data_len; a++) {
   msg_send.data[a]  = senddata[a];  
  }
  msg_send.len = data_len;
  msg_send.RTR = FALSE;
  
  INIT_PIT0();

	EnableInterrupts;


  for(;;) {}

}

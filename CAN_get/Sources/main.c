#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "CAN.h"

#define LED PORTC_PC4
#define LED_dir DDRC_DDRC4

struct Can_Msg msg_get;
 
void INIT_PLL(void) {        //��ʼ�����໷
  CLKSEL_PLLSEL = 0;        //�ڲ�����ʱ����Դ�ھ���
  PLLCTL_PLLON = 0;         //�ر�PLL
  SYNR = 0x47;
  REFDV = 0x41;
  POSTDIV = 0x00;           //PLL:64M;BUS:32M
  PLLCTL_PLLON = 1;         //��PLL
  
  _asm(nop);
  _asm(nop);                //�ȴ���������������
  while (CRGFLG_LOCK == 0);  //�ȴ�PLL�ȶ�
  CLKSEL_PLLSEL = 1;        //ѡ��PLL��Ϊʱ��Դ
}

#pragma CODE_SEG __NEAR_SEG NON_BANKED  //�жϽ��պ���(���������ڷǷ�ҳ��)
                                        //NON_BANKED�ǵ�Ƭ������ֱ��Ѱַ������
                                        // __NEAR_SEG���߱������������ڹ̶�ҳ�У��Է�������ҳ����

void interrupt VectorNumber_Vcan0rx CAN_receive(void) {  //VectorNumber_Vcan0rx��MC9S12XEP100.h��
  if (MSCAN0GetMsg(&msg_get)) {           //�ж��Ƿ��кϷ��ı�׼֡�յ�
    ;
  } else {                                //����
    for(;;);
  }
  if (!MSCAN0SendMsg(msg_get))
    for(;;);
}

#pragma CODE_SEG DEFAULT    //������������Ĭ�����򣨷�ҳ����
void main(void) {
  DisableInterrupts;
  INIT_PLL();
  LED_dir = 1;
  LED = 1;
  INIT_CAN0();
  EnableInterrupts;
  
  for(;;) {}
}

#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */

#include "CAN.h"

#define LED PORTC_PC4
#define LED_dir DDRC_DDRC4
#define ID 0x0001
#define data_len 3
struct Can_Msg msg_send;
unsigned char a;
unsigned char senddata[3] = {'Z','A','C'};

void INIT_PLL(void) { //��ʼ�����໷
  CLKSEL_PLLSEL = 0;  //�ڲ�����ʱ����Դ�ھ���
  PLLCTL_PLLON = 0; //�ر�PLL
  SYNR = 0x47;
  REFDV = 0x41;  //PLL:64Mhz;BUS:32Mhz
  PLLCTL_PLLON = 1;  //��PLL
  _asm(nop);
  _asm(nop);//�ȴ�������������
  while (CRGFLG_LOCK == 0);//�ȴ�PLL�ȶ�
  CLKSEL_PLLSEL = 1;  //ѡ��PLL��Ϊʱ��Դ
}


void main(void) {
  DisableInterrupts;  //�ر������ж�
  INIT_PLL();
  LED_dir = 1;
  LED = 1;
  INIT_CAN0();
  EnableInterrupts;
  
  msg_send.id = ID;
  for(a = 0; a < data_len; a++) {
    msg_send.data[a]  = senddata[a];  
  }
  msg_send.len = data_len;
  msg_send.RTR = FALSE;
  
  for(;;) {
    if(!MSCAN0SendMsg(msg_send)) {
      for(;;);
    }
  }
}

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

void INIT_PLL(void) { //初始化锁相环
  CLKSEL_PLLSEL = 0;  //内部总线时钟来源于晶振
  PLLCTL_PLLON = 0; //关闭PLL
  SYNR = 0x47;
  REFDV = 0x41;  //PLL:64Mhz;BUS:32Mhz
  PLLCTL_PLLON = 1;  //打开PLL
  _asm(nop);
  _asm(nop);//等待两个机器周期
  while (CRGFLG_LOCK == 0);//等待PLL稳定
  CLKSEL_PLLSEL = 1;  //选择PLL作为时钟源
}


void main(void) {
  DisableInterrupts;  //关闭所有中断
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

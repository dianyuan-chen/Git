#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "CAN.h"

#define LED PORTC_PC4
#define LED_dir DDRC_DDRC4

struct Can_Msg msg_get;
 
void INIT_PLL(void) {        //初始化锁相环
  CLKSEL_PLLSEL = 0;        //内部总线时钟来源于晶振
  PLLCTL_PLLON = 0;         //关闭PLL
  SYNR = 0x47;
  REFDV = 0x41;
  POSTDIV = 0x00;           //PLL:64M;BUS:32M
  PLLCTL_PLLON = 1;         //打开PLL
  
  _asm(nop);
  _asm(nop);                //等待连两个机器周期
  while (CRGFLG_LOCK == 0);  //等待PLL稳定
  CLKSEL_PLLSEL = 1;        //选择PLL作为时钟源
}

#pragma CODE_SEG __NEAR_SEG NON_BANKED  //中断接收函数(将函数置于非分页区)
                                        //NON_BANKED是单片机可以直接寻址的区域
                                        // __NEAR_SEG告诉编译器函数放在固定页中，以访问其他页函数

void interrupt VectorNumber_Vcan0rx CAN_receive(void) {  //VectorNumber_Vcan0rx在MC9S12XEP100.h中
  if (MSCAN0GetMsg(&msg_get)) {           //判断是否有合法的标准帧收到
    ;
  } else {                                //错误
    for(;;);
  }
  if (!MSCAN0SendMsg(msg_get))
    for(;;);
}

#pragma CODE_SEG DEFAULT    //后续代码置于默认区域（分页区）
void main(void) {
  DisableInterrupts;
  INIT_PLL();
  LED_dir = 1;
  LED = 1;
  INIT_CAN0();
  EnableInterrupts;
  
  for(;;) {}
}

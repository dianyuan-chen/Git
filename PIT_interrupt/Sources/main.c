#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "MC9S12XEP100.h"

#define LED PORTC_PC4
#define LED_dir DDRC_DDRC4
#define ID 0x0001
#define data_len 3
unsigned char a;
unsigned char senddata[3] = {'Z','A','C'};

struct CAN_Msg {             //定义接收报文的结构体
  unsigned int id;
  Bool RTR;                 //远程发送请求 0数据帧 1远程帧
  unsigned char data[8];
  unsigned char len;
};


struct CAN_Msg msg_send;     //定义结构体变量

void INIT_PLL(void) {        //初始化锁相环
  CLKSEL_PLLSEL = 0;        //内部总线时钟来源于晶振
  //PLLCTL_PLLON = 0;         //关闭PLL
  //SYNR = 0x47;
  //REFDV = 0x41;
  //POSTDIV = 0x00;           //PLL:64M;BUS:32M
  //PLLCTL_PLLON = 1;         //打开PLL
  
  //_asm(nop);
  //_asm(nop);                //等待连两个机器周期
  //while (CRGFLG_LOCK == 0);  //等待PLL稳定
  //CLKSEL_PLLSEL = 1;        //选择PLL作为时钟源
}

void INIT_PIT0(void) {
  PITCFLMT_PITE = 0;  //关闭PIT PITTF标志位清零
  PITCE_PCE0 = 1; //PCE0通道使能
  PITMUX = 0x00;  //通道0、1、2、3使用微定时器基准0计数
  PITMTLD0 = 100 - 1;
  PITLD0 = 40000 - 1;//T = (PITMTLD+1)*(PITLD + 1)/Fbus = 1000ms
  PITINTE_PINTE0 = 1;//使能PIT
  PITCFLMT_PITE = 1;
}

void INIT_CAN0(void) {
  if (CAN0CTL0_INITRQ == 0)  //查询是否进入初始化
    CAN0CTL0_INITRQ = 1;    //进入初始化状态
  while (CAN0CTL1_INITAK == 0);//初始化握手标志,初始化模式使能(确认） 
  CAN0BTR0_SJW = 0;         //同步跳转宽度：1Tq时钟周期
  CAN0BTR0_BRP  =0;         //比特率预分频器：0
  CAN0BTR1 = 0x1c;          //TSEG1:13Tq,TSEG2:2Tq 
                            //位时间=比特率预分频器*(1+TSEG1+TSEG2)/CANCLK=1
                            //波特率=1/位时间 = 250kb/s
  CAN0IDMR0 = 0xFF;
  CAN0IDMR1 = 0xFF;
  CAN0IDMR2 = 0xFF;
  CAN0IDMR3 = 0xFF;
  CAN0IDMR4 = 0xFF;
  CAN0IDMR5 = 0xFF;
  CAN0IDMR6 = 0xFF;
  CAN0IDMR7 = 0xFF;          // 关闭滤波器
  
  CAN0CTL1 = 0xC0;           //使能MSCAN模块，使用总线时钟源
  CAN0CTL0 = 0x00;           //返回一般模式运行
  while (CAN0CTL1_INITAK);    //等待回到一般运行模式
  while (CAN0CTL0_SYNCH == 0);//等待涌总线时钟同步
  CAN0RIER_RXFIE = 1;        //打开接受中断
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
  CAN0TFLG = send_buf;     // 清 TXx 标志 (缓冲器准备发送)

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

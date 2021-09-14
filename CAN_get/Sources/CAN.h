struct Can_Msg {             //定义接收报文的结构体
  unsigned int id;
  Bool RTR;                 //远程发送请求 0数据帧 1远程帧
  unsigned char data[8];
  unsigned char len;
};

void INIT_CAN0(void); 

Bool MSCAN0SendMsg (struct Can_Msg msg);
Bool MSCAN0GetMsg (struct Can_Msg *msg);
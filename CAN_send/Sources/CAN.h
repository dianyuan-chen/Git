struct Can_Msg {  //���屨�Ľṹ��
  unsigned int id;
  Bool RTR; //Զ�̷������� 0����֡ 1Զ��֡
  unsigned char data[8];
  unsigned char len;
};
void INIT_CAN0(void);
Bool MSCAN0SendMsg(struct Can_Msg msg);
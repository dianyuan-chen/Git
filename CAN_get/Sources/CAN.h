struct Can_Msg {             //������ձ��ĵĽṹ��
  unsigned int id;
  Bool RTR;                 //Զ�̷������� 0����֡ 1Զ��֡
  unsigned char data[8];
  unsigned char len;
};

void INIT_CAN0(void); 

Bool MSCAN0SendMsg (struct Can_Msg msg);
Bool MSCAN0GetMsg (struct Can_Msg *msg);
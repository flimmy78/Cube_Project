#include "ProtocolHandle.h"
#include <stdio.h>
#include "ComBuff.h"
void HeartBeat_P_Handle(Protocol_Info_T* pi){
//	HEARTBEAT_PROTOCOL_T hp = pi->protocol.HeartBeat_P;
	 
	//Protocol_Send_Transpond(pi);
	HEARTBEAT_PROTOCOL_T* Heartbeat_P = pi->ParameterList;
	printf("�յ�����:%d\r\n", Heartbeat_P->para1);
	
//	Uart1_DMA_Sender.WriteByte(&Uart1_DMA_Sender,'C');
}

void Run_P_Handle(Protocol_Info_T* pi){
	//RUN_PROTOCOL_T rp = pi->protocol.Run_P;
//	printf("�յ��˶�ָ��\r\n");
}

void Rotate_P_Handle(Protocol_Info_T* pi){
	//ROTATE_PROTOCOL_T rop = pi->protocol.Rotate_P;
//	printf("�յ�ƫ����ָ��\r\n");
}

 

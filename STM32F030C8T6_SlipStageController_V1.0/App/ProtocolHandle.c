#include "ProtocolHandle.h"
#include <stdio.h>
#include "ComBuff.h"
#include "StepMotor.h"
 
void HeartBeat_P_Handle(Protocol_Info_T* pi){ 
	HEARTBEAT_PROTOCOL_T* Heartbeat_P = pi->ParameterList;
	printf("�յ�����:%d\r\n", Heartbeat_P->para1); 
}

//��ָ������ת��
void Cmd_P_Handle(Protocol_Info_T* pi){ 
	CMD_PROTOCOL_T* Cmd_P = pi->ParameterList;
	printf("�յ�ָ��:\r\n");
	uint16_t integer  = Cmd_P->para2 << 8 | Cmd_P->para3;//����
	uint16_t decimals = Cmd_P->para4 << 8 | Cmd_P->para5;//С��
	float offset = decimals;
	while(1){
		offset = offset / 10.0;
		if((int16_t)offset % 10 == 0)
			break;
	}
	offset = integer + offset;
	printf("����:%d, �˶�ƫ������%f,�ٶȣ�%d\r\n",Cmd_P->para1, offset, Cmd_P->para6);
	stepMotor.Run_Offset(Cmd_P->para1, offset, Cmd_P->para6);
}
//��ָ���Ƕ�ת��
void Cmd_A_P_Handle(Protocol_Info_T* pi){ 
	CMD_A_PROTOCOL_T* Cmd_P = pi->ParameterList;
	printf("�յ�ָ��:\r\n");
	uint16_t integer  = Cmd_P->para2 << 8 | Cmd_P->para3;//����
	uint16_t decimals = Cmd_P->para4 << 8 | Cmd_P->para5;//С��
	float offset = decimals;
	while(1){
		offset = offset / 10.0;
		if((int16_t)offset % 10 == 0)
			break;
	}
	offset = integer + offset;
	printf("�˶��Ƕȣ�%f,�ٶȣ�%d\r\n", offset, Cmd_P->para6);
	stepMotor.Run_Angle(Cmd_P->para1, offset, Cmd_P->para6);
}

#include "ProtocolHandle.h"
#include <stdio.h>
#include "ComBuff.h" 
#include "MotorControl.h"
#include "RudderControl.h" 
//#include <iar_dlmalloc.h>


void Heap_Use(Protocol_Info_T* pi){
  //__iar_dlmalloc_stats();
	printf("��ע��\r\n");
}
 
void HeartBeat_P_Handle(Protocol_Info_T* pi){
	HeartBeat_P_T* hp = pi->ParameterList;
	printf("�յ�����:%d\r\n", hp->para1); 
//	Uart1_DMA_Sender.WriteByte(&Uart1_DMA_Sender,'C');
}

void Run_Protocol_Handle(Protocol_Info_T* pi){
	Run_P_T* rp = pi->ParameterList;
	uint16_t l_speed, r_speed;
	MOTOR_DIR l_dir, r_dir;
	printf("\r\nִ���˶�ָ��\r\n");
	motor_L->Exec_Protocol = motor_R->Exec_Protocol = *pi;
	Heart_Flag = 1;//������λ
	//�ٶȻ�ȡ
	l_speed = rp->para2 << 8 | rp->para3;
	r_speed = rp->para5 << 8 | rp->para6;
	if(l_speed > 0 && l_speed < 15)//�ٶ����15
		l_speed = 15;
	if(r_speed > 0 && r_speed < 15)
		r_speed = 15;
	//�����ȡ
	l_dir = (MOTOR_DIR)rp->para1;
	r_dir = (MOTOR_DIR)rp->para4;
	
	//�ظ�ָ�����
	if(!(motor_L->DirCMD == l_dir && motor_L->SpeedCMD == l_speed))
		motor_L->Motor_Run(motor_L, l_dir, l_speed);
	if(!(motor_R->DirCMD == r_dir && motor_R->SpeedCMD == r_speed))
		motor_R->Motor_Run(motor_R, r_dir, r_speed);
	
	printf("���ַ���%d, �ٶȣ�%d\t���ַ���%d, �ٶȣ�%d\r\n", rp->para1, rp->para2 << 8 | rp->para3, rp->para4, rp->para5 << 8 | rp->para6 );
}

void Rudder_Protocol_Handle(Protocol_Info_T* pi){
	Rudder_P_T* rp = pi->ParameterList;
	RudderX->Angle_CMD = rp->X_Angle;
	RudderY->Angle_CMD = rp->Y_Angle;
	printf("������ƣ�%d, %d\r\n", (uint8_t)rp->X_Angle, (uint8_t)rp->Y_Angle);
}

 

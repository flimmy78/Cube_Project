#include "ProtocolHandle.h" 
#include "ComBuff.h" 
//#include <iar_dlmalloc.h>
#include "MotorWingDriver.h"
#include "MotorHeadDriver.h"
#include <stdlib.h>
#include <string.h>

void Heap_Use(Protocol_Info_T* pi){
//  __iar_dlmalloc_stats();
}
 
//###################################Э��У�麯����###################################

int8_t self_check(Protocol_Info_T* pi){
	if(System_Mode == Normal)
		return 1;
	else 
		return -1;
}

uint8_t CheckPower(void);

void ask_send(uint8_t seq){
  ASK_PROTOCOL_T ask_p = {0};
  ask_p.para1 = seq;
  Protocol_Send(ASK_PROTOCOL ,&ask_p, sizeof(ASK_PROTOCOL_T));
}
/****************************************************
	������:	angle_protocol_check
	����:		˫�ֽǶȿ��Ƶ��Э��У��
	����:		ANGLE_PROTOCOL_TЭ��ʵ��ָ��
	����ֵ:	
	����:		liyao 2015��9��8��14:10:51
****************************************************/
int8_t angle_protocol_check(Protocol_Info_T* pi){
	ANGLE_PROTOCOL_T* ap = pi->ParameterList;
	if(!(ap->para16 <=1))//�Ƿ���ָ��У��
		return -1;
	return 0;
}

/****************************************************
	������:	wing_protocol_check
	����:		�����Э��У��
	����:		WING_PROTOCOL_TЭ��ʵ��ָ��
	����ֵ:	-1������ٶȲ������� -2���ҳ��ٶȲ������� -3���Ƿ���ָ���������
	����:		liyao 2015��9��8��14:10:51
****************************************************/
int8_t wing_protocol_check(Protocol_Info_T* pi){
	WING_PROTOCOL_T* wp = pi->ParameterList;
	if(!(wp->para2 >=0 && wp->para2 <=100))//����ٶ�У��
		return -1;
	if(!(wp->para4 >=0 && wp->para4 <=100))//�ҳ��ٶ�У��
		return -2;
	if(!(wp->para6 >= 0 && wp->para6 <=1))//�Ƿ���ָ��У��
		return -3;
	return 0;
}
/****************************************************
	������:	Run_Protocol_Handle
	����:		˫�ֿ�������Э�麯��
	����:		liyao 2015��10��16��15:21:46
****************************************************/
void Run_Protocol_Handle(Protocol_Info_T* pi){
	RUN_PROTOCOL_T* rp = pi->ParameterList;
	#ifdef PRINT_ERR 
			printf("�յ��˶�ָ��\r\n");
	#endif
	#ifndef PRINT_ERR 
	ask_send(rp->para7);
	#endif
	if(rp->para8 == 1){ 
		if(CheckPower() == 0) return;
		//�����������˶�
	}
}

/****************************************************
	������:	Wing_Protocol_Handle
	����:		����������Э�麯��
	����:		liyao 2015��10��16��15:21:46
****************************************************/
void Wing_Protocol_Handle(Protocol_Info_T* pi){
	WING_PROTOCOL_T* wp = pi->ParameterList;
	#ifndef PRINT_ERR 
	ask_send(wp->para5);
	#endif
	
	if(CheckPower() == 0) return;
	if(wp->para6 == 1){
		#ifdef PRINT_ERR 
			printf("�յ����ָ��\r\n");
		#endif
		//�������˶� 
		Motor_WL.Exec_Protocol = *pi;
		Motor_WR.Exec_Protocol = *pi;
		if(wp->para1 > (WING_MAX_ANGLE - 10))
			wp->para1 = (WING_MAX_ANGLE - 10);
		if(wp->para3 > (WING_MAX_ANGLE - 10))
			wp->para3 = (WING_MAX_ANGLE - 10);
		Motor_WL.SetWingMotorParam(&Motor_WL, wp->para2, wp->para1);
		Motor_WR.SetWingMotorParam(&Motor_WR, wp->para4, wp->para3);	
		Motor_WL.ExecuteWingMotor(&Motor_WL);
		Motor_WR.ExecuteWingMotor(&Motor_WR);
	}
	
}

/****************************************************
	������:	Head_Protocol_Handle
	����:		ͷ����������Э�麯��
	����:		liyao 2015��10��16��15:21:46
****************************************************/
void Head_Protocol_Handle(Protocol_Info_T* pi){
	HEAD_PROTOCOL_T* hp = pi->ParameterList;
	#ifndef PRINT_ERR 
	ask_send(hp->para3);
	#endif
	if(hp->para4 == 1){
		#ifdef PRINT_ERR 
			printf("�յ�ͷ��ָ��\r\n");
		#endif 
		if(CheckPower() == 0) return;
		Motor_H.Exec_Protocol = *pi;
		if(abs((int)Motor_H.Angle_Cur - hp->para1) < 5 && hp->para2 != 0  && Motor_H.RunState == MOTOR_H_STOP)
			return;
		//����ͷ���˶�
		Motor_H.SetHeadMotorParam(&Motor_H, hp->para2, hp->para1);
		Motor_H.ExecuteHeadMotor(&Motor_H);
	}
}

/****************************************************
	������:	Head_Protocol_Handle
	����:		˫��ָ���Ƕ�����Э�麯��
	����:		liyao 2015��10��16��15:21:46
****************************************************/
void Angle_Protocol_Handle(Protocol_Info_T* pi){
	ANGLE_PROTOCOL_T* ap =  pi->ParameterList;
	#ifndef PRINT_ERR 
	ask_send(ap->para16);
	#endif
	if(ap->para16 == 1){
		if(CheckPower() == 0) return;
	}
}

/****************************************************
	������:	PDR_Correnct_Protocol_Handle
	����:		��λУ������Э�麯��
	����:		liyao 2015��10��20��15:26:37
****************************************************/
void PDR_Correnct_Protocol_Handle(Protocol_Info_T* pi){
	PDRCORRECT_PROTOCOL_T* pcp = pi->ParameterList;
	#ifndef PRINT_ERR 
	ask_send(pcp->para14);
	#endif
}

/****************************************************
	������:	OdometerScram_Protocol_Handle
	����:		˫�ָ������תָ���Ƕ�
	����:		liyao 2015��12��24��16:28:42
****************************************************/
void OdometerScram_Protocol_Handle(Protocol_Info_T* pi){
	ODOMETERSCRAM_T* osp = pi->ParameterList;
	#ifndef PRINT_ERR 
	ask_send(osp->para16);
	#endif

}

/****************************************************
	������:	Compensate_Protocol_Handle
	����:		�����ֲ�������
	����:		liyao 2016��1��9�� 19:20:22
****************************************************/
void Compensate_Protocol_Handle(Protocol_Info_T* pi){
	COMPENSATE_PROTOCOL_T* cp =  pi->ParameterList;
	#ifndef PRINT_ERR 
	ask_send(cp->para42);
	#endif

}


/****************************************************
	������:	ReportSnVersion_Protocol
	����:		�ϱ�����汾��оƬChipID
	����:		2016��8��23��11:46:40
****************************************************/
void ReportSnVersion_Protocol(){
	SENDSNVERSION_PROTOCOL_T sendSnVersion_protocol = {0}; 
	uint8_t verSN[32] = {'E','0','1','M','O','T','0','4','V',MAJORVERSION,MINORVERSION,BATEVERSION,VDATE};
	
  verSN[20] = *((uint8_t*)(CHIPIDADDR + 11));
	verSN[21] = *((uint8_t*)(CHIPIDADDR + 10));
	verSN[22] = *((uint8_t*)(CHIPIDADDR + 9));
	verSN[23] = *((uint8_t*)(CHIPIDADDR + 8));
	verSN[24] = *((uint8_t*)(CHIPIDADDR + 7));
	verSN[25] = *((uint8_t*)(CHIPIDADDR + 6));
	verSN[26] = *((uint8_t*)(CHIPIDADDR + 5));
	verSN[27] = *((uint8_t*)(CHIPIDADDR + 4));
	verSN[28] = *((uint8_t*)(CHIPIDADDR + 3)); 
	verSN[29] = *((uint8_t*)(CHIPIDADDR + 2));
	verSN[30] = *((uint8_t*)(CHIPIDADDR + 1));
	verSN[31] = *((uint8_t*)(CHIPIDADDR + 0)); 
	
	memcpy(&sendSnVersion_protocol, verSN, sizeof(uint8_t) * 32);
  Protocol_Send(SENDSNVERSION_PROTOCOL ,&sendSnVersion_protocol, sizeof(SENDSNVERSION_PROTOCOL_T));
}


/****************************************************
	������:	GetSnVersion_Protocol_Handle
	����:		��ȡ����汾��оƬChipID
	����:		liyao 2016��5��11��09:34:02
****************************************************/
void GetSnVersion_Protocol_Handle(Protocol_Info_T* pi){
	GETSNVERSION_PROTOCOL_T* gp =  pi->ParameterList; 
	#ifndef PRINT_ERR 
	ask_send(gp->para3);
	#endif
	if(gp->para1 == 0x01){ 
		ReportSnVersion_Protocol();
	}else if(gp->para1 == 0x02){
		NVIC_SystemReset();
	}
} 


/****************************************
������:	UpLoadState
��ע: �ϴ�״̬�����˵�ǰ״̬��ÿ500ms��ʼ��(1�ֽ�/5ms)��Ƶ�ʷ���һ������
****************************************/
int32_t FloatTime = 0;
void UpLoadState(void)
{
	uint8_t nomalError=0;
	uint8_t crashError=0;
	//��鹩���Ƿ��쳣 �쳣��ֹͣ�˶�
	if(CheckPower() == 0){  
		crashError |= (1<<3);
		crashError |= (1<<4); 
	};
	//Protocol_Info_T protocol_info = Create_Protocol_Info(sizeof(STATE_PROTOCOL_T),STATE_PROTOCOL,NULL,NULL);
	STATE_PROTOCOL_T state_protocol = {0}; 
//	state_protocol.para1 = Motor1.motorDir;//������1�������֣� 
//  state_protocol.para2 = Motor1.motorSpeed;//��������            
//  state_protocol.para3 = Motor2.motorDir;   //������2�������֣� 
//  state_protocol.para4 = Motor2.motorSpeed;//��������    
 
	state_protocol.para5 = (int)Motor_H.Angle_Cur;    //ͷ���Ƕ� 	    
  state_protocol.para6 = Motor_WL.Angle_Cur;        //���Ƕ� 
	state_protocol.para7 = Motor_WR.Angle_Cur; 		   //�ҳ�Ƕ�
//	state_protocol.para8 = Motor1.Current>>4;
//	state_protocol.para9 = Motor2.Current>>4;
	state_protocol.para10 = Motor_H.Current>>4;
	state_protocol.para11 = Motor_WL.Current>>4;
  state_protocol.para12 = 0;   //��� 
//	if(Motor1.CurrentStrategy ==OBSTACLE_CURRENT){
//		crashError |= 0x01;
//	}
//	if(Motor2.CurrentStrategy ==OBSTACLE_CURRENT){
//		crashError |= 0x01<<1;
//	}
	//---para13---start---
//	nomalError |=	(Motor1.runningflag<<1);//�����Ƿ��˶�
//	nomalError |=	(Motor2.runningflag<<2);//�����Ƿ��˶�
	if(Motor_H.RunState == MOTOR_H_RUN)//ͷ���Ƿ��˶�
		nomalError |= 1 << 3; 
	if(Motor_WL.RunState == MOTOR_W_RUN)//����Ƿ��˶�
		nomalError |= 1 << 4;     
	if(Motor_WR.RunState == MOTOR_W_RUN)//�ҳ��Ƿ��˶�
		nomalError |= 1 << 5;  
//	if(MOTOR_FLOAT_A == 0){//�����Ƿ�����
//		nomalError |=	(1<<6);
//		if(FloatTime == 0){
//			FloatTime = microsecond; 
//		}else if(Elapse_Us(microsecond, FloatTime) > 10*1000*1000){
//			ScramFlag = 1; 
//		}
//	}
//	if(MOTOR_FLOAT_B == 0){//�����Ƿ�����
//		nomalError |=	(1<<7);
//		if(FloatTime == 0){
//			FloatTime = microsecond; 
//		}else if(Elapse_Us(microsecond, FloatTime) > 10*1000*1000){
//			ScramFlag = 1; 
//		}
//	}
//	if(MOTOR_FLOAT_A == 1 && MOTOR_FLOAT_B == 1) FloatTime = 0;
//	
  state_protocol.para13 = nomalError;   //������Ϣ��λ ��1  
	//---para13---end--- 
  state_protocol.para14 = crashError;	//������Ϣ��λ ��1  
	Protocol_Send(STATE_PROTOCOL ,&state_protocol, sizeof(STATE_PROTOCOL_T));

//	protocol_info.len = sizeof(ODOMETER_PROTOCOL_T);
//	protocol_info.type = ODOMETER_PROTOCOL;
	ODOMETER_PROTOCOL_T odometer_protocol = {0}; 
//	odometer_protocol.para1 = Motor1.encoder>>24;//�������
//  odometer_protocol.para2 = Motor1.encoder>>16;//�������        
//  odometer_protocol.para3 = Motor1.encoder>>8;   //�������
//  odometer_protocol.para4 = Motor1.encoder;//������� 
//  odometer_protocol.para5 = Motor2.encoder>>24;		//�������           
//  odometer_protocol.para6 = Motor2.encoder>>16;   //�������           
//  odometer_protocol.para7 = Motor2.encoder>>8;		//�������           
//  odometer_protocol.para8 = Motor2.encoder; 	//�������          
  odometer_protocol.para9 = 0; 	//����           
  odometer_protocol.para10 = 0;	 //����    
	Protocol_Send(ODOMETER_PROTOCOL ,&odometer_protocol, sizeof(ODOMETER_PROTOCOL_T));

	//20151020-tc-modify
	DEADRECKONING_PROTOCOL_T deadreckoning_protocol = {0}; 
  deadreckoning_protocol.para14 = 0;	 //����   
	Protocol_Send(DEADRECKONING_PROTOCOL ,&deadreckoning_protocol, sizeof(DEADRECKONING_PROTOCOL_T));

}
 


 

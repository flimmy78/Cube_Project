#ifndef __PROTOCOL__H
#define __PROTOCOL__H 
#include <stdint.h> 
#include "FrameConfig.h"
//###################################����Э����###################################
/****************************************************
	�ṹ����:	STATE_PROTOCOL_T
	����: ����״̬Э��ʵ��
	���ߣ�liyao 2016��4��4��13:03:42
****************************************************/ 
typedef struct{
	uint8_t para1;
}State_P_T; 

/****************************************************
	�ṹ����:	STATE_PROTOCOL_T
	����: ������״̬Э��ʵ��
	���ߣ�liyao 2015��9��8��14:10:51
****************************************************/
typedef struct{
	uint8_t para1;//������1�������֣�
	uint8_t para2;//�����ٶȸ�8
	uint8_t para3;//�����ٶȵ�8
	uint8_t para4;//������2�������֣�
	uint8_t para5;//�����ٶȸ�8
	uint8_t para6;//�����ٶȵ�8
	uint8_t para7;
	uint8_t para8;
	uint8_t para9;
	uint8_t para10;
	uint8_t para11; 
	uint8_t para12;
	uint8_t para13;
}STATE_PROTOCOL_T;

/****************************************************
	�ṹ����:	ASK_PROTOCOL_T
	����: Ӧ��Э��ʵ��
	���ߣ�liyao 2015��9��8��14:10:51
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	uint8_t para1;//���
	uint8_t checksum;
	uint8_t tail;
}ASK_PROTOCOL_T;

//###################################����Э����###################################
/****************************************************
	�ṹ����:	HEARTBEAT_PROTOCOL_T
	����:	����Э��ʵ��
	���ߣ�liyao 2016��4��4��13:06:27
	��:		fd 00 09 02 57 f8
****************************************************/
typedef struct{
	uint8_t para1;//������
}HeartBeat_P_T;
 
/****************************************************
	�ṹ����:	RUN_PROTOCOL_T
	����:	˫�ֿ�������Э��ʵ��
	���ߣ�liyao 2015��9��8��14:10:51
	��:		fd 01 01 00 32 01 00 32 ff 00 f8
****************************************************/
typedef struct{
	int8_t para1;//���ַ���
	uint8_t para2;//�����ٶȸ�8λ
	uint8_t para3;//�����ٶȵ�8λ
	int8_t para4;//���ַ���
	uint8_t para5;//�����ٶȸ�8λ
	uint8_t para6;//�����ٶȵ�8λ
	uint8_t serial;
}Run_Protocol_T;

//----------------------������--------------------------------
#define TO_MODULE_ACTION(SrcModule,TargetModule,Action) (SrcModule<<4|TargetModule)<<8|Action
 
/*ģ����*/
typedef enum{ 
	PAD_MODULE	 = 0x01,
	RASPBERRY_MODULE = 0x02, 
	MOTOR_MODULE = 0x03, 
}MODULE;
 

/*Э��Ŀ��弰����*/
typedef enum { 
	REPORT_STATE = TO_MODULE_ACTION(MOTOR_MODULE, PAD_MODULE, 0x82),
	
	HEART_BEAT	 = TO_MODULE_ACTION(PAD_MODULE, MOTOR_MODULE, 0x80),
  HEAP_USE	 = TO_MODULE_ACTION(PAD_MODULE, MOTOR_MODULE, 0xAA), 
	
	RUN_PROTOCOL = TO_MODULE_ACTION(PAD_MODULE, MOTOR_MODULE, 0x01),
  
}MODULE_ACTION;
 
extern void Protocol_Init(void);
 















#endif


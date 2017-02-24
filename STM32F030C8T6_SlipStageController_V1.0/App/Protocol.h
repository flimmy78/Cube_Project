#ifndef __PROTOCOL__H
#define __PROTOCOL__H 
#include <stdint.h> 
#include "FrameConfig.h"
//###################################����Э����###################################

/****************************************************
	�ṹ����:	ASK_PROTOCOL_T
	����: Ӧ��Э��ʵ��
	���ߣ�liyao 2015��9��8��14:10:51
****************************************************/
typedef struct{
	uint8_t para1;//�������
}ASK_PROTOCOL_T;

 
typedef struct{
	uint8_t para1;//�˶����� 1�� 0��
	uint8_t para2;//�ƶ������8 mm
	uint8_t para3;//�ƶ������8 mm
	uint8_t para4;//С�����ָ�8	mm
	uint8_t para5;//С�����ֵ�8 mm
	uint8_t para6;//�ٶ� 0-100
}CMD_PROTOCOL_T; 

typedef struct{
	uint8_t para1; //�˶����� 1�� 0��
	uint8_t para2; //ת���Ƕȸ�8 ��
	uint8_t para3; //ת���Ƕȵ�8 ��
	uint8_t para4; //ת���Ƕ�С����8 ��
	uint8_t para5; //ת���Ƕ�С����8 ��
	uint8_t para6; //�ٶ� 0-100 
}CMD_A_PROTOCOL_T;  

//###################################����Э����###################################
/****************************************************
	�ṹ����:	HEARTBEAT_PROTOCOL_T
	����:	����Э��ʵ��
	���ߣ�liyao 2016��4��4��13:06:27
	��:		fd 00 09 02 57 f8
****************************************************/
typedef struct{
	uint8_t para1;//������
}HEARTBEAT_PROTOCOL_T;


//----------------------������--------------------------------
#define TO_MODULE_ACTION(SrcModule,TargetModule,Action) (SrcModule<<4|TargetModule)<<8|Action
 
/*ģ����*/
typedef enum{ 
	SERVER_MODULE	 = 0x01,
	CONTROL_MODULE = 0x02,
}MODULE;
 

/*Э��Ŀ��弰����*/
typedef enum { 
  ANSWER         = TO_MODULE_ACTION(CONTROL_MODULE, SERVER_MODULE, 0xF0),
	CMD_PROTOCOL   = TO_MODULE_ACTION(SERVER_MODULE, CONTROL_MODULE, 0x01),
	CMD_A_PROTOCOL = TO_MODULE_ACTION(SERVER_MODULE, CONTROL_MODULE, 0x02),
}MODULE_ACTION; 
 
extern void Protocol_Init(void);
 















#endif


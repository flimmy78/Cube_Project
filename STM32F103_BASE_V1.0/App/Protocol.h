#ifndef __PROTOCOL__H
#define __PROTOCOL__H 
#include <stdint.h> 
#include "FrameConfig.h"
//###################################����Э����###################################

///****************************************************
//	�ṹ����:	ASK_PROTOCOL_T
//	����: Ӧ��Э��ʵ��
//	���ߣ�liyao 2015��9��8��14:10:51
//****************************************************/
//typedef struct{
//	uint8_t para1;//�������
//}ASK_PROTOCOL_T;

/****************************************************
	�ṹ����:	STATE_PROTOCOL_T
	����: ����״̬Э��ʵ��
	���ߣ�liyao 2016��4��4��13:03:42
****************************************************/ 
typedef struct{
	uint8_t para1;
}State_P_T; 
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
 


//----------------------������--------------------------------
#define TO_MODULE_ACTION(SrcModule,TargetModule,Action) (SrcModule<<4|TargetModule)<<8|Action
 
/*ģ����*/
typedef enum{ 
	PAD_MODULE	 = 0x01,
	MAIN_MODULE = 0x02, 
	ULT_MODULE	 = 0x03,
	MOTOR_MODULE= 0x04,
	INFRARED_MODULE = 0x05,
	PROJECTION_MODULE = 0x06,
	CHARGING_MODULE = 0x07,
	FACEMOT_MODULE = 0x08,
	HEADWING_MODULE = 0x09,
	SLIDING_MODULE = 0x0A,
	LIGHT_MODULE = 0x0B,
	POWER_MODULE = 0x0C,
	WAREHOUSE_MODULE = 0x0D, 
}MODULE;
 

/*Э��Ŀ��弰����*/
typedef enum { 
	REPORT_STATE = TO_MODULE_ACTION(HEADWING_MODULE, PAD_MODULE, 0x82),	
	HEART_BEAT	 = TO_MODULE_ACTION(PAD_MODULE, HEADWING_MODULE, 0x80),
  HEAP_USE	 = TO_MODULE_ACTION(PAD_MODULE, HEADWING_MODULE, 0xAA), 
  
}MODULE_ACTION;
 
extern void Protocol_Init(void);
 















#endif


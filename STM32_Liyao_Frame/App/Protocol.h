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

/****************************************************
	�ṹ����:	STATE_PROTOCOL_T
	����: ����״̬Э��ʵ��
	���ߣ�liyao 2016��4��4��13:03:42
****************************************************/ 
typedef struct{
	uint8_t h_angle;//ͷ���Ƕ�
	uint8_t w_l_angle;//���Ƕ�
	uint8_t w_r_angle;//�ҳ�Ƕ�
	uint8_t h_electricity[2];//ͷ������
	uint8_t w_electricity[2];//������
	uint8_t runstate;//����״̬
	uint8_t errno;//�����־
}STATE_PROTOCOL_T; 
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



//*******************ͷ������Э�� ƽ���������*****************************//
//fd 00 0B 18 05 01 05 05 05 05 05 05 05 05 51 f8
/****************************************************
	�ṹ����:	CMD_PROTOCOL_T
	����:	����Э��
	���ߣ�liyao 2016��4��4��13:06:27
	��:		
****************************************************/
typedef struct{
	uint8_t para1; 
	uint8_t para2; 
	uint8_t para3; 
	uint8_t para4; 	
  uint8_t para5; 
	uint8_t para6; 
	uint8_t para7; 
	uint8_t para8;
}CMD_PROTOCOL_T;


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
    
  CMD_RUN	     = TO_MODULE_ACTION(PAD_MODULE, FACEMOT_MODULE, 0x01),
  CMD_R_SPEED	 = TO_MODULE_ACTION(PAD_MODULE, FACEMOT_MODULE, 0x02),
  CMD_W_PARA	 = TO_MODULE_ACTION(PAD_MODULE, FACEMOT_MODULE, 0x03),
  CMD_R_PARA	 = TO_MODULE_ACTION(PAD_MODULE, FACEMOT_MODULE, 0x04),
  CMD_R_PWR	   = TO_MODULE_ACTION(PAD_MODULE, FACEMOT_MODULE, 0x05),
  CMD_R_STATE	 = TO_MODULE_ACTION(PAD_MODULE, FACEMOT_MODULE, 0x06),
  CMD_R_SN	   = TO_MODULE_ACTION(PAD_MODULE, FACEMOT_MODULE, 0x07),
  
}MODULE_ACTION;
 
extern void Protocol_Init(void);
 















#endif


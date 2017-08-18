#ifndef __PROTOCOL__H
#define __PROTOCOL__H 
#include <stdint.h> 
#include "FrameConfig.h"
#include "ProtocolFrame.h" 

//����ָ��
#define OPEN_ULT 1					//����������
#define CLOSE_ULT 2					//�رճ�����
#define CHANGE_INTERVAL 3		//�ϱ�����
#define BARD_RATE 4					//������
#define SENSITIVITY 5       //������
#define DEAD_ZONE 6					//ä������

//����ָ��Ӧ��
#define OP_ACK_OK 1
#define OP_ACK_ERR 0
//###################################����Э����###################################
/****************************************************
	�ṹ����:	Distance_T
	����: �����������ϱ�
	���ߣ�liyao 2017��7��20��17:03:51
****************************************************/
typedef struct{
	uint8_t ID;
	uint8_t Distance_H;
	uint8_t Distance_L;
	uint8_t HOLD1;
	uint8_t HOLD2;
}Distance_T;

///****************************************************
//	�ṹ����:	ASK_PROTOCOL_T
//	����: Ӧ��Э��ʵ��
//	���ߣ�liyao 2015��9��8��14:10:51
//****************************************************/
//typedef struct{
//	uint8_t para1;//�������
//}ASK_PROTOCOL_T;

//###################################����Э����###################################
/****************************************************
	�ṹ����:	Allot_Seq_T
	����: ��λ������ID
	���ߣ�liyao 2017��7��19��21:48:58
****************************************************/ 
typedef struct{
	uint8_t SN_H;
	uint8_t SN_L;
	uint8_t ID;
}Allot_ID_T; 

/****************************************************
	�ṹ����:	OP_Ctrl_T
	����: ��λ������Э��
	���ߣ�liyao 2017��7��19��21:48:58
****************************************************/ 
typedef struct{
	uint8_t ID;
	uint8_t Cmd;
	uint8_t Data_H;
	uint8_t Data_L;
}OP_Ctrl_T; 

/****************************************************
	�ṹ����:	Main_Answer_T
	����: ��λ��Ӧ��Э��
	���ߣ�liyao 2017��7��19��21:48:58
****************************************************/ 
typedef struct{
	uint8_t ID;
	uint8_t Seq;
	uint8_t Action;
}Main_Answer_T; 


/****************************************************
	�ṹ����:	HEARTBEAT_PROTOCOL_T
	����:	����Э��ʵ��
	���ߣ�liyao 2017��7��19��21:35:57
	��:		
****************************************************/
typedef struct{
	uint8_t Seq;//������
	uint8_t para2;
}Main_Heart_T;


//----------------------������--------------------------------
/*ģ����*/
#define ULT_MODULE  0x02
#define MAIN_MODULE 0x01 

/*Э��Ŀ��弰����*/
//����Э��
#define  SN_REPORT    TO_MODULE_ACTION(ULT_MODULE, MAIN_MODULE, 0x01) //SN�ϱ�
#define  DISTANCE_REPORT    TO_MODULE_ACTION(ULT_MODULE, MAIN_MODULE, 0x02) //�����������ϱ�
#define  ANSWER       TO_MODULE_ACTION(ULT_MODULE, MAIN_MODULE, 0xF0)	//Э��Ӧ��
#define  OP_ANSWER    TO_MODULE_ACTION(ULT_MODULE, MAIN_MODULE, 0xF1) //����Ӧ��
//����Э��
#define  ALLOT_ID		TO_MODULE_ACTION(MAIN_MODULE, ULT_MODULE, 0x01) //ID��ȡ
#define  OP_CTRL			TO_MODULE_ACTION(MAIN_MODULE, ULT_MODULE, 0x02) //����ָ��
#define  MAIN_ANSWER	TO_MODULE_ACTION(MAIN_MODULE, ULT_MODULE, 0xF0) //��λ��Ӧ��
#define  MAIN_HEART	  TO_MODULE_ACTION(MAIN_MODULE, ULT_MODULE, 0xE0) //����

extern void Protocol_Init(void);
extern void RobSend(uint8_t* data, uint8_t len);















#endif


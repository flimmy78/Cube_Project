#ifndef __PROTOCOL__H
#define __PROTOCOL__H 
#include <stdint.h> 
#include "FrameConfig.h"
#include "ProtocolFrame.h" 

//###################################����Э����###################################

/****************************************************
	�ṹ����:	Ptcl_Ack_T
	����: Ӧ��Э��ʵ��
	���ߣ�liyao 2017��10��13��
****************************************************/
typedef struct{
	uint8_t Seq;
}__attribute((packed))Ptcl_Ack_T;

/****************************************************
	�ṹ����:	OP_Ack_T
	����: ����Ӧ��Э��ʵ��
	���ߣ�liyao 2017��10��13��
****************************************************/
typedef struct{
	uint8_t Action;
	uint8_t Seq;
	uint8_t Result;
}__attribute((packed))OP_Ack_T;
//###################################����Э����###################################
/****************************************************
	�ṹ����:	Super_Ack_T
	����: ��λ��Ӧ��Э��ʵ��
	���ߣ�liyao 2017��10��13��
****************************************************/
typedef struct{	
	uint8_t Action;
	uint8_t Seq;
	uint8_t Reserved;
}__attribute((packed))Super_Ack_T;


/*ģ����*/
#define REMOTE_MODULE  0x01
#define THIS_MODULE  	 0x02 

/*Э��Ŀ��弰����*/
//����Э��
#define  PTCL_ACK    		TO_MODULE_ACTION(THIS_MODULE, REMOTE_MODULE, 0xF0)	//Э��Ӧ��
#define  OP_ACK    			TO_MODULE_ACTION(THIS_MODULE, REMOTE_MODULE, 0xF1)	//����Ӧ��
//����Э��
#define  SUPER_ACK    	TO_MODULE_ACTION(REMOTE_MODULE, THIS_MODULE, 0xF0)		//��λ��Ӧ��

extern void Protocol_Init(void);















#endif


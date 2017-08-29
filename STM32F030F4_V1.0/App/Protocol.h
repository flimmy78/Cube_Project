#ifndef __PROTOCOL__H
#define __PROTOCOL__H 
#include <stdint.h> 
#include "FrameConfig.h"
#include "ProtocolFrame.h" 

//###################################����Э����###################################

/****************************************************
	�ṹ����:	Template_Send_T
	����: ����Э��ʾ��
	���ߣ�liyao 2015��9��8��14:10:51
****************************************************/
typedef struct{
	uint8_t para1;//�������
}Template_Send_T;

//###################################����Э����###################################
/****************************************************
	�ṹ����:	Template_Recv_T
	����: ����Э��ʾ��
	���ߣ�liyao 2015��9��8��14:10:51
****************************************************/
typedef struct{
	uint8_t para1;//�������
}Template_Recv_T;


//----------------------������--------------------------------
/*ģ����*/
#define TEST1_MODULE  0x01
#define TEST2_MODULE  0x02 

/*Э��Ŀ��弰����*/
//����Э��
#define  SEND_TYPE    TO_MODULE_ACTION(TEST1_MODULE, TEST2_MODULE, 0x01)
//����Э��
#define  RECV_TYPE		TO_MODULE_ACTION(TEST2_MODULE, TEST1_MODULE, 0x01) //ID��ȡ

extern void Protocol_Init(void);















#endif


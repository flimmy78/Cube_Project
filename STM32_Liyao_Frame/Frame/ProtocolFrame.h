#ifndef __PROTOCOLFRAME_H__
#define __PROTOCOLFRAME_H__
#include "FrameConfig.h"
#include "Protocol.h" 
#include "LOG.h"
#include "Queue.h"
/*-----�ַ�ת��-----
FD->FE 7D
F8->FE 78
FE->FE 7E		*/

#define PROTOCOL_SINGLE_BUFF 100
 
/*Э������  ����   ����  ת��*/
typedef enum{ SEND, RECEIVE, TRANSPOND }PROTOCOL_TYPE;
   
//---------------------------�����------------------------------------- 
typedef struct _PROTOCOL_INFO_T Protocol_Info_T ; 
typedef struct _PROTOCOL_DESC_T Protocol_Desc_T ; 
typedef struct _PROTOCOL_RESOLVER_T Protocol_Resolver_T;
/****************************************************
	�ṹ����:	Protocol_Desc_T
	����: Э������
	���ߣ�liyao 2015��9��8��14:10:51
****************************************************/
struct _PROTOCOL_DESC_T{ 
	MODULE_ACTION ModuleAction;
	MODULE TargetModule;
	MODULE SrcModule;
	uint8_t 		ProtocolSize;	//�����ṹ���С
	uint8_t		Serial; 
	void (*Send)(uint8_t* Data, uint8_t Len);
	void (*Handle)(Protocol_Info_T*);
	int8_t (*Check)(void*);
};

/****************************************************
	�ṹ����:	Protocol_Info_T
	����: Э����Ϣ����
	���ߣ�liyao 2015��9��8��14:10:51
****************************************************/
struct _PROTOCOL_INFO_T{
	uint8_t Head;		//֡ͷ
	uint8_t Standby1;//Ԥ��
	uint8_t Plen;		//֡��
	uint8_t Module; //Ŀ��ģ��
	uint8_t Serial;	//���
	uint8_t Action;	//ָ���� 
	void* ParameterList;//����
	uint8_t CheckSum;//У��� 
	uint8_t Tail;		//֡β 
	uint8_t ParaLen;	//��������
	uint8_t AllLen;		//�ܳ���
	Protocol_Desc_T* ProtocolDesc;//Э������
	void (*Handle)(Protocol_Info_T*);
	void (*Ack)(Protocol_Info_T*);
	int8_t (*Check)(void*);
};


/****************************************************
	�ṹ����:	Protocol_Resolver_T
	����: Э�������
	���ߣ�liyao 2016��9��7��17:55:45
****************************************************/
struct _PROTOCOL_RESOLVER_T{ 
	Queue_Head_T  *Protocol_Queue; 
	Protocol_Info_T pi;   
	uint8_t  Cnt; 				//������������
	uint8_t  ParaData[PROTOCOL_SINGLE_BUFF];
	uint8_t  Index;
	int8_t 	 Is_FE;
	uint16_t CheckSum;
	uint8_t  Recv_State;
	int8_t (*Protocol_Put)(Protocol_Resolver_T* pr,uint8_t* datas,uint8_t len);
	void (*Fetch_Protocol)(Protocol_Resolver_T* pr);
};



//###################################������################################### 
#if PROTOCOL_RESOLVER_1
	extern Protocol_Resolver_T *ProtocolResolver_1; 
#endif
#if PROTOCOL_RESOLVER_2
	extern Protocol_Resolver_T *ProtocolResolver_2; 
#endif
#if PROTOCOL_RESOLVER_3
	extern Protocol_Resolver_T *ProtocolResolver_3; 
#endif
#if PROTOCOL_RESOLVER_4
	extern Protocol_Resolver_T *ProtocolResolver_4; 
#endif

extern void ProtocolFrame_Init(void);
//extern Protocol_Info_T Create_Protocol_Info(int8_t len,SEND_ACTION type,void (*handle)(Protocol_Info_T*),int8_t (*check)(void*)); 
//int8_t Send_To_Uart(Protocol_Info_T* protocol_info);
//int8_t Send_To_Buff(Protocol_Info_T* protocol_info);
extern void Protocol_Send(MODULE_ACTION Module_Action,void* Data,uint8_t Len);
extern void Protocol_Send_Transpond(Protocol_Info_T* pi);
extern void FetchProtocols(void);
uint16_t char_special(uint8_t num);
extern int8_t Protocol_Register(Protocol_Desc_T* Desc_T,PROTOCOL_TYPE Protocol_Type);
#endif

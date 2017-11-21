#ifndef __PROTOCOLFRAME_H__
#define __PROTOCOLFRAME_H__
#include "FrameConfig.h"
//#include "Protocol.h" 
#include "LOG.h"
#include "Queue.h"
/*-----�ַ�ת��-----
FD->FE 7D
F8->FE 78
FE->FE 7E		*/
#define PROTOCOL_HEAD 0xFD
#define PROTOCOL_TAIL 0xF8

#define SRC_MODULE_Pos 16
#define TARGET_MODULE_Pos 8
#define ACTION_MODULE_Pos 0
#define u8 uint8_t 
#define HandleAdd(FunName)   extern void FunName(Protocol_Info_T* pi)
#define CUSTOM_MODULE  0xEE				//����ʱָ��ģ��
#define BROADCAST_MODULE 0xFF
//��������ת���ɹ㲥����
#define TO_BROADCAST_MODULE_ACTION(MODULE_ACTION) (MODULE_ACTION | 0x0000FF00)
//��ϳ�uint32_t���͵�������
#define TO_MODULE_ACTION(SrcModule,TargetModule,Action) ((SrcModule << SRC_MODULE_Pos)|(TargetModule << TARGET_MODULE_Pos)|(Action << ACTION_MODULE_Pos))
//����������ȡ��Ϣ
#define PULL_SRC_MODULE(val)					((val  >> SRC_MODULE_Pos) & 0xFF)
#define PULL_TARGET_MODULE(val)				((val >> TARGET_MODULE_Pos) & 0xFF)
#define PULL_ACTION_MODULE(val)				((val >> ACTION_MODULE_Pos) & 0xFF)
//�䳤Э��
#define ELONGATE_SIZE 0xFF 
#define UART_BUFFSIZE 100
/*Э������  ����   ����  ת��*/
typedef enum{ SEND, RECEIVE, TRANSPOND }PROTOCOL_TYPE;
typedef enum{OK_P, LEN_ERR_P, DATALEN_ERR_P,CHECKSUM_ERR_P, TAIL_ERR_P, EQUALS_ERR_P}PROTOCOL_Status;
//---------------------------�����------------------------------------- 
typedef struct _PROTOCOL_INFO_T Protocol_Info_T ; 
typedef struct _PROTOCOL_DESC_T Protocol_Desc_T ; 
typedef struct _PROTOCOL_RESOLVER_T Protocol_Resolver_T;

///****************************************************
//	�ṹ����:	Module_Action_T
//	����: Э��ģ���ָ����
//	���ߣ�liyao 2017��7��19��15:49:04
//****************************************************/
//typedef struct {
//	MODULE Src;
//	MODULE Target;
//	uint8_t Action;
//}Module_Action_T;

/****************************************************
	�ṹ����:	Protocol_Desc_T
	����: Э������
	���ߣ�liyao 2017��7��19��14:46:20
****************************************************/
struct _PROTOCOL_DESC_T{ 
	uint32_t ModuleAction;
	uint8_t ProtocolSize;	//�����ṹ���С
	uint8_t	Serial; 			//��ʷЭ����(��ˮ��)
	PROTOCOL_TYPE ProtocolType;	//Э������
	void (*Send)(uint8_t* Data, uint8_t Len);//���ͻص�����(����Э����ʵ��)
	void (*Transpond)(uint8_t* Data, uint8_t Len);//ת���ص�����(ת��Э����ʵ��)
	void (*Handle)(Protocol_Info_T*);							//����ص�����(���պ�����ʵ��)
	int8_t (*Check)(void*);												//Э�������麯��(����Э���ʵ��)
	void (*Ack)(Protocol_Info_T*);								//Ӧ����(����Э���ʵ��)
	
};
/****************************************************
	�ṹ����:	Protocol_Info_T
	����: Э����Ϣ����
	���ߣ�liyao 2017��7��19��14:42:51
****************************************************/
struct _PROTOCOL_INFO_T{
	uint8_t Head;		//֡ͷ
	uint8_t Action;//ָ����
	uint8_t  SrcModule;//Դģ��
	uint8_t	TargetModule;//Ŀ��ģ��
	uint8_t Serial;	//���
	uint8_t DataLen;//���ݳ���
	void* ParameterList;//����
	uint8_t CheckSum;//У��� 
	uint8_t Tail;		//֡β 
	uint8_t AllLen;		//�ܳ���
	Protocol_Desc_T* ProtocolDesc;//Э������
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
	uint8_t*  ParaData;
	//uint8_t  Index;
	int8_t 	 Is_FE;
	uint16_t CheckSum;
	uint8_t  Recv_State;
	int8_t (*Protocol_Put)(Protocol_Resolver_T* pr,uint8_t* datas,uint8_t len);
	void (*Fetch_Protocol)(Protocol_Resolver_T* pr);
};



//###################################������################################### 
#if PROTOCOL_RESOLVER_1 || PROTOCOL_RESOLVER_IT_1
	extern Protocol_Resolver_T *ProtocolResolver_1; 
#endif
#if PROTOCOL_RESOLVER_2 || PROTOCOL_RESOLVER_IT_2
	extern Protocol_Resolver_T *ProtocolResolver_2; 
#endif
#if PROTOCOL_RESOLVER_3 || PROTOCOL_RESOLVER_IT_3
	extern Protocol_Resolver_T *ProtocolResolver_3; 
#endif
#if PROTOCOL_RESOLVER_4 || PROTOCOL_RESOLVER_IT_4
	extern Protocol_Resolver_T *ProtocolResolver_4; 
#endif
#if PROTOCOL_RESOLVER_5 || PROTOCOL_RESOLVER_IT_5
	extern Protocol_Resolver_T *ProtocolResolver_5; 
#endif


#define ProtocolFrame_IRQHandler() do{if(PaddingProtocol()) return;}while(0)
extern void ProtocolFrame_Init(void);
//extern Protocol_Info_T Create_Protocol_Info(int8_t len,SEND_ACTION type,void (*handle)(Protocol_Info_T*),int8_t (*check)(void*)); 
//int8_t Send_To_Uart(Protocol_Info_T* protocol_info);
//int8_t Send_To_Buff(Protocol_Info_T* protocol_info); 
extern void Protocol_Send(uint32_t ModuleAction,void* Data,uint8_t Len);
extern void Protocol_Send_Transpond(Protocol_Info_T* pi);
extern int8_t PaddingProtocol(void);
extern void FetchProtocols(void);
extern int8_t Protocol_Register(Protocol_Desc_T* Desc_T, PROTOCOL_TYPE Protocol_Type); 
#endif

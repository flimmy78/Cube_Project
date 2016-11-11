#ifndef __PROTOCOL__H
#define __PROTOCOL__H 
#include <stdint.h> 
#include "FrameConfig.h"
//###################################����Э����###################################
/****************************************************
	�ṹ����:	STATE_PROTOCOL_T
	����: ������״̬Э��ʵ��
	���ߣ�liyao 2015��9��8��14:10:51
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	uint8_t para1;//������1�������֣�
	uint8_t para2;//��������
	uint8_t para3;//������2�������֣�
	uint8_t para4;//��������
	uint8_t para5;//ͷ���Ƕ�
	uint8_t para6;//���Ƕ�
	uint8_t para7;//�ҳ�Ƕ�
	uint8_t para8;//���ֵ���
	uint8_t para9;//���ֵ���
	uint8_t para10;//ͷ������
	uint8_t para11;//˫�����
	uint8_t para12;//Reserved
	uint8_t para13;//Reserved
	uint8_t para14;//������Ϣ��λ ��1
	uint8_t checksum;
	uint8_t tail;
}STATE_PROTOCOL_T;

/****************************************************
	�ṹ����:	ODOMETER_PROTOCOL_T
	����: ���������Э��
	���ߣ�tc 2015��9��12��21:10:51
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	uint8_t para1;//�������
	uint8_t para2;//�������
	uint8_t para3;//�������
	uint8_t para4;//�������
	uint8_t para5;//�������
	uint8_t para6;//�������
	uint8_t para7;//�������
	uint8_t para8;//�������
	uint8_t para9;//����
	uint8_t para10;//����
	uint8_t checksum;
	uint8_t tail;
}ODOMETER_PROTOCOL_T;

/****************************************************
	�ṹ����:	DEADRECKONING_PROTOCOL_T
	����: ��λ��ϢЭ��
	���ߣ�tc 2015��9��16��15:30:51
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	uint8_t para1;//��λX����
	uint8_t para2;//��λX����
	uint8_t para3;//��λX����
	uint8_t para4;//��λX����
	uint8_t para5;//��λY����
	uint8_t para6;//��λY����
	uint8_t para7;//��λY����
	uint8_t para8;//��λY����
	uint8_t para9;//��λTH����
	uint8_t para10;//��λTH����
	uint8_t para11;//��λTH����
	uint8_t para12;//��λTH����
	uint8_t para13;//����
	uint8_t para14;//����
	uint8_t checksum;
	uint8_t tail;
}DEADRECKONING_PROTOCOL_T;

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

/****************************************************
	�ṹ����:	RESPOND_PROTOCOL_T
	����: ִ�н������Э��
	���ߣ�liyao 2015��10��16��16:33:41
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	uint8_t para1;//���
	uint8_t para2;//��λ����������
	uint8_t para3;//��λ��֡����
	uint8_t para4;//ָ���Ƿ����
	uint8_t checksum;
	uint8_t tail;
}RESPOND_PROTOCOL_T;

/****************************************************
	�ṹ����:	SELFINS_PROTOCOL_T
	����: �Լ����ϱ�
	���ߣ�liyao 2016��1��25��14:20:04
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	uint8_t para1;//����λͼ
	uint8_t para2;//
	uint8_t para3;//˫�����
	uint8_t para4;//˫�����
	uint8_t para5;//ͷ������
	uint8_t para6;//ͷ������
	uint8_t para7;//�������
	uint8_t para8;//�ҳ�����
	uint8_t para9;//ͷ������
	uint8_t para10;//ͷ������
	uint8_t para11;//����
	uint8_t para12;//����
	uint8_t para13;//����
	uint8_t para14;//���
	uint8_t checksum;
	uint8_t tail;
}SELFINS_PROTOCOL_T;

/****************************************************
	�ṹ����:	SENDSNVERSION_PROTOCOL_T
	����:	���Ͱ汾�ż�оƬChipID
	���ߣ�liyao 2016��5��11��09:11:26
	��:			
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	uint8_t para1; //��Ʒ�ͺţ�E01
	uint8_t para2;	
	uint8_t para3;
	uint8_t para4; //�������ͣ�000
	uint8_t para5; 
	uint8_t para6; 
	uint8_t para7; //Ӳ��ƽ̨:00
	uint8_t para8; 
	uint8_t para9; //����汾��V438
	uint8_t para10; 
	uint8_t para11; 
	uint8_t para12; 
	uint8_t para13; //�������ڣ�20160525
	uint8_t para14; 
	uint8_t para15; 
	uint8_t para16;  
	uint8_t para17;  
	uint8_t para18;  
	uint8_t para19;  
	uint8_t para20;
	uint8_t para21;//96λΨһID
	uint8_t para22;
	uint8_t para23;
	uint8_t para24;
	uint8_t para25;
	uint8_t para26;
	uint8_t para27;
	uint8_t para28;
	uint8_t para29;
	uint8_t para30;
	uint8_t para31;
	uint8_t para32; 
	uint8_t para33; //���
	uint8_t checksum;
	uint8_t tail;
}SENDSNVERSION_PROTOCOL_T;
//###################################����Э����###################################
/****************************************************
	�ṹ����:	RUN_PROTOCOL_T
	����:	˫�ֿ�������Э��ʵ��
	���ߣ�liyao 2015��9��8��14:10:51
	��:		fd 01 02 00 10 00 00 10 01 01 01 f8
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	int8_t para1;//���ַ���
	uint8_t para2;//�����ٶȸ�8λ
	uint8_t para3;//�����ٶȵ�8λ
	int8_t para4;//���ַ���
	uint8_t para5;//�����ٶȸ�8λ
	uint8_t para6;//�����ٶȵ�8λ
	uint8_t para7;//���
	int8_t para8;//�Ƿ���ָ��
	uint8_t checksum;
	uint8_t tail;
}RUN_PROTOCOL_T;

/****************************************************
	�ṹ����:	ANGLE_PROTOCOL_T
	����:	˫��ָ���Ƕ�����Э��ʵ��
	���ߣ�liyao 2015��9��8��14:10:51
	��:			
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	uint8_t para1;//���ַ���
	uint8_t para2;//�����ٶȸ�8λ
	uint8_t para3;//�����ٶȵ�8λ
	uint8_t para4;//�����˶�����
	uint8_t para5;//�����˶�����
	uint8_t para6;//�����˶�����
	uint8_t para7;//�����˶�����
	uint8_t para8;//���ַ���
	uint8_t para9;//�����ٶȸ�8λ
	uint8_t para10;//�����ٶȵ�8λ
	uint8_t para11;//�����˶�����
	uint8_t para12;//�����˶�����
	uint8_t para13;//�����˶�����
	uint8_t para14;//�����˶�����
	uint8_t para15;//���
	int8_t para16;//�Ƿ���ָ��
	uint8_t checksum;
	uint8_t tail;
}ANGLE_PROTOCOL_T;

/****************************************************
	�ṹ����:	HEAD_PROTOCOL_T
	����:	ͷ����������Э��ʵ��
	���ߣ�liyao 2015��9��8��14:10:51
	��:		fd 02 f0 00 88 01 f7 f8
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	uint8_t para1;//ͷ���Ƕ�
	uint8_t para2;//ͷ���ٶ�
	uint8_t para3;//���
	uint8_t para4;//�Ƿ���ָ��
	uint8_t checksum;
	uint8_t tail;
}HEAD_PROTOCOL_T;

/****************************************************
	�ṹ����:	WING_PROTOCOL_T
	����:	����������Э��ʵ��
	���ߣ�liyao 2015��9��8��14:10:51
	��:		fd 03 10 00 20 00 88 01 f7 f8
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	int8_t para1;//���Ƕ�
	int8_t para2;//����ٶ�
	int8_t para3;//�ҳ�Ƕ�
	int8_t para4;//�ҳ��ٶ�
	int8_t para5;//���
	int8_t para6;//�Ƿ���ָ��
	uint8_t checksum;
	uint8_t tail;
}WING_PROTOCOL_T;
/****************************************************
	�ṹ����:	PDRCORRECT_PROTOCOL_T
	����:	��λ����Э��ʵ��
	���ߣ�liyao 2015��10��20��13:57:15
	��:		
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	uint8_t para1;//��λX����
	uint8_t para2;//��λX����
	uint8_t para3;//��λX����
	uint8_t para4;//��λX����
	uint8_t para5;//��λY����
	uint8_t para6;//��λY����
	uint8_t para7;//��λY����
	uint8_t para8;//��λY����
	uint8_t para9;//��λTH����
	uint8_t para10;//��λTH����
	uint8_t para11;//��λTH����
	uint8_t para12;//��λTH����
	uint8_t para13;//���
	uint8_t para14;//�Ƿ���ָ��
	uint8_t checksum;
	uint8_t tail;
}PDRCORRECT_PROTOCOL_T;

/****************************************************
	�ṹ����:	ODOMETERSCRAM_T
	����:	˫�ָ������תָ���Ƕ�
	���ߣ�liyao 2015��12��24��16:23:07
	��:			
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	uint8_t para1;//���ַ���
	uint8_t para2;//�����ٶȸ�8λ
	uint8_t para3;//�����ٶȵ�8λ
	uint8_t para4;//�����˶�����
	uint8_t para5;//�����˶�����
	uint8_t para6;//�����˶�����
	uint8_t para7;//�����˶�����
	uint8_t para8;//���ַ���
	uint8_t para9;//�����ٶȸ�8λ
	uint8_t para10;//�����ٶȵ�8λ
	uint8_t para11;//�����˶�����
	uint8_t para12;//�����˶�����
	uint8_t para13;//�����˶�����
	uint8_t para14;//�����˶�����
	uint8_t para15;//���
	int8_t para16;//�Ƿ���ָ��
	uint8_t checksum;
	uint8_t tail;
}ODOMETERSCRAM_T;

/****************************************************
	�ṹ����:	COMPENSATE_T
	����:	�����ֲ���
	���ߣ�liyao 2016��1��9�� 19:05:07
	��:			
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	uint8_t para1;//��ֱ����
	uint8_t para2;//����1.1
	uint8_t para3;//����1.2
	uint8_t para4;//����1.3
	uint8_t para5;//����1.4
	uint8_t para6;//����2.1
	uint8_t para7;//����2.2
	uint8_t para8;//����2.3
	uint8_t para9;//����2.4
	uint8_t para10;//����3.1
	uint8_t para11;//����3.2
	uint8_t para12;//����3.3
	uint8_t para13;//����3.4
	uint8_t para14;//����4.1
	uint8_t para15;//����4.2
	uint8_t para16;//����4.3
	uint8_t para17;//����4.4
	uint8_t para18;//����5.1
	uint8_t para19;//����5.2
	uint8_t para20;//����5.3
	uint8_t para21;//����5.4
	uint8_t para22;//����6.1
	uint8_t para23;//����6.2
	uint8_t para24;//����6.3
	uint8_t para25;//����6.4
	uint8_t para26;//����7.1
	uint8_t para27;//����7.2
	uint8_t para28;//����7.3
	uint8_t para29;//����7.4
	uint8_t para30;//����8.1
	uint8_t para31;//����8.2
	uint8_t para32;//����8.3
	uint8_t para33;//����8.4
	uint8_t para34;//����9.1
	uint8_t para35;//����9.2
	uint8_t para36;//����9.3
	uint8_t para37;//����9.4
	uint8_t para38;//����10.1
	uint8_t para39;//����10.2
	uint8_t para40;//����10.3
	uint8_t para41;//����10.4
	uint8_t para42;//���
	uint8_t checksum;
	uint8_t tail;
}COMPENSATE_PROTOCOL_T;

/****************************************************
	�ṹ����:	GETSNVERSION_PROTOCOL_T
	����:	��ȡ�汾�ż�оƬChipID
	���ߣ�liyao 2016��5��11��09:13:26
	��:	
****************************************************/
typedef struct{
	uint8_t head;
	uint8_t type;
	uint8_t para1; //���� 0x01��ȡChipID�Ͱ汾��
	uint8_t para2; //Ԥ��
	uint8_t para3; //���
	uint8_t checksum;
	uint8_t tail;
}GETSNVERSION_PROTOCOL_T;

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
  HEAP_USE	     = TO_MODULE_ACTION(MAIN_MODULE, MOTOR_MODULE, 0xAA), 
  RUN_PROTOCOL	 = TO_MODULE_ACTION(MAIN_MODULE, MOTOR_MODULE, 0x01),   
  HEAD_PROTOCOL	 = TO_MODULE_ACTION(MAIN_MODULE, MOTOR_MODULE, 0x02),  
  WING_PROTOCOL	 = TO_MODULE_ACTION(MAIN_MODULE, MOTOR_MODULE, 0x03),  
  ANGLE_PROTOCOL	 = TO_MODULE_ACTION(MAIN_MODULE, MOTOR_MODULE, 0x04),  
  PDRCORRECT_PROTOCOL	 = TO_MODULE_ACTION(MAIN_MODULE, MOTOR_MODULE, 0x05),  
  ODOMETERSCRAM_PROTOCOL	 = TO_MODULE_ACTION(MAIN_MODULE, MOTOR_MODULE, 0x06),  
  COMPENSATE_PROTOCOL	 = TO_MODULE_ACTION(MAIN_MODULE, MOTOR_MODULE, 0x10),  
  GETSNVERSION_PROTOCOL	 = TO_MODULE_ACTION(MAIN_MODULE, MOTOR_MODULE, 0x11),  
  
   
  STATE_PROTOCOL	 = TO_MODULE_ACTION(MOTOR_MODULE, MAIN_MODULE, 0x01),
  ASK_PROTOCOL	 = TO_MODULE_ACTION(MOTOR_MODULE, MAIN_MODULE, 0x02),
  ODOMETER_PROTOCOL	 = TO_MODULE_ACTION(MOTOR_MODULE, MAIN_MODULE, 0x03),
  DEADRECKONING_PROTOCOL	 = TO_MODULE_ACTION(MOTOR_MODULE, MAIN_MODULE, 0x04),
  RESPOND_PROTOCOL	 = TO_MODULE_ACTION(MOTOR_MODULE, MAIN_MODULE, 0x05),
  SELFINS_PROTOCOL	 = TO_MODULE_ACTION(MOTOR_MODULE, MAIN_MODULE, 0x06),
  SENDSNVERSION_PROTOCOL	 = TO_MODULE_ACTION(MOTOR_MODULE, MAIN_MODULE, 0x07),
}MODULE_ACTION;
 
extern void Protocol_Init(void);
 















#endif


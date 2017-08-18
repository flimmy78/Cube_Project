#ifndef __FRAME_CONFIG_H__
#define __FRAME_CONFIG_H__
#include "usart.h"
//#define LL_LIB
#define DEBUG_USART   huart1 //���Դ���
#define PRINT_ERR 1          //�򿪵���
//#define UART1_DMA_SENDER 1   //���ڷ�����
//#define UART1_DMA_RECEIVER 100 //���ڽ�����
#define PROTOCOL_RESOLVER_IT_1 1 //�������ݽ�����
#define PROTOCOL_CHECKSUM 0
//#define LOG_OUT
#define MCU_TYPE 030
#define PROTOCOL_VERSION 3 //1 E01�ϰ汾Э��  2 E01S�°汾Э�� 3 �����ǻ�Э��
//#define LOAD_MONITOR_ON


#if MCU_TYPE == 030
	#define UID_ADDR 0x1FFFF7AC
#elif MCU_TYPE == 103
	#define UID_ADDR 0x1FFFF7E8
#endif



#endif


#ifndef __FRAME_CONFIG_H__
#define __FRAME_CONFIG_H__
#include "usart.h"
#define DEBUG_USART   huart1 //���Դ���
#define PRINT_ERR 1          //�򿪵���
#define UART1_DMA_SENDER 1   //���ڷ�����
#define UART1_DMA_RECEIVER 100 //���ڽ�����
#define PROTOCOL_RESOLVER_1 1 //�������ݽ�����
#define PROTOCOL_CHECKSUM 0
#define LOG_OUT 1
#define MCU_TYPE 030
#define PROTOCOL_VERSION 1 //1 E01�ϰ汾Э��  2 E01S�°汾Э��
//#define LOAD_MONITOR_ON
#endif


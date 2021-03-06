#ifndef __FRAME_CONFIG_H__
#define __FRAME_CONFIG_H__
#include "usart.h"
#define DEBUG_USART   huart2 //调试串口
#define PRINT_ERR 1          //打开调试
#define UART1_DMA_SENDER 1   //串口发送者
#define UART1_DMA_RECEIVER 1 //串口接收者
#define UART2_DMA_SENDER 1   //串口发送者
#define UART2_DMA_RECEIVER 1 //串口接收者 
#define PROTOCOL_CHECKSUM 0
#define PROTOCOL_RESOLVER_1 1
#define PROTOCOL_RESOLVER_2 1
#define LOG_OUT 1
#define MCU_TYPE 030
#define PROTOCOL_VERSION 1 //1 E01老版本协议  2 E01S新版本协议
#endif


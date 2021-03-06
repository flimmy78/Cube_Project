#ifndef __FRAME_CONFIG_H__
#define __FRAME_CONFIG_H__
#include "usart.h"
#define DEBUG_USART   huart1 //调试串口
#define UART1_DMA_SENDER 1   //串口发送者
#define UART1_DMA_RECEIVER 1 //串口接收者
#define UART2_DMA_SENDER 1   //超声波串口发送者
#define UART2_DMA_RECEIVER 1 //超声波串口接收者
#define UART3_DMA_SENDER 1   //蓝牙串口发送者
#define UART3_DMA_RECEIVER 1 //蓝牙串口接收者
#define UART4_DMA_SENDER 1   //MPU6050串口发送者
#define UART4_DMA_RECEIVER 1 //MPU6050串口接收者

#define PROTOCOL_RESOLVER_2 1//串口数据解析者
#define PROTOCOL_RESOLVER_3 1//串口数据解析者
#define PROTOCOL_RESOLVER_4 1//串口数据解析者
//#define PROTOCOL_RESOLVER_2 1


#define LOG_OUT 1
#define PROTOCOL_VERSION  2  //1 E01老版本协议  2 E01S新版本协议
#define PROTOCOL_CHECKSUM 0  //校验和是否生效
#define MCU_TYPE 103
#define PRINT_ERR 1
#endif


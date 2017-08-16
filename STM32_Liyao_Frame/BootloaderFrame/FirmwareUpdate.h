#ifndef __FIRMWARE_UPDATE_H__
#define __FIRMWARE_UPDATE_H__
#include <stdint.h>

//#define PRINT_ERR
#define USART USART4	
#define PAGE_SIZE          0x400U
#define UART_FLAG_TC                        (0x00000040U)              /*!< UART transmission complete                */
/*----------Э���ʽ--------------------*/
/*
									������дЭ��
֡ͷ ָ��  ���ȸ�8 ���ȵ�8 �̼�У��� Э��У��� 	֡β
0xFD 0xA0   0xXX    0xXX  		0xXX			 0xXX			0xF8

									������תЭ��
֡ͷ ָ��  ���ȸ�8 ���ȵ�8 �̼�У��� Э��У��� 	֡β
0xFD 0xA1   0xXX    0x00  		0x00			 0xXX			0xF8


����ֵ��error1    У��ʹ���
				error2		�������յ����ݣ�������½���
				error3		�̼�У��ʹ���
*/
typedef struct {
	uint8_t Head;
	uint16_t Firmware_Size;
	uint16_t Firmware_Check_Sum;
	uint8_t Firmware_Real_Sum;
	uint8_t Tail;
	uint8_t State;
	
	uint8_t Type;//֡����
	uint8_t Len;//֡����
	uint8_t Sum;//֡У���
	
}Firmware_Info_T;
extern Firmware_Info_T* Firmware_Info;

extern void FirmwareUpdate_Init(void);
extern void SYSTICK_IRQHandler(void);
extern void FirmwareUpdate_Run(void);
extern void UART_IRQHandler(void);
void Uart_Init(void);
#endif




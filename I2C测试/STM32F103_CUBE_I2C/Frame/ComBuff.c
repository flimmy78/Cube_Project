#include "ComBuff.h"
#include "stm32f1xx_hal.h"
#include "LOG.h" 
#include <stdio.h>
#include <string.h>  
//--------------------------�ڲ���������-----------------------------
//UART1 ���շ����߶���ͻ������ռ����
#ifdef UART1_DMA_SENDER 
	DMA_Sender_T Uart1_DMA_Sender = {0}; 
#endif 
#ifdef UART1_DMA_RECEIVER 
	DMA_Receiver_T Uart1_DMA_Receiver = {0}; 
#endif 
 
//--------------------------�ڲ���������-----------------------------
int8_t _ReadByte(DMA_Receiver_T* udr,uint8_t* data);
int16_t _ReadTo(DMA_Receiver_T* udr, uint8_t value, uint8_t *data, uint8_t len);
int16_t _Read(DMA_Receiver_T* udr, uint8_t *data, uint8_t len); 
	
int8_t _Write(DMA_Sender_T* uds, uint8_t *data, uint8_t len);
int8_t _WriteByte(DMA_Sender_T* uds,uint8_t data);
int8_t _KeepTransmit(DMA_Sender_T* uds);
/****************************************************
	������:	ComBuff_Init
	����:		��ʼ��ȫ��ͨ�Ż�����
	����:		liyao 2016��4��4��22:02:12 
****************************************************/
void ComBuff_Init(void){ 
//UART1 ��ʼ��
#ifdef UART1_DMA_SENDER
  Uart1_DMA_Sender.DMA_Send_Queue = Queue_Link_Init(0);
  Uart1_DMA_Sender.Uart = &huart1;
  Uart1_DMA_Sender.Dma = huart1.hdmatx;
  Uart1_DMA_Sender.Write = _Write;
  Uart1_DMA_Sender.WriteByte = _WriteByte;
  Uart1_DMA_Sender.KeepTransmit = _KeepTransmit;
#endif 

#ifdef UART1_DMA_RECEIVER
  Uart1_DMA_Receiver.DMA_Recv_Queue = Queue_Link_Init(0);
  Uart1_DMA_Receiver.Uart = &huart1;
  Uart1_DMA_Receiver.Dma = huart1.hdmarx;
  Uart1_DMA_Receiver.Recv_Cur = 0;
  Uart1_DMA_Receiver.Reversal = 0;
  Uart1_DMA_Receiver.Data     = MALLOC(UART1_DMA_RECV_SIZE);
  Uart1_DMA_Receiver.DMA_BuffSize = UART1_DMA_RECV_SIZE;
  Uart1_DMA_Receiver.ReadByte = _ReadByte;
  Uart1_DMA_Receiver.ReadTo = _ReadTo;
  Uart1_DMA_Receiver.Read = _Read;
#endif  
}

/****************************************************
	������:	_Write
	����:	ͨ������DMA���̷���һ������
	����:	������ , �������
	����ֵ���ɹ�0 ʧ��-1
	����:	liyao 2016��9��14��10:55:11
****************************************************/
int8_t _Write(DMA_Sender_T* uds,uint8_t* data, uint8_t len){ 
  if(uds->OverFlag){//����δ�������뻺����
    Log.info("DMA BUSY���뻺��\r\n");
    Queue_Link_Put(uds->DMA_Send_Queue, data, len); 
    return 0;
  }
  uint16_t cnt = 0;
  if((cnt = Queue_Link_OutSize(uds->DMA_Send_Queue)) > 0){
    uds->Data = MALLOC(cnt); 
    MALLOC_CHECK(uds->Data,"_Write");
    Queue_Link_Get(uds->DMA_Send_Queue, uds->Data);
    uds->Len = cnt;
  }else{
    uds->Data = MALLOC(len);
    MALLOC_CHECK(uds->Data,"_Write");
    memcpy(uds->Data, data, len);
    uds->Len = len;
  }
  HAL_UART_Transmit_DMA(uds->Uart, uds->Data, uds->Len);
  uds->OverFlag = 1; 
  return 0;
}

/****************************************************
	������:	_WriteByte
	����:		ͨ�����ڼ��̷���1�ֽ�
	����:		������ , �������
	����ֵ���ɹ�0 ʧ��-1
	����:		liyao 2016��9��14��10:55:07
****************************************************/
int8_t _WriteByte(DMA_Sender_T* uds,uint8_t data){ 
	while(uds->OverFlag); 
    uds->Data = MALLOC(1); 
    MALLOC_CHECK(uds->Data,"_Write");
    uds->Data[0] = data;
	  HAL_UART_Transmit_DMA(uds->Uart, uds->Data, 1); 
	  uds->OverFlag = 1; 
	return 0;
}


/****************************************************
	������:	_ReadByte
	����:		�Ӵ��ڻ�������ȡ1���ֽ�
	����:		������ , �������
	����ֵ���ɹ�0 ʧ��-1
	����:		liyao 2016��9��8��15:17:44
****************************************************/
int8_t _ReadByte(DMA_Receiver_T* udr,uint8_t* data){ 
	uint16_t maxCur = (udr->DMA_BuffSize -  udr->Uart->hdmarx->Instance->CNDTR);
	if(udr->Reversal == 1 && maxCur >= udr->Recv_Cur){
		Log.error("DMA����������\r\n");
		return -1;
	}
	if((udr->Reversal == 0 && udr->Recv_Cur < maxCur) || udr->Reversal == 1){ 
		*data = udr->Data[udr->Recv_Cur]; 
		if(++udr->Recv_Cur % UART1_DMA_RECV_SIZE == 0)
			udr->Recv_Cur = udr->Reversal = 0;
		return 0;
	}
	return -1;
}

/****************************************************
	������:	_ReadTo
	����:		�Ӵ��ڻ�������ȡN���ֽ�,ֱ������value
	����:		������,ƥ��ֵ,��������,�������鳤��
	����ֵ���ɹ���ȡ�����ֽ��� ��������ָ������-1 ��������ռ䲻�� -2
	����:		liyao 2016��9��8��15:17:49	
****************************************************/
int16_t _ReadTo(DMA_Receiver_T* udr, uint8_t value, uint8_t *data, uint8_t len){
	uint16_t i = 0,tmp_Recv_Cur = udr->Recv_Cur, maxCur = (udr->DMA_BuffSize - udr->Uart->hdmarx->Instance->CNDTR);
	uint8_t tmp_Reversal = udr->Reversal;
	if(udr->Reversal == 1 && maxCur >= udr->Recv_Cur){
		Log.error("DMA����������\r\n");
		return -1;
	}
	for(i = 0; i < len; i++){ 
		if((tmp_Reversal == 0 && tmp_Recv_Cur < maxCur) || tmp_Reversal == 1){
			data[i] = udr->Data[tmp_Recv_Cur]; 
			if(++tmp_Recv_Cur % udr->DMA_BuffSize == 0)
				tmp_Recv_Cur = tmp_Reversal = 0;
			if(data[i] == value){
				udr->Recv_Cur = tmp_Recv_Cur;
				udr->Reversal = tmp_Reversal;
				return i+1;
			}
		}else{
			//Log.waring("��ָ������\r\n");
			return -1;
		}
	}
	Log.error("��������ռ䲻������ָ������\r\n");
	return -2;
}

/****************************************************
	������:	_Read
	����:		�Ӵ��ڻ�������ȡN���ֽ�
	����:		������,��������,�������鳤��
	����ֵ���ɹ���ȡ�����ֽ��� ������������-1 
	����:		liyao 2016��9��8��15:17:49	
****************************************************/
int16_t _Read(DMA_Receiver_T* udr, uint8_t *data, uint8_t len){
	uint16_t i = 0,cnt = 0, maxCur = (udr->DMA_BuffSize - udr->Uart->hdmarx->Instance->CNDTR); 
	if(udr->Reversal == 1 && maxCur >= udr->Recv_Cur){
		Log.error("DMA����������\r\n");
		return -1;
	}
	for(i = 0; i < len; i++){
		if((udr->Reversal == 0 && udr->Recv_Cur < maxCur) || udr->Reversal == 1){
			data[i] = udr->Data[udr->Recv_Cur]; 
			cnt++;
			if(++udr->Recv_Cur % udr->DMA_BuffSize == 0)
				udr->Recv_Cur = udr->Reversal = 0;
		}else{
			return cnt;
		}
	} 
	return -1;
}

/****************************************************
	������:	_KeepTransmit
	����:		����Ƶ�����ݱ����뻺��ʱ����Ҫ�˺����������ͻ���������
	����:		������
	����ֵ��dmaδ�����귵��-1���ɹ�����0 
  ����:		liyao 2016��11��4��14:26:35	
****************************************************/
int8_t _KeepTransmit(DMA_Sender_T* uds){
  if(uds->OverFlag){//����δ�������뻺���� 
    return -1;
  }
  uint16_t cnt = 0;
  if((cnt = Queue_Link_OutSize(uds->DMA_Send_Queue)) > 0){
    uds->Data = MALLOC(cnt); 
    MALLOC_CHECK(uds->Data,"_Write");
    Queue_Link_Get(uds->DMA_Send_Queue, uds->Data);
    uds->Len = cnt; 
    HAL_UART_Transmit_DMA(uds->Uart, uds->Data, uds->Len);
  } 
  return 0;
}
 
/****************************************************
	������:	fputc
	����: 	printf�ض���
	����:		liyao 2015��9��8��14:10:51
****************************************************/
int fputc(int ch,FILE *f)
{
//	#ifndef PRINT_ERR
//		Queue_Put(Uart_Tx_Queue, &ch); 
//	#else
  while(HAL_UART_Transmit(&DEBUG_USART, (uint8_t*)&ch, 1, 100) == HAL_BUSY);
//	#endif
	return(ch);	   
}
 

/****************************************************
	������:	Buff_To_Uart
	����:		�ӻ�������ȡ��һ���ֽڷ���������
	����:		liyao 2015��9��8��14:10:51
****************************************************/
void Buff_To_Uart(void){
	int32_t data;
	uint8_t count = 5;
	while(count--)
		if(Queue_Get(Uart_Tx_Queue,&data) == 0){ 
			#ifdef UART1_DMA_SENDER
				Uart1_DMA_Sender.WriteByte(&Uart1_DMA_Sender,data);
			#else
				USART_SendData_Block(DEBUG_USART, data);  
			#endif 
		}else{
			break;
		}
}
 

//--------------------------------ϵͳ�ص�����-------------------------------------

 
/****************************************************
        ������: HAL_UART_TxCpltCallback
        ����:   DMA��������ж� �ص�
        ����:   liyao 2016��9��14��10:55:11
****************************************************/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
  #ifdef UART1_DMA_SENDER
    if(huart == Uart1_DMA_Sender.Uart){
        Uart1_DMA_Sender.OverFlag = 0;
        FREE(Uart1_DMA_Sender.Data);
    }
  #endif
} 


/****************************************************
        ������: HAL_UART_RxCpltCallback
        ����:   DMA��������ж� �ص�
        ����:   liyao 2016��9��14��10:55:11
****************************************************/ 
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
  #ifdef UART1_DMA_RECEIVER
    if(huart == Uart1_DMA_Receiver.Uart){
      if(++Uart1_DMA_Receiver.Reversal == 2)
        Log.error("USART1_DMA_�������ݱ�����\r\n");
    } 
    HAL_UART_Receive_DMA(huart, Uart1_DMA_Receiver.Data, Uart1_DMA_Receiver.DMA_BuffSize);
  #endif 
}
 



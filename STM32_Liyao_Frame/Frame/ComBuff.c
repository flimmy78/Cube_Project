#include "ComBuff.h"
#if MCU_TYPE == 103
  #include "stm32f1xx_hal.h"
#elif MCU_TYPE == 030 
  #include "stm32f0xx_hal.h"
#endif
#include "LOG.h" 
#include <stdio.h>
#include <string.h>  
#include "ProtocolFrame.h"
#include "TaskTimeManager.h"
//--------------------------�ڲ���������-----------------------------
//UART1 ���շ����߶���ͻ������ռ����
#ifdef UART1_DMA_SENDER 
	DMA_Sender_T Uart1_DMA_Sender = {0}; 
#endif 
#ifdef UART1_DMA_RECEIVER 
	DMA_Receiver_T Uart1_DMA_Receiver = {0};  
#endif 

#ifdef UART2_DMA_SENDER 
	DMA_Sender_T Uart2_DMA_Sender = {0}; 
#endif 
#ifdef UART2_DMA_RECEIVER 
	DMA_Receiver_T Uart2_DMA_Receiver = {0}; 
#endif 
#ifdef UART3_DMA_SENDER 
	DMA_Sender_T Uart3_DMA_Sender = {0}; 
#endif 
#ifdef UART3_DMA_RECEIVER 
	DMA_Receiver_T Uart3_DMA_Receiver = {0}; 
#endif 
#ifdef UART4_DMA_SENDER 
	DMA_Sender_T Uart4_DMA_Sender = {0}; 
#endif 
#ifdef UART4_DMA_RECEIVER 
	DMA_Receiver_T Uart4_DMA_Receiver = {0}; 
#endif 
//--------------------------�ڲ���������-----------------------------
#if UART1_DMA_RECEIVER || UART2_DMA_RECEIVER || UART3_DMA_RECEIVER || UART4_DMA_RECEIVER
static int8_t _ReadByte(DMA_Receiver_T* udr,uint8_t* data);
static int16_t _ReadTo(DMA_Receiver_T* udr, uint8_t value, uint8_t *data, uint8_t len);
static int16_t _Read(DMA_Receiver_T* udr, uint8_t *data, uint8_t len); 
#endif

#if UART1_DMA_SENDER	|| UART2_DMA_SENDER || UART3_DMA_SENDER || UART4_DMA_SENDER
static int8_t _Write(DMA_Sender_T* uds, uint8_t *data, uint8_t len);
static int8_t _WriteByte(DMA_Sender_T* uds,uint8_t data);
static int8_t _KeepTransmit(DMA_Sender_T* uds);
#endif

	
#if UART1_DMA_SENDER	|| UART2_DMA_SENDER || UART3_DMA_SENDER || UART4_DMA_SENDER
/****************************************************
	������:	DMA_Sender_Init
	����:		��ʼ��������
	����:		liyao 2016��11��16��
****************************************************/
void DMA_Sender_Init(DMA_Sender_T* Uart_DMA_Sender, UART_HandleTypeDef* huart){
  Uart_DMA_Sender->DMA_Send_Queue = Queue_Link_Init(0);
  Uart_DMA_Sender->Uart = huart;
  Uart_DMA_Sender->Write = _Write;
  Uart_DMA_Sender->WriteByte = _WriteByte;
  Uart_DMA_Sender->KeepTransmit = _KeepTransmit;
}
#endif

#if UART1_DMA_RECEIVER || UART2_DMA_RECEIVER || UART3_DMA_RECEIVER || UART4_DMA_RECEIVER
/****************************************************
	������:	DMA_Receiver_Init
	����:		��ʼ��������
	����:		liyao 2016��11��16��
****************************************************/
void DMA_Receiver_Init(DMA_Receiver_T* Uart_DMA_Receiver, UART_HandleTypeDef* huart,uint16_t BuffSize){
 
  Uart_DMA_Receiver->DMA_Recv_Queue = Queue_Link_Init(0);
  Uart_DMA_Receiver->Uart = huart;
  Uart_DMA_Receiver->Recv_Cur = 0;
  Uart_DMA_Receiver->Reversal = 0;
  Uart_DMA_Receiver->Data     = MALLOC(BuffSize);
  MALLOC_CHECK(Uart_DMA_Receiver->Data,"");
  Uart_DMA_Receiver->DMA_BuffSize = BuffSize;
  Uart_DMA_Receiver->ReadByte = _ReadByte;
  Uart_DMA_Receiver->ReadTo = _ReadTo;
  Uart_DMA_Receiver->Read = _Read;
  huart->hdmarx->Instance->CNDTR = BuffSize;
	
  HAL_UART_Receive_DMA(huart, Uart_DMA_Receiver->Data, BuffSize); 
}
#endif

/****************************************************
	������:	ComBuff_Init
	����:		��ʼ��ȫ��ͨ�Ż�����
	����:		liyao 2016��4��4��22:02:12 
****************************************************/
void ComBuff_Init(void){
//UART1 ��ʼ��
#ifdef UART1_DMA_SENDER
  DMA_Sender_Init(&Uart1_DMA_Sender, &huart1);
#endif  
#ifdef UART1_DMA_RECEIVER
  DMA_Receiver_Init(&Uart1_DMA_Receiver, &huart1, UART1_DMA_RECV_SIZE); 
#endif  
#ifdef UART2_DMA_SENDER
  DMA_Sender_Init(&Uart2_DMA_Sender, &huart2);
#endif  
#ifdef UART2_DMA_RECEIVER
  DMA_Receiver_Init(&Uart2_DMA_Receiver, &huart2, UART2_DMA_RECV_SIZE); 
#endif 
#ifdef UART3_DMA_SENDER
  DMA_Sender_Init(&Uart3_DMA_Sender, &huart3);
#endif  
#ifdef UART3_DMA_RECEIVER
  DMA_Receiver_Init(&Uart3_DMA_Receiver, &huart3, UART3_DMA_RECV_SIZE); 
#endif
#ifdef UART4_DMA_SENDER
  DMA_Sender_Init(&Uart4_DMA_Sender, &huart4);
#endif  
#ifdef UART4_DMA_RECEIVER
  DMA_Receiver_Init(&Uart4_DMA_Receiver, &huart4, UART4_DMA_RECV_SIZE); 
#endif 
#ifdef __TASKTIMEMANAGER_H__
	/*-----------������������-----------------*/
  TaskTime_Add(TaskID++, TimeCycle(0,30), SenderKeepTransmit, Real_Mode);
	/*-----------Э���������-----------------*/
  TaskTime_Add(TaskID++, TimeCycle(0,30), PaddingProtocol, Real_Mode);
#endif
 }
#if UART1_DMA_SENDER	|| UART2_DMA_SENDER || UART3_DMA_SENDER || UART4_DMA_SENDER
/****************************************************
	������:	_Write
	����:	ͨ������DMA���̷���һ������
	����:	������ , �������
	����ֵ���ɹ�0 ʧ��-1
	����:	liyao 2016��9��14��10:55:11
****************************************************/
static int8_t _Write(DMA_Sender_T* uds,uint8_t* data, uint8_t len){ 
  if(uds->OverFlag){//����δ�������뻺����
    Log.info("DMA BUSY���뻺��\r\n");
    Queue_Link_Put(uds->DMA_Send_Queue, data, len); 
    return 0;
  }
  uint16_t cnt = 0;
  if((cnt = Queue_Link_OutSize(uds->DMA_Send_Queue)) > 0){
    Queue_Link_Put(uds->DMA_Send_Queue, data, len); 
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
static int8_t _WriteByte(DMA_Sender_T* uds,uint8_t data){ 
	while(uds->OverFlag) ;
    uds->Data = MALLOC(1); 
    MALLOC_CHECK(uds->Data,"_Write");
    uds->Data[0] = data;
		uds->Len = 1;
	  HAL_UART_Transmit_DMA(uds->Uart, uds->Data, uds->Len); 
	  uds->OverFlag = 1; 
	return 0;
}

/****************************************************
	������:	_KeepTransmit
	����:		����Ƶ�����ݱ����뻺��ʱ����Ҫ�˺����������ͻ���������
	����:		������
	����ֵ��dmaδ�����귵��-1���ɹ�����0 
  ����:		liyao 2016��11��4��14:26:35	
****************************************************/
static int8_t _KeepTransmit(DMA_Sender_T* uds){
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
#endif

#if UART1_DMA_RECEIVER || UART2_DMA_RECEIVER || UART3_DMA_RECEIVER || UART4_DMA_RECEIVER
/****************************************************
	������:	_ReadByte
	����:		�Ӵ��ڻ�������ȡ1���ֽ�
	����:		������ , �������
	����ֵ���ɹ�0 ʧ��-1
	����:		liyao 2016��9��8��15:17:44
****************************************************/
static int8_t _ReadByte(DMA_Receiver_T* udr,uint8_t* data){ 
	uint16_t maxCur = (udr->DMA_BuffSize -  udr->Uart->hdmarx->Instance->CNDTR);
	if(udr->Reversal == 1 && maxCur >= udr->Recv_Cur){
		Log.error("DMA����������,������ǰ��������\r\n");
		udr->Recv_Cur = udr->Reversal = 0;//���������Ѻ��ݴ���������
		return -1;
	}
	if((udr->Reversal == 0 && udr->Recv_Cur < maxCur) || udr->Reversal == 1){ 
		*data = udr->Data[udr->Recv_Cur]; 
		if(++udr->Recv_Cur % udr->DMA_BuffSize == 0)
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
static int16_t _ReadTo(DMA_Receiver_T* udr, uint8_t value, uint8_t *data, uint8_t len){
	uint16_t i = 0,tmp_Recv_Cur = udr->Recv_Cur, maxCur = (udr->DMA_BuffSize - udr->Uart->hdmarx->Instance->CNDTR);
	uint8_t tmp_Reversal = udr->Reversal;
	if(udr->Reversal == 1 && maxCur >= udr->Recv_Cur){
		Log.error("DMA����������,������ǰ��������\r\n");
		udr->Recv_Cur = udr->Reversal = 0;//���������Ѻ��ݴ���������
		
		return -1;
	}
  if((udr->Recv_Cur ==  maxCur) && (0 == udr->Reversal))
    return 0;
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
static int16_t _Read(DMA_Receiver_T* udr, uint8_t *data, uint8_t len){
	uint16_t i = 0,cnt = 0, maxCur = (udr->DMA_BuffSize - udr->Uart->hdmarx->Instance->CNDTR); 
	if(udr->Reversal == 1 && maxCur >= udr->Recv_Cur){
		Log.error("DMA����������,������ǰ��������\r\n");
		udr->Recv_Cur = udr->Reversal = 0;//���������Ѻ��ݴ���������
		return -1;
	}
  if((udr->Recv_Cur ==  maxCur) && (0 == udr->Reversal))
    return 0;
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
	if(i == len)
			return cnt;
	return -1;
}
#endif

 
/****************************************************
	������:	fputc
	����: 	printf�ض���
	����:		liyao 2015��9��8��14:10:51
****************************************************/
__weak int fputc(int ch,FILE *f)
{
//	#ifndef PRINT_ERR
//		Queue_Put(Uart_Tx_Queue, &ch); 
//	#else
#ifdef LL_LIB
	LL_USART_TransmitData8(DEBUG_USART,ch);
#else
		while( HAL_UART_Transmit(&DEBUG_USART, (uint8_t*)&ch, 1, 100) == HAL_BUSY);
#endif
//  
	//Uart1_DMA_Sender.Write(&Uart1_DMA_Sender,(uint8_t*)&ch, 1);
	//while(HAL_UART_Transmit(&huart3, (uint8_t*)&ch, 1, 100) == HAL_BUSY);
//	#endif
	return(ch);	   
}
 

/****************************************************
	������:	Buff_To_Uart
	����:		�ӻ�������ȡ��һ���ֽڷ���������
	����:		liyao 2015��9��8��14:10:51
****************************************************/
void Buff_To_Uart(void){
//	int32_t data;
//	uint8_t count = 5;
//	while(count--)
//		if(Queue_Get(Uart_Tx_Queue,&data) == 0){ 
//			#ifdef UART1_DMA_SENDER
//				Uart1_DMA_Sender.WriteByte(&Uart1_DMA_Sender,data);
//			#else
////				USART_SendData_Block(DEBUG_USART, data);  
//			#endif 
//		}else{
//			break;
//		}
}
 

//--------------------------------ϵͳ�ص�����-------------------------------------

#if UART1_DMA_SENDER	|| UART2_DMA_SENDER || UART3_DMA_SENDER || UART4_DMA_SENDER
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
  #ifdef UART2_DMA_SENDER
    if(huart == Uart2_DMA_Sender.Uart){
        Uart2_DMA_Sender.OverFlag = 0;
        FREE(Uart2_DMA_Sender.Data);
    }
  #endif
  #ifdef UART3_DMA_SENDER
    if(huart == Uart3_DMA_Sender.Uart){
        Uart3_DMA_Sender.OverFlag = 0;
        FREE(Uart3_DMA_Sender.Data);
    }
  #endif
  #ifdef UART4_DMA_SENDER
    if(huart == Uart4_DMA_Sender.Uart){
        Uart4_DMA_Sender.OverFlag = 0;
        FREE(Uart4_DMA_Sender.Data);
    }
  #endif    
} 
#endif

/****************************************************
        ������: HAL_UART_RxCpltCallback
        ����:   DMA��������ж� �ص�
        ����:   liyao 2016��9��14��10:55:11
****************************************************/ 
#if  UART1_DMA_RECEIVER || UART2_DMA_RECEIVER || UART3_DMA_RECEIVER || UART4_DMA_RECEIVER
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
  #ifdef UART1_DMA_RECEIVER
    if(huart == Uart1_DMA_Receiver.Uart){
      if(++Uart1_DMA_Receiver.Reversal == 2){
        Log.error("USART1_DMA_�������ݱ�����\r\n");
				Uart1_DMA_Receiver.Reversal = 0;
			}
      HAL_UART_Receive_DMA(huart, Uart1_DMA_Receiver.Data, Uart1_DMA_Receiver.DMA_BuffSize); 
    } 
  #endif 
  #ifdef UART2_DMA_RECEIVER
    if(huart == Uart2_DMA_Receiver.Uart){
      if(++Uart2_DMA_Receiver.Reversal == 2){
        Log.error("USART2_DMA_�������ݱ�����\r\n");
				Uart2_DMA_Receiver.Reversal = 0;
			}
      HAL_UART_Receive_DMA(huart, Uart2_DMA_Receiver.Data, Uart2_DMA_Receiver.DMA_BuffSize); 
    } 
  #endif 
  #ifdef UART3_DMA_RECEIVER
    if(huart == Uart3_DMA_Receiver.Uart){
      if(++Uart3_DMA_Receiver.Reversal == 2){
        Log.error("USART3_DMA_�������ݱ�����\r\n");
				Uart3_DMA_Receiver.Reversal = 0;
			}
      HAL_UART_Receive_DMA(huart, Uart3_DMA_Receiver.Data, Uart3_DMA_Receiver.DMA_BuffSize); 
    } 
  #endif  
  #ifdef UART4_DMA_RECEIVER
    if(huart == Uart4_DMA_Receiver.Uart){
      if(++Uart4_DMA_Receiver.Reversal == 2){
        Log.error("USART4_DMA_�������ݱ�����\r\n");
				Uart4_DMA_Receiver.Reversal = 0;
			}
      HAL_UART_Receive_DMA(huart, Uart4_DMA_Receiver.Data, Uart4_DMA_Receiver.DMA_BuffSize); 
    } 
  #endif      
}
#endif
 
#if  UART1_DMA_RECEIVER || UART2_DMA_RECEIVER || UART3_DMA_RECEIVER || UART4_DMA_RECEIVER ||\
		 UART1_DMA_SENDER	  || UART2_DMA_SENDER   || UART3_DMA_SENDER   || UART4_DMA_SENDER
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart){
	#ifdef UART1_DMA_SENDER
	if(huart == Uart1_DMA_Sender.Uart){
	  HAL_UART_Transmit_DMA(Uart1_DMA_Sender.Uart, Uart1_DMA_Sender.Data, Uart1_DMA_Sender.Len); 
		Log.error("huart1 TX DMA error\r\n");
	}
	#endif
	#ifdef UART2_DMA_SENDER
	if(huart == Uart2_DMA_Sender.Uart){
	  HAL_UART_Transmit_DMA(Uart2_DMA_Sender.Uart, Uart2_DMA_Sender.Data, Uart2_DMA_Sender.Len); 
		Log.error("huart2 TX DMA error\r\n");
	}
	#endif
	#ifdef UART3_DMA_SENDER
	if(huart == Uart3_DMA_Sender.Uart){
	  HAL_UART_Transmit_DMA(Uart3_DMA_Sender.Uart, Uart3_DMA_Sender.Data, Uart3_DMA_Sender.Len); 
		Log.error("huart3 TX DMA error\r\n");
	}
	#endif
	#ifdef UART4_DMA_SENDER
	if(huart == Uart4_DMA_Sender.Uart){
	  HAL_UART_Transmit_DMA(Uart4_DMA_Sender.Uart, Uart4_DMA_Sender.Data, Uart4_DMA_Sender.Len); 
		Log.error("huart4 TX DMA error\r\n");
	}
	#endif
	
	#ifdef UART1_DMA_RECEIVER
	if(huart == Uart1_DMA_Receiver.Uart){
		HAL_UART_Receive_DMA(huart, Uart1_DMA_Receiver.Data, UART1_DMA_RECV_SIZE); 
		Log.error("huart1 RX DMA error\r\n");
	}
	#endif
	#ifdef UART2_DMA_RECEIVER
	if(huart == Uart2_DMA_Receiver.Uart){
		HAL_UART_Receive_DMA(huart, Uart2_DMA_Receiver.Data, UART2_DMA_RECV_SIZE);
		Log.error("huart2 RX DMA error\r\n"); 
	}
	#endif
	#ifdef UART3_DMA_RECEIVER
	if(huart == Uart3_DMA_Receiver.Uart){
		HAL_UART_Receive_DMA(huart, Uart3_DMA_Receiver.Data, UART3_DMA_RECV_SIZE);
		Log.error("huart3 RX DMA error\r\n"); 
	}
	#endif
	#ifdef UART4_DMA_RECEIVER
	if(huart == Uart4_DMA_Receiver.Uart){
		HAL_UART_Receive_DMA(huart, Uart4_DMA_Receiver.Data, UART4_DMA_RECV_SIZE); 
		Log.error("huart4 RX DMA error\r\n");
	}
	#endif
}
#endif
//--------------------------------��ݵ���-------------------------------------
#if UART1_DMA_SENDER	|| UART2_DMA_SENDER || UART3_DMA_SENDER || UART4_DMA_SENDER
/****************************************************
        ������: SenderKeepTransmit
        ����:   �������䷢���߻������еĻ�������
        ����:   liyao 2016��9��14��10:55:11
****************************************************/   
__weak void SenderKeepTransmit(void){
#if UART1_DMA_SENDER == 1
  Uart1_DMA_Sender.KeepTransmit(&Uart1_DMA_Sender);
#endif
#if UART2_DMA_SENDER == 1
  Uart2_DMA_Sender.KeepTransmit(&Uart2_DMA_Sender);
#endif
#if UART3_DMA_SENDER == 1
  Uart3_DMA_Sender.KeepTransmit(&Uart3_DMA_Sender);
#endif
#if UART4_DMA_SENDER == 1
  Uart4_DMA_Sender.KeepTransmit(&Uart4_DMA_Sender);
#endif
}
#else
__weak void SenderKeepTransmit(void){;}
#endif

#if  (PROTOCOL_RESOLVER_1 && UART1_DMA_RECEIVER) || (PROTOCOL_RESOLVER_2 && UART2_DMA_RECEIVER)|| (PROTOCOL_RESOLVER_3 && UART3_DMA_RECEIVER) || (PROTOCOL_RESOLVER_4 && UART4_DMA_RECEIVER)
/****************************************************
        ������: PaddingProtocol
        ����:   �Ӵ��ڻ������ж�ȡ���ݵ�Э�������
        ����:   liyao 2016��9��14��10:55:11
****************************************************/ 
__weak void PaddingProtocol(void){
	#define BUFFSIZE 100
	int8_t cnt = 0;
	uint8_t data[BUFFSIZE] = {0};  
	#if PROTOCOL_RESOLVER_1 && UART1_DMA_RECEIVER
    if((cnt = Uart1_DMA_Receiver.ReadTo(&Uart1_DMA_Receiver,0xf8,data,BUFFSIZE))>0)
      ProtocolResolver_1->Protocol_Put(ProtocolResolver_1,data,cnt);
  #endif
	#if PROTOCOL_RESOLVER_2 && UART2_DMA_RECEIVER
		if((cnt = Uart2_DMA_Receiver.ReadTo(&Uart2_DMA_Receiver,0xf8,data,BUFFSIZE))>0)
			ProtocolResolver_2->Protocol_Put(ProtocolResolver_2,data,cnt);  
	#endif
	#if PROTOCOL_RESOLVER_3 && UART3_DMA_RECEIVER
		if((cnt = Uart3_DMA_Receiver.ReadTo(&Uart3_DMA_Receiver,0xf8,data,BUFFSIZE))>0)
			ProtocolResolver_3->Protocol_Put(ProtocolResolver_3,data,cnt);  
	#endif
	#if PROTOCOL_RESOLVER_4 && UART4_DMA_RECEIVER
		if((cnt = Uart4_DMA_Receiver.ReadTo(&Uart4_DMA_Receiver,0xf8,data,BUFFSIZE))>0)
			ProtocolResolver_4->Protocol_Put(ProtocolResolver_4,data,cnt);  
	#endif
}
#else
__weak void PaddingProtocol(void){;}
#endif

#if PROTOCOL_RESOLVER_IT_1 || PROTOCOL_RESOLVER_IT_2 || PROTOCOL_RESOLVER_IT_3 || PROTOCOL_RESOLVER_IT_4
__weak uint8_t  PaddingProtocol_IT(void){
#if PROTOCOL_RESOLVER_IT_1
	if(((READ_REG(USART1->ISR) & USART_ISR_RXNE) != RESET) && ((READ_REG(USART1->CR1) & USART_CR1_RXNEIE) != RESET)){
		uint8_t UartData =  READ_REG(USART1->RDR);
		ProtocolResolver_1->Protocol_Put(ProtocolResolver_1,&UartData,1);
		return 1;
	}
#endif
#if PROTOCOL_RESOLVER_IT_2
	if(((READ_REG(USART2->ISR) & USART_ISR_RXNE) != RESET) && ((READ_REG(USART2->CR1) & USART_CR1_RXNEIE) != RESET)){
		uint8_t UartData =  READ_REG(USART2->RDR);
		ProtocolResolver_2->Protocol_Put(ProtocolResolver_2,&UartData,1);
		return 1;
	}
#endif
#if PROTOCOL_RESOLVER_IT_3
	if(((READ_REG(USART3->ISR) & USART_ISR_RXNE) != RESET) && ((READ_REG(USART3->CR1) & USART_CR1_RXNEIE) != RESET)){
		uint8_t UartData =  READ_REG(USART3->RDR);
		ProtocolResolver_3->Protocol_Put(ProtocolResolver_3,&UartData,1);
		return 1;
	}
#endif
#if PROTOCOL_RESOLVER_IT_4
	if(((READ_REG(USART4->ISR) & USART_ISR_RXNE) != RESET) && ((READ_REG(USART4->CR1) & USART_CR1_RXNEIE) != RESET)){
		uint8_t UartData =  READ_REG(USART4->RDR);
		ProtocolResolver_4->Protocol_Put(ProtocolResolver_4,&UartData,1);
		return 1;
	}
#endif
	return 0;
}
#endif
//--------------------------------printfʵ��-------------------------------------
void printch(char ch);
void printdec(int dec);
void printflt(double flt);
void printstr(char* str);
void printbin(int bin);
void printhex(int hex);
void StringFormat(char* target, uint8_t type,const char* fmt,va_list vp);

void printch(char ch)
{
#ifdef LL_LIB
		LL_USART_TransmitData8(DEBUG_USART,ch);
#else
	while(HAL_UART_Transmit(&DEBUG_USART, (uint8_t*)&ch, 1, 100) == HAL_BUSY);
#endif
}

void mvprintf(const char* fmt,va_list vp){
	StringFormat(NULL, 0, fmt, vp);
}

void msprintf(char* target,const char* fmt,va_list vp){
	StringFormat(target, 1, fmt, vp);
}

void mprintf(const char* fmt, ...)
{
    va_list vp;
    va_start(vp, fmt);
		mvprintf(fmt, vp);
    va_end(vp);
}

void StringFormat(char* target,uint8_t type,const char* fmt,va_list vp){
	double vargflt = 0;
    int  vargint = 0;
    char* vargpch = NULL;
    char vargch = 0;
    const char* pfmt = NULL;
    pfmt = fmt;

    while(*pfmt)
    {
        if(*pfmt == '%')
        {
            switch(*(++pfmt))
            {
                
                case 'c':
                    vargch = va_arg(vp, int); 
                    /*    va_arg(ap, type), if type is narrow type (char, short, float) an error is given in strict ANSI
                        mode, or a warning otherwise.In non-strict ANSI mode, 'type' is allowed to be any expression. */
                    printch(vargch);
                    break;
                case 'd':
                case 'i':
                    vargint = va_arg(vp, int);
                    printdec(vargint);
                    break;
                case 'f':
                    vargflt = va_arg(vp, double);
                    /*    va_arg(ap, type), if type is narrow type (char, short, float) an error is given in strict ANSI
                        mode, or a warning otherwise.In non-strict ANSI mode, 'type' is allowed to be any expression. */
                    printflt(vargflt);
                    break;
                case 's':
                    vargpch = va_arg(vp, char*);
                    printstr(vargpch);
                    break;
                case 'b':
                case 'B':
                    vargint = va_arg(vp, int);
                    printbin(vargint);
                    break;
                case 'x':
                case 'X':
                    vargint = va_arg(vp, int);
                    printhex(vargint);
                    break;
                case '%':
                    printch('%');
                    break;
                default:
                    break;
            }
            pfmt++;
        }
        else
        {
						if(type == 0)
							printch(*pfmt++);
						else if(type == 1)
							*target++ = *pfmt++;
        }
    }
}


void printdec(int dec)
{
	char buff[20] = {0};
	int8_t i = 0;
	int num = dec;
	do{
		num /= 10;
		i++;
	}while(num != 0);
	
	if(dec < 0){
		buff[0] = '-';
		i++;
	}
	while(--i >= 0){
		buff[i] = dec%10 + '0';
		dec /= 10;
	}
	printstr(buff);
}

void printflt(double flt)
{
    int tmpint = 0;
    tmpint = (int)flt;
    printdec(tmpint);
    printch('.');
    flt = flt - tmpint;
    tmpint = (int)(flt * 1000000);
    printdec(tmpint);
}

void printstr(char* str)
{
    while(*str)
    {
        printch(*str++);
    }
}

void printbin(int bin)
{
    if(bin == 0)
    {
        printstr("0b");
        return;
    }
    printbin(bin/2);
    printch( (char)(bin%2 + '0'));
}

void printhex(int hex)
{
    if(hex==0)
    {
        printstr("0x");
        return;
    }
    printhex(hex/16);
    if(hex < 10)
    {
        printch((char)(hex%16 + '0'));
    }
    else
    {
        printch((char)(hex%16 - 10 + 'a' ));
    }
}



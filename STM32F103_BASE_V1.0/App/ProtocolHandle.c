#include "ProtocolHandle.h"
#include <stdio.h>
#include "ComBuff.h" 
//#include <iar_dlmalloc.h>

void Heap_Use(Protocol_Info_T* pi){
  //__iar_dlmalloc_stats();
	printf("��ע��\r\n");
}
 
void HeartBeat_P_Handle(Protocol_Info_T* pi){
	HeartBeat_P_T* hp = pi->ParameterList;
	printf("�յ�����:%d\r\n", hp->para1); 
	Protocol_Send_Transpond(pi);
//	Uart1_DMA_Sender.WriteByte(&Uart1_DMA_Sender,'C');
}
 


 

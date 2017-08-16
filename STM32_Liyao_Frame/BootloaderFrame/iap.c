#include "iap.h"
#include <string.h>
#include "stm32f0xx_hal.h" 
IAP_FUN  JumpApp;
__ASM void MSR_MSP(unsigned int addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}


//��ת��Ӧ�ó����
//AppAddr:�û�������ʼ��ַ.
void iap_load_app(unsigned int AppAddr)
{
	if(((*(unsigned int*)AppAddr)&0x2FFE0000)==0x20000000)	//���ջ����ַ�Ƿ�Ϸ�.
	{
		JumpApp = (IAP_FUN)*(unsigned int*)(AppAddr+4);				//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
		MSR_MSP(*(unsigned int*)AppAddr);					     				//��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
		__set_PRIMASK(1);															 				// �ر�ȫ���ж�
		JumpApp();									    						   				//��ת��APP.
	}
}



#include "ChipTools.h"
#include <stdlib.h>
#include <string.h>
uint32_t UID[3];
uint32_t UID_M;
void ChipTools_Init(void){
	/*----------��ȡоƬΨһID------------*/
	memcpy(UID, (void*)UID_ADDR, 12);
	UID_M=(UID[0]>>1)+(UID[1]>>2)+(UID[2]>>3);
	srand(UID_M); 
}
/****************************************************
	������:	CheckEndian
	����:		��С���б�
	����:		liyao 2017��7��26��21:15:10 
****************************************************/
void CheckEndian(void)
{  
	union { char c[4]; unsigned long l; }
	endian_test = { { 'l', '?', '?', 'b' } };
  printf("%c\r\n", ((char)endian_test.l));
}  




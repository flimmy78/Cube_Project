#include <stdint.h>

#define PAGE_COUNT GRAM_SIZE/2048+1
typedef  void (*IAP_FUN)(void);				//����һ���������͵Ĳ���.
#define  FLASH_APP_ADDR		0x08001000  	//��һ��Ӧ�ó�����ʼ��ַ(�����FLASH)

void iap_load_app(unsigned int appxaddr);


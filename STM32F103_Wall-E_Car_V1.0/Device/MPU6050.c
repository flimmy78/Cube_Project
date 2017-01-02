#include "MPU6050.h"
#include <stdlib.h>

MPU6050_T _MPU6050;
MPU6050_T* MPU6050;
static void _Put_Byte(MPU6050_T* mpu6050,uint8_t data){
	if(data == 0x53 && mpu6050->Last_Para == 0x55){
		mpu6050->head = 0x55;
		mpu6050->type = 0x53;
		mpu6050->count = 0;
		return;
	}
	mpu6050->Last_Para = data;
	if(mpu6050->head == 0)
		return;
	*((&mpu6050->para1)+mpu6050->count) = data;
	mpu6050->count++;
	if(mpu6050->count == 9){
		uint8_t Real_Sum = 0;
		for(uint8_t i = 0; i < 10; i++){
			Real_Sum += *((uint8_t*)mpu6050+i);
		}
		if(mpu6050->checksum != Real_Sum){//У���
			//printf("%X,%X\r\n", mpu6050->checksum,Real_Sum);
			mpu6050->MPU6050_Clean(mpu6050);
			return;
		}
 
		mpu6050->X = ((short)((mpu6050->para2<<8) | mpu6050->para1))*180/32768 ;
		mpu6050->Y = ((short)((mpu6050->para4<<8) | mpu6050->para3))*180/32768 ;
		
		
		mpu6050->Z = ((short)((mpu6050->para6<<8) | mpu6050->para5))*180/32768 ; 
		mpu6050->T = ((short)((mpu6050->para8<<8) | mpu6050->para7))/340.0+36.53 ;  
		mpu6050->Pitch = mpu6050->X - mpu6050->Y;
		mpu6050->Roll  = mpu6050->X + mpu6050->Y;
		mpu6050->IsReady = 1;
		//}
	}
}


/*static int16_t _Value_Limit(int16_t val){
	if(val > 180)
		return 180;
	else if(val < 0)
		return 0;
	else
		return val;
}*/

MPU6050_T _Get_MPU6050(MPU6050_T* mpu6050){
//	for(uint8_t i = 0; i < 11; i++){//ÿ����11�������ֽ�
//		mpu6050->Put_Byte(mpu6050,_Uart4_Buff[22+i]);//��������Ϊ�Ƕ���Ϣ 
//	}
	if(mpu6050->IsReady == 0)
		return *mpu6050; 
//	int16_t Z_O = 0;
	mpu6050->MPU6050_Clean(mpu6050);
	mpu6050->_Last_X = mpu6050->X;
	mpu6050->_Last_Y = mpu6050->Y;
	mpu6050->_Last_Z = mpu6050->Z; 
	mpu6050->_Last_T = mpu6050->T; 
	
	
	/*mpu6050->X = 360-(((mpu6050->para2<<8) | mpu6050->para1)*180/32768)-180;
	mpu6050->Y = ((mpu6050->para4<<8) | mpu6050->para3)*180/32768;
	Z_O = ((mpu6050->para6<<8) | mpu6050->para5)*180/32768;
	if(Z_O >= 180){
		mpu6050->Z = 90 - (360 - Z_O);
	}else{
		mpu6050->Z = Z_O + 90;
	}
	if(mpu6050->Z < 0)
		mpu6050->Z = 180;
	mpu6050->T = ((short)(mpu6050->para8<<8) | mpu6050->para7)/340.0+36.53; */
	/*mpu6050->X = _Value_Limit(mpu6050->X);
	mpu6050->Y = _Value_Limit(mpu6050->Y);
	mpu6050->Z = _Value_Limit(mpu6050->Z);
	mpu6050->T = _Value_Limit(mpu6050->T);*/ 
	mpu6050->MPU6050_Clean(mpu6050);
	return *mpu6050;
}

void _MPU6050_Clean(MPU6050_T* mpu6050){
	mpu6050->head = mpu6050->type = mpu6050->count = mpu6050->IsReady = 0;
}
void MPU6050_Init(void){
	MPU6050= &_MPU6050;
	memset(MPU6050, 0, sizeof(MPU6050_T)); 
	MPU6050->Put_Byte = _Put_Byte;
	MPU6050->Get_MPU6050 = _Get_MPU6050;
	MPU6050->MPU6050_Clean = _MPU6050_Clean;
	
	//PID_Init(&MPU6050->pid_p, 0.1, 0.03, 0, 1, PWM_MAX); 
	//PID_Init(&MPU6050->pid_r, 0.1, 0.03, 0, 1, PWM_MAX); 
}
	





















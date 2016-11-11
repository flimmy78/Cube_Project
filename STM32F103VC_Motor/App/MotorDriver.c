//###########################################################################
//
// FILE:    MotorDrive.c
//
// TITLE:   �����������������
//
// ASSUMPTIONS:
//
//
// DESCRIPTION:
//         
//
//###########################################################################
#include "stm32f1xx_hal.h"
#include "main.h"  
#include "MotorWingDriver.h"
#include "MotorHeadDriver.h" 
#include <stdio.h>
#include <string.h>
#include "MotorDriver.h"

#define HEADSPEEDMAX 100
#define WINGSPEEDMAX 100
 
MOTOR_HEAD_PARM Motor_H;	//ͷ�����
MOTOR_WING_PARM Motor_WL;	//���
MOTOR_WING_PARM Motor_WR;	//�ҳ�

void MotorInt(void);
void ScanMotorLimit(void); 

uint8_t PowerFlag = 0; 
uint8_t CheckPower(void){//�ߵ�ƽ���� �͵�ƽ�쳣
	//printf("%d,%d\r\n",GPIO_ReadInputDataBit(GPIOB, GPIO_PIN_13) , GPIO_ReadInputDataBit(GPIOB, GPIO_PIN_14));
	return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) & HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14);//22V24V������
}

void ScanMotorLimit(void){
	ScanHeadMotorLimit();
	ScanWingMotorLimit();
}

/****************************************************
������:MotorInt
��ע: �����ʼ��
****************************************************/
void MotorInt(void)	// ��ʼ��������� 
{
	
	Motor_H.MotorHeadInit = MotorHeadInit;
	Motor_WL.MotorWingInit = MotorWingInit;
	Motor_WR.MotorWingInit = MotorWingInit;
	 
	Motor_H.MotorHeadInit(&Motor_H, ID_MOTOR_H, TIM4, GPIOD, GPIO_PIN_11);
	Motor_WL.MotorWingInit(&Motor_WL, ID_MOTOR_LC, TIM4, GPIOB, GPIO_PIN_10);
	Motor_WR.MotorWingInit(&Motor_WR, ID_MOTOR_RC, TIM4, GPIOB, GPIO_PIN_9);
}
 
























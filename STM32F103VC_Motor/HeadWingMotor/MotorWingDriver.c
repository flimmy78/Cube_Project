 
#include "MotorDriver.h"
#include "MotorWingDriver.h" 
#include <stdio.h>
#include <string.h>
#include "TaskTimeManager.h"
static void LimitTrigger(MOTOR_WING_PARM* motor);
static void Motor_WProtectCheck(MOTOR_WING_PARM* motor);
static void ExecSpeedPWM(TIM_TypeDef* TIMx, int16_t PWM);
/****************************************************
������:MotorWingInit
����: �����ʵ���ʼ��
����:	��Ң 2015��9��14��13:44:23
****************************************************/	 
void MotorWingInit(MOTOR_WING_PARM* motor,MOTOR_ID motor_id, TIM_TypeDef* TIMx,	GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin){
	memset(motor, 0,sizeof(MOTOR_WING_PARM)); 
	
	motor->TIMx = TIMx;
	motor->GPIOx = GPIOx;
	motor->GPIO_Pin = GPIO_Pin;
	motor->Angle_Code = 0.45;
	motor->Encoder = ENCODER_CON_RES;							//��ʼ��������ֵ
	motor->EncoderLast = ENCODER_CON_RES;							//��ʼ��������ֵ
	motor->LimitState = WING_LIMIT_FALSE;					//��ʼ����λΪ��
	motor->Speed_Code = (WING_MAX_PWM - WING_BASE_PWM) / 100;			//�����ٶȶ�Ӧ��PWM
	motor->SelfState = SELF_W_ERROR;
	motor->Gain_PWM = 1;
	motor->EncoderMAX = WING_ENC_MAX;
	
	motor->ExecSpeedPWM = ExecSpeedPWM;						//��ֵPWM���ú���
	motor->ExecuteWingMotor = ExecuteWingMotor;		//��ִֵ�к���
	motor->StopWingMotor = StopWingMotor;					//��ֵֹͣ����	
	motor->LimitTrigger = LimitTrigger;						//��ֵ��λ��������
	motor->KeepWingMotor = KeepWingMotor;					//��ֵ���ֺ���	
	motor->MotorProtectCheck = Motor_WProtectCheck; //��ֵ�����������
	motor->SetWingMotorParam = SetWingMotorParam;	//��ֵ���ò�������
}

/****************************************************
������:_Motor_Bottom
����: �������λ
����:	��Ң 2015��9��14��13:44:23
****************************************************/	 
int8_t _Motor_Bottom(MOTOR_WING_PARM* motor_WL,MOTOR_WING_PARM* motor_WR, uint16_t speed){
	int8_t Finish_L = 0,Finish_R = 0;
	motor_WL->MotorDir = motor_WR->MotorDir = DIR_WING_D;
	HAL_GPIO_WritePin(motor_WL->GPIOx, motor_WL->GPIO_Pin, (GPIO_PinState)DIR_WING_D);
	HAL_GPIO_WritePin(motor_WR->GPIOx, motor_WR->GPIO_Pin, (GPIO_PinState)DIR_WING_D);
	motor_WL->ExecSpeedPWM(motor_WL->TIMx, speed);
	motor_WR->ExecSpeedPWM(motor_WR->TIMx, speed); 
	motor_WL->EncoderLast = motor_WL->Encoder;
	motor_WR->EncoderLast = motor_WR->Encoder;
	
	do 
	{ 
		DelayMS(300);
		motor_WL->Diff_Encoder = motor_WL->Encoder - motor_WL->EncoderLast;
		motor_WL->EncoderLast = motor_WL->Encoder;
		//����������λ
		if( GET_LIMIT_WING_L == WING_LIMIT_TRUE && Finish_L == 0)
		{
		  motor_WL->StopWingMotor(motor_WL);
			LimitTrigger(motor_WL);
			Finish_L = 1;
			#ifdef PRINT_ERR 	        
		    printf("�����¹�λ���\r\n");
			#endif
		}
		
		
		 //�ҵ��������λ
		if(GET_LIMIT_WING_R == WING_LIMIT_TRUE && Finish_R == 0)
		{
			motor_WR->StopWingMotor(motor_WR);
			LimitTrigger(motor_WR);
			Finish_R = 1;
			#ifdef PRINT_ERR 
		    printf("�ҳ���¹�λ���\r\n");
			#endif
		}
		motor_WR->Diff_Encoder = motor_WR->Encoder - motor_WR->EncoderLast;
		motor_WR->EncoderLast = motor_WR->Encoder;
		
		if(motor_WL->Diff_Encoder == 0 && Finish_L == 0){
			motor_WL->StopWingMotor(motor_WL);
			Finish_L = -1;
			#ifdef PRINT_ERR 	        
		    printf("�����¹�λ��ʱ\r\n");
			#endif
		}
		if(motor_WR->Diff_Encoder == 0 && Finish_R == 0){
			motor_WR->StopWingMotor(motor_WR);
			Finish_R = -1;
			#ifdef PRINT_ERR 	        
		    printf("�ҳ���¹�λ��ʱ\r\n");
			#endif
		}
   }while( !(Finish_L && Finish_R));
	 motor_WL->Encoder = motor_WR->Encoder = 0;
	return Finish_L + Finish_R;
}
/****************************************************
������:_Motor_TryUP
����: �����ܹ��˶�����СPWM
����:	��Ң 2015��9��14��13:44:23
****************************************************/	 
void _Motor_TryUP(MOTOR_WING_PARM* motor_WL,MOTOR_WING_PARM* motor_WR){
	int8_t Finish_L = 0,Finish_R = 0;
	motor_WL->MotorDir = motor_WR->MotorDir = DIR_WING_U;
	HAL_GPIO_WritePin(motor_WL->GPIOx, motor_WL->GPIO_Pin, (GPIO_PinState)DIR_WING_U);
	HAL_GPIO_WritePin(motor_WR->GPIOx, motor_WR->GPIO_Pin, (GPIO_PinState)DIR_WING_U);
	
	do{
		motor_WL->ExecSpeedPWM(motor_WL->TIMx, motor_WL->PWM_Base);
		if(Finish_L == 0)
			motor_WL->PWM_Base += 10;
		
		if(motor_WL->EncoderSUM != 0 && Finish_L == 0){
			Finish_L = 1;
			#ifdef PRINT_ERR 
		    printf("������С�ܶ�PWM:%d\r\n",motor_WL->PWM_Base);
			#endif
		}
		
		if(motor_WL->PWM_Base == WING_MAX_PWM){
			Finish_L = 1;
			motor_WL->PWM_Base = 0;
			#ifdef PRINT_ERR 
		    printf("����������С�ܶ�PWMʧ��\r\n");
			#endif
		}
		motor_WR->ExecSpeedPWM(motor_WR->TIMx, motor_WR->PWM_Base);
		if(Finish_R == 0)
			motor_WR->PWM_Base += 10;
		if(motor_WR->EncoderSUM != 0 && Finish_R == 0){
			Finish_R = 1;
			#ifdef PRINT_ERR 
		    printf("�ҳ����С�ܶ�PWM:%d\r\n",motor_WR->PWM_Base);
			#endif
		}
		
		if(motor_WR->PWM_Base == WING_MAX_PWM){
			Finish_R = 1;
			motor_WR->PWM_Base = 0;
			#ifdef PRINT_ERR 
		    printf("�����ҳ����С�ܶ�PWMʧ��\r\n");
			#endif
		}
		DelayMS(25);
		//printf("%d,%d,������%f\r\n",motor_WL->PWM_Base,motor_WR->PWM_Base,motor_WR->Gain_PWM);
	
	}while(!(Finish_L && Finish_R));
	/*if(motor_WL->PWM_Base >0 && motor_WR->PWM_Base>0){
		if(motor_WL->PWM_Base > motor_WR->PWM_Base){
			motor_WL->Gain_PWM = (float)motor_WL->PWM_Base / motor_WR->PWM_Base;
			printf("������%f\r\n", motor_WR->Gain_PWM);
		}else{
			motor_WR->Gain_PWM = (float)motor_WR->PWM_Base / motor_WL->PWM_Base;
			printf("%d,%d,������%f\r\n",motor_WL->PWM_Base,motor_WR->PWM_Base,motor_WR->Gain_PWM);
		}
	}*/
	
}
/****************************************************
������:_Motor_Top
����: ������˶�������
����:	��Ң 2015��9��14��13:44:23
****************************************************/	 
int8_t _Motor_Top(MOTOR_WING_PARM* motor_WL,MOTOR_WING_PARM* motor_WR){
	int8_t Finish_L = 0,Finish_R = 0;
	motor_WL->EncoderSUM = motor_WR->EncoderSUM = 0;
	motor_WL->MotorDir = motor_WR->MotorDir = DIR_WING_U;
	HAL_GPIO_WritePin(motor_WL->GPIOx, motor_WL->GPIO_Pin, (GPIO_PinState)DIR_WING_U);
	HAL_GPIO_WritePin(motor_WR->GPIOx, motor_WR->GPIO_Pin, (GPIO_PinState)DIR_WING_U);
	motor_WL->ExecSpeedPWM(motor_WL->TIMx, WING_INIT_PWM);
	motor_WR->ExecSpeedPWM(motor_WR->TIMx, WING_INIT_PWM);
	motor_WL->EncoderLast = motor_WL->Encoder;
	motor_WR->EncoderLast = motor_WR->Encoder;
	do{
		DelayMS(300);
		motor_WL->Diff_Encoder = motor_WL->Encoder - motor_WL->EncoderLast;
		motor_WL->EncoderLast = motor_WL->Encoder;
		//�������ﶥ��
		if(motor_WL->Diff_Encoder == 0 && Finish_L == 0)
		{
			motor_WL->EncoderMAX = motor_WL->EncoderSUM ;
			motor_WL->Angle_Code = motor_WL->EncoderMAX * 1.0 / WING_MAX_ANGLE;//1���Ӧ������ֵ
			Finish_L = 1;
			#ifdef PRINT_ERR 	        
			printf("�����Ϲ�λ���,Encoder_L:%d,Code:%f\r\n",motor_WL->EncoderSUM,motor_WL->Angle_Code);
			#endif
		}
		
		motor_WR->Diff_Encoder = motor_WR->Encoder - motor_WR->EncoderLast;
		motor_WR->EncoderLast = motor_WR->Encoder;
		//�ҵ�����ﶥ��
		if( motor_WR->Diff_Encoder == 0 && Finish_R == 0)
		{
			motor_WR->EncoderMAX = motor_WR->EncoderSUM ;
			motor_WR->Angle_Code = motor_WR->EncoderMAX * 1.0 / WING_MAX_ANGLE;//1���Ӧ������ֵ
			Finish_R = 1;
			
			#ifdef PRINT_ERR 
		    printf("�ҳ���Ϲ�λ���,Encoder_R:%d,Code:%f\r\n",motor_WR->EncoderSUM,motor_WR->Angle_Code);
			#endif
		}
		//��ȡ��¼������˶�����
		ReadCurrent();
		motor_WL->ElectricityLast = motor_WR->ElectricityLast = motor_WL->Electricity;
	}while( !(Finish_L && Finish_R));
	return Finish_L + Finish_R;
}

/****************************************************
������:Wing_Self_Inspection
����: ִ�г������ʼ��
����:	��Ң 2015��9��14s��13:44:23
****************************************************/	 
void Wing_Self_Inspection(){ 
	MOTOR_WING_PARM* motor_WL = &Motor_WL,* motor_WR = &Motor_WR;
	#ifdef PRINT_ERR 
		printf("����ʼ����ʼ\r\n");
	#endif
	//�����λ 
	if(_Motor_Bottom(motor_WL, motor_WR, WING_INIT_PWM) <= 0){
		#ifdef PRINT_ERR 
		printf("����¹�λ��ʱ\r\n");
		#endif
		
	}
	//_Motor_TryUP(motor_WL, motor_WR);
	//��������
	if(_Motor_Top(motor_WL, motor_WR) <= 0){
		#ifdef PRINT_ERR 
		printf("��������г�ʱ\r\n");
		#endif
	}
	//�����λ
	if(_Motor_Bottom(motor_WL, motor_WR, 100) <= 0){ 
		#ifdef PRINT_ERR 
		printf("����»�λ��ʱ\r\n");
		#endif
	}
	
	/*if(	motor_WL->EncoderMAX == 0	||
			motor_WL->Angle_Code == 0 ||
			motor_WR->EncoderMAX == 0	||
			motor_WR->Angle_Code == 0 
		){
			#ifdef PRINT_ERR 
			printf("�Լ�����쳣\r\n");
			#endif
			return;
		}*/
	motor_WL->SelfState = motor_WR->SelfState = SELF_W_DONE;
	#ifdef PRINT_ERR 
		printf("����ʼ�����\r\n");
	#endif
}
	
/****************************************************
������:ExecSpeedPWM
����: �������PWM
****************************************************/	 
void ExecSpeedPWM(TIM_TypeDef* TIMx, int16_t PWM){
	if(PWM > WING_MAX_PWM) PWM = WING_MAX_PWM;
	if(PWM < 0) PWM = 0;
	TIM_SetCompare1(TIMx, PWM);
}



/****************************************************
������:SetWingMotorParam
����: ���õ������
����:	���ʵ�壬�ٶȣ����򣬽Ƕȣ�0-90��
����:	��Ң 2015��9��14��13:44:23
****************************************************/
void SetWingMotorParam(MOTOR_WING_PARM* motor, uint16_t speed, int16_t angle){
	//if(motor->SelfState == SELF_W_ERROR) return;
	if(speed != 0)
		speed = speed * motor->Speed_Code + WING_BASE_PWM;
	
	if(angle > WING_MAX_ANGLE)  angle = WING_MAX_ANGLE;//Ŀ��ǶȺϷ���У��
	motor->Angle_CMD = angle;
	int16_t offset_angle = angle - motor->Angle_Cur;	//��������ڵ�ǰλ�õĽǶȲ�
	if(offset_angle > 0) 
		motor->MotorDirCMD = DIR_WING_U;
	else
		motor->MotorDirCMD = DIR_WING_D;
	offset_angle = offset_angle < 0?-offset_angle:offset_angle;//ȡ����ֵ
	
	motor->EncoderCMD = (offset_angle * motor->Angle_Code);		 //����Ƕ�ƫ���Ӧ����ֵ
	//printf("cur�Ƕȣ�%d,�Ƕ�enc��%d",motor->Angle_Cur,motor->EncoderCMD);
	motor->SpeedCMD = speed; 																	 //�����˶��ٶ�
	
}

/****************************************************
������:ExecuteWingMotor
����: ִ�е������
����:	��Ң 2015��9��14��13:44:23
****************************************************/	 
void ExecuteWingMotor(MOTOR_WING_PARM* motor){
	//ִ�е������
	if(motor->SpeedCMD == 0){
		motor->KeepWingMotor(motor);
		return;
	}
	motor->MotorDir = motor->MotorDirCMD;//��ʶ��ǰ����
	motor->Speed = motor->SpeedCMD;			 //��ʶ��ǰ�ٶ�
	motor->Encoder = motor->EncoderCMD;	 //Ŀ�����ֵȡֵ
	motor->RunState = MOTOR_W_RUN; 				 //���õ�ǰ�˶�״̬
	
	if(motor->Angle_CMD == 0){
		motor->MotorDir = motor->MotorDirCMD = DIR_WING_D;
		//motor->Encoder = motor->EncoderCMD = 800;
		motor->Down_Flag = 1;
	}
	/*if(motor->MotorDirCMD == DIR_WING_D){//����������Ϊ�������岢���ϻ���
		uint8_t para_speed = (motor->Speed - WING_BASE_PWM)/motor->Speed_Code;
		motor->Speed = WING_SLOWLY_PWM - (WING_SLOWLY_PWM / 100 * para_speed);  //����pwm
		HAL_GPIO_WritePin(motor->GPIOx, motor->GPIO_Pin, (GPIO_PinState)DIR_WING_U);
		motor->Speed = motor->PWM_Base;
	}*/
	HAL_GPIO_WritePin(motor->GPIOx, motor->GPIO_Pin, (GPIO_PinState)motor->MotorDirCMD);
	motor->ExecSpeedPWM(motor->TIMx, motor->Speed);//ִ�е���ٶ�
}

/****************************************************
������:StopWingMotor
����: ���ֹͣ
����:	��Ң 2015��9��14��13:44:23
****************************************************/
void StopWingMotor(MOTOR_WING_PARM* motor){
	if(motor->MotorDirCMD == DIR_WING_D)
		HAL_GPIO_WritePin(motor->GPIOx, motor->GPIO_Pin, (GPIO_PinState)DIR_WING_U);
	else if(motor->MotorDirCMD == DIR_WING_U)
		HAL_GPIO_WritePin(motor->GPIOx, motor->GPIO_Pin, (GPIO_PinState)DIR_WING_D); 
	motor->ExecSpeedPWM(motor->TIMx, 0);//���õ���ٶ�Ϊ0
	motor->Encoder = 0;
}

/****************************************************
������:KeepWingMotor
����: ���ֹͣ
����:	��Ң 2015��9��14��13:44:23
****************************************************/
void KeepWingMotor(MOTOR_WING_PARM* motor){
	motor->RunState = MOTOR_W_KEEP;												//���õ�ǰ�˶�״̬
	//motor->MotorDirCMD = DIR_WING_U;										//���÷���Ϊ����
	//motor->Speed = motor->SpeedCMD = motor->PWM_Base*0.7;//WING_KEEP_SPEED * motor->Speed_Code; 	//���ñ����ٶ�
	
	/*if(motor->MotorDirCMD == DIR_WING_U){
		HAL_GPIO_WritePin(motor->GPIOx, motor->GPIO_Pin, (GPIO_PinState)DIR_WING_D);
	}else if(motor->MotorDirCMD == DIR_WING_D){ 
		HAL_GPIO_WritePin(motor->GPIOx, motor->GPIO_Pin, (GPIO_PinState)DIR_WING_U);
	}*/
	HAL_GPIO_WritePin(motor->GPIOx, motor->GPIO_Pin, (GPIO_PinState)DIR_WING_U);
	motor->ExecSpeedPWM(motor->TIMx, WING_KEEP_PWM);				  //ִ�е���ٶ�
	//respond_protocol_send(&motor->Exec_Protocol);
	#ifdef PRINT_ERR 
	//printf("KEEP����\r\n");
	#endif
}

/****************************************************
������:LimitTrigger
����: �����λ����
����:	��Ң 2015��10��14��22:26:22
****************************************************/
void LimitTrigger(MOTOR_WING_PARM* motor){
	motor->EncoderSUM = 0;								//��ʼ��������Զ�λ
	motor->Protect_Time = 0;							//��ʼ����������
	motor->Angle_Cur = 0;									//��ʼ����ǰ�Ƕ�
	motor->LimitState = WING_LIMIT_TRUE;	//������λ��־
}

/****************************************************
������:Motor_WProtectCheck
����: ����������
����:	��Ң 2015��10��14��22:26:22
****************************************************/
void Motor_WProtectCheck(MOTOR_WING_PARM* motor){
	if(motor->RunState == MOTOR_W_STOP){		//�˶�״̬Ϊֹͣ
		motor->Protect_Time = 0;
	}else{
		if(motor->Protect_Time++ == 2000){		//�������ۼӵ�2000 * 30 = 60��
			motor->Protect_Time = 0;					//��ʼ������������
			motor->StopWingMotor(motor);			//���ֹͣ
			#ifdef PRINT_ERR 
			printf("�������������\r\n");
			#endif
		}
	}
}

/****************************************************
������:ScanWingMotorLimit
����: ɨ������λ���
����:	��Ң 2015��9��14��13:44:23
****************************************************/
void ScanWingMotorLimit(void){
	
	//�˶�������ָ��ʱ���ڱ�����û�б仯�����������
	Motor_WL.MotorProtectCheck(&Motor_WL);
	Motor_WR.MotorProtectCheck(&Motor_WR);
	//�������ʱ��˲ʱ����
	if(Motor_WL.Down_Flag == 1){
		Motor_WL.Down_Flag ++;
		Motor_WL.ExecSpeedPWM(Motor_WL.TIMx, WING_DOWN_PWM); 
	}else if(Motor_WL.Down_Flag == 2){
		Motor_WL.Down_Flag = 0;
		Motor_WL.ExecSpeedPWM(Motor_WL.TIMx, 0);
	}
	
	if(Motor_WR.Down_Flag == 1){
		Motor_WR.Down_Flag ++;
		Motor_WR.ExecSpeedPWM(Motor_WR.TIMx, WING_DOWN_PWM);
	}else if(Motor_WR.Down_Flag == 2){
		Motor_WR.Down_Flag = 0;
		Motor_WR.ExecSpeedPWM(Motor_WR.TIMx, 0);
	}
	//�����λ���
	if(GET_LIMIT_WING_L == WING_LIMIT_TRUE){
		Motor_WL.LimitTrigger(&Motor_WL);
		if(Motor_WL.MotorDirCMD == DIR_WING_D){	//��ֹ�����û���ü����Ͼʹ�����λ
			Motor_WL.StopWingMotor(&Motor_WL);//���ֹͣ
			Motor_WL.RunState = MOTOR_W_STOP;					//�����˶�״̬
		}
		#ifdef PRINT_ERR 
				//printf("�������λ\r\n");
		#endif
	}else{
		Motor_WL.LimitState = WING_LIMIT_FALSE; //������λ��־
	}

	//�ҳ���λ���
	if(GET_LIMIT_WING_R == WING_LIMIT_TRUE){
		Motor_WR.LimitTrigger(&Motor_WR);
		if(Motor_WR.MotorDirCMD == DIR_WING_D){	//��ֹ�����û���ü����Ͼʹ�����λ
			Motor_WR.StopWingMotor(&Motor_WR);//���ֹͣ
			Motor_WR.RunState = MOTOR_W_STOP;					//�����˶�״̬
		}
		#ifdef PRINT_ERR 
			//printf("�������λ\r\n");
		#endif
	}else{
		Motor_WR.LimitState = WING_LIMIT_FALSE; //������λ��־
	}
}




















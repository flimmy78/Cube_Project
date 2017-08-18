#include "InfraredDriver.h"
#include <string.h>
#include "tim.h"
#ifdef INFRARED_SEND
Infrared_s_t infrared_s;
Infrared_s_t* Infrared_s = &infrared_s;
void _InfraredSendBit(uint8_t PWM_Cnt, int16_t Bit);
void _InfraredClean_S(void);

/****************************************************
	������:	INF_S_TIM_PWM_PulseFinishedCallback
	����:		PWMÿ���ڻص�
	����:		liyao 2017��8��16��
****************************************************/
void INF_S_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim){
	if(htim == &INF_CAL_TIM){
		if(Infrared_s->PWM_Cnt > 0){	//���巢����
			Infrared_s->PWM_Cnt--;
		}else{	//���巢����ϴ���״̬��
			switch (Infrared_s->InfState){
				case InfStart_S://״̬��InfStart_S��������Start�ź�
					Infrared_s->InfState = InfHop_S;
					_InfraredClean_S();
					Infrared_s->Data = ((uint8_t*)&Infrared_s->BuffData)[Infrared_s->BuffIndex++];//׼��Ҫ���͵�����
					_InfraredSendBit(PWM_START,INF_START);//���Ϳ�ʼ�ź�
					break;
				case InfHop_S:	//״̬��InfHop_S����������������
					Infrared_s->InfState = InfSData_S;
					_InfraredSendBit(PWM_BIT,__HAL_TIM_GET_COMPARE(&INF_CAL_TIM, INF_CHANNEL) == INF_RESET?INF_SET:INF_RESET);//���͵�ǰ�෴����
					break;
				case InfSData_S://״̬��InfSData_S������������λ
					Infrared_s->InfState = InfCheck_S;
					if((Infrared_s->Data >> (7 - Infrared_s->Index++))& 0x01){//����bitλ
						_InfraredSendBit(PWM_BIT,INF_SET);
					}else{
						_InfraredSendBit(PWM_BIT,INF_RESET);
					}
					break;
				case InfCheck_S://״̬��InfCheck_S��������Ƿ�����һ���ֽڻ��߷������
					Infrared_s->InfState = InfHop_S;//��ת��������������״̬
					if(Infrared_s->Index == 8){//��鵱ǰ�ֽ��Ƿ������
						Infrared_s->Index = 0;
						if(Infrared_s->BuffIndex < sizeof(Infrared_Data_t)){
							Infrared_s->Data = ((uint8_t*)&Infrared_s->BuffData)[Infrared_s->BuffIndex++];//������һ�ֽ�
						}else{
							Infrared_s->InfState = InfSStop_S;//��ת��ֹͣ״̬
						}
					}
//					_InfraredSendBit(PWM_BIT,__HAL_TIM_GET_COMPARE(&INF_CAL_TIM, INF_CHANNEL) == INF_RESET?INF_SET:INF_RESET);
					break;
				case InfSStop_S://״̬��InfSStop_S��������Stop�ź�
					Infrared_s->InfState = InfFree_S;//��־���ݽ������
					_InfraredSendBit(PWM_STOP,INF_STOP);//����ֹͣ�ź�
					HAL_TIM_PWM_Stop_IT(&INF_CAL_TIM, INF_CHANNEL);//�ر�PWM
					break;
				case InfFree_S:
					break;
			}
		}
	}
}
	
void Infrared_s_Init(void){
	memset(Infrared_s, 0, sizeof(Infrared_s_t));
}

/****************************************************
	������:	_InfraredClean
	����:		����Infrared����
	����:		liyao 2017��8��16��
****************************************************/
void _InfraredClean_S(void){
	Infrared_s->Data = Infrared_s->Index = Infrared_s->PWM_Cnt = Infrared_s->BuffIndex = 0;
}

/****************************************************
	������:	_InfraredSendBit
	����:		����Bit
	����:		liyao 2017��8��16��
****************************************************/
void _InfraredSendBit(uint8_t PWM_Cnt, int16_t Bit){
	Infrared_s->PWM_Cnt = PWM_Cnt;
	__HAL_TIM_SET_COUNTER(&INF_CAL_TIM, 0);
	__HAL_TIM_SET_COMPARE(&INF_CAL_TIM, INF_CHANNEL, Bit);
}

/****************************************************
	������:	InfraredSendData
	����:		����1�ֽں�������
	����:		liyao 2017��8��16��
	��ע:		start�źţ��͵�ƽ800us��+����ʼλ+ data + ~data + �ߵ�ƽ����
					�����ݼ��400us
****************************************************/
int InfraredSendData(Infrared_s_t* Inf, Infrared_Data_t* Data){
	if(Inf->InfState != InfFree_S){
		return -1;
	} 
	
	
	Inf->InfState = InfStart_S; 
	memcpy(&Inf->BuffData, Data, sizeof(Infrared_Data_t));
	__HAL_TIM_SET_COMPARE(&INF_CAL_TIM, INF_CHANNEL, INF_SET);
	HAL_TIM_PWM_Start_IT(&INF_CAL_TIM, INF_CHANNEL);
	return 0;
}
#endif


#ifdef INFRARED_RECV
Infrared_r_t infrared_r;
Infrared_r_t* Infrared_r = &infrared_r;
/****************************************************
	������:	INF_R_GPIO_EXTI_Callback
	����:		��������������жϻص�
	����:		liyao 2017��8��16��
****************************************************/
void INF_R_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == INF_DATA_Pin){	
		if(Infrared_r->InfState == InfNULL_R)	//����ϵͳδ��ʼ��ʱͻ���ж�
			return;
		if(Infrared_r->InfState == InfReady_R && HAL_GPIO_ReadPin(INF_DATA_GPIO_Port, INF_DATA_Pin) == GPIO_PIN_RESET){ //�׸��½����ж�
			Infrared_r->InfState = InfStart_R;		//�ƶ�״̬��
			INF_TIMER_START();//��ʱ��ʼ
		}else if(Infrared_r->InfState == InfStart_R && HAL_GPIO_ReadPin(INF_DATA_GPIO_Port, INF_DATA_Pin) == GPIO_PIN_SET){//���׸�������
			uint16_t Timed = INF_TIMER_GETCOUNT();
			if(Timed > INF_START_DELAY && Timed < (INF_START_DELAY+INF_DATA_DELAY)){//�����½��ص������ؾ�����ʱ���Ƿ���Ͽ�ʼ�ź�
				Infrared_r->InfState = InfRcvData_R;//�ƶ�״̬��				
				INF_TIMER_START_CD(INF_DATA_DELAY); //����ʱ��ʼ
			}else{		//�����Ͽ�ʼ�ź�����״̬��
				Infrared_r->InfState = InfReady_R;
				INF_TIMER_STOP();
			}
		}else if(Infrared_r->InfState == InfWaitHop_R){	//�յ������ź� ������ʱ����ʱ�ж�
			Infrared_r->InfState = InfRcvData_R;			//�ƶ�״̬��
			INF_TIMER_START_CD(INF_DATA_DELAY);				//����ʱ��ʼ
		}
	}
}

/****************************************************
	������:	INF_R_TIM_PeriodElapsedCallback
	����:		�����ʱ���ö�ʱ���ص�
	����:		liyao 2017��8��16��
****************************************************/
void INF_R_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim == &INF_DELAY_TIM){
		HAL_TIM_Base_Stop_IT(&INF_DELAY_TIM);
		if(Infrared_r->InfState == InfRcvData_R){		//�������ʱ���״̬
			Infrared_r->InfState = InfWaitHop_R;			//�ƶ�״̬��
			uint8_t bit = (uint8_t)HAL_GPIO_ReadPin(INF_DATA_GPIO_Port, INF_DATA_Pin); //����
			Infrared_r->Recv_Data |= (bit << (7 - Infrared_r->Recv_Cnt));							 //����
			HAL_GPIO_TogglePin(SYNC_GPIO_Port, SYNC_Pin);	//��ת������
			if(Infrared_r->Recv_Cnt++ == 7){	//1�ֽ�����
				uint8_t* data = (uint8_t*)&Infrared_r->InfData;
				data[Infrared_r->InfIndex++] = Infrared_r->Recv_Data;//���������
				if(Infrared_r->InfIndex == sizeof(Infrared_Data_t)){ //����������
					Infrared_r->InfState = InfFullData_R;//�������������
					HAL_GPIO_WritePin( SYNC_GPIO_Port, SYNC_Pin,GPIO_PIN_RESET);
					return;
				}else{//������δ�������������¸��ֽ�
					Infrared_r->InfState = InfWaitHop_R;
					Infrared_r->Recv_Cnt = Infrared_r->Recv_Data = 0; 	 //��0
				}
			}
			INF_TIMER_START_CD(INF_DATA_TIMEOUT);				//����ȴ���ʱ����ʱ��ʼ
		}else if(Infrared_r->InfState == InfWaitHop_R){//ָ��ʱ����δ�������� ��������
			Infrared_r->InfState = InfReady_R;
			Infrared_r->Recv_Cnt = Infrared_r->Recv_Data = Infrared_r->InfIndex = 0; 	 //��0
		}
	}
}

/****************************************************
	������:	InfraredRecvData
	����:		�������ݽ��պ���
	����:		liyao 2017��8��16��
****************************************************/
int InfraredRecvData(Infrared_r_t* Inf_r, Infrared_Data_t* Data){
	if(Inf_r->InfState == InfFullData_R){
		memcpy(Data, &Inf_r->InfData, sizeof(Infrared_Data_t));
		Infrared_r->InfIndex = Infrared_r->Recv_Cnt = Infrared_r->Recv_Data = 0;
		Infrared_r->InfState = InfReady_R;
		return 1;
	}
	return -2;
}

/****************************************************
	������:	Infrared_r_Init
	����:		�������ݽ��ճ�ʼ������
	����:		liyao 2017��8��16��
****************************************************/
void Infrared_r_Init(void){
	memset(Infrared_r, 0, sizeof(Infrared_r_t));
	Infrared_r->InfState = InfReady_R;//Infrared_r->InfIndex
}

#endif


#include "UltrasonicDriver.h"
extern Ultrasonic_T Ultrasonic;
void InfraredSendSelf(void){
	if(Ultrasonic.ID){
		Log.error("������δ����ID\r\n");
		return;
	}
	Infrared_Data_t data = {0};
	data.ID = Ultrasonic.ID;
	data.CMD = INF_SELF_CMD;
	data.Data = 0;
	data.CheckSum = (uint8_t)(data.ID+data.CMD+data.Data);
	InfraredSendData(Infrared_s, &data);
	
}












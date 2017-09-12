#include "TaskTimeManager.h"  
#include "LOG.h"
//static TaskTime_T TaskTimeLink[TASK_MAX_COUNT] = {0};
#ifdef LOAD_MONITOR_ON 
uint32_t TIMER_Tmp = 0, TIMER_Bak = 0;
#endif
TimeTaskInfo_T OSInfo;	//ϵͳ����״̬
TaskTime_T* TaskTime_Head = &OSInfo.TThead;//����ͷ  
uint8_t TaskID = 0;
uint64_t jiffies = 0;//��ϵͳ�������������Ľ��ĵ�����
//��ʱ����
static TaskTime_T *tmpTaskTime,*prevTaskTime,*nextTaskTime;

/****************************************************
	������:	TaskTime_Check_ID
	����:	
	����:	liyao 2015��9��8��14:10:51
****************************************************/
int8_t TaskTime_Check_ID(int8_t id){
	if(id > TASK_MAX_COUNT ){
		Log.error("TaskTime_Check_ID����\r\n");
		return -1;
	}
	return 0;
};

///****************************************************
//	������:	BaseClock_Init
//	����:	Systick����ʱ������
//	����:	liyao 2015��9��8��14:10:51
//****************************************************/
//void BaseClock_Init(){
//	//���õδ�Ĵ���
//	SysTick_Config(SYSTICK_COUNT);
//	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
//}


/****************************************************
	������:	TaskTime_Init
	����:	��ʱ������г�ʼ��
	����:	liyao 2015��9��8��14:10:51
****************************************************/
void TaskTime_Init(){
	TaskTime_Head->_next = TaskTime_Head->_prev = TaskTime_Head;
	TaskTime_Head->Alias = -1;	 //����id  
	TaskTime_Head->_TaskID = -1;	 //����
	TaskTime_Head->Priority= -1;//���ȼ� 
	TaskTime_Head->_TaskState = TASK_SUSPEND; //����״̬
	TaskTime_Head->TaskCycle = 1000;			//��������
	TaskTime_Head->_TaskCycleCount = TaskTime_Head->TaskCycle;//�״����е���ʱ 
	TaskTime_Head->RunCount = 0;
	TaskTime_Head->RunElapsed = 0;
	TaskTime_Head->StoreCount = 0;
	#ifdef LOAD_MONITOR_ON
		TaskTime_Head->_TaskState = TASK_INIT; //����״̬
		TaskTime_Head->Run = TaskTime_Monitor;//������ָ��
		TaskTime_Monitor_Init();
	#endif
	
	OSInfo.TaskFreeSize = TASK_MAX_COUNT;//����������
	OSInfo.TaskSize = 0;//����������
//	for(i = 0; i < TASK_MAX_COUNT; i++){  
//		TaskTimeLink[i].Alias = -1;
//		TaskTimeLink[i]._TaskID = -1;
//	}
//	EXTI_IT_ENABLE();
};

/****************************************************
	������:	_TaskTime_AllocID
	����:	��������ID
	����:	liyao 2017��9��12��
****************************************************/
int8_t _TaskTime_AllocID(void){
	static int8_t seq = 0;
	Realloc:	
	seq = (seq+1)%TASK_MAX_COUNT;
	tmpTaskTime = TaskTime_Head->_next;
	while(tmpTaskTime != TaskTime_Head){
		if(tmpTaskTime->_TaskID  == seq){
			goto Realloc;
		}
		tmpTaskTime = tmpTaskTime->_next;
	} 
	return seq;
}
/****************************************************
	������:	TaskTime_Add
	����:	��Ӷ�ʱ����
	����:	ʶ���,����ms,���ȼ�,ִ�к���
	����ֵ:	>0������ID���ɹ��� -1����������ʧ�ܣ�
	����:	liyao 2016��8��5��11:52:16
****************************************************/
int8_t TaskTime_Add(int8_t alias,uint16_t TaskCycle ,void(*Run)(void), TASK_MODE TaskMode){
	TaskTime_T* newTaskTime = NULL;
	if(OSInfo.TaskSize == TASK_MAX_COUNT){//������
		printf("������\r\n");
		return -1; 
	}
	//for(i = 0; i<TASK_MAX_COUNT; i++){//Ѱ������ջ��λ
	//	if(TaskTimeLink[i]._TaskID == -1){
	newTaskTime = MALLOC(sizeof(TaskTime_T));
	MALLOC_CHECK(newTaskTime,"TaskTime_Add");
	//newTaskTime = &TaskTimeLink[i];
	newTaskTime->Alias = alias;			 //����id  
	newTaskTime->_TaskID = _TaskTime_AllocID();			 //����
	newTaskTime->Priority		= alias;//���ȼ� Priority
	newTaskTime->_TaskState = TASK_INIT;		 //����״̬
	newTaskTime->TaskCycle = TaskCycle;//��������
	newTaskTime->_TaskCycleCount = (TaskCycle + alias * TASK_FIRST_DELAY);//�״����е���ʱ 
	newTaskTime->Run = Run;//������ָ��
	newTaskTime->RunCount = 0;
	newTaskTime->RunElapsed = 0; 
	newTaskTime->StoreCount = 0;
	newTaskTime->_next = NULL;
	newTaskTime->TaskMode = TaskMode;
	OSInfo.TaskSize++;
	OSInfo.TaskFreeSize--; 
 
 
	//�׸�����
	if(TaskTime_Head->_next == TaskTime_Head){
		SYSTICK_IT_DISABLE();//�жϹ�(��ֹ�жϷ���) 
		TaskTime_Head->_next = TaskTime_Head->_prev = newTaskTime; 
		  newTaskTime->_next =   newTaskTime->_prev = TaskTime_Head;
		SYSTICK_IT_ENABLE(); 
	}else{//�������� 
		tmpTaskTime = TaskTime_Head->_next;
		while(tmpTaskTime != TaskTime_Head){//�������������
			if(tmpTaskTime->Priority <= newTaskTime->Priority){//��ǰ�������ȼ������������ȼ����� 
				tmpTaskTime = tmpTaskTime->_next; 
				if(tmpTaskTime == TaskTime_Head){//ѭ���������Ȼû��ƥ�䵽
					SYSTICK_IT_DISABLE();//�жϹ�(��ֹ�жϷ���) 
					prevTaskTime = TaskTime_Head->_prev;
					prevTaskTime->_next = newTaskTime;			//	A_Next = N 
					tmpTaskTime->_prev = newTaskTime;				//	B_Prev = N
					newTaskTime->_prev = prevTaskTime;			//	N_Prev = A
					newTaskTime->_next = tmpTaskTime;				//	N_Next = B
					SYSTICK_IT_ENABLE(); 
				}
				continue;
			}else{//��ǰ�������ȼ������������ȼ��ͻ���� 	A_Next = B    B_Prev = A
				SYSTICK_IT_DISABLE();//�жϹ�(��ֹ�жϷ���) 
				prevTaskTime = tmpTaskTime->_prev;
				prevTaskTime->_next = newTaskTime;			//	A_Next = N 
				tmpTaskTime->_prev = newTaskTime;				//	B_Prev = N
				newTaskTime->_prev = prevTaskTime;			//	N_Prev = A
				newTaskTime->_next = tmpTaskTime;				//	N_Next = B 
				SYSTICK_IT_ENABLE(); 
				break;
			}
		}
	}
	return newTaskTime->_TaskID;
}

/****************************************************
	������:	_TaskTime_Remove
	����:		�Ƴ�����
	����:		�������ʱ���ص�����id
	����ֵ:	>0���ɹ� -1��ʧ�� 
	����:		liyao 2016��8��5��11:55:36
****************************************************/
static int8_t _TaskTime_Remove(int8_t id){
	if(TaskTime_Check_ID(id) < 0){
		return -1;
	}
	tmpTaskTime = TaskTime_Head->_next;
	while(tmpTaskTime != TaskTime_Head){
		if(tmpTaskTime->_TaskID  == id){
			SYSTICK_IT_DISABLE();//�жϹ�(��ֹ�жϷ���)
			prevTaskTime = tmpTaskTime->_prev;
			nextTaskTime = tmpTaskTime->_next;
			prevTaskTime->_next = nextTaskTime;
			nextTaskTime->_prev = prevTaskTime;
			SYSTICK_IT_ENABLE(); 
			OSInfo.TaskSize--;
			OSInfo.TaskFreeSize++;
			FREE(tmpTaskTime);
//			memset(tmpTaskTime, 0, sizeof(TaskTime_T));
//			tmpTaskTime->Alias = -1;
//			tmpTaskTime->_TaskID = -1;
			break;
		}
		tmpTaskTime = tmpTaskTime->_next;
	} 
	return 0;
}

/****************************************************
	������:	TaskTime_Remove
	����:		�Ƴ�����(���)
	����:		�������ʱ���ص�����id
	����ֵ:	>0���ɹ� -1��ʧ�� 
	����:		liyao 2016��8��5��11:55:36
****************************************************/
int8_t TaskTime_Remove(int8_t id){ 
	TaskTime_T* Cur_Task = NULL;
	if(TaskTime_Check_ID(id) < 0)
		return -1;
	
	Cur_Task = TaskTime_Head->_next;
	while(Cur_Task != TaskTime_Head){
		if(Cur_Task->_TaskID == id){
			Cur_Task->_TaskState = TASK_REMOVE;
			return 0;
		}
		Cur_Task = Cur_Task->_next;
	} 
	return -2;
}

/****************************************************
	������:	TaskTime_SuspendTask
	����:		��������
	����:		����id
	����ֵ:	>0���ɹ� -1��ʧ�� 
	����:		liyao 2016��8��5��11:55:36
****************************************************/
int8_t TaskTime_SuspendTask(int8_t id){ 
	TaskTime_T* Cur_Task = NULL;
	if(TaskTime_Check_ID(id) < 0)
		return -1;
	Cur_Task = TaskTime_Head->_next;
	while(Cur_Task != TaskTime_Head){
		if(Cur_Task->_TaskID == id){
			Cur_Task->_TaskState = TASK_SUSPEND;
			return 0;
		}
		Cur_Task = Cur_Task->_next;
	}  
	return -2;
}

/****************************************************
	������:	TaskTime_RecoverTask
	����:		�ָ�����
	����:		����id 
	����ֵ:	>0���ɹ� -1��ʧ�� 
	����:		liyao 2016��8��5��11:55:36
****************************************************/
int8_t TaskTime_RecoverTask(int8_t id){
	TaskTime_T* Cur_Task = NULL; 
	if(TaskTime_Check_ID(id) < 0)
		return -1;
	Cur_Task = TaskTime_Head->_next;
	while(Cur_Task != TaskTime_Head){
		if(Cur_Task->_TaskID == id){
			Cur_Task->_TaskState = TASK_RECOVER;
			return 0;
		}
		Cur_Task = Cur_Task->_next;
	}   
	return -2;
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void HAL_SYSTICK_Callback(void)
{
	jiffies++;
	TaskTime_T *tmpTaskTimeH = TaskTime_Head; 
	#ifdef LOAD_MONITOR_ON
		COUNTER_ON; 
	#endif 
	do{
		switch(tmpTaskTimeH->_TaskState){
			case TASK_INIT:
			case TASK_WAIT:   
				if(--tmpTaskTimeH->_TaskCycleCount <= 0){
					tmpTaskTimeH->_TaskState = TASK_READY; //״̬��Ϊ��ִ�� 
					tmpTaskTimeH->_TaskCycleCount = tmpTaskTimeH->TaskCycle;//������
				}
				break;
			case TASK_READY:
				if(--tmpTaskTimeH->_TaskCycleCount <= 0){
					tmpTaskTimeH->StoreCount++;	//�ۼ�δ��ִ�д���
					tmpTaskTimeH->_TaskCycleCount = tmpTaskTimeH->TaskCycle;//������
				}
				break;
			case TASK_SUSPEND: 
				break;
			case TASK_RECOVER: 
				tmpTaskTimeH->_TaskCycleCount = tmpTaskTimeH->TaskCycle;//������
				//tmpTaskTimeH->RunCount = tmpTaskTimeH->TaskCycle;
				tmpTaskTimeH->_TaskState = TASK_WAIT;
			
				break;
			case TASK_REMOVE:  
				break;
			default:
				break;
		}
		tmpTaskTimeH = tmpTaskTimeH->_next; 
	}while(tmpTaskTimeH != TaskTime_Head);
//	#ifdef LOAD_MONITOR_ON
//		COUNTER_OFF; 
//	#endif 
}

/****************************************************
	������:	TaskTime_Run
	����:		��ʱ����������к���
	ע��:		���ü�����������С�������������
	����:		liyao 2016��8��5��14:37:00
****************************************************/
void TaskTime_Run(void){
	#ifdef LOAD_MONITOR_ON
		COUNTER_OFF;
	#endif
	tmpTaskTime = TaskTime_Head;
	do{ 
		switch(tmpTaskTime->_TaskState){
			case TASK_INIT:
				break;
			case TASK_WAIT:
				break;
			case TASK_READY:
				#ifdef LOAD_MONITOR_ON
					TIMER_Tmp = LOAD_TIMX->CNT;
					COUNTER_ON; 
				#endif
				tmpTaskTime->_TaskState = TASK_WAIT;
				do{
					tmpTaskTime->Run();
					tmpTaskTime->RunCount++;
				}while(--tmpTaskTime->StoreCount >= 0 && tmpTaskTime->TaskMode == Count_Mode);
				EXTI_IT_DISABLE();
				tmpTaskTime->StoreCount = 0;
				EXTI_IT_ENABLE();
				if(tmpTaskTime->TaskMode == Single_Mode){
					_TaskTime_Remove(tmpTaskTime->_TaskID);
					return;
				}
				#ifdef LOAD_MONITOR_ON
					COUNTER_OFF; 
					if(TIMER_Bak > 0){
						tmpTaskTime->RunElapsed += (TIMER_Bak - TIMER_Tmp);
						TIMER_Bak = 0;
					}else{
						uint16_t res = LOAD_TIMX->CNT - TIMER_Tmp;
						tmpTaskTime->RunElapsed += res;
					}
				#endif
				break;
			case TASK_SUSPEND: 
				break;
			case TASK_RECOVER: 
				break;
			case TASK_REMOVE: 
				_TaskTime_Remove(tmpTaskTime->_TaskID);
				return;
			default:
				break;
		}
		tmpTaskTime = tmpTaskTime->_next;
	}while(tmpTaskTime != TaskTime_Head);
}

#ifdef LOAD_MONITOR_ON 
/****************************************************
	������:	TaskTime_Monitor_Init
	����:		���ؼ��Ӷ�ʱ������ ��ʼ��
	����:		liyao 2016��8��8��16:41:22
****************************************************/
static void TaskTime_Monitor_Init(void){ 
	ENABLE_TIMX; 
  LOAD_TIMX->ARR = (uint32_t)0xFFFF ; 
#if MCU_TYPE == 103
  LOAD_TIMX->PSC = (uint32_t)7200;
#elif MCU_TYPE == 030 
  LOAD_TIMX->PSC = (uint32_t)4800;
#endif
	LOAD_TIMX->CNT = 0;
}

/****************************************************
	������:	TaskTime_Monitor
	����:		���ؼ��Ӷ�ʱ������
	����:		liyao 2016��8��8��16:41:22
****************************************************/
static void TaskTime_Monitor(void){
	float CPU_Use = 0;
	mprintf("-----ϵͳ����-----\r\n");
	tmpTaskTime = TaskTime_Head;
	do{
		mprintf("���:%d\t����:%d\t���ȼ�:%d\t���:%dms\t���д���:%d\t��ʱ����:%d\tʹ����:%.2f%%\r\n",
			tmpTaskTime->Alias,
			tmpTaskTime->_TaskID,
			tmpTaskTime->Priority,
			tmpTaskTime->TaskCycle,
			tmpTaskTime->RunCount,
			tmpTaskTime->StoreCount,
			tmpTaskTime->RunElapsed/100.0
		);
		SYSTICK_IT_DISABLE();
		tmpTaskTime->RunCount = tmpTaskTime->StoreCount = tmpTaskTime->RunElapsed = 0;
		SYSTICK_IT_ENABLE();
		tmpTaskTime = tmpTaskTime->_next;
	}while(tmpTaskTime != TaskTime_Head);
	CPU_Use = LOAD_TIMX->CNT/100.0;
	if(CPU_Use > 100.00)
		CPU_Use = 100.00;
	mprintf("CPUʹ���ʣ�%.2f%%,��ʱ����%d\r\n", CPU_Use,LOAD_TIMX->CNT);
	//mprintf("ʱ���:%d\r\n", TIM2->CNT);
	TIMER_Bak = LOAD_TIMX->CNT;
	LOAD_TIMX->CNT = 0;
}
#endif

/****************************************************
	������:	delayUS
	����:	΢����ʱ
	����:	΢��ֵ
	����:	liyao 2016��8��9��15:35:35
****************************************************/
void DelayUS(int32_t us){ 
	uint32_t SysTickVal = SysTick->VAL;
	uint32_t SysTickVal_Last = SysTickVal;
	us *= SYSTICK_1US_COUNT;
	while(us > 0){
		SysTickVal = SysTick->VAL;
		if(SysTickVal_Last > SysTickVal){
			us -= (SysTickVal_Last - SysTickVal);
		}else if(SysTickVal_Last < SysTickVal){
			us -= (SysTickVal_Last + (SYSTICK_COUNT - SysTickVal)); 
		} 
		SysTickVal_Last = SysTickVal;
	}
}

/****************************************************
	������:	delayMS
	����:	������ʱ
	����:	����ֵ
	����:	liyao 2016��8��9��15:34:42
****************************************************/
void DelayMS(int32_t ms){ 
	uint32_t SysTickVal = SysTick->VAL;
	uint32_t SysTickVal_Last = SysTickVal;
	ms *= SYSTICK_COUNT;
	while(ms > 0){ 
		SysTickVal = SysTick->VAL;
		if(SysTickVal_Last > SysTickVal){
			ms -= (SysTickVal_Last - SysTickVal);
		}else if(SysTickVal_Last < SysTickVal){
			ms -= (SysTickVal_Last + (SYSTICK_COUNT - SysTickVal)); 
		}
		SysTickVal_Last = SysTickVal;
	} 
}

/****************************************************
	������:	delayS
	����:	����ʱ
	����:	��ֵ
	����:	liyao 2015��9��8��14:10:51
****************************************************/
void DelayS(int32_t s){ 
	uint32_t SysTickVal = SysTick->VAL;
	uint32_t SysTickVal_Last = SysTickVal;
	s *= SYSTICK_COUNT * 1000;
	while(s > 0){
		SysTickVal = SysTick->VAL;
		if(SysTickVal_Last > SysTickVal){
			s -= (SysTickVal_Last - SysTickVal);
		}else if(SysTickVal_Last < SysTickVal){
			s -= (SysTickVal_Last + (SYSTICK_COUNT - SysTickVal)); 
		} 
		SysTickVal_Last = SysTickVal;
	} 
}







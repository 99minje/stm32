#include "main.h"
extern DAC_HandleTypeDef hdac1;
extern TIM_HandleTypeDef htim1;
extern UART_HandleTypeDef huart2;
#include "heart.h"

void msecint(void)
{	static int i=0;
	i = (i<82000) ? i+1 : 0;
	//if (i % 500) HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port,LED_GREEN_Pin);
	HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R,( (int) hr[i]) * 8);
}
void hr_tr(void)
{
	HAL_TIM_Base_Start_IT(&htim1);
	HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
	while(1)
	{

	}
}

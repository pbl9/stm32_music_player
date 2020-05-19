/*
 * callbacks.c
 *
 *  Created on: Mar 24, 2019
 *      Author: pawel
 */


#include "callbacks.h"

/*void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	char callback_msg[16];
	sprintf(callback_msg,"DAC_Ptr:%d",dac_pointer);
	send_message(callback_msg,strlen(callback_msg));
	//HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_1,DAC_ALIGN_12B_R,samples_buffer[dac_pointer]);

	if(dac_pointer<(SAMPLES_BUFF_SIZE-1))
	{
		dac_pointer++;
	}
	else
	{
		dac_pointer=0;
	}
}*/

#include "Ass-03.h"

void Data_Grabber_Task(void const *argument)
{
	osSignalWait(1,osWaitForever);
	safe_printf("Hello from Data Grabber Task\n");

	//int second_counter = 0;
	int pos = 0;

	while(1) {
		osDelay(1);

		// if paused, don't run
		if(!get_start()) continue;

		set_ADC_Value(pos, get_DMA_Value(pos));

		pos++;
		if(pos > 999) {
			pos = 0;

			int pos = get_ADC_Pos();
			pos++;
			if(pos > 9) pos = 0;
			set_ADC_Pos(pos);
		}
	}
}

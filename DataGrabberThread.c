#include "Ass-03.h"

// state 0, looking for high, state 1, looking for lower
int state = 0;

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

		// copy values from dma array to our adc array
		set_ADC_Value(pos, get_DMA_Value(pos));

		if(pos % 10 == 0) {
			int p = pos / 10;
			beat_values[p] = get_DMA_Value(pos);
		}

		// update and reset positions for adc
		pos++;
		if(pos > 999) {
			pos = 0;

			int pos = get_ADC_Pos();
			pos++;
			if(pos > 9) pos = 0;
			set_ADC_Pos(pos);

			// 3072
			// calculate bpm
			/*if(state == 0) {

			}

			int bpm = num_over_threshold / 1 * 60;
			safe_printf("bpm: %d\n", bpm);*/

			// reset values
			for(int i = 0; i < 10; i++) beat_values[i] = 0;
		}
	}
}

#include "Ass-03.h"

// state 0, looking for high, state 1, looking for lower
int state = 0;

void Data_Grabber_Task(void const *argument)
{
	osSignalWait(1,osWaitForever);
	safe_printf("Hello from Data Grabber Task\n");

	//int second_counter = 0;
	int pos = 0;
	int num_beats = 0;
	int count = 0;
	int beat_pos = 0;

	while(1) {
		osDelay(1);

		// if paused, don't run
		if(!get_start()) continue;

		// copy values from dma array to our adc array
		set_ADC_Value(pos, get_DMA_Value(pos));

		// every 10 milliseconds, get value for heartrate
		if(count % 10 == 0) {
			beat_pos++;
			beat_values[beat_pos % 999] = get_DMA_Value(pos);
		}

		if(count % 10000 == 0) {
			num_beats = 0;
		}

		// update and reset positions for adc
		pos++;
		count++;
		// every second
		if(pos > 999) {
			pos = 0;

			int pos = get_ADC_Pos();
			pos++;
			if(pos > 9) pos = 0;
			set_ADC_Pos(pos);
		}

		// every x seconds
		if(count > (1 * 1000) - 1) {
			count = 0;


			for(int i = 0; i < NUM_SECONDS * 100; i++) {
				if(state == 0) {
					if(beat_values[i] < 2048) state = 1;
				}
				else if(state == 1) {
					if(beat_values[i] > 2048 + 100) {
						state = 0;
						num_beats++;
					}
				}
			}

			if(num_beats == 1) num_beats = 0;
			int bpm = (int)(((double)num_beats / (double)NUM_SECONDS) * (double)60);

			char *t;
			asprintf(&t, "%d", bpm);
			osMutexWait(myMutex01Handle, osWaitForever);
			BSP_LCD_DisplayStringAt(rectangles[3].x + rectangles[3].width / 2, rectangles[3].y + rectangles[3].height / 2, "   ", CENTER_MODE);
			BSP_LCD_DisplayStringAt(rectangles[3].x + rectangles[3].width / 2, rectangles[3].y + rectangles[3].height / 2, t, CENTER_MODE);
			osMutexRelease(myMutex01Handle);
			free(t);

		}
	}
}

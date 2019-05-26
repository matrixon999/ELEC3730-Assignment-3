//     $Date: 2018-05-22 06:24:02 +1000 (Tue, 22 May 2018) $
// $Revision: 1330 $
//   $Author: Peter $

#include "Ass-03.h"

//
// This task can be used as the main pulse rate application as it takes
// input from the front panel.
//
// *** MAKE UPDATES TO THE CODE AS REQUIRED ***
//
// Draw the boxes that make up the buttons and take action when the
// buttons are pressed. See suggested updates for the touch panel task
// that checks for button presses. Can do more in that task.

void draw_screen() {
	osMutexWait(myMutex01Handle, osWaitForever);

	BSP_LCD_Clear(LCD_COLOR_BLACK);
	BSP_LCD_SetFont(&Font12);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	for(int i = 0; i < sizeof(rectangles) / sizeof(Rectangle); i++)
	{
	  BSP_LCD_DrawRect(rectangles[i].x, rectangles[i].y, rectangles[i].width, rectangles[i].height);
	  if(strcmp(rectangles[i].text, "") != 0) {
		  BSP_LCD_DisplayStringAt(rectangles[i].x + rectangles[i].width / 2, rectangles[i].y + rectangles[i].height / 2, rectangles[i].text, CENTER_MODE);
	  }
	}

	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	BSP_LCD_DrawRect(rectangles[0].x, rectangles[0].y, rectangles[0].width, rectangles[0].height);
	BSP_LCD_DisplayStringAt(rectangles[0].x + rectangles[0].width / 2, rectangles[0].y + rectangles[0].height / 2, "Start", CENTER_MODE);

	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);

	BSP_LCD_DrawRect(55-1,15-1,250+1,150+1);
	osMutexRelease(myMutex01Handle);
}


void Ass_03_Task_02(void const * argument)
{

	uint32_t loop=0;

	Coordinate display;

	osSignalWait(1,osWaitForever);
	safe_printf("Hello from Task 2 - Pulse Rate Application (touch screen input)\n");

	draw_screen();

	while (1)
	{
		ButtonPress bp = 0;
		osEvent event = osMessageGet(myQueue01Handle, osWaitForever);
		if (event.status == osEventMessage) {
			bp = event.value.v;
			//safe_printf("BP: %d\n", bp);

			if(!get_snake_time()) {
				switch(bp) {
					case Start:
					{
						safe_printf("Start\n");

						set_start(!get_start());

						osMutexWait(myMutex01Handle, osWaitForever);
						if(get_start()) {
							BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
							BSP_LCD_DrawRect(rectangles[0].x, rectangles[0].y, rectangles[0].width, rectangles[0].height);
							BSP_LCD_DisplayStringAt(rectangles[0].x + rectangles[0].width / 2, rectangles[0].y + rectangles[0].height / 2, "Start", CENTER_MODE);
						} else {
							BSP_LCD_SetTextColor(LCD_COLOR_RED);
							BSP_LCD_DrawRect(rectangles[0].x, rectangles[0].y, rectangles[0].width, rectangles[0].height);
							// double string display because start is longer than stop and leaves bits there.
							BSP_LCD_DisplayStringAt(rectangles[0].x + rectangles[0].width / 2, rectangles[0].y + rectangles[0].height / 2, "     ", CENTER_MODE);
							BSP_LCD_DisplayStringAt(rectangles[0].x + rectangles[0].width / 2, rectangles[0].y + rectangles[0].height / 2, "Stop", CENTER_MODE);
						}

						osMutexRelease(myMutex01Handle);

					} break;

					case Zoom:
					{
						safe_printf("Zoom\n");
					} break;

					case Load:
					{
						safe_printf("Load\n");
						set_start(false);

						// redraw stop
						osMutexWait(myMutex01Handle, osWaitForever);
						BSP_LCD_SetTextColor(LCD_COLOR_RED);
						BSP_LCD_DrawRect(rectangles[0].x, rectangles[0].y, rectangles[0].width, rectangles[0].height);
						// double string display because start is longer than stop and leaves bits there.
						BSP_LCD_DisplayStringAt(rectangles[0].x + rectangles[0].width / 2, rectangles[0].y + rectangles[0].height / 2, "     ", CENTER_MODE);
						BSP_LCD_DisplayStringAt(rectangles[0].x + rectangles[0].width / 2, rectangles[0].y + rectangles[0].height / 2, "Stop", CENTER_MODE);

						osMutexRelease(myMutex01Handle);

						// get file path
						char* file_path = malloc(4 + 3);
						strcpy(file_path, "file");
						char *file_num;
						asprintf(&file_num, "%d", get_memory_location());
						strcat(file_path, file_num);

						// declare data variables
						uint8_t *data;
						int length;

						// read data
						bool result = SD_Read_File(file_path, &data, &length);
						if(!result) {
							safe_printf("Failed to read data\n");
						}

						// stop dma
						HAL_ADC_Stop_DMA(&hadc1);

						//memcpy(get_ADC_Array(i), &data[i * 1000], 2000);

						// copy data
						safe_printf("Copying data\n");
						for(int i = 0; i < 10; i++) {
							set_ADC_Pos(i);
							for(int j = 0; j < 2000; j+=2) {
								uint16_t val = ((uint16_t)data[j+1] << 8) | data[j];
								set_ADC_Value(j/2, val);
							}
						}

						safe_printf("Done copying\n");

						// print last 10 values
						for(int i = 0; i < 100; i++) {
							safe_printf("%d\n", get_ADC_Single_Value(9, 1000 - i - 1));
						}

						// free variables
						free(data);
						free(file_path);
						free(file_num);

						// reset position and dma
						set_ADC_Pos(0);
						HAL_StatusTypeDef status = HAL_ADC_Start_DMA(&hadc1, (uint32_t *)get_ADC_Value_p(), 1000);
						if (status != HAL_OK)
						{
							safe_printf("ERROR: Task 4 HAL_ADC_Start_DMA() %d\n", status);
						}

						set_loaded(true);

						// finshed loading, redraw whole graph, go to beginning, pause
					} break;

					case Store:
					{
						set_start(false);
						osSemaphoreWait(myBinarySem06Handle, osWaitForever);
						safe_printf("Store\n");

						// allocate data
						uint16_t **data = get_ADC_Total_p();

						char* file_path = malloc(4 + 3);
						strcpy(file_path, "file");
						char *file_num;
						asprintf(&file_num, "%d", get_memory_location());
						strcat(file_path, file_num);

						bool result = SD_Write_File_Offset(file_path, data, get_ADC_Pos());
						if(!result) {
							safe_printf("Failed to write file\n");
						}

						for(int i = 0; i < 100; i++) {
							safe_printf("%d\n", get_ADC_Single_Value(9, 1000-i-1));
						}

						free(file_path);
						free(file_num);

						osSemaphoreRelease(myBinarySem06Handle);
						set_start(true);

					} break;

					case Minus:
					{
						// update memory
						// redraw

						int mem_loc = get_memory_location() - 1;
						set_memory_location(mem_loc);
						if(mem_loc < 1) mem_loc = 1;

						char *str;
						asprintf(&str, "%d", mem_loc);
						rectangles[7].text = str;

						osMutexWait(myMutex01Handle, osWaitForever);
						BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
						BSP_LCD_DisplayStringAt(rectangles[7].x + rectangles[7].width / 2, rectangles[7].y + rectangles[7].height / 2, rectangles[7].text, CENTER_MODE);
						osMutexRelease(myMutex01Handle);

						free(str);

					} break;

					case Plus:
					{
						// update val
						int mem_loc = get_memory_location() + 1;
						set_memory_location(mem_loc);
						if(mem_loc < 1) mem_loc = 1;

						// case to string
						char *str;
						asprintf(&str, "%d", mem_loc);
						rectangles[7].text = str;

						// draw
						osMutexWait(myMutex01Handle, osWaitForever);
						BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
						BSP_LCD_DisplayStringAt(rectangles[7].x + rectangles[7].width / 2, rectangles[7].y + rectangles[7].height / 2, rectangles[7].text, CENTER_MODE);
						osMutexRelease(myMutex01Handle);

						free(str);
					} break;

					case Mode:
					{
						// Snake time
						/*safe_printf("Snake time\n");
						set_start(false);
						set_snake_time(true);
						run_snake_time();
						draw_screen();
						set_snake_time(false);
						set_start(true);*/

						uint8_t *data = malloc(10);
						if(data == NULL) {
							safe_printf("Failed to allocate\n");
						}
						for(uint8_t i = 0; i < 10; i++) {
							data[i] = i;
						}

						bool result = SD_Write_File("data1", data, 10);
						if(result == false) {
							safe_printf("Shit\n");
						}

						uint8_t* data2;
						int length;
						result = SD_Read_File("data1", &data2, &length);
						if(result == false) {
							safe_printf("Shit2\n");
						}

						safe_printf("Length read: %d\n", length);
						for(uint8_t i = 0; i < 10; i++) {
							safe_printf("data2: %d %u\n", i, data2[i]);
						}

						free(data);
						free(data2);

					} break;

					default:
					{
						safe_printf("Nothing\n");
					}
				}
			}
		}

		/*if (getfp(&display) == 0)
		{
			if((display.y > YOFF+5) && (display.y < YOFF+YSIZE-5) &&
				(display.x > XOFF+5) && (display.x < XOFF+XSIZE-5))
			{
				osMutexWait(myMutex01Handle, osWaitForever);
				BSP_LCD_FillCircle(display.x, display.y, 2);
				osMutexRelease(myMutex01Handle);
				loop++;
				safe_printf("Task 2: %d (got  %3d,%3d)\n", loop, display.x, display.y);


			}
		}*/
	}
}

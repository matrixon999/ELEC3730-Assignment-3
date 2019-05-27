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
	for(int i = 0; i < sizeof(rectangles) / sizeof(Rectangle) - 2; i++)
	{
	  BSP_LCD_DrawRect(rectangles[i].x, rectangles[i].y, rectangles[i].width, rectangles[i].height);
	  if(strcmp(rectangles[i].text, "") != 0) {
		  BSP_LCD_DisplayStringAt(rectangles[i].x + rectangles[i].width / 2, rectangles[i].y + rectangles[i].height / 2, rectangles[i].text, CENTER_MODE);
	  }
	}

	// draw little stuff
	BSP_LCD_SetFont(&Font8);
	BSP_LCD_DrawRect(rectangles[10].x, rectangles[10].y, rectangles[10].width, rectangles[10].height);
	BSP_LCD_DisplayStringAt(rectangles[10].x + rectangles[10].width / 2, rectangles[10].y + rectangles[10].height / 2, rectangles[10].text, CENTER_MODE);
	BSP_LCD_DrawRect(rectangles[11].x, rectangles[11].y, rectangles[11].width, rectangles[11].height);
	BSP_LCD_DisplayStringAt(rectangles[11].x + rectangles[11].width / 2, rectangles[11].y + rectangles[11].height / 2, rectangles[11].text, CENTER_MODE);

	// draw start button
	BSP_LCD_SetFont(&Font12);
	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	BSP_LCD_DrawRect(rectangles[0].x, rectangles[0].y, rectangles[0].width, rectangles[0].height);
	BSP_LCD_DisplayStringAt(rectangles[0].x + rectangles[0].width / 2, rectangles[0].y + rectangles[0].height / 2, "Start", CENTER_MODE);

	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);

	// graph square
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

						// draw start button
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

						safe_printf("Copying data\n");

						// copy data
						for(int i = 0; i < 10; i++)
						{
							memcpy(get_ADC_Array(i), &data[i * 2000], 2000);
						}

						/*for(int i = 0; i < 10; i++) {
							set_ADC_Pos(i);
							uint32_t new_pos = i*2000;

							for(int j = 0; j < 2000; j+=2) {

								uint16_t val = ((uint16_t)data[new_pos + j+1] << 8) | data[new_pos + j];

								set_ADC_Value(j/2, val);
							}
						}*/

						safe_printf("Done copying\n");

						// free variables
						free(data);
						free(file_path);
						free(file_num);

						// alert that we;ve loaded
						set_loaded(true);
					} break;

					case Store:
					{
						safe_printf("Store\n");
						set_start(false);

						// allocate data
						uint16_t **data = get_ADC_Total_p();

						// file name stuff
						char* file_path = malloc(4 + 3);
						strcpy(file_path, "file");
						char *file_num;
						asprintf(&file_num, "%d", get_memory_location());
						strcat(file_path, file_num);

						// write file data
						bool result = SD_Write_File_Offset(file_path, data, get_ADC_Pos());
						if(!result) {
							safe_printf("Failed to write file\n");
						}

						// free data
						free(file_path);
						free(file_num);

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
						safe_printf("Snake time\n");
						set_start(false);
						set_snake_time(true);
						run_snake_time();
						draw_screen();
						set_snake_time(false);
						set_start(true);
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

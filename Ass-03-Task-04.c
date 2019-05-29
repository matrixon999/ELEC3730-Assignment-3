//     $Date: 2018-05-22 06:24:02 +1000 (Tue, 22 May 2018) $
// $Revision: 1330 $
//   $Author: Peter $

#include "Ass-03.h"

// This is the task that reads the analog input. A buffer is divided in two to
// allow working on one half of the buffer while the other half is being
// loaded using the DMA controller.
//
// This task also plots the wave form to the screen.
//
// *** MAKE UPDATES TO THE CODE AS REQUIRED ***
//
// Note that there needs to be a way of starting and stopping the display.

#define XOFF 55
#define YOFF 15
#define XSIZE 250
#define YSIZE 150
#define XPOS 5

static uint16_t xpos=0;
static uint16_t ypos=0;
static uint16_t last_xpos=0;
static uint16_t last_ypos=0;

void draw_loaded() {
	if(get_debug_mode()) {
		safe_printf(CONSOLE_BLUE("Drawing loaded data\n"));
	}
	// reset position values
	xpos=0;
	ypos=0;
	last_xpos=0;
	last_ypos=0;

	// loc kdrawing mutex
	osMutexWait(myMutex01Handle, osWaitForever);

	// clear whole box, probably not needed
	/*// draw bounding box
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillRect(XOFF,YOFF,XSIZE-1,YSIZE-1);*/

	// for each second
	for(int i = 0; i < 10; i++) {
	  // update our ad position
	  set_ADC_Pos(i);
	  // calculate xpos for this second
	  xpos = i * 25;

	  // 25 pixels per second,
	  // for each one
	  for(int j = 0; j < 25; j++)
	  {
		  // draw black vertical to clear last value
		  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		  BSP_LCD_DrawVLine(XOFF+xpos,YOFF,YSIZE);

		  // set colour back to green
		  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);

		  // calculate ypos
		  uint16_t val = get_ADC_Value(j*40);
		  ypos = ((val)*(150))/(4096);
		  // draw point
		  BSP_LCD_DrawLine(XOFF+last_xpos,YOFF+last_ypos,XOFF+xpos,YOFF+ypos);
		  // update positions
		  last_xpos=xpos;
		  last_ypos=ypos;
		  xpos++;
	  }
	}
	// release mutex
	osMutexRelease(myMutex01Handle);
}

void Ass_03_Task_04(void const * argument)
{
  uint16_t i;
  HAL_StatusTypeDef status;

	xpos=0;
	ypos=0;
	last_xpos=0;
	last_ypos=0;

  osSignalWait(1,osWaitForever);
  safe_printf("Hello from Task 4 - Analog Input (turn ADC knob or use pulse sensor)\n");

  // Draw a box to plot in
  osMutexWait(myMutex01Handle, osWaitForever);
  BSP_LCD_DrawRect(XOFF-1,YOFF-1,XSIZE+1,YSIZE+1);
  osMutexRelease(myMutex01Handle);

  // Start the conversion process
  // set dma address to our array
  status = HAL_ADC_Start_DMA(&hadc1, (uint32_t *)get_DMA_Value_p(), 1000);
  if (status != HAL_OK)
  {
	  safe_printf(CONSOLE_RED("ERROR: Task 4 HAL_ADC_Start_DMA() %d\n"), status);
  }

  // value for checking if we need to reset everything after playing snake
  bool reset_after_snake = false;

  // scale factor for drawing different analog times
  int x = 500;

  // Start main loop
  while (1)
  {
	  // calculate analog value
	  x = get_analog_value() * 50;

	  // if we just loaded, draw and reset
	  if(get_loaded()) {
		  //draw
		  draw_loaded();

		  // reset position values
		  xpos=0;
			ypos=0;
			last_xpos=0;
			last_ypos=0;
		  // reset flags

		  // reset adc array
		  osMutexWait(globalVariableHandle, osWaitForever);
		  for(int i = 0; i < 10; i++)
		  {
			  for(int j = 0; j < 1000; j++)
			  {
				  ADC_Value[i][j] = 0;
			  }
		  }
		  osMutexRelease(globalVariableHandle);
		  set_ADC_Pos(0);

		  set_loaded(false);
		  continue;
	  }

	  // if we're paused, just wait and loop
	  if(!get_start()) {
		  osDelay(100);
		  continue;
	  }
	  // if we're playing snake, flag reset
	  if(get_snake_time()) {
		  reset_after_snake = true;
	  }
	  // after playing snake, reset positions
	  else if(reset_after_snake) {
		  xpos = 0;
		  ypos = 0;
		  last_xpos=0;
		  last_ypos=0;
	  }

	  // Wait for first half of buffer
	  osSemaphoreWait(myBinarySem05Handle, osWaitForever);
	  osMutexWait(myMutex01Handle, osWaitForever);
	  // draw first values
	  for(i=0;i<500;i=i+x)
	  {
		  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		  BSP_LCD_DrawVLine(XOFF+xpos,YOFF,YSIZE);
		  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);

		  ypos=(uint16_t)((uint32_t)(get_DMA_Value(i))*YSIZE/4096);
		  BSP_LCD_DrawLine(XOFF+last_xpos,YOFF+last_ypos,XOFF+xpos,YOFF+ypos);
		  last_xpos=xpos;
		  last_ypos=ypos;
		  xpos++;
		  if (last_xpos>=XSIZE-1) break;
	  }
	  osMutexRelease(myMutex01Handle);
	  if (last_xpos>=XSIZE-1)
	  {
		  xpos=0;
		  last_xpos=0;
	  }

	  // Wait for second half of buffer
	  osSemaphoreWait(myBinarySem06Handle, osWaitForever);
	  HAL_GPIO_WritePin(GPIOD, LD4_Pin, GPIO_PIN_SET);
	  osMutexWait(myMutex01Handle, osWaitForever);
	  // draw second values
	  for(i=0;i<500;i=i+x)
	  {
		  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		  BSP_LCD_DrawVLine(XOFF+xpos,YOFF,YSIZE);
		  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);

		  ypos=(uint16_t)((uint32_t)(get_DMA_Value(i+500))*YSIZE/4096);
		  BSP_LCD_DrawLine(XOFF+last_xpos,YOFF+last_ypos,XOFF+xpos,YOFF+ypos);
		  last_xpos=xpos;
		  last_ypos=ypos;
		  xpos++;
		  if (last_xpos>=XSIZE-1) break;
	  }
	  osMutexRelease(myMutex01Handle);
	  if (last_xpos>=XSIZE-1)
	  {
		  xpos=0;
		  last_xpos=0;
	  }
	  HAL_GPIO_WritePin(GPIOD, LD4_Pin, GPIO_PIN_RESET);
  }
}

// STEPIEN: Add callback functions to see if this can be used for double buffering equivalent

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
	osSemaphoreRelease(myBinarySem05Handle);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	HAL_GPIO_WritePin(GPIOD, LD3_Pin, GPIO_PIN_SET);
	osSemaphoreRelease(myBinarySem06Handle);
	HAL_GPIO_WritePin(GPIOD, LD3_Pin, GPIO_PIN_RESET);
}


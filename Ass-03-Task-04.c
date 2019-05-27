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
	safe_printf("Drawing loaded data\n");
	xpos=0;
	ypos=0;
	last_xpos=0;
	last_ypos=0;

	osMutexWait(myMutex01Handle, osWaitForever);

	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillRect(XOFF,YOFF,XSIZE-1,YSIZE-1);
	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);

	/*
	 * for 25 pixels
	 * x = xoff + i * 25
	 */

	for(int i = 0; i < 10; i++) {
	  set_ADC_Pos(i);
	  xpos = i * 25;

	  //for(int j=0;j<1000;j += 1000/40)
	  for(int j = 0; j < 25; j++)
	  {
		  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		  BSP_LCD_DrawVLine(XOFF+xpos,YOFF,YSIZE);
		  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
		  //ypos=(uint16_t)((uint32_t)(get_ADC_Value(j*40))*YSIZE/4096);

		  // scale to 0 - 135
		  uint16_t val = get_ADC_Value(j*40);
		  ypos = ((val)*(150))/(4096);
		  //safe_printf("val: %d - ypos: %d\n", val, ypos);
		  BSP_LCD_DrawLine(XOFF+last_xpos,YOFF+last_ypos,XOFF+xpos,YOFF+ypos);
		  last_xpos=xpos;
		  last_ypos=ypos;

		  xpos++;
	  }
	}
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
  //uint16_t xpos=0;
  //uint16_t ypos=0;
  //uint16_t last_xpos=0;
  //uint16_t last_ypos=0;

  osSignalWait(1,osWaitForever);
  safe_printf("Hello from Task 4 - Analog Input (turn ADC knob or use pulse sensor)\n");

  // Draw a box to plot in
  osMutexWait(myMutex01Handle, osWaitForever);
  BSP_LCD_DrawRect(XOFF-1,YOFF-1,XSIZE+1,YSIZE+1);
  osMutexRelease(myMutex01Handle);

  // Start the conversion process

  // fix
  status = HAL_ADC_Start_DMA(&hadc1, (uint32_t *)get_DMA_Value_p(), 1000);
  if (status != HAL_OK)
  {
	  safe_printf("ERROR: Task 4 HAL_ADC_Start_DMA() %d\n", status);
  }

  bool reset_after_snake = false;


  int x = 500;

  // Start main loop
  while (1)
  {
	  x = get_analog_value() * 50 - 1;

	  if(get_loaded() == true) {
		  draw_loaded();
		  xpos=0;
			ypos=0;
			last_xpos=0;
			last_ypos=0;
		  set_loaded(false);
		  continue;
	  }

	  if(!get_start()) {
		  osDelay(100);
		  continue;
	  }
	  if(get_snake_time()) {
		  reset_after_snake = true;
	  } else if(reset_after_snake) {
		  last_xpos=0;
		  last_ypos=0;
	  }

	  // Wait for first half of buffer
	  osSemaphoreWait(myBinarySem05Handle, osWaitForever);
	  osMutexWait(myMutex01Handle, osWaitForever);
	  for(i=0;i<500;i=i+x)
	  {
		  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		  BSP_LCD_DrawVLine(XOFF+xpos,YOFF,YSIZE);
		  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);

		  ypos=(uint16_t)((uint32_t)(get_DMA_Value(i))*YSIZE/4096);
		  BSP_LCD_DrawLine(XOFF+last_xpos,YOFF+last_ypos,XOFF+xpos,YOFF+ypos);
		  // BSP_LCD_FillRect(xpos,ypos,1,1);
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
	  for(i=0;i<500;i=i+x)
	  {
		  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		  BSP_LCD_DrawVLine(XOFF+xpos,YOFF,YSIZE);
		  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);

		  ypos=(uint16_t)((uint32_t)(get_DMA_Value(i+500))*YSIZE/4096);
		  BSP_LCD_DrawLine(XOFF+last_xpos,YOFF+last_ypos,XOFF+xpos,YOFF+ypos);
		  // BSP_LCD_FillCircle(xpos,ypos,2);
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


//     $Date: 2018-05-22 06:24:02 +1000 (Tue, 22 May 2018) $
// $Revision: 1330 $
//   $Author: Peter $

// Assignment 3 include file

#ifndef ASS_03_H_
#define ASS_03_H_

// These are defines for changing the colours of the output text
#define CONSOLE_RED(str)     "\x1b[31m" str "\x1b[0m"
#define CONSOLE_GREEN(str)   "\x1b[32m" str "\x1b[0m"
#define CONSOLE_YELLOW(str)  "\x1b[33m" str "\x1b[0m"
#define CONSOLE_BLUE(str)    "\x1b[34m" str "\x1b[0m"
#define CONSOLE_MAGENTA(str) "\x1b[35m" str "\x1b[0m"
#define CONSOLE_CYAN(str)    "\x1b[36m" str "\x1b[0m"
#define CONSOLE_RESET   "\x1b[0m"

// Standard includes
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "stm32f4xx_hal.h"
#include "usart.h"
#include "touch_panel.h"
#include "openx07v_c_lcd.h"
#include "fatfs.h"
#include "adc.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include <math.h>

// OS handles
extern osThreadId defaultTaskHandle;
extern osThreadId myTask02Handle;
extern osThreadId myTask03Handle;
extern osThreadId myTask04Handle;
extern osTimerId myTimer01Handle;

extern osSemaphoreId myBinarySem01Handle;
extern osSemaphoreId myBinarySem02Handle;
extern osSemaphoreId myBinarySem03Handle;
extern osSemaphoreId myBinarySem04Handle;
extern osSemaphoreId myBinarySem05Handle;
extern osSemaphoreId myBinarySem06Handle;
extern osMessageQId myQueue01Handle;
extern osMutexId myMutex01Handle; // Protect LCD
extern osMutexId myMutex02Handle; // Protect console output

// Assignment tasks
extern void Ass_03_Task_01(void const *argument);
extern void Ass_03_Task_02(void const *argument);
extern void Ass_03_Task_03(void const *argument);
extern void Ass_03_Task_04(void const *argument);

// Library functions
extern uint8_t BSP_TP_Init(void);
extern uint8_t BSP_TP_GetDisplayPoint(Coordinate *pDisplay);

// Front panel input
extern uint8_t getfp(Coordinate *display);

// STEPIEN: Safe printf() to ensure that only one task can write to
//          the console at a time

#define safe_printf(fmt, ...) \
	osMutexWait(myMutex02Handle, osWaitForever); \
	printf(fmt, ##__VA_ARGS__); \
	osMutexRelease(myMutex02Handle);

//
// ADD YOUR CODE
//

// global variables

static bool debug_mode_enabled = false;
static int analog_value = 10;
static FIL MyFile;

extern int string_parser(char *inp, char **array_of_words_p[]);
extern FRESULT scan_files(char *path);
extern bool is_char_numeric(char c);
extern int to_int(char *str);
extern bool is_numeric(char *str);

#define XOFF 55
#define YOFF 15
#define XSIZE 250
#define YSIZE 150
#define XPOS 5

typedef enum {
	Nothing,
	Start,
	Zoom,
	Load,
	Store,
	Minus,
	Plus,
	Mode
} ButtonPress;


typedef struct {
	int x, y, width, height;
	char *text;
	ButtonPress bp;
} Rectangle;

static Rectangle rectangles[] = {
 {XPOS, YOFF, 45, 45, "Start", Start}, //start
 {XPOS, YOFF+50, 45, 45, "Zoom", Zoom}, //zoom
 {XPOS, YOFF+(50*2), 45, 25, "BPM", 0}, // pulse title
 {XPOS, YOFF+(50*2)+25, 45, 45, "00", 0}, // pulse indicator
 {XPOS, YOFF+(50*3)+25, 45, 45, "Load", Load}, // Load

 {XPOS+(50), YOFF+(50*3)+25, 45, 45, "Store", Store}, //store
 {XPOS+(50*2), YOFF+(50*3)+25, 45, 45, "-", Minus}, // -
 {XPOS+(50*3), YOFF+(50*3)+25, 45, 45, "1", 0}, // num
 {XPOS+(50*4), YOFF+(50*3)+25, 45, 45, "+", Plus}, // +
 {XPOS+(50*5), YOFF+(50*3)+25, 45, 45, "Mode", Mode}, //switch mode

 {XPOS+(50), YOFF+(150), 20, 20, "0s"}, // time stamp left
 {XPOS+(50*6)-20, YOFF+(150), 20, 20, "10s"}, // time stamp right
};

#endif /* ASS_03_H_ */

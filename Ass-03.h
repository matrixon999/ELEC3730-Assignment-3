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

extern osThreadId dataGrabberHandle;

extern osSemaphoreId myBinarySem01Handle;
extern osSemaphoreId myBinarySem02Handle;
extern osSemaphoreId myBinarySem03Handle;
extern osSemaphoreId myBinarySem04Handle;
extern osSemaphoreId myBinarySem05Handle;
extern osSemaphoreId myBinarySem06Handle;
extern osMessageQId myQueue01Handle;
extern osMutexId myMutex01Handle; // Protect LCD
extern osMutexId myMutex02Handle; // Protect console output

extern osMutexId globalVariableHandle; // Protect global variables
extern osMutexId SDCardMutexHandle; // TODO this

// Assignment tasks
extern void Ass_03_Task_01(void const *argument);
extern void Ass_03_Task_02(void const *argument);
extern void Ass_03_Task_03(void const *argument);
extern void Ass_03_Task_04(void const *argument);

extern void Data_Grabber_Task(void const *argument);

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

static bool just_loaded = false;
extern bool get_loaded();
extern void set_loaded(bool);

static volatile int ADC_Pos = 0;
extern int get_ADC_Pos();
extern void set_ADC_Pos(int val);

static uint16_t DMA_Buffer[1000];
extern uint16_t get_DMA_Value(int i);
extern uint16_t **get_DMA_Value_p();


static uint16_t ADC_Value[10][1000];
extern uint16_t get_ADC_Value(int i);
extern void set_ADC_Value(int i, uint16_t val);
extern uint16_t *get_ADC_Value_p();
extern uint16_t **get_ADC_Total_p();

extern uint16_t *get_Copy_ADC_Array(int i);

extern uint16_t get_ADC_Single_Value(int pos, int i);
extern uint16_t *get_ADC_Array(int i);

static bool debug_mode_enabled = false;
extern bool get_debug_mode_status();
extern void set_debug_mode_status(bool);

static int analog_value = 10;
extern int get_analog_value();
extern void set_analog_value(int);

static int memory_location = 1;
extern int get_memory_location();
extern void set_memory_location(int);

static FIL MyFile;
extern FIL get_File_Handle();
extern void set_File_Handle(FIL);

static bool start = true;
extern bool get_start();
extern void set_start(bool);

// function declarations

extern int string_parser(char *inp, char **array_of_words_p[]);
extern FRESULT scan_files(char *path);
extern bool is_char_numeric(char c);
extern int to_int(char *str);
extern bool is_numeric(char *str);

// functions for commad array
extern void debug_func(uint8_t num_args, char **args);
extern void analog_func(uint8_t num_args, char **args);
extern void ls_func(uint8_t num_args, char **args);
extern void cd_func(uint8_t num_args, char **args);
extern void mkdir_func(uint8_t num_args, char **args);
extern void cp_func(uint8_t num_args, char **args);
extern void rm_func(uint8_t num_args, char **args);
extern void cwd_func(uint8_t num_args, char **args);
extern void help_func(uint8_t num_args, char **args);
extern void mv_func(uint8_t num_args, char **args);
extern void ren_func(uint8_t num_args, char **args);
extern void tree_func(uint8_t num_args, char **args);
extern void cat_func(uint8_t num_args, char **args);
extern void head_func(uint8_t num_args, char **args);
extern void tail_func(uint8_t num_args, char **args);
extern void clear_func(uint8_t num_args, char **args);

typedef struct {
	char *function_name;
	char *friendly_name;
	void (*function_pointer)(uint8_t num_args, char **args);
	char *help_string;
} command;

static const command Commands[] = {
	{"debug", "Debug", &debug_func, "debug {on/off}"},
	{"analog", "Analog", &analog_func, "analog {num}"},
	{"ls", "List Files", &ls_func, "ls"},
	{"cd", "Change Directory", &cd_func, "cd {name}"},
	{"mkdir", "Make Directory", &mkdir_func, "mkdir {name}"},
	{"cp", "Copy File/Directory", &cp_func, "cp {src} {dst}"},
	{"rm", "Remove File/Directory", &rm_func, "rm {file}"},
	{"cwd", "Current Working Directory", &cwd_func, "cwd"},

	{"help", "Help", &help_func, "help {command}"},
	{"mv", "Move File", &mv_func, "mv {src} {dst}"}, // TODO make sure handles files and directories
	{"ren", "Rename File", &ren_func, "ren {old} {new}"},
	{"tree", "Print File Tree", &tree_func, "tree"}, // TODO maybe specify dir
	{"cat", "Print File", &cat_func, "cat {file}"},
	{"head", "Print First 10 lines of file", &head_func, "head {file}"},
	{"tail", "Print Last 10 lines of file", &tail_func, "tail {file}"},
	{"clear", "Clears the screen", &clear_func, "clear"},
};

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
 {XPOS+(50*3), YOFF+(50*3)+25, 45, 45, "1", 0}, // num // pos 7
 {XPOS+(50*4), YOFF+(50*3)+25, 45, 45, "+", Plus}, // +
 {XPOS+(50*5), YOFF+(50*3)+25, 45, 45, "Mode", Mode}, //switch mode

 {XPOS+(50), YOFF+(150), 20, 20, "0s"}, // time stamp left
 {XPOS+(50*6)-20, YOFF+(150), 20, 20, "10s"}, // time stamp right
};
extern Rectangle* get_rectangles();

static int rectangles_length = sizeof(rectangles) / sizeof(Rectangle);
extern int get_rectangles_length();

// SNAKE STUFF

typedef enum {
  Left = 'L',
  Right = 'R',
  Up = 'U',
  Down = 'D',
} JoystickButton;

typedef struct {
	int x;
	int y;
} vector;

static bool snake_time = false;
extern bool get_snake_time();
extern void set_snake_time(bool);

extern void run_snake_time();
extern uint8_t GetJoystick();
extern uint8_t ReadJoystick();

#endif /* ASS_03_H_ */

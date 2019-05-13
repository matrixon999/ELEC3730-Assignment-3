//     $Date: 2018-05-22 06:24:02 +1000 (Tue, 22 May 2018) $
// $Revision: 1330 $
//   $Author: Peter $

#include "Ass-03.h"

// This is the console task. Can use your code from the previous assignment
// and expand.
//
// The example code is very basic, showing access to the SD card. Note that
// if the SD card is access from both this task and the pulse rate monitor
// task, a MUTEX will be required.
//
// *** MAKE UPDATES TO THE CODE AS REQUIRED ***
//

uint8_t myReadFile();
uint8_t myWriteFile();

//FIL MyFile;
FIL MyFile2, MyFile3;
FRESULT Status;

void debug_func(uint8_t num_args, char **args)
{
	// if no arguments, print status
	if(num_args == 0) {
		printf("Debug mode: %s\n", debug_mode_enabled ? "Enabled" : "Disabled");
	}
	else {
		if(strcmp(args[0], "on") == 0) {
			debug_mode_enabled = true;
		}
		else if(strcmp(args[0], "off") == 0) {
			debug_mode_enabled = false;
		}
		else {
			printf(CONSOLE_RED("Unknown parameter given\n"));
		}
	}
}

void analog_func(uint8_t num_args, char **args)
{
	// if no arguments, print status
	if(num_args == 0) {
		printf("Analog Value Currently: %d\n", analog_value);
	}
	else {
		if(is_numeric(args[0])) {
			int val = to_int(args[0]);
			analog_value = val;
		}
		else {
			printf(CONSOLE_RED("Non numeric argument given.\n"));
		}
	}
}

void ls_func(uint8_t num_args, char **args)
{
	// list present working directory files

	FATFS fs;
	FRESULT res;
	char buff[256];

	strcpy(buff, SD_get_Cwd());
	res = SD_Scan_Directory(buff);
}

void cd_func(uint8_t num_args, char **args)
{
	// if no arguments, print status
	if(num_args == 0) {
		printf("No argument given.\n");
	}
	else {
		bool result = SD_Change_Directory(args[0]);
		if(!result) {
			safe_printf(CONSOLE_RED("Directory does not exist.\n"));
		}
	}
}

void mkdir_func(uint8_t num_args, char **args)
{
	// if no arguments, print status
	if(num_args == 0) {
		printf("No argument given.\n");
	}
	else {
		bool result = SD_Create_Directory(args[0]);
		if(!result) {
			safe_printf(CONSOLE_RED("Directory already exists.\n"));
		}
	}
}

void cp_func(uint8_t num_args, char **args)
{
	// if no arguments, print status
	if(num_args == 0) {
		printf("No argument given.\n");
	}
	else {
		safe_printf("file 1 %s\n", args[0]);
		safe_printf("file 2 %s\n", args[1]);
		bool result = SD_Copy(args[0], args[1]);
		if(!result) {
			safe_printf(CONSOLE_RED("Copy Failed.\n"));
		}
	}
}

void rm_func(uint8_t num_args, char **args)
{
	// if no arguments, print status
	if(num_args == 0) {
		printf("No argument given.\n");
	}
	else {
		bool result = SD_Delete(args[0]);
		if(!result) {
			safe_printf(CONSOLE_RED("Delete Failed.\n"));
		}
	}
}

void cwd_func(uint8_t num_args, char **args)
{
	SD_Cwd();
}

// struct to hold information about mathematical functions
typedef struct {
	char *function_name;
	char *friendly_name;
	void (*function_pointer)(uint8_t num_args, char **args);
	char *help_string;
} command;

const command Commands[] = {
	{"debug", "Debug", &debug_func, "debug {on/off}"},
	{"analog", "Analog", &analog_func, "analog {num}"},
	{"ls", "List Files", &ls_func, "ls"}, // TODO maybe fix to deal with changed directories
	{"cd", "Change Directory", &cd_func, "cd {name}"},
	{"mkdir", "Make Directory", &mkdir_func, "mkdir {name}"},
	{"cp", "Copy File/Directory", &cp_func, "cp {src} {dst}"},
	{"rm", "Remove File/Directory", &rm_func, "rm {name}"},
	{"cwd", "Current Working Directory", &cwd_func, "cwd"},
};

void CommandProcessor(char *str)
{
	// separates input into words
	char **words;
	int word_count = string_parser(str, &words);

	// number of commands, used for looping over them
	int num_of_commands = sizeof(Commands) / sizeof(command);

	// loop over each one
	for(int i = 0; i < num_of_commands; i++)
	{
		// if the name matches
		if(strcmp(words[0], Commands[i].function_name) == 0)
		{
			// execute the function with given arguments
			Commands[i].function_pointer(word_count - 1, &words[1]);
			return;
		}
	}

	// handle command not found

	printf(CONSOLE_RED(" Command not found.\n"));
}

static char *in_string;
static int in_initialized = 0;
void Parser()
{
	if(in_initialized == 0)
	{
		if(in_string != NULL)
		{
			free(in_string);
		}
		in_string = malloc(2);
		if(in_string == NULL) {
			printf("Fucked up malloc\n");
		}
		strcpy(in_string, "");
		in_initialized = 1;
	}

	char c;

	c = getchar();
	if ((c < 32) | (c > 126))
	{
		if(c == 127) {
			safe_printf("%c", (char)127);
			in_string[strlen(in_string)-1] = '\0';
		}
		else if(c == 13) {
			safe_printf("%c", c);
			CommandProcessor(in_string);
			in_initialized = 0;
		}
	}
	else
	{
		in_string = realloc(in_string, 1 + strlen(in_string) + 1);
		if(in_string == NULL) {
			printf("Something fucked up big time\n");
		}
		strcat(in_string, &c);
		safe_printf("%c", c);
	}
}

void Ass_03_Task_01(void const * argument)
{
	/*rectangles[0] = (Rectangle){XPOS, YOFF, 45, 45, "Start", Start};
	rectangles[1] = (Rectangle){XPOS, YOFF+50, 45, 45, "Zoom", Zoom};
	rectangles[2] = (Rectangle){XPOS, YOFF+(50*2), 45, 25, "BPM", 0};
	rectangles[3] = (Rectangle){XPOS, YOFF+(50*2)+25, 45, 45, "00", 0};
	rectangles[4] = (Rectangle){XPOS, YOFF+(50*3)+25, 45, 45, "Load", Load};

	rectangles[5] = (Rectangle){XPOS+(50), YOFF+(50*3)+25, 45, 45, "Store", Store};
	rectangles[6] = (Rectangle){XPOS+(50*2), YOFF+(50*3)+25, 45, 45, "-", Minus};
	rectangles[7] = (Rectangle){XPOS+(50*3), YOFF+(50*3)+25, 45, 45, "1", 0};
	rectangles[8] = (Rectangle){XPOS+(50*4), YOFF+(50*3)+25, 45, 45, "+", Plus};
	rectangles[9] = (Rectangle){XPOS+(50*5), YOFF+(50*3)+25, 45, 45, "Mode", Mode};

	rectangles[10] = (Rectangle){XPOS+(50), YOFF+(150), 20, 20, "0s"};
	rectangles[11] = (Rectangle){XPOS+(50*6)-20, YOFF+(150), 20, 20, "10s"};*/

  uint32_t loop=0;
  uint8_t ts[100];
  uint16_t i;
  Coordinate display;
  char c;

  safe_printf("Hello from Task 1 - Console (serial input)\n");
  safe_printf("INFO: Initialise LCD and TP first...\n");

  // STEPIEN: Initialize and turn on LCD and calibrate the touch panel
  BSP_LCD_Init();
  BSP_LCD_DisplayOn();
  BSP_TP_Init();
  //
  // Signal other tasks to start
  osSignalSet(myTask02Handle, 1);
  osSignalSet(myTask03Handle, 1);
  osSignalSet(myTask04Handle, 1);

  // Display welcome message
  osMutexWait(myMutex01Handle, osWaitForever);
  BSP_LCD_Clear(LCD_COLOR_BLACK);
  BSP_LCD_SetFont(&Font12);
  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
//  BSP_LCD_DisplayStringAt(5, 5, (uint8_t*)"ELEC3730 Assignment 3 (v03 $Rev: 1330 $)",LEFT_MODE);
//  BSP_LCD_DisplayStringAt(5,20, (uint8_t*)"This demonstration code can be used",LEFT_MODE);
//  BSP_LCD_DisplayStringAt(5,35, (uint8_t*)"as a starting point for the assignment.",LEFT_MODE);
  osMutexRelease(myMutex01Handle);

  myWriteFile();

  while (1)
  {
	  Parser();
  }

}

uint8_t myReadFile()
{
#define READ_FILE "Hello.txt"
#define BUFF_SIZE 256
	uint8_t rtext[BUFF_SIZE];
	FRESULT res;
	uint32_t bytesread;

	// Open file Hello.txt
	if((res = f_open(&MyFile, READ_FILE, FA_READ)) != FR_OK)
	{
		safe_printf("ERROR: Opening '%s'\n", READ_FILE);
		return 1;
	}
	safe_printf("Task 1: Opened file '%s'\n", READ_FILE);

	// Read data from file
	if ((res = f_read(&MyFile, rtext, BUFF_SIZE-1, &bytesread)) != FR_OK)
	{
		safe_printf("ERROR: Reading '%s'\n", READ_FILE);
		f_close(&MyFile);
		return 1;
	}
	rtext[bytesread] = '\0';
	safe_printf("Task 1: Read: '%s'\n", rtext);

	// Close file
	f_close(&MyFile);

	return 0;
}

uint8_t myWriteFile()
{
#define WRITE_FILE "There.txt"
	FRESULT res;
	UINT byteswritten;

	// Open file There.txt
	if((res = f_open(&MyFile, WRITE_FILE, FA_CREATE_ALWAYS | FA_WRITE)) != FR_OK)
	{
		safe_printf("ERROR: Opening '%s'\n", WRITE_FILE);
		return 1;
	}
	safe_printf("Task 1: Opened file '%s'\n", WRITE_FILE);

	// Write to file
	if ((res = f_write(&MyFile, "Hello", 6, &byteswritten)) != FR_OK)
	{
		safe_printf("ERROR: Writing '%s'\n", WRITE_FILE);
		f_close(&MyFile);
		return 1;
	}
	safe_printf("Task 1: Written: %d bytes\n", byteswritten);

	// Close file
	f_close(&MyFile);


	return 0;
}

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

/*uint8_t myReadFile();
uint8_t myWriteFile();*/

//FIL MyFile;
FIL MyFile2, MyFile3;
FRESULT Status;

void debug_func(uint8_t num_args, char **args)
{
	// if no arguments, print status
	if(num_args == 0) {
		safe_printf("Debug mode: %s\n", get_debug_mode_status ? "Enabled" : "Disabled");
	}
	else {
		if(strcmp(args[0], "on") == 0) {
			debug_mode_enabled = true;
		}
		else if(strcmp(args[0], "off") == 0) {
			debug_mode_enabled = false;
		}
		else {
			safe_printf(CONSOLE_RED("Unknown parameter given\n"));
		}
	}
}

void analog_func(uint8_t num_args, char **args)
{
	// if no arguments, print status
	if(num_args == 0) {
		safe_printf("Analog Value Currently: %d\n", analog_value);
	}
	else {
		if(is_numeric(args[0])) {
			int val = to_int(args[0]);
			set_analog_value(val);

			set_ADC_Pos(0);
		}
		else {
			safe_printf(CONSOLE_RED("Non numeric argument given.\n"));
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
		safe_printf("No argument given.\n");
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
		safe_printf("No argument given.\n");
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
		safe_printf("No argument given.\n");
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
		safe_printf("No argument given.\n");
	}
	else if(num_args == 1) {
		if(strcmp(args[0], "rf") == 0) {
			safe_printf("Deleting everything in folder\n");
			char *cwd = SD_get_Cwd();
			SD_Delete_Everything(cwd);

			free(cwd);
		} else {
			bool result = SD_Delete(args[0]);
			if(!result) {
				safe_printf(CONSOLE_RED("Delete Failed.\n"));
			}
		}
	}
}

void cwd_func(uint8_t num_args, char **args)
{
	SD_Cwd();
}

void mv_func(uint8_t num_args, char **args)
{
	if(num_args < 2) {
		safe_printf("Incorrect number of argument given.\n");
	}
	else {
		if(!SD_Move_File(args[0], args[1])) {
			safe_printf(CONSOLE_RED("Failed to move file.\n"));
		}
	}
}

void ren_func(uint8_t num_args, char **args)
{
	if(num_args < 2) {
		safe_printf("Incorrect number of argument given.\n");
	}
	else {
		if(!SD_Move_File(args[0], args[1])) {
			safe_printf(CONSOLE_RED("Failed to rename file.\n"));
		}
	}
}

void tree_func(uint8_t num_args, char **args)
{
	bool result = true;
	if(num_args == 0) {
		char *cwd = SD_get_Cwd();
		result = SD_Tree(cwd);
		free(cwd);
	} else {
		result = SD_Tree(args[0]);
	}

	if(!result) {
		safe_printf(CONSOLE_RED("Failed to print tree.\n"));
	}
}

void cat_func(uint8_t num_args, char **args)
{
	if(num_args == 0) {
		safe_printf("Invalid number of arguments given.\n");
		return;
	}
	char *data;
	int length;
	bool result = SD_Read_File(args[0], &data, &length);
	if(!result) {
		safe_printf(CONSOLE_RED("Failed to read file %s.\n"), args[0]);
		return;
	}

	for(int i = 0; i < length; i++) {
		safe_printf("%c", data[i]);
	}
	safe_printf("\n");
}

void head_func(uint8_t num_args, char **args)
{
	if(num_args == 0) {
		safe_printf("Invalid number of arguments given.\n");
		return;
	}
	char *data;
	int length;
	bool result = SD_Read_File(args[0], &data, &length);
	if(!result) {
		safe_printf(CONSOLE_RED("Failed to read file %s.\n"), args[0]);
		return;
	}

	for(int i = 0; i < 10; i++) {
		safe_printf("%c", data[i]);
	}
	safe_printf("\n");
}

void tail_func(uint8_t num_args, char **args)
{
	if(num_args == 0) {
		safe_printf("Invalid number of arguments given.\n");
		return;
	}
	char *data;
	int length;
	bool result = SD_Read_File(args[0], &data, &length);
	if(!result) {
		safe_printf(CONSOLE_RED("Failed to read file %s.\n"), args[0]);
		return;
	}

	for(int i = length - 10 - 1; i < length; i++) {
		safe_printf("%c", data[i]);
	}
	safe_printf("\n");
}

void clear_func(uint8_t num_args, char **args)
{
	for(int i = 0; i < 50; i++) {
		safe_printf("\n");
	}
}

void help_func(uint8_t num_args, char **args)
{
	if(num_args == 0) {
		safe_printf(CONSOLE_YELLOW("Help: All functions.\n"));
		for(int i = 0; i < get_rectangles_length(); i++)
		{
			safe_printf(CONSOLE_YELLOW("%s\n"), Commands[i].help_string);
		}
	}
	else if(num_args == 1)
	{
		for(int i = 0; i < get_rectangles_length(); i++)
		{
			if(strcmp(args[0], Commands[i].function_name) == 0)
			{
				safe_printf(CONSOLE_YELLOW("%s\n"), Commands[i].help_string);
				return;
			}
		}

		safe_printf(CONSOLE_RED("Command not found\n"));
	}
}

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

	safe_printf(CONSOLE_RED(" Command not found.\n"));
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
			safe_printf("Fucked up malloc\n");
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
			safe_printf("Something fucked up big time\n");
		}
		strcat(in_string, &c);
		safe_printf("%c", c);
	}
}

void Ass_03_Task_01(void const * argument)
{
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
  osMutexRelease(myMutex01Handle);

  while (1)
  {
	  Parser();
  }
}

/*uint8_t myReadFile()
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
*/

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

FIL MyFile2, MyFile3;
FRESULT Status;

// function to enable and disable debug mode
void debug_func(uint8_t num_args, char **args)
{
	// if no arguments, print status
	if(num_args == 0) {
		safe_printf("Debug mode: %s\n", get_debug_mode_status() ? "Enabled" : "Disabled");
	}
	// else, check second argument
	else {
		if(strcmp(args[0], "on") == 0) {
			set_debug_mode_status(true);
			safe_printf("Debug mode is on\n");
		}
		else if(strcmp(args[0], "off") == 0) {
			set_debug_mode_status(false);
			safe_printf("Debug mode is off\n");
		}
		else {
			safe_printf(CONSOLE_RED("Unknown parameter given\n"));
		}
	}
}

// function to set the analog value
void analog_func(uint8_t num_args, char **args)
{
	// if no arguments, print status
	if(num_args == 0) {
		safe_printf("Analog Value Currently: %d\n", get_analog_value());
	}
	else {
		// make sure argument given is numeric
		if(is_numeric(args[0])) {
			// convert given argument to int
			int val = to_int(args[0]);
			// set the analog value
			set_analog_value(val);

			// create string for updating screen
			char* new_string = malloc(4);
			sprintf(new_string, "%ds", val);

			// update screen
			osMutexWait(myMutex01Handle, osWaitForever);
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_SetFont(&Font8);
			BSP_LCD_DisplayStringAt(rectangles[11].x + rectangles[11].width / 2, rectangles[11].y + rectangles[11].height / 2, "   ", CENTER_MODE);
			BSP_LCD_DisplayStringAt(rectangles[11].x + rectangles[11].width / 2, rectangles[11].y + rectangles[11].height / 2, new_string, CENTER_MODE);
			BSP_LCD_SetFont(&Font12);
			//BSP_LCD_DrawRect(55-1,15-1,250+1,150+1); // don't know if we need this
			osMutexRelease(myMutex01Handle);

			// free string
			free(new_string);

			safe_printf("Analog Value now: %d\n", get_analog_value());
		}
		else {
			safe_printf(CONSOLE_RED("Non numeric argument given.\n"));
		}
	}
}

// function to print files in working directory
void ls_func(uint8_t num_args, char **args)
{
	// list present working directory files
	FRESULT res;
	char *buff = SD_get_Cwd();
	if(buff == NULL) {
		safe_printf(CONSOLE_RED("Failed to print directory listing.\n"));
		 return;
	}

	res = SD_Scan_Directory(buff);

	free(buff);
}

// function to change working directory
void cd_func(uint8_t num_args, char **args)
{
	if(num_args == 0) {
		safe_printf("No argument given.\n");
	}
	else {
		// change the directory
		bool result = SD_Change_Directory(args[0]);
		// check success
		if(!result) {
			safe_printf(CONSOLE_RED("Directory does not exist.\n"));
		}
	}
}

// function to make directory
void mkdir_func(uint8_t num_args, char **args)
{
	if(num_args == 0) {
		safe_printf("No argument given.\n");
	}
	else {
		// try to create directory
		bool result = SD_Create_Directory(args[0]);
		// check success
		if(!result) {
			safe_printf(CONSOLE_RED("Unable to create folder.\n"));
		}
	}
}

// function to copy files and folders
void cp_func(uint8_t num_args, char **args)
{
	if(num_args == 0) {
		safe_printf("No argument given.\n");
	}
	else {
		// copy file
		bool result = SD_Copy(args[0], args[1]);
		// check success
		if(!result) {
			safe_printf(CONSOLE_RED("Copy Failed.\n"));
		}
	}
}

// function to remove files and folders
void rm_func(uint8_t num_args, char **args)
{
	if(num_args == 0) {
		safe_printf("No argument given.\n");
	}
	else if(num_args == 1) {
		// doesn't actually handle flags correctly, but just for fun
		// if argument is rf, delete everything
		if(strcmp(args[0], "rf") == 0) {
			safe_printf("Deleting everything in folder\n");
			// get current working directory
			char *cwd = SD_get_Cwd();
			// delete everything
			SD_Delete_Everything(cwd);

			// free data
			free(cwd);
		} else {
			// else, just delete file given in first argument
			bool result = SD_Delete(args[0]);
			// check success
			if(!result) {
				safe_printf(CONSOLE_RED("Delete Failed.\n"));
			}
		}
	}
}

// function to print working directory
void cwd_func(uint8_t num_args, char **args)
{
	// prints out working directory
	SD_Cwd();
}

// function to move files and folders
void mv_func(uint8_t num_args, char **args)
{
	// make sure there enough arguments are given
	if(num_args < 2) {
		safe_printf("Incorrect number of argument given.\n");
	}
	else {
		// move file and check success
		if(!SD_Move_File(args[0], args[1])) {
			safe_printf(CONSOLE_RED("Failed to move file.\n"));
		}
	}
}

// function to rename files and folders
void ren_func(uint8_t num_args, char **args)
{
	// make sure enough arguments given
	if(num_args < 2) {
		safe_printf("Incorrect number of argument given.\n");
	}
	else {
		// move is also used for renaming, check success
		if(!SD_Move_File(args[0], args[1])) {
			safe_printf(CONSOLE_RED("Failed to rename file.\n"));
		}
	}
}

// function to print out directory tree
void tree_func(uint8_t num_args, char **args)
{
	bool result = true;
	// if no arguments given
	if(num_args == 0) {
		// get current working directory
		char *cwd = SD_get_Cwd();
		// print tree from here
		result = SD_Tree(cwd);
		// free string
		free(cwd);
	} else {
		// if argument given, print tree from that directory
		result = SD_Tree(args[0]);
	}

	// check success
	if(!result) {
		safe_printf(CONSOLE_RED("Failed to print tree.\n"));
	}
}

// function to print data in a file
void cat_func(uint8_t num_args, char **args)
{
	// check correct number of arguments given
	if(num_args == 0) {
		safe_printf("Invalid number of arguments given.\n");
		return;
	}

	// variables for to hold data read from file
	char *data;
	int length;
	// read data from file
	bool result = SD_Read_File(args[0], &data, &length);
	// check success
	if(!result) {
		safe_printf(CONSOLE_RED("Failed to read file %s.\n"), args[0]);
		return;
	}

	// print out every character
	for(int i = 0; i < length; i++) {
		safe_printf("%c", data[i]);
	}
	safe_printf("\n");

	// free file data
	free(data);
}

// function to print just the first 10 lines of file
void head_func(uint8_t num_args, char **args)
{
	// check file given as argument
	if(num_args == 0) {
		safe_printf("Invalid number of arguments given.\n");
		return;
	}

	// variables for to hold data read from file
	char *data;
	int length;
	// read data from file
	bool result = SD_Read_File(args[0], &data, &length);
	// check success
	if(!result) {
		safe_printf(CONSOLE_RED("Failed to read file %s.\n"), args[0]);
		return;
	}

	// print out every character
	for(int i = 0; i < 10; i++) {
		safe_printf("%c", data[i]);
	}
	safe_printf("\n");

	// free file data
	free(data);
}

// function to print just the last 10 lines of file
void tail_func(uint8_t num_args, char **args)
{
	// check file given as argument
	if(num_args == 0) {
		safe_printf("Invalid number of arguments given.\n");
		return;
	}
	// variables for to hold data read from file
	char *data;
	int length;
	// read data from file
	bool result = SD_Read_File(args[0], &data, &length);
	// check success
	if(!result) {
		safe_printf(CONSOLE_RED("Failed to read file %s.\n"), args[0]);
		return;
	}

	// print characters
	for(int i = length - 10 - 1; i < length; i++) {
		safe_printf("%c", data[i]);
	}
	safe_printf("\n");

	// free file data
	free(data);
}

// function to clear the screen
void clear_func(uint8_t num_args, char **args)
{
	// just prints 50 newlines
	// just hides the screen, but it's all we can do from here
	for(int i = 0; i < 50; i++) {
		safe_printf("\n");
	}
}

// function to print out help for all commands
void help_func(uint8_t num_args, char **args)
{
	// if no arguments given, print help for all commands
	if(num_args == 0) {
		safe_printf(CONSOLE_YELLOW("Help: All functions.\n"));
		for(int i = 0; i < get_rectangles_length(); i++)
		{
			safe_printf(CONSOLE_YELLOW("%s\n"), Commands[i].help_string);
		}
	}
	// if argument given, print help for that one
	else if(num_args == 1)
	{
		// loop over all commands
		for(int i = 0; i < get_rectangles_length(); i++)
		{
			// check if it's the commands given
			if(strcmp(args[0], Commands[i].function_name) == 0)
			{
				// print it's help string
				safe_printf(CONSOLE_YELLOW("%s\n"), Commands[i].help_string);
				return;
			}
		}

		safe_printf(CONSOLE_RED("Command not found\n"));
	}
}

// function splits input string into words and then processes them
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

// variables to hold input string data and state
static char *in_string;
static int in_initialized = 0;
void Parser()
{
	// if string needs to be initialised or reset
	if(in_initialized == 0)
	{
		// if data in string, free it
		if(in_string != NULL)
		{
			free(in_string);
		}
		// allocate data for new string
		in_string = malloc(2);
		// check fail
		if(in_string == NULL) {
			safe_printf(CONSOLE_RED("Failed to allocate data for input string\n"));
		}
		// setup string
		strcpy(in_string, "");
		in_initialized = 1;
	}

	// get character from console
	char c = getchar();
	// check if it's a command ASCII character
	if ((c < 32) | (c > 126))
	{
		// if backspace
		if(c == 127) {
			// print backspace
			safe_printf("%c", (char)127);
			// remove last character from input strung
			in_string[strlen(in_string)-1] = '\0';
		}
		// if enter\return
		else if(c == 13) {
			// print newline
			safe_printf("%c", c);
			// process string
			CommandProcessor(in_string);
			// flag that string needs to be reset
			in_initialized = 0;
		}
	}
	// if just normal alphanumeric stuff
	else
	{
		// allocate more memory for string
		in_string = realloc(in_string, 1 + strlen(in_string) + 1);
		// check fail
		if(in_string == NULL) {
			safe_printf(CONSOLE_RED("Failed to allocate memory for string\n"));
		}

		// add to string
		strcat(in_string, &c);
		// print what we got
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
  osSignalSet(dataGrabberHandle, 1);

  // Display welcome message
  osMutexWait(myMutex01Handle, osWaitForever);
  BSP_LCD_Clear(LCD_COLOR_BLACK);
  BSP_LCD_SetFont(&Font12);
  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
  osMutexRelease(myMutex01Handle);

  // loop and handle input
  while (1)
  {
	  Parser();
  }
}

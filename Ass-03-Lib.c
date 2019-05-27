//     $Date: 2018-05-16 16:40:22 +1000 (Wed, 16 May 2018) $
// $Revision: 1321 $
//   $Author: Peter $

#include "Ass-03.h"

//
// PLACE YOUR COMMON CODE HERE
//

bool is_char_numeric(char c)
{
	return c >= '0' && c <= '9';
}

bool is_numeric(char *str)
{
	for(int i = 0; i < strlen(str); i++)
	{
		if(!is_char_numeric(str[i])) return false;
	}

	return true;
}

int to_int(char *str)
{
	int ret;
	int result = sscanf(str, "%d", &ret);

	return ret;
}

int string_parser(char *inp, char **array_of_words_p[])
{

   int string_length = 0, letterCount = 0, wordCount = 0;
   for (int i = 0; inp[i] != '\0'; i++){
     string_length++;
   }

   char *wordArray = (char*)malloc(sizeof(char) * string_length);
   int elements = 0;
   for (int i = 0; inp[i] != '\0'; i++){
     if (inp[i] != 8){
       wordArray[elements] = inp[i];
       elements++;
     }
     else if (elements > 0){
       elements--;
     }

   }
   wordArray[elements] = '\0';



   for (int i = 0; inp[i] != '\0'; i++){
     if ((i > 0 && inp[i] == ' ' && inp[i-1] != ' ')
           || (inp[i+1] == '\0' && inp[i] != ' ')){
       wordCount++;
     }
   }

   *array_of_words_p = (char**)malloc(sizeof(char*) * wordCount);
   wordCount = 0;

   for (int i = 0; wordArray[i] != '\0'; i++){
     if ((i > 0 && wordArray[i] == ' ' && wordArray[i-1] != ' ') || (wordArray[i+1] == '\0' && wordArray[i] != ' ')){
       if (wordArray[i+1] == '\0'){
         letterCount++;
       }
       (*array_of_words_p)[wordCount] = (char*)malloc(sizeof(char)*(letterCount+1));

       letterCount = 0;
       wordCount++;
     }
     if (i > 0 && wordArray[i] == ' ' && wordArray[i-1] == ' '){
       continue;
     }
     else {
       letterCount++;
     }
   }

   wordCount = 0;
   letterCount = 0;


   for (int i = 0; wordArray[i] != '\0'; i++){
     if ((i > 0 && wordArray[i] == ' ' && wordArray[i-1] != ' ') || (wordArray[i+1] == '\0' && wordArray[i] != ' ')){
       if (wordArray[i+1] == '\0'){
         (*array_of_words_p)[wordCount][letterCount] = wordArray[i];
         letterCount++;
       }
       (*array_of_words_p)[wordCount][letterCount] = '\0';

       wordCount++;
       letterCount = 0;
     }
     else if (wordArray[i] == ' '){
       continue;
     }
     else{

       (*array_of_words_p)[wordCount][letterCount] = wordArray[i];
       letterCount++;
     }
   }
   free(wordArray);

   return wordCount;
 printf("This code has been commented out, explained at the top of the source code!");
 return 0;
}

uint16_t get_DMA_Value(int i)
{
	osMutexWait(globalVariableHandle, osWaitForever);
	uint16_t result = DMA_Buffer[i];
	osMutexRelease(globalVariableHandle);

	return result;
}

uint16_t **get_DMA_Value_p()
{
	osMutexWait(globalVariableHandle, osWaitForever);
	uint16_t **result = &DMA_Buffer;
	osMutexRelease(globalVariableHandle);

	return result;
}

bool get_loaded()
{
	osMutexWait(globalVariableHandle, osWaitForever);
	bool result = just_loaded;
	osMutexRelease(globalVariableHandle);

	return result;
}

void set_loaded(bool val)
{
	osMutexWait(globalVariableHandle, osWaitForever);
	just_loaded = val;
	osMutexRelease(globalVariableHandle);
}

int get_ADC_Pos()
{
	osMutexWait(globalVariableHandle, osWaitForever);
	int result = ADC_Pos;
	osMutexRelease(globalVariableHandle);

	return result;
}

void set_ADC_Pos(int val)
{
	osMutexWait(globalVariableHandle, osWaitForever);
	ADC_Pos = val;
	osMutexRelease(globalVariableHandle);
}

uint16_t get_ADC_Value(int i)
{
	osMutexWait(globalVariableHandle, osWaitForever);
	int pos = ADC_Pos;
	//safe_printf("ADC_Pos value %d\n", pos);
	//safe_printf("i value %d\n", i);

	uint16_t result = ADC_Value[pos][i];

	osMutexRelease(globalVariableHandle);

	//safe_printf("returning value %d\n", result);

	return result;
}

uint16_t *get_ADC_Value_p()
{
	osMutexWait(globalVariableHandle, osWaitForever);
	uint16_t *result = &ADC_Value[ADC_Pos];
	osMutexRelease(globalVariableHandle);

	//safe_printf("address: 0x%X\n", result);

	return result;
}

uint16_t **get_ADC_Total_p()
{
	osMutexWait(globalVariableHandle, osWaitForever);
	uint16_t **result = ADC_Value;
	osMutexRelease(globalVariableHandle);

	return result;
}

void set_ADC_Value(int i, uint16_t val)
{
	osMutexWait(globalVariableHandle, osWaitForever);
	ADC_Value[ADC_Pos][i] = val;
	osMutexRelease(globalVariableHandle);
}

uint16_t get_ADC_Single_Value(int pos, int i)
{
	osMutexWait(globalVariableHandle, osWaitForever);
	uint16_t result = ADC_Value[pos][i];
	osMutexRelease(globalVariableHandle);

	return result;
}

uint16_t *get_ADC_Array(int i)
{
	osMutexWait(globalVariableHandle, osWaitForever);
	uint16_t *result = ADC_Value[i];
	osMutexRelease(globalVariableHandle);

	return result;
}

uint16_t *get_Copy_ADC_Array(int i)
{
	osMutexWait(globalVariableHandle, osWaitForever);
	uint16_t *data = malloc(2000);
	if(data == NULL) {
		safe_printf("Failed to allocate memory for data copy\n");
		return NULL;
	}
	memcpy(data, ADC_Value[i], 2000);
	osMutexRelease(globalVariableHandle);

	return data;
}

bool get_debug_mode_status()
{
	osMutexWait(globalVariableHandle, osWaitForever);
	bool result = debug_mode_enabled;
	osMutexRelease(globalVariableHandle);

	return result;
}

void set_debug_mode_status(bool val)
{
	osMutexWait(globalVariableHandle, osWaitForever);
	debug_mode_enabled = val;
	osMutexRelease(globalVariableHandle);
}

int get_analog_value()
{
	osMutexWait(globalVariableHandle, osWaitForever);
	int result = analog_value;
	osMutexRelease(globalVariableHandle);

	return result;
}

void set_analog_value(int val)
{
	osMutexWait(globalVariableHandle, osWaitForever);
	analog_value = val;
	osMutexRelease(globalVariableHandle);
}

FIL get_File_Handle()
{
	osMutexWait(globalVariableHandle, osWaitForever);
	FIL result = MyFile;
	osMutexRelease(globalVariableHandle);

	return result;
}

void set_File_Handle(FIL val)
{
	osMutexWait(globalVariableHandle, osWaitForever);
	MyFile = val;
	osMutexRelease(globalVariableHandle);
}

int get_rectangles_length()
{
	osMutexWait(globalVariableHandle, osWaitForever);
	int result = rectangles_length;
	osMutexRelease(globalVariableHandle);

	return result;
}

int get_memory_location()
{
	osMutexWait(globalVariableHandle, osWaitForever);
	int result = memory_location;
	osMutexRelease(globalVariableHandle);

	return result;
}

void set_memory_location(int val)
{
	osMutexWait(globalVariableHandle, osWaitForever);
	memory_location = val;
	osMutexRelease(globalVariableHandle);
}

bool get_start()
{
	osMutexWait(globalVariableHandle, osWaitForever);
	bool result = start;
	osMutexRelease(globalVariableHandle);

	return result;
}

void set_start(bool val)
{
	osMutexWait(globalVariableHandle, osWaitForever);
	start = val;
	osMutexRelease(globalVariableHandle);
}

bool get_snake_time()
{
	osMutexWait(globalVariableHandle, osWaitForever);
	bool result = snake_time;
	osMutexRelease(globalVariableHandle);

	return result;
}

void set_snake_time(bool val)
{
	osMutexWait(globalVariableHandle, osWaitForever);
	snake_time = val;
	osMutexRelease(globalVariableHandle);
}

Rectangle* get_rectangles()
{
	osMutexWait(globalVariableHandle, osWaitForever);
	Rectangle* result = rectangles;
	osMutexRelease(globalVariableHandle);

	return result;
}

FRESULT SD_Scan_Directory(char *path)
{
	FRESULT res;
	DIR dir;
	UINT i;
	static FILINFO fno;

	int num_files = 0;
	int num_folders = 0;


	res = f_opendir(&dir, path);                       /* Open the directory */
	if (res == FR_OK) {
		for (;;) {
			res = f_readdir(&dir, &fno);                   /* Read a directory item */
			if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
//			if (fno.fattrib & AM_DIR) {                    /* It is a directory */
//				i = strlen(path);
//				sprintf(&path[i], "/%s", fno.fname);
//				res = SD_Scan_Directory(path);                    /* Enter the directory */
//				if (res != FR_OK) break;
//				path[i] = 0;
//			} else {                                       /* It is a file. */
//				safe_printf("%s/%s\n", path, fno.fname);
//			}

			if(fno.fattrib & AM_DIR) {
				num_folders++;
				safe_printf(CONSOLE_BLUE("%s/%s\n"), path, fno.fname);
			}
			else {
				num_files++;
				safe_printf(CONSOLE_CYAN("%s/%s\n"), path, fno.fname);
			}

			//safe_printf("%s/%s\n", path, fno.fname);
		}
		f_closedir(&dir);
	}

	return res;
}

bool SD_Change_Directory(char *str)
{
	FRESULT res = f_chdir(str);
	if(res != FR_OK) {
		safe_printf("Failed to Change Directory to %s.\n", str);
		safe_printf("Error code: %d\n", res);
		return false;
	}

	return true;
}

bool SD_Create_Directory(char *str)
{
	FRESULT res = f_mkdir(str);
	if(res != FR_OK) {
		safe_printf("Failed to Create Directory %s.\n", str);
		safe_printf("Error code: %d\n", res);
		return false;
	}

	return true;
}

bool SD_Copy(char *src, char *dst)
{
	FRESULT res = f_open(&MyFile, src, FA_READ);
	if(res != FR_OK) {
		safe_printf("Failed to open file %s\n", src);
		safe_printf("Error code: %d\n", res);
		return false;
	}

	FSIZE_t size = f_size(&MyFile);

	uint8_t* data = calloc(size, 1);
	if(data == NULL) {
		safe_printf("Failed to allocate memory for reading file.\n");
		safe_printf("Error code: %d\n", res);
		return false;
	}
	uint32_t bytes_read;

	res = f_read(&MyFile, data, size-1, &bytes_read);
	if(res != FR_OK) {
		safe_printf("Failed to read file %s\n", src);
		safe_printf("Error code: %d\n", res);
		free(data);
		f_close(&MyFile);
		return false;
	}

	f_close(&MyFile);



	// read destination file
	res = f_open(&MyFile, dst, FA_CREATE_NEW | FA_WRITE);
	if(res != FR_OK) {
		safe_printf("Failed to open file %s\n", src);
		safe_printf("Error code: %d\n", res);
		free(data);
		return false;
	}

	uint32_t bytes_written;
	res = f_write(&MyFile, data, bytes_read, &bytes_written);
	if(res != FR_OK) {
		safe_printf("Failed to write to new file.\n");
		safe_printf("Error code: %d\n", res);
		free(data);
		f_close(&MyFile);
		return false;
	}

	f_close(&MyFile);

	free(data);
	return true;
}

char *SD_get_Cwd()
{
	char *buf = malloc(255);
	FRESULT res = f_getcwd(buf, 255);
	if(res != FR_OK)
	{
		safe_printf("Failed to get current working directory.\n");
		safe_printf("Error code: %d\n", res);
		return "";
	}

	return buf;
}

bool SD_Cwd()
{
	char buf[255];
	FRESULT res = f_getcwd(buf, 255);
	if(res != FR_OK)
	{
		safe_printf("Failed to get current working directory.\n");
		safe_printf("Error code: %d\n", res);
		return false;
	}

	safe_printf("%s\n", buf);

	return true;
}

bool SD_Delete(char *str)
{
	FRESULT res = f_unlink(str);
	if(res != FR_OK) {
		safe_printf("Failed to delete file %s.\n", str);
		safe_printf("Error code: %d\n", res);
		return false;
	}

	return true;
}

bool SD_Write_File(char *file_path, uint8_t* data, int length)
{
	FRESULT res = f_open(&MyFile, file_path, FA_WRITE | FA_CREATE_ALWAYS);
	if(res != FR_OK) {
		safe_printf("Failed to open file %s.\n", file_path);
		safe_printf("Error code: %d\n", res);
		return false;
	}

	uint32_t bytes_written;
	res = f_write(&MyFile, data, length, &bytes_written);
	if(res != FR_OK) {
		safe_printf("Failed to write to file %s.\n", file_path);
		safe_printf("Error code: %d\n", res);
		f_close(&MyFile);
		return false;
	}

	safe_printf("Wrote %d bytes\n", bytes_written);

	safe_printf("Write to file successful\n");

	f_close(&MyFile);

	return true;
}

bool SD_Write_File_Offset(char *file_path, uint16_t** data, int offset)
{
	FRESULT res = f_open(&MyFile, file_path, FA_WRITE | FA_CREATE_ALWAYS);
	if(res != FR_OK) {
		safe_printf("Failed to open file %s.\n", file_path);
		safe_printf("Error code: %d\n", res);
		return false;
	}

	bool done = false;
	int pos = offset;
	while(1) {
		uint16_t *data_b = get_Copy_ADC_Array(pos);

		UINT bytes_written;
		res = f_write(&MyFile, data_b, 2000, &bytes_written);
		if(res != FR_OK) {
			safe_printf("Failed to write to file %s.\n", file_path);
			safe_printf("Error code: %d\n", res);
			f_close(&MyFile);
			return false;
		}

		free(data_b);

		pos++;
		if(pos > 9) pos = 0;

		if(pos == offset) break;
	}

	/*for(int i = offset; i <= offset-1; i++)
	{
		safe_printf("Apparently i can't C\n");
		if(i > 9) i = 0;
		safe_printf("i %d\n", i);
		char *data_b = (char*)(*data[i]);

		uint32_t bytes_written;
		res = f_write(&MyFile, data_b[i], length, &bytes_written);
		if(res != FR_OK) {
			safe_printf("Failed to write to file %s.\n", file_path);
			safe_printf("Error code: %d\n", res);
			return false;
		}
	}*/

	safe_printf("Write to file successful\n");

	f_close(&MyFile);

	return true;
}

bool SD_Read_File(char *file_path, uint8_t **data, int* length)
{
	FRESULT res = f_open(&MyFile, file_path, FA_READ);
	if(res != FR_OK) {
		safe_printf("Failed to open file %s\n", file_path);
		safe_printf("Error code: %d\n", res);
		return false;
	}

	FSIZE_t size = f_size(&MyFile);
	safe_printf("Size: %d\n", size);

	uint8_t* data_read = calloc(size, 1);
	if(data_read == NULL) {
		safe_printf("Failed to allocate memory for reading file.\n");
		safe_printf("Error code: %d\n", res);
		return false;
	}

	uint32_t bytes_read;
	res = f_read(&MyFile, data_read, size, &bytes_read);
	if(res != FR_OK) {
		safe_printf("Failed to read file %s\n", file_path);
		safe_printf("Error code: %d\n", res);
		free(data);
		f_close(&MyFile);
		return false;
	}

	safe_printf("Bytes read: %d\n", bytes_read);

	*data = data_read;
	*length = bytes_read;
	f_close(&MyFile);

	return true;
}


FRESULT SD_Delete_Everything(char *path)
{
	FRESULT res;
	DIR dir;
	UINT i;
	static FILINFO fno;

	res = f_opendir(&dir, path);                       /* Open the directory */
	if (res == FR_OK) {
		for (;;) {
			res = f_readdir(&dir, &fno);                   /* Read a directory item */
			if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */

			SD_Delete(fno.fname);

			/*if(fno.fattrib & AM_DIR) {
				safe_printf("Want to delete: %s - %s", path, fno.fname);
			}
			else {
				safe_printf("Want to delete: %s - %s", path, fno.fname);
			}*/

			//safe_printf("%s/%s\n", path, fno.fname);
		}
		f_closedir(&dir);
	}

	return res;
}

bool SD_Move_File(char *old, char *new)
{
	FRESULT res = f_rename(old, new);
	if(res != FR_OK) {
		safe_printf(CONSOLE_RED("Copy Failed.\n"));
		return false;
	}

	return true;
}

bool SD_Tree(char *path)
{
	FRESULT res;
	DIR dir;
	UINT i;
	static FILINFO fno;


	res = f_opendir(&dir, path);
	if (res == FR_OK) {
		for (;;) {
			res = f_readdir(&dir, &fno);
			if (res != FR_OK || fno.fname[0] == 0) break;
			if (fno.fattrib & AM_DIR) {
				i = strlen(path);
				sprintf(&path[i], "/%s", fno.fname);
				res = SD_Tree(path);
				if (res != FR_OK) break;
				path[i] = 0;
			} else {
				printf("%s/%s\n", path, fno.fname);
			}
		}
		f_closedir(&dir);
	}

	return res == FR_OK;
}

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


FRESULT SD_Scan_Directory(char *path)
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
//			if (fno.fattrib & AM_DIR) {                    /* It is a directory */
//				i = strlen(path);
//				sprintf(&path[i], "/%s", fno.fname);
//				res = SD_Scan_Directory(path);                    /* Enter the directory */
//				if (res != FR_OK) break;
//				path[i] = 0;
//			} else {                                       /* It is a file. */
//				safe_printf("%s/%s\n", path, fno.fname);
//			}

			safe_printf("%s/%s\n", path, fno.fname);
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
	char buf[255];
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

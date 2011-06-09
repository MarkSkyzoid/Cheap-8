/**********************************************/
/***************** CHEAP-8 ********************/
/************* chip-8 emulator ****************/
/********* written by Marco Vallario **********/
/**************** June 2011 *******************/
/**********************************************/

#include "memory.h"

//load a rom in the memory
void load_rom(const char * file_name)
{
	printf("Loading: %s\n", file_name);
		
	// Open file
	FILE * pFile = fopen(file_name, "rb");
	if (pFile == NULL)
	{
		fputs ("File error", stderr); 
		exit(1);
	}

	// Check file size
	fseek(pFile , 0 , SEEK_END);
	long lSize = ftell(pFile);
	rewind(pFile);
	printf("Filesize: %d\n", (int)lSize);
	
	// Allocate memory to contain the whole file
	char * buffer = (char*)malloc(sizeof(char) * lSize);
	if (buffer == NULL) 
	{
		fputs ("Memory error", stderr); 
		exit(1);
	}

	// Copy the file into the buffer
	size_t result = fread (buffer, 1, lSize, pFile);
	if (result != lSize) 
	{
		fputs("Reading error",stderr); 
		exit(1);
	}

	// Copy buffer to Chip8 memory
	if((4096-512) > lSize)
	{
        int i = 0;
		for(i = 0; i < lSize; ++i)
			memory[i + 512] = buffer[i];
	}
	else
		printf("Error: ROM too big for memory");
	
	// Close file, free buffer
	fclose(pFile);
	free(buffer);
}

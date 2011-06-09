/**********************************************/
/***************** CHEAP-8 ********************/
/************* chip-8 emulator ****************/
/********* written by Marco Vallario **********/
/**************** June 2011 *******************/
/**********************************************/

#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdio.h>

//important memory types
typedef unsigned char byte; //8 bit memory type
typedef unsigned short word;  //16 bit memory type

//4k array of byte to emulate the system memory
byte memory[4096];

//load a rom in the memory
void load_rom(const char* file_name);

#endif

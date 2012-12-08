/**********************************************/
/***************** CHEAP-8 ********************/
/************* chip-8 emulator ****************/
/********* written by Marco Vallario **********/
/**************** June 2011 *******************/
/**********************************************/

#ifndef _CPU_H_
#define _CPU_H_

#include <time.h>
#include "memory.h"
#include "graphics.h"
#include "keyboard.h"

//function to initialize the CPU
void init_cpu();

//function to handle the CPU's work
void cpu();

byte draw_flag;

void sleep(unsigned int mseconds);

#endif

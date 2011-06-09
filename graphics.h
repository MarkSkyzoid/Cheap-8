/**********************************************/
/***************** CHEAP-8 ********************/
/************* chip-8 emulator ****************/
/********* written by Marco Vallario **********/
/**************** June 2011 *******************/
/**********************************************/

#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <stdio.h>
#include <allegro.h>
#include "memory.h"

//draw the screen to the surface to display it to the user
void draw_screen(byte* screen, BITMAP* buffer);

#endif

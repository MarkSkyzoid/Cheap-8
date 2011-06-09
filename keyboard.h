/**********************************************/
/***************** CHEAP-8 ********************/
/************* chip-8 emulator ****************/
/********* written by Marco Vallario **********/
/**************** June 2011 *******************/
/**********************************************/

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <allegro.h>
#include "memory.h"

//Chip-8 had an hexadeciaml keyboard so we need to map it on our keyboard
//I will use this scheme
// OUR KEYBOARD            CHIP-8 KEYBOARD
/* 1 2 3 4        =>       1 2 3 C
   Q W E R        =>       4 5 6 D
   A S D F        =>       7 8 9 E
   Z X C V        =>       A 0 B F
*/


//now we need two functions: one to check if a specific key is pressed and one to wait until a key is pressed and then store the key in a register
byte check_key(byte key_pressed);

byte wait_input();

#endif

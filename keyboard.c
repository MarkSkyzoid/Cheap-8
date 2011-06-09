/**********************************************/
/***************** CHEAP-8 ********************/
/************* chip-8 emulator ****************/
/********* written by Marco Vallario **********/
/**************** June 2011 *******************/
/**********************************************/

#include "keyboard.h"

//now we need two functions: one to check if a specific key is pressed and one to wait until a key is pressed and then store the key in a register
byte check_key(byte key_pressed)
{
    byte return_value = 0; //this flag indicates if the key is pressed
    
    if(key[KEY_1] && key_pressed == 0x0) return_value = 1;
    else if(key[KEY_2] && key_pressed == 0x1) return_value = 1;
    else if(key[KEY_3] && key_pressed == 0x2) return_value = 1;
    else if(key[KEY_4] && key_pressed == 0x3) return_value = 1;
    
    else if(key[KEY_Q] && key_pressed == 0x4) return_value = 1;
    else if(key[KEY_W] && key_pressed == 0x5) return_value = 1;
    else if(key[KEY_E] && key_pressed == 0x6) return_value = 1;
    else if(key[KEY_R] && key_pressed == 0x7) return_value = 1;
    
    else if(key[KEY_A] && key_pressed == 0x8) return_value = 1;
    else if(key[KEY_S] && key_pressed == 0x9) return_value = 1;
    else if(key[KEY_D] && key_pressed == 0xA) return_value = 1;
    else if(key[KEY_F] && key_pressed == 0xB) return_value = 1;
    
    else if(key[KEY_Z] && key_pressed == 0xC) return_value = 1;
    else if(key[KEY_X] && key_pressed == 0xD) return_value = 1;
    else if(key[KEY_C] && key_pressed == 0xE) return_value = 1;
    else if(key[KEY_V] && key_pressed == 0xF) return_value = 1;

    return return_value;
}

byte wait_input()
{
    byte wait = 1; //this flag indicates if the program have to wait untile the key is pressed
    byte return_key; //the key pressed to return to the calling function
    
    while(wait)
    {
        if(key[KEY_ESC])
            exit(0);
            
        if(key[KEY_1])
        { 
            return_key = 0x0;
            wait = 0;
        }
        else if(key[KEY_2])
        { 
            return_key = 0x1;
            wait = 0;
        }
        else if(key[KEY_3])
        { 
            return_key = 0x2;
            wait = 0;
        }
        else if(key[KEY_4])
        { 
            return_key = 0x3;
            wait = 0;
        }
        
        else if(key[KEY_Q])
        { 
            return_key = 0x4;
            wait = 0;
        }
        else if(key[KEY_W])
        { 
            return_key = 0x5;
            wait = 0;
        }
        else if(key[KEY_E])
        { 
            return_key = 0x6;
            wait = 0;
        }
        else if(key[KEY_R])
        { 
            return_key = 0x7;
            wait = 0;
        }
    
        else if(key[KEY_A])
        { 
            return_key = 0x8;
            wait = 0;
        }
        else if(key[KEY_S])
        { 
            return_key = 0x9;
            wait = 0;
        }
        else if(key[KEY_D])
        { 
            return_key = 0xA;
            wait = 0;
        }
        else if(key[KEY_F])
        { 
            return_key = 0xB;
            wait = 0;
        }
    
        else if(key[KEY_Z])
        { 
            return_key = 0xC;
            wait = 0;
        }
        else if(key[KEY_X])
        { 
            return_key = 0xD;
            wait = 0;
        }
        else if(key[KEY_C])
        { 
            return_key = 0xE;
            wait = 0;
        }
        else if(key[KEY_V])
        { 
            return_key = 0xF;
            wait = 0;
        }
    } 
        
    return return_key;
}

/**********************************************/
/***************** CHEAP-8 ********************/
/************* chip-8 emulator ****************/
/********* written by Marco Vallario **********/
/**************** June 2011 *******************/
/**********************************************/

#include "graphics.h"


//draw the screen to the surface to display it to the user
void draw_screen(byte* screen, BITMAP* buffer)
{
    clear(buffer); //we clear the buffer bitmap to write on it again
    
    byte x = 0, y = 0; //coordinates for looping trough screen

    //loops trough the screen
    for(y = 0; y < 32; ++y)
    {
        for(x = 0; x < 64; ++x)
        {
            if(screen[x + y*64] == 1) //if the pixel at x,y is active
            {
                int j = 0, i = 0;
                for( i=x*8;i<(8*x+8);++i)
                    for( j=y*8;j<(8*y+8);++j)
                        putpixel(buffer, i, j, makecol(0, 255, 0)); //draw it green
            }
            else
            {
                int j = 0, i = 0;
                for( i=x*8;i<(8*x+8);++i)
                    for( j=y*8;j<(8*y+8);++j)
                        putpixel(buffer, i, j, makecol(0, 0, 0)); //draw it black
            }
        }
    }                    
}
                

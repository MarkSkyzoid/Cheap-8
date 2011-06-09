/**********************************************/
/***************** CHEAP-8 ********************/
/************* chip-8 emulator ****************/
/********* written by Marco Vallario **********/
/**************** June 2011 *******************/
/**********************************************/

#include <stdio.h>
#include <allegro.h>
#include "cpu.h"

extern byte gfx_screen[64*32]; //we call the screen from the cpu.h

int main(int argc, char** argv)
{
    allegro_init(); // Initialize Allegro
    
    install_keyboard(); //initialize keyboard
    
    set_color_depth(32); //color depth set to 32bpp
    set_gfx_mode(GFX_AUTODETECT, 512, 256,0,0); // Change our graphics mode to 512x256
    
    set_window_title("Cheap-8! Chip-8 Emulator by Marco Vallario");
    
    BITMAP* back_buf = create_bitmap(SCREEN_W, SCREEN_H); //the back buffer for double buffering
    
    init_cpu(); //initialize the cpu
    
    if(argv[1] != "")
        load_rom(argv[1]); //we load the rom
    else
        printf("USAGE: Cheap-8 rom_path\n");
    
    //MAIN LOOP while the user does not press ESC
    while(!key[KEY_ESC])
    {   
        //int i = 0;
        //for(i = 0; i < 400000; ++i); //an arbitrary number of loops to slow the cpu cycle
        
        sleep(1); //wait one millisecond
        cpu(); //we emulate the cpu
        
        //if a draw call was made we draw the screen
        if(draw_flag) 
        {
            draw_screen(gfx_screen, back_buf);
            blit(back_buf, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
            draw_flag = 0;
        }
     }
     
	return 0;
}
END_OF_MAIN();

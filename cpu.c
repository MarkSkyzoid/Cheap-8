/**********************************************/
/***************** CHEAP-8 ********************/
/************* chip-8 emulator ****************/
/********* written by Marco Vallario **********/
/**************** June 2011 *******************/
/**********************************************/

#include "cpu.h"


//array of 16 unsigned char(1 byte) to emulate the registers
//first 15 register are general purpose registers. The 16th is the "carry-flag" register
unsigned char V[16];

//there are timing register
//the first timing reegister is called Delay Timer. It is active whenever it is non-zero. Once it reaches zero it deactivates. It is decremented every cycle
byte DT;
//the second timing register is the Sound Timer. It produces a BEEP when it is non-zero. Once it reaches zero it deactivates. It is decremented every cycle
byte ST;

//here are declarations of the Program Counter(PC) and Memory Address Register(I). Both are 16-bit registers
static word PC, I;

//the stack to handle return addresses from subroutine calls. Chip-8 allows up to 16 nested sub-calls
word stack[16];
byte stack_counter; //the position in the stack

//here we define an array corresponding to the chip-8's screen. The original screen resolution is 64x32 pixels and it was monochrome
byte gfx_screen[64*32];


//chip-8 has a set of built-in characters.
//They are handled like sprites so each row of a character sizes 8-bit.
//Each character has 5 rows. 
//They are stored in memory area from 0x000 to 0x1FF.
byte fontset[80] =
{ 
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

//function to initialize the CPU
void init_cpu()
{
    //set the PC to 0x200
    //in the real system addresses from 0x000 to 0x200 are reserved to the intrpreter
    //since we are emulating the system this memory is empty
    //so the PC starts from the memory address(0x200) where the program is loaded
    PC = 0x200;
    
    //we reset the memory address register
    I = 0;

    //we also need to set the registers to 0
    word i = 0, j = 0;
    for(i = 0; i < 16; ++i)
        V[i] = 0;

    //set the stack_counter to 0(firts position in the stack)
    stack_counter = 0;
    
    //we reset the stack
    for(i = 0; i < 16; ++i)
        stack[i] = 0;

    //set timing register to zero
    DT = 0;
    ST = 0;   
    //we clear the memory
    for(i = 0; i < 4096; ++i)
        memory[i] = 0;
    
    //we load the built-in characters in to the memory from 0x000 to 0x1FF
    for(i = 0; i < 80; ++i)
        memory[i] = fontset[i];

    //we also erase the screen
    for(i = 0; i < 32; ++i)
        for(j = 0; j < 64; ++j)
            gfx_screen[j + (i * 64)] = 0;
            
    //we don't want to draw the screen each cycle
    draw_flag = 0;
    
    srand(time(NULL)); //we set a new seed for the random algorithm
}

//cpu function to handle the cpu work
void cpu()
{
    word opcode = ((memory[PC]<<8) + memory[PC+1]); //opcode(2 byte) of the program in memory. Remember: every element of memory is 1 byte

    //variables for screen and sprites
    word screen_x = 0, screen_y = 0; //cordinates on the screen
    word sprite_data; //the data of a row of a sprite
    
    //variables for counting in loops
    byte i = 0; //for-loops index

    //Here we do operations based on opcodes
    switch(opcode&0xF000)
    {
        //opcodes: 00??
        case 0x0000:
            switch(opcode&0x00FF)
            {
                //there are many opcodes which start with 0 so we need to process the right one

                //opcode: 0x00E0
                //this opcode clears the screen
                case 0x00E0:
                    //we loop trough the screen and set each pixel to 0
                    for(screen_y = 0; screen_y < 32; ++screen_y)
                        for(screen_x = 0; screen_x < 64; ++screen_x)
                            gfx_screen[screen_x + screen_y*64] = 0;
                            
                    draw_flag = 1; //after clearing the screen we want it to be redrawn

                    PC += 2;
                    break;

                //opcode: 0x00EE
                //this opcode returns from a subroutine call
                case 0x00EE:
                    --stack_counter; //set the position to the last address in the stack
                    PC = stack[stack_counter]; //copy the last address in the stack to the PC
                    PC += 2; //go to the next opcode
                    break;
            }
            break;

        //opcode: 1NNN
        //this opcode jumps to address NNN
        case 0x1000:
            PC = (opcode&0x0FFF); //jump to the address NNN
            break;

        //opcode: 2NNN
        //this opcode jumps to the subroutine at address NNN
        case 0x2000:
            stack[stack_counter] = PC; //set the PC as the current stack's position value
            ++stack_counter; //point to the next stack position
            PC = (opcode&0x0FFF); //jump to the address NNN
            break;

        //opcode: 3XRR
        //this opcode skips next istruction if register V[X] == constant RR
        case 0x3000:
            if(V[((opcode&0x0F00)>>8)] == (opcode&0x00FF)) //we check if the register X is equal to the value RR
                PC += 4; //we add 4 bytes to the PC to skip to the next instruction
            else
                PC += 2; //we add 2 bytes to the PC to read the next opcode

            break;

        //opcode: 4XRR
        //this opcode skips next intruction if register V[X] != constant RR
        case 0x4000:
            if(V[((opcode&0x0F00)>>8)] != (opcode&0x00FF)) //we check if the register X is NOT equal to the value RR
                PC += 4; //we add 4 bytes to the PC to skip to the next instruction
            else
                PC += 2; //we add 2 bytes to the PC to read the next opcode

            break;

        //opcode: 5XY0
        //this opcode skips the next instruction if register V[X] == V[Y]
        case 0x5000:
            if(V[((opcode&0x0F00)>>8)] == V[((opcode&0x00F0)>>4)]) //we check if registers are equal
                PC +=4; //we add 4 bytes to the PC to skip the next instruction
            else
                PC += 2; //we add 2 bytes to the PC to read the next opcode

            break;

        //Example opcode(6XRR) to understand why to do these operations: 6XRR
        //6:  the opcode
        //X:  the register number(so V[X])
        //RR: costant
        //This opcode copies the costant RR to the register V[X]
        //opcode&0xF000 returns 0x6000(the opcode)
        //opcode&0x0F00 returns 0x0X00(the register - first operand); we switch this 8 bits to the right to have 0x000X(which is the right number)
        //opcode&0x00FF returns 0x00RR(the costant - second operand)
        case 0x6000:
            V[((opcode&0x0F00)>>8)] = opcode&0x00FF; //copy RR to V[X]
            PC += 2; //we add 2 bytes to the PC to read the next opcode
            break;

        //opcode: 7XRR
        //this opcode adds constant RR to register V[X]
        case 0x7000:
            V[((opcode&0x0F00)>>8)] += (opcode&0x00FF); //adds RR to V[X]
            PC += 2; //we add 2 bytes to the PC to read the next opcode
            break;

        //opcodes: 8XY?
        case 0x8000:
            //there are many opcodes which start with 8 so we need to process the right one
            switch(opcode&0x000F) //the last number is important
            {
                //opcode: 8XY0
                //this opcode moves register V[Y] into V[X]
                case 0x0000:
                    V[((opcode&0x0F00)>>8)] = V[((opcode&0x00F0)>>4)]; //copy V[Y] in V[X]
                    PC += 2; //we add 2 bytes to the PC to read the next opcode
                    break;

                //opcode: 8XY1 (TO CHECK)
                //this opcode does a bitwise OR to register V[Y] with register V[X] and stores result into register V[X]
                case 0x0001:
                    V[((opcode&0x0F00)>>8)] |= V[((opcode&0x00F0)>>4)]; //V[X] = V[X] AND V[Y]
                    PC += 2; //we add 2 bytes to the PC to read the next opcode
                    break;

                //opcode: 8XY2 (TO CHECK)
                //this opcode does a bitwise AND to register V[Y] with register V[X] and stores result into register V[X]
                case 0x0002:
                    V[((opcode&0x0F00)>>8)] &= V[((opcode&0x00F0)>>4)]; //V[X] = V[X] OR V[Y]
                    PC += 2; //we add 2 bytes to the PC to read the next opcode
                    break;

                //opcode: 8XY3 (TO CHECK)
                //this opcode does a bitwise EXLUSIVE OR to register V[Y] with register V[X] and stores result into register V[X]
                case 0x0003:
                    V[((opcode&0x0F00)>>8)] ^= V[((opcode&0x00F0)>>4)]; //V[X] = V[X] XOR V[Y]
                    PC += 2; //we add 2 bytes to the PC to read the next opcode
                    break;

                //opcode: 8XY4
                //this opcode adds register V[Y] to V[X], stores result in register V[X] and the carry stored in register VF(carry flag)
                case 0x0004:
                    if(V[((opcode&0x0F00)>>8)] > (0xFF - V[((opcode&0x00F0)>>4)])) //if the sum is greater than byte's max value(255)
                        V[0xF] = 1; //there is a carry
                    else
                        V[0xF] = 0; //no carry

                    V[((opcode&0x0F00)>>8)] += V[((opcode&0x00F0)>>4)]; //adds V[Y] to V[X] and stores in V[X]
                    PC += 2; //we add 2 bytes to the PC to read the next opcode
                    break;

                //opcode: 8XY5
                //this opcode subtracts register V[Y] from V[X], stores result in V[X] and the borrow is stored in register VF(carry flag)
                case 0x0005:
                    if(V[((opcode&0x0F00)>>8)] < V[((opcode&0x00F0)>>4)]) //if V[Y] is greater than V[X] then there is a borrow
                        V[0xF] = 0;
                    else
                        V[0xF] = 1;

                    V[((opcode&0x0F00)>>8)] -= V[((opcode&0x00F0)>>4)]; //subtracts V[Y] from V[X] and stores in V[X]
                    PC += 2; //we add 2 bytes to the PC to read the next opcode
                    break;

                //opcode: 8X06
                //this opcode shifts the V[X] register one position right
                //the last bit goes to VF(carry flag)
                case 0x0006:
                    V[0xF] = V[((opcode&0x0F00)>>8)]&0x1; //copy the last bit to VF
                    V[((opcode&0x0F00)>>8)]>>=1; //shifts the register right
                    PC += 2; //we add 2 bytes to the PC to read the next opcode
                    break;

                //opcode: 8XY7
                //this opcode subtracts register V[X] from V[Y], stores result in V[X] and the borrow is stored in register VF(carry flag)
                case 0x0007:
                    if(V[((opcode&0x00F0)>>4)] < V[((opcode&0x0F00)>>8)]) //if V[X] is greater than V[Y] then there is a borrow
                        V[0xF] = 0;
                    else
                        V[0xF] = 1;

                    V[((opcode&0x0F00)>>8)] = V[((opcode&0x00F0)>>4)] - V[((opcode&0x0F00)>>8)]; //subtracts V[X] from V[Y] and stores in V[X]
                    PC += 2; //we add 2 bytes to the PC to read the next opcode
                    break;//this opcode

                //opcode: 8X0E
                //this opcode shifts the V[X] register one position left
                //the 7th bit goes to VF(carry flag)
                case 0x000E:
                    V[0xF] = (V[((opcode&0x0F00)>>8)])>>7; //copy the 7th bit to VF
                    V[((opcode&0x0F00)>>8)]<<=1; //shifts the register left
                    PC += 2; //we add 2 bytes to the PC to read the next opcode
                    break;
            }
            break;

            //opcode: 9XY0
            //this opcode skips next instruction if register V[X] != register V[Y]
            case 0x9000:
                if(V[((opcode&0x0F00)>>8)] != V[((opcode&0x00F0)>>4)]) //we check if register are NOT equal
                    PC +=4; //we add 4 bytes to the PC to skip the next instruction
                else
                    PC += 2; //we add 2 bytes to the PC to read the next opcode

                break;

            //opcode: ANNN
            //this opcode loads NNN to the Memory Address Register(I)
            case 0xA000:
                I = (opcode&0x0FFF);
                PC += 2; //we add 2 bytes to the PC to read the next opcode
                break;

            //opcode: BNNN
            //this opcode jumps to the address NNN+V[0]
            case 0xB000:
                PC = (opcode&0x0FFF) + V[0];
                break;

            //opcode: CXKK
            //this opcode load into register V[X] a random number(byte) AND(bitwise operator) KK
            case 0xC000:
                V[((opcode&0x0F00)>>8)] = (rand()%0xFF) & (opcode&0x00FF); //we set V[X] = random byte AND KK
                PC += 2; //we add 2 bytes to the PC to read the next opcode
                break;

            //opcode: DXYN
            //this opcode draws at coordinates V[X],V[Y] N byte of a sprite wich location is stored in Memory Address Register(I)
            //Drawing operates in XOR mode so if there is a collision VF is set to 1
            //sprites' width is always 8 and their height is equal to N. N max value is 15. So the largest sprites we can have is 8x15
            case 0xD000:
            {
                screen_x = V[(opcode & 0x0F00) >> 8];       //set the X screen coordinate to V[X]
                screen_y = V[(opcode & 0x00F0) >> 4];       //set the Y screen coordinate to V[X]
                byte height = opcode & 0x000F;              //sprite's heigth
                byte sprite_line;                           //row of the sprite  
 
                
                byte yline; //current row
                byte xline; //cyrrent pixel in the current row
                
                V[0xF] = 0; //we reset VF to reset the collision state
                
                for (yline = 0; yline < height; ++yline)
                {
                    if(I + yline > 0xFFF && I + yline < 0x00) //not valid addressess
                    {
                        PC += 2; //jump to next istruction
                        break;
                    }
                    sprite_line = memory[I + yline]; //copy the yline-row of the sprite to sprite_line
                    
                    //now we need to chek if there is a collision
                    //so we loop each row and compare the bits of sprite with the bits of the screen
                    //if they both are 1 then there is a collision(OLDSCHOOL PIXEL PERFECT COLLISION ;))
                    for(xline = 0; xline < 8; ++xline) //to scan each bit of the sprite_line row
                    {
                        if((sprite_line & (0x80 >> xline)) != 0) //if the current bit in row is non-zero
                        {
                            if(gfx_screen[(screen_x + xline + ((screen_y + yline) * 64))] == 1) //and if also the screen position corresponding to the current sprite's bit is non-zero
                                V[0xF] = 1; //there is a collision! So we update VF
                                                             
                            gfx_screen[screen_x + xline + ((screen_y + yline) * 64)] ^= 1; //we update the screen in XOR mode
                        }
                    }
                }    
                draw_flag = 1; //after clearing the screen we want it to be redrawn    
                PC += 2;//go to the next instruction
                break;
            }
                break;

            //opcodes: EX??
            case 0xE000:
                //there are many opcodes which start with 8 so we need to process the right one
                switch(opcode&0x00FF)
                {
                    //opcode: EX9E
                    //this opcode skips the next instruction if the user presses the key stored in V[X]
                    case 0x009E:
                        if(check_key(V[((opcode&0x0F00)>>8)])) //if the key pressed equals to the key in V[X]
                            PC += 4; //skip next instruction
                        else
                            PC += 2; //go to the next instruction
                        break;

                    //opcode: EXA1
                    //this opcode skips the next instruction if the user doesn't press the key stored in V[X]
                    case 0x00A1:
                        if(!check_key(V[((opcode&0x0F00)>>8)])) //if the key pressed does NOT equal to the key in V[X]
                            PC += 4; //skip next instruction
                        else
                            PC += 2; //go to the next instruction
                        break;
                }
                break;
                
            //opcodes: FX??
            case 0xF000:
                //there are many opcodes which start with 8 so we need to process the right one
                switch(opcode&0x00FF)
                {
                    //opcode: FX07
                    //this opcode sets the value of V[X] to the value of DT(delay timer)
                    case 0x0007:
                        V[((opcode&0x0F00)>>8)] = DT; //copy the value of DT into V[X]
                        PC += 2; //go to the next instruction
                        break;
                        
                    //opcode: FX0A
                    //this opcode waits for a keypress and stores the key pressed in register V[X]
                    case 0x000A:
                        V[((opcode&0x0F00)>>8)] = wait_input(); //wait the keypress and store the key in V[X]
                        PC += 2; //go to the next instruction
                        break;
                        
                    //opcode: FX15
                    //this opcode sets the delay timer(DT) to the value in register V[X]
                    case 0x0015:
                        DT = V[((opcode&0x0F00)>>8)]; //DT is set to the value in V[X]
                        PC += 2; //go to the next instruction
                        break;
                    
                    //opcode: FX18
                    //this opcode sets the sound timer(ST) to the value in register V[X]
                    case 0x0018:
                        ST = V[((opcode&0x0F00)>>8)]; //ST is set to the value in V[X]
                        PC += 2; //go to the next instruction
                        break;
                        
                    //opcode: FX1E
                    //this opcode adds the memory address register(I) to V[X] and stores the result in I
                    case 0x001E:
                        if(I + V[(opcode & 0x0F00) >> 8] > 0xFFF)	// VF is set to 1 when range overflow (I+VX>0xFFF), and 0 when there isn't.
					       V[0xF] = 1;
					    else
						  V[0xF] = 0;
						  
                        I += V[((opcode&0x0F00)>>8)]; //we add V[X] to I and store the result in I
                        PC += 2; //go to the next instruction
                        break;
                        
                    //opcode: FX29
                    //this opcode sets I(memory address register) to point to the location of the sprite of the character in V[X]
                    case 0x0029:
                        //to find the correct character we multiply the value in V[X] by 5
                        //this because every character sprite is 5 bytes high
                        I = V[((opcode&0x0F00)>>8)] * 0x5; 
                        PC += 2; //go to the next instruction
                        break;
                    
                    //opcode: FX33
                    //this opcode stores BCD representation of the value in V[X] in memory locations I, I+1, and I+2.
                    //This means that hundreds digit is stored in location I, tens digit in location I+1 and ones digit in location I + 2
                    case 0x0033:
                        memory[I] = V[((opcode&0x0F00)>>8)] / 100;      //hundreds digit
                        memory[I+1] = (V[((opcode&0x0F00)>>8)]/10)%10;  //tens digit
                        memory[I+2] = (V[((opcode&0x0F00)>>8)]%100)%10; //ones digit
                        PC += 2; //go to the next instruction
                        break;
                    
                    //opcode: FX55
                    //this opcode stores registers from V[0] to V[X] in memory starting at location in I(memory address register)
                    case 0x0055:
	   	               for(i = 0; i <= ((opcode&0x0F00)>>8); ++i)
	   	                   memory[I+i] = V[i]; //copy the content of the current register at location I + register index
	   	                
	   	               // On the original interpreter, when the operation is done, I = I + X + 1.
					   I += ((opcode & 0x0F00) >> 8) + 1;
	   	                
	   	               PC += 2; //go to the next instruction
                       break;
                        
                    //opcode: FX65
                    //this opcode stores in registers from V[0] to V[X] values contained in memory starting at location in I(memory address register)
                    case 0x0065:
	   	               for(i = 0; i <= ((opcode&0x0F00)>>8); ++i)
	   	                   V[i] = memory[I+i]; //copy the content of the current location I in current register
	   	                   
	   	                // On the original interpreter, when the operation is done, I = I + X + 1.
					    I += ((opcode & 0x0F00) >> 8) + 1;
	   	                
	   	                PC += 2; //go to the next instruction
                        break;
                        
                }
                break;
                
                default:
                    //invalid opcode
                    exit(0);
                    break;
    }
    

    //now we update the two timers
    //Delay Timer
    if(DT > 0)
        --DT;
    //Sound Timer
    if(ST > 0)
    {   
        if(ST == 1) printf("BEEP!\n\a"); //if the Sound Timer is 1 then play beep!
        --ST;  
    }
}

void sleep(unsigned int mseconds)
{
    clock_t goal = mseconds + clock();
    while (goal > clock());
}
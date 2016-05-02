#include <avr/interrupt.h>
typedef unsigned char byte;
byte buttonsA = 0, buttonsB = 0, mask = 0;
// the setup function runs once when you press reset or power the board
void setup() {
  //configure pins for proper I/O, 2 and 3 are input (latch and clock) Rx (0) is also left as input
  //Any pin from 4-7 could be used as output, but we will use pin 7
  //Moved to loop because of strange issues using setup
  asm volatile(
    "SETUP:\n"
    //load port configuration into R16 (1 is OUTPUT 0 is INPUT)
    "LDI R16, 0b11110010\n"
    //set PORTD configuration (DDRD pins 0-7). 
    //NOTE: OUT is faster than STS when available
    "OUT 0x0A, R16\n"
    //disable pull-up resistors (need pull-down resistors wired to pin 2 and 3)
    "IN R17, 0x35\n"
    "LDI R16, 0b00010000\n"
    "OR R16, R17\n"
    "OUT 0x35, R16\n"
    //Enable interrupts
    "SEI\n"
    //configuration for EICRA
    //INT0 will interrupt on toggle, INT1 on rising edge
    "LDI R16, 0b00001101\n"
    "STS 0x69, R16\n"
    //enable interrupt on pin2 (INT0). Pin3 is enabled later
    "SBI 0x1D, 0\n"
    :
    :
    :"r16", "r17", "cc", "memory"
    );
}
//loop currently contains test code for ensuring the Arduino SNES interface works
//Pin 4 is wired to pin 2 so it acts as the latch pulse
//pin 5 is wired to pin 3 so it acts as the clock pulse
//buttonsA and B are the sample scripted button presses with 1 representing pressed and 0 not pressed
//the button bits are as follows 
//buttonsA: B|Y|Select|Start|Up|Down|Left|right
//buttonsB: A|X|L|R|notUsed|notUsed|notUsed|notUsed
void loop(){
  /*asm("SBI 0x0B, 7");
  asm("SBI 0x0B, 4");
  delay(24);
  asm("CBI 0x0B, 4");
  for(int i = 0; i < 16; i++){
  asm ("SBI 0x0B, 5");
  delay(12);
  asm ("CBI 0x0B, 5");
  }*/
  //Do nothing unless interrupted
  //pin2 (INT0) interrupts on the latch pules
  //pin3 (INT1) interrupts on the clock pulses
}

//jumped to when SNES sends a latch pulse (INT0 Interrupt is triggered)
    ISR(INT0_vect){
      //Generate random button presses
      buttonsA = rand() % 256;
      buttonsB = rand() % 256;
      buttonsA = buttonsA | 0b11111111;
      buttonsB = buttonsB | 0b00111111;
    asm volatile(
      "EXT_INT0:\n"
        //check if latch is high or low, jump if high
        "SBIC 0x09, 2\n"
        "JMP LATCH_RISING_EDGE\n"
        "SBI 0x0B, 7\n"
        //load bitmask
        "MOV R16, %2\n"
        //output first button press on falling edge of latch
      "FIRST_BUTTON_READ:\n"
        //AND output with bitmask
        "AND R16, %0\n"
        //check if button is pressed and output
        "LSR %2\n"
        "CPI R16, 0\n"
        "BREQ FIRST_BUTTON_OUT\n"
        "JMP EXIT_LATCH\n"
        
      //code if latch is on rising edge. Should be used to prep bitmask.
      "LATCH_RISING_EDGE:\n"
        //set up bitmask
        "LDI %2, 0b10000000\n"
        "JMP EXIT_LATCH\n"

      "FIRST_BUTTON_OUT:\n"
        "CBI 0x0B, 7\n"
        
      "EXIT_LATCH:\n"
      //Enable interrupt on pin 3 (INT1) to receive clock pulses
        "SBIS 0x1D, 1\n"
        "SBI 0x1D, 1\n"
        
        :"+r"(buttonsA),"+r"(buttonsB), "+r"(mask)
        :
        :"memory", "r16", "r17"
        //wait for next interrupt
      );};
      
    //jumpted to when SNES sends clock pulse. Should be used to output the next button press
    ISR(INT1_vect){
    asm volatile(
      "EXT_INT1:\n"
        "SBIS 0x09, 3\n"
        "JMP CLOCK_FALLING_EDGE\n"
        "SBI 0x0B, 7\n"
        //check if first 8 button presses are finished
        //if they are, move the next 8 instructions to buttonsA (%0) and reset the bitmask
        "CPI %2, 0\n"
        "BREQ LOAD_LAST_BUTTONS\n"
      "BUTTON_READ:\n"
        //AND output with bitmask
        "MOV R16, %2\n"
        "AND R16, %0\n"
        //check if button is pressed and output
        "CPI R16, 0\n"
        "BREQ BUTTON_OUT\n"
        "JMP EXIT\n"
        
      "LOAD_LAST_BUTTONS:\n"
        "MOV %0, %1\n"
        "LDI %2, 0b1000000\n"
        "JMP BUTTON_READ\n"
      
      "BUTTON_OUT:\n"
        "CBI 0x0B, 7\n"

      "EXIT:\n"
        "LSR %2\n"
      "CLOCK_FALLING_EDGE:\n"
        
        :"+r"(buttonsA),"+r"(buttonsB), "+r"(mask)
        :
        :"memory", "r16"
        //*/
        );
        //*/
        };

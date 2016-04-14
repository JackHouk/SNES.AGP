#include <avr/interrupt.h>
// the setup function runs once when you press reset or power the board
void setup() {
  //configure pins for proper I/O, 2 and 3 are input (latch and clock) Rx (0) is also left as input
  //Any pin from 4-7 could be used as output, but we will use pin 7
  //Moved to loop because of strange issues using setup
  asm volatile(
    "SETUP:\n"
    "push R16\n"
    //load port configuration into R16 (1 is OUTPUT 0 is INPUT)
    "LDI R16, 0b11110010\n"
    //set PORTD configuration (DDRD pins 0-7). 
    //NOTE: OUT is faster than STS when available
    "OUT 0x0A, R16\n"
    //activate pull-up resistors on pins 2 & 3
    "LDI R16, 0b00001100\n"
    "OUT 0x0B, R16\n"
    /*//disable pull-up resistors (need pull-down resistors wired to pin 2 and 3)
    "IN R17, 0x35\n"
    "LDI R16, 0b00010000\n"
    "OR R16, R17\n"
    "OUT 0x35, R16\n"
    //Enable interrupts*/
    "SEI\n"
    //configuration for EICRA
    //INT0 will interrupt on toggle, INT1 interrupts on rising edge
    "LDI R16, 0b00001001\n"
    "STS 0x69, R16\n"
    //enable interrupt on pin2 (INT0) and pin3 (INT1)
    "SBI 0x1D, 0\n"
    "SBI 0x1D, 1\n"
    "LDI R16, 0\n"
    "pop R16\n"
    );
}
void loop(){
  //Listen for pulses from SNES, and output pulses to represent button states
  //pin2 (INT0) interrupts on the latch pules
  //pin3 (INT1) interrupts on the clock pulses
  asm volatile(
    //save R16 and R17 so it can be restored at end of function
    "push R16\n"
    "push R17\n"
    //wait for interrupts
    "WAIT_INT:\n"
      "NOP\n"
      "JMP WAIT_INT\n"
      );
}

//jumped to when SNES sends a latch pulse (INT0 Interrupt is triggered)
    ISR(INT0_vect){
    asm volatile(
      "EXT_INT0:\n"
        "SBI 0x0B, 7\n"/*
        //check if latch is high or low, jump if high
        "SBIC 0x09, 2\n"
        "JMP LATCH_RISING_EDGE\n"
        //output first button press on falling edge of latch
        //AND output with bitmask
        "AND R16, R18\n"
        //shift output left
        "LSL R18\n"
        //check if button is pressed and output
        "IN R17, 0x09\n"
        "OR R16, R17\n"
        "OUT 0x0B, R16\n"
        //reset bitmask
        "LDI R16, 0b10000000\n"
      //code if latch is on rising edge. Should be used to prep data for output, load things from memory, etc.
      "LATCH_RISING_EDGE:\n"
      "CBI 0x0B, 7\n"
        //set up R16 as bitmask for output
        "LDI R16, 0b10000000\n"
        //(NOT IMPLEMENTED)load values from memory for button presses
        //currently using ldi to load hard coded test values, 0 is pressed 1 is not pressed
        "LDI R18, 0b00000000\n"
        "LDI R19, 0b00000000\n"
        //wait for next interrupt*/
      );};
      
    //jumpted to when SNES sends clock pulse. Should be used to output the next button press
    ISR(INT1_vect){
    asm volatile(
      "EXT_INT1:\n"
      "CBI 0x0B, 7\n"/*
        //check if first 8 button presses are finished
        "LDI R17, 0\n"
        "CPSE R17, R18\n"
        //if they are, move the next 8 instructions to the R18 register
        "MOV R18, R19\n"
        //code if latch is on falling edge. Should be used to output the first bit to the SNES.
      "BUTTON_OUT:\n"
      "CBI 0x0B, 7\n"
        //output first button press on falling edge of latch
        //AND output with bitmask
        "AND R16, R18\n"
        //shift output left
        "LSL R18\n"
        //check if button is pressed and output
        "IN R17, 0x09\n"
        "OR R16, R17\n"
        "OUT 0x0B, R16\n"
        //reset bitmask
        "LDI R16, 0b10000000\n"
        //BUTTON_OUT returns and waits for next interrupt*/
        );};

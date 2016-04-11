
// the setup function runs once when you press reset or power the board
void setup() {
  //configure pins for proper I/O, 2 and 3 are input (latch and clock) Rx (0) is also left as input
  asm(
    "push R16\n"
    //load port configuration into R16 (1 is in 0 is out)
    "LDI R16, 0b11110010\n"
    //set PORTD configuration (DDRD pins 0-7). 
    //NOTE: OUT is faster than STS when available
    "OUT 0x0A, R16\n"
    //Enable interrupts
    "SEI\n"
    "pop R16\n"
    ::
    );
}

// the loop function runs over and over again forever
void loop() {
  //Listen for pulses from SNES, and output pulses to represent button states
  //pin2 (INT0) interrupts on the latch pules
  //pin3 (INT1) interrupts on the clock pulses
  asm(
    //save R16 so it can be restored at end of function
    "push R16\n"
    //enable interrupt on pin2 (INT0) and pin3 (INT1)
    "SBI 0x1D, 0\n"
    "SBI 0x1D, 1\n"
    
    //configuration for EICRA
    //INT0 will interrupt on toggle, INT1 interrupts on rising edge
    "LDI R16, 0b00001101\n"
    "STS 0x69, R16\n"

    //jumped to when SNES sends a latch pulse (INT0 Interrupt is triggered)
    "EXT_INT0:\n"
      //check if latch is high or low
      "SBIS 0x09, 2\n"
        "JMP LATCH_FALLING_EDGE\n"
      //code if latch is on rising edge. Should be used to prep data for output, load things from memory, etc.
      "LATCH_RISING_EDGE:\n"

      //code if latch is on falling edge. Should be used to output the first bit to the SNES.
      "LATCH_FALLING_EDGE:\n"
        //LATCH_FALLING_EDGE should sleep at end and wait for next interrupt (should be the first clock pulse)
        "SLEEP\n"
    //jumpted to when SNES sends clock pulse. Should be used to output the next button press
    "EXT_INT1:\n"
    //end of asm function
    "POP R16\n"
    ::
    );
}

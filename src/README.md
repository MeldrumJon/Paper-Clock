# Paper Clock Software

## Tools

- AVR ISP Programmer or [arduinoISP](https://www.arduino.cc/en/tutorial/arduinoISP).
- (optional--for debugging) USB to TTY serial adapter.

## Software

- AVR-GCC toolchain
- avrdude

Install the AVR-GCC toolchain and avrdude (and update your `PATH` variable, 
if applicable).

## Programming

1. Connect the AVR ISP programmer pins to the ICSP header on the PCB (or the 
SPI pins on the ATMega328p).
    - Update the `program` and `fuses` recipes in the makefile to use your AVR 
    ISP programmer.
2. (optional--for debugging) Connect the ATMega328p's TX pin to RX on the TTY 
serial adapter.
    - Update the `serial` recipe in the makefile to use your USB to TTY serial 
    adapter.
3. Run `make fuses` to set the fuses on the ATMega328p.
    - If the ATMega328p was previously set to use an external clock or crystal 
    oscillator, an external clock or crystal oscillator will be needed to set 
    the fuses.
4. Run `make program` to program the ATMega328p with the clock software.

![Software Display](../../assets/Image.jpg)

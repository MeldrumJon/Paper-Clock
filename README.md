# Paper-Clock

Minimalistic clock with an e-ink display.

![Photo of Waveshare ePaper module showing the clock](../assets/Image.jpg)

## Parts

- [Waveshare 2.9" ePaper Module](https://www.waveshare.com/wiki/2.9inch_e-Paper_Module)
    - One of the newer revisions with a voltage regulator and logic level converter.
- DS1337+ RTC
- ATMega328p-PU
    - With fuses set to use internal 8MHz clock (use Makefile: `make fuses`)
- Rotary encoder with push button switch ([like these](https://www.amazon.com/dp/B0197X1UZY/ref=cm_sw_em_r_mt_dp_U_0rAEDbP2TPZJZ))
- Wuerth 614105150721 Micro USB connector
- 32.768kHz crystal, 6pF load capacitance
- 10uF electrolytic capacitor
- 3x 0.1uF ceramic capacitors
- Breakaway headers

## Tools

- AVR ISP Programmer or [arduinoISP](https://www.arduino.cc/en/tutorial/arduinoISP)
- USB to TTY serial adapter for debugging

## Schematic

![Paper Clock Schematic](../assets/Schematic.png?raw=true)

## Setup

1. Install the AVR-GCC toolchain (and update your `PATH` variable, if necessary)
2. Connect the AVR ISP programmer pins to the SPI pins on the ATMega328p.
3. Connect the ATMega328p's TX pin to RX on the TTY serial adapter.
4. Update the `program` and `fuses` recipes in the makefile to use your AVR 
ISP programmer.
5. Update the `serial` recipe in the makefile to use your USB to TTY serial 
adapter.
6. Run `make fuses` to set the fuses on the ATMega328p.
    - If the ATMega328p was previously set to use an external clock or crystal 
    oscillator, an external clock or crystal oscillator will be needed to set 
    the fuses.

## Programming

Run `make program` to program the ATMega328p.


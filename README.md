# Paper-Clock

Minimalistic clock with an e-ink display.

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



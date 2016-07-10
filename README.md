# Tiaracon 2016 PCB badge
http://tiaracon.org/
ATtiny-based charlieplexed LED flasher derived from https://git.io/fireflies with different artwork and fancy pcb

## Hardware Description
This project is designed as the badge for tiaracon 2016. The circuit is intended to be a simple soldering project based on the ATTiny using only a handful of through-hole components. It uses 3 I/O pins to drive 6 charlieplexed LEDs, and an additional I/O pin for a mode button.

## Bill of Materials:
* 6x 3mm or 5mm LEDs with .1" spacing
* 3x 150 Ohm Resistor
* 1x ATTiny85 8-DIP
* 1x 20mm CR2032 holder and battery
* 1x 6x6mm SPST-NO mode button

In quantity, BOM cost is just over $2 (unless, like today, ATTinys are $4 each on digikey instead of $1)

##Building
All of the components are labeled right on the board. The ATTiny goes on the BACK of the board, the side with the ponys. Note the location of pin 1. All other components can go on either side, though it seems like LEDS on front and everything else on the back looks best.

##Software Description
This code is an Arduino sketch. I used the Sparkfun TinyAVR Programmer and follow the linked hookup guide to get it working: https://www.sparkfun.com/products/11801

The code for lighting the Charlieplexed LED's is cut and pasted from Jason Webb's project linked above. The code for sensing the capacitive buttons is based on Paul Stoffregen's updated CapacitiveSensor libarry: https://github.com/PaulStoffregen/CapacitiveSensor 

I rely heavily on global variables, and use the state and values to determine what to display on any given iteration. This allows me to responsively check the capsense button without dealing with interrupts.




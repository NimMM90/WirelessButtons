# BLE HID buttons/Rotary Encoders using an Adafruit Feather nRF52 Bluefruit LE - nRF52832

Rather drastic changes compared to the upstream repo to simplify certain things for my use-case, and to fix certain issues. 

![Front of the 488 style wheel](https://github.com/WemansG90/WirelessButtons/blob/master/Images/Front.jpg)

_**IMPORTANT!**_

It seems the newer versions of the board library for the Feather BLE removed some bits in the AdafruitHID section. I'm on 0.18.5 at time of writing.

</br>

## Parts
**For the 12 button 4 encoder version**:
 - 1 x [Feather BLE](https://www.adafruit.com/product/3406), only tested on the nRF52832. Might(?) work on the nRF52840
 - 12 Normally Open buttons of your choosing
 - 4 EC11 style encoders with a buttons
 - 24 x Signal Diodes (one per button)
 - Hookup wire to reach all your buttons and encoders
 - Soldering supplies
 - [A battery compatible with the Feather](https://learn.adafruit.com/bluefruit-nrf52-feather-learning-guide/power-management). I'm using an 18650 instead of a soft-cell, as I have a pair of internally protected 3500mAh ones. 
    - The Feather can charge this battery with it's onboard charger, but this is limited at 100mA, so an external charger might be prudent.

 **Optional**:
  - a small perfboard/protoboard to do the diode matrix on and mount the feather
  - sockets for the feather/said board so you don't have to desolder the feather


</br>

## Key matrix
**Info**: 
- https://www.baldengineer.com/arduino-keyboard-matrix-tutorial.html
- https://learn.adafruit.com/bluefruit-nrf52-feather-learning-guide/device-pinout

**Library used**: 
- https://github.com/Chris--A/Keypad

**Pins used** :
 - rows: A0-A3 
 - columns: A4-A7
## Rotary Encoders

**Library Used**
- https://github.com/j-bellavance/CommonBusEncoders

This allows us to save on some pins and have a rather readable way to read CW/CCW rotations. Point of note is that the encoder switch and mode switching is not used here, as these switches are wired into the above matrix

**Pins used**
- Bus A: 16
- Bus B: 15
- Switch Bus: 14 (Not hooked up)
- Encoder 1: 7
- Encoder 2: 11
- Encoder 3: 26
- Encoder 4: 27

</br>

## Debug vs Production
Commenting/Uncommenting the following define toggles the Serial debugging: 
```
#define PRODUCTION 1
```
If this is defined, there'll be NO DEBUG sent to Serial. Comment this out if you're debugging/changing stuff. 

</br>

## Limitations / Set up for your situation

I tweaked the original code a bit to suit my needs. Given that the HID setup allows for a maximum of 24 buttons as it's set up, this is perfect for the 12 button 4 encoder setup I use:
- 10 buttons on the wheel
- 2 buttons for the paddle shifters
- 4 buttons for the clicky encoders

This gives us our 4x4 switch matrix. The Encoders than fill the remaining space the 3 bytes used in the HID section give us

### Pins To Avoid
- Pin 31 is for reading battery voltage and can't be used as an input. 
- RXD/TXD (pins 8 & 6) should not be used for Rotary encoders
- Unsure if this matters for inputs, but the docs say that SCL/SDA don't have pull-up resistors. They seemed to work, but when I implemented the matrix input, I no longer needed them and avoided them.
- Maybe avoid pin 20 (DFU). If it's tied LOW it'll reset/wipe the device. Basically, go read the pinouts doc for the feather. There's lots of useful info there about pins to watch out for.

### Changing the number of buttons/ENcoders

- Change NUMBER_OF_BUTTONS in vars.h
- Change ROWS/COLS to be whatever works for your wiring.
- expand the 2d keys array to match.
- Keep the HID byte shenanigans into account (buttoncount will always be multiples of 8?)

</br>

## Future upgrades?

- The current "smash all the diodes on the switches/some perfboard" approach is rather messy. Getting round to learning how to use KiCad and making a PCB to wire everything to would be preferable. 
![Rat's Nest](https://github.com/WemansG90/WirelessButtons/blob/master/Images/Rats_Nest.jpg)
- Moving from the NRF52 feather to a wireless/BLE enabled RP2040 board might be interesting [Example: Arduino Nano RP2040 Connect](https://launch.arduino.cc/nano-rp2040) 


</br>

## Major Revisions compared to Upstream

The Upstream master code is available in the upstream branch for reference, should the original repo vanish

### Removals
- All the LED and power handling code has been removed
- The offending callback function for said LED/power handling breaking everyting has been defenestrated 
- Key matrix setup is a clearly readable 2D array again, to match the Keypad examples better.
- Removed some debugging toggles
- Hardcoded the blue LED being off, wouldn't see it anyway in a sealed wheel/box

### Battery Monitoring
 - Reverted to the 12bit 3.0V example in [here](https://learn.adafruit.com/bluefruit-nrf52-feather-learning-guide/nrf52-adc)
 - Set MONITOR_BATTERY to true.
 - VBATPIN should be 31 (assuming a feather BLE unit)
 - "close enough" on my 18650s to be useful





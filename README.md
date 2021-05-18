Wireless Buttons & Encoders!
=========================================

Seeing as how upstream master doesn't seem to work at all at the moment, this is an attempt at a simpler/dumbed down, but working, version of a wireless buttonbox/sim wheel.

**IMPORTANT**!
--------

It seems the newer versions of the board library for the Feather BLE removed some bits in the AdafruitHID section. I'm on 0.18.5 at time of writing

Parts
-----
**For the 12 button 4 encoder version**:
 - 1 x [Feather BLE](https://www.adafruit.com/product/3406), only tested on the nRF52832. Might(?) work on the nRF52840
 - 12 Normally Open buttons of your choosing
 - 4 EC11 encoders with a button
 - 24 x Signal Diodes (one per button)
 - Hookup wire to reach all your buttons and encoders
 - The ability to solder :)
   - Soldering iron! 
 - [A battery compatible with the Feather](https://learn.adafruit.com/bluefruit-nrf52-feather-learning-guide/power-management). I'm using an 18650 instead of a soft-cell, as I have a pair of internally protected 3500mAh ones. 

 **Optional**:
  - a small perfboard/protoboard to do the diode matrix on and mount the feather
  - sockets for the feather/said board so you don't have to desolder the feather

Key matrix
------------
**Info**: 
- https://www.baldengineer.com/arduino-keyboard-matrix-tutorial.html
- https://learn.adafruit.com/bluefruit-nrf52-feather-learning-guide/device-pinout

**Library used**: 
- https://github.com/Chris--A/Keypad

Wiring all this up neatly is a bit more work, but it allows you to free up more pins. The diodes are to prevent ghosting.

Wiring
---------
**Pins used** :
 - rows: A0-A3 
 - columns: A4-A7

**TODO: Add wiring diagram**

Rotary Encoders
---------------------

*WIP: Testing out CommonBusEncoders to clean up some more pins, and have overall neater encoder handling**


Limitations / Set up for your situation
---------------------------------------

I tweaked the original code a bit to suit my needs. Given that the HID setup allows for a maximum of 24 buttons as it's set up, this is perfect for the 12 button 4 encoder setup I use:
- 10 buttons on the wheel
- 2 buttons for the paddle shifters
- 4 buttons for the clicky encoders

This gives us our 4x4 switch matrix. The remaining switches are filled in my the Encoder rotation, clockwise and counterclockwise being a button each. This gives us an extra 8 buttons to assign

**Major Changes/Reverted sections**
- All the LED and power handling code has been removed
- The offending callback function breaking everyting has been defenestrated as it was of no further use
- Key matrix setup is a clearly readable 2D array again, to match the Keypad examples better.
- Removed some debugging toggles
- Hardcoded the blue LED being off, wouldn't see it anyway in a sealed wheel/box

**Original info**
 - "pinouts" for your ROWS/COLS of the matrix are in rowPins and colPins respectively.
 - "button numbers" start at 0. All push buttons in the matrix are assumed to come before anything else. Rotary encoder outputs should begin at NUMBER_OF_BUTTONS (button numbers are zero relative).
 - encoderConfiguration:
  *TODO depending on how CommonBusEncoders end up working*
 - It seems HID inputs must be a multiple of 8 bits. Things didn't work for me otherwise. I'm no HID expert, so I just made everything a multiple of 8 and got on with it.  *Knowing far less than Neil about how HID does it's black magicks, I didn't touch any of this and stuck to the multiples of 8*.
 - Rotary Encoders are *manually configured* to appear as buttons.  So if you change the number of buttons, you might need to change the encoder button numbers as well.
Pins Warning
------------
- 31 is for reading lipo voltage. Don't use it for an input. 
- RXD/TXD (pins 8 & 6) should not be used for Rotary encoders
- Unsure if this matters for inputs, but the docs say that SCL/SDA don't have pull-up resistors. They seemed to work, but when I implemented the matrix input, I no longer needed them and avoided them.
- Maybe avoid pin 20 (DFU). If it's tied LOW it'll reset/wipe the device. Basically, go read the pinouts doc for the feather. There's lots of useful info there about pins to watch out for.

Changing the number of buttons
------------------------------
- Change NUMBER_OF_BUTTONS in vars.h
- Change ROWS/COLS to be whatever works for your wiring.
- expand the 2d keys array to match.
- remember the overall byte (multiples of 8) limit/figure out how to not be stuck on this yourself by figuring out BLE HID for this board

Battery Monitoring
------------------
 - Reverted to example in [here](https://learn.adafruit.com/bluefruit-nrf52-feather-learning-guide/nrf52-adc)
 - Set MONITOR_BATTERY to true.
 - VBATPIN should be 31 (assuming a feather BLE unit)
 - "close enough" on my 18650s to be useful

Charging
--------
The Feather has a charger on board, which might work for smaller lipo packs if you leave the USB port accessible/make a pigtail to have it accessible. For 18650s or similar I'd just make the battery easy to remove and have a stand-alone charger

Debug vs Production
-------------------

There's a very important #define in vars.h.
The one that reads:

```
#define PRODUCTION 1
```

If this is defined, there'll be NO DEBUG sent to Serial.
Comment this out if you're debugging/changing stuff. You'll get a sensible summary and also notifications when buttons are pressed/released.


I don't have any encoders
-------------------------

**TODO**


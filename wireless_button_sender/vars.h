#ifndef __MASHER_VARS_
#define __MASHER_VARS_

// Comment this out if in dev, so that you can see Serial output.
// Its needed to NOT wait for Serial to appear, if in prod
//
// This should be defined, if building for the real button plate.
#define PRODUCTION 1

#ifndef PRODUCTION
#define DEBUG 1
#endif

#ifdef DEBUG
#define DEBUG_MONITOR_BATTERY 1       // set to 1 to see debug Serial output
#define DEBUG_BATTERY_NOTIFICATIONS 1 // set if you want to see when BLE battery level is updateds
// #define DEBUG_LOOPS_PER_SECOND 1
#define DEBUG_BUTTON_PRESSES 1 // set to 1 to see output of button presses
#endif

/* Battery Monitoring */
// What pins is the RGB LED connected to?
const int VBATPIN = 31;            // Pin from which VBAT (lipo) can be read. Adafruit nRF Feather specific.
const bool MONITOR_BATTERY = true; // set to 1 to perform monitoring/reading of pin at all

/* Buttons ... the Matrix */
// Vars affecting # of buttons reported over HID.
// By default, we make the buttons array on a multiple of 8 bits (rounded up).
// ::Note:: These values affect other struct sizes
const int NUMBER_OF_BUTTONS = 20;

// Send state every time through loop()
// Originally put in place to debug why some switch operations were missed @ Windows.
const bool ALWAYS_SEND_STATE = false;

// 11 buttons, 2 paddles, a 5-way switch and 2 encoders brings us to 20
// Define a 4x5 matrix (for the 20 buttons)
const byte ROWS = 4;
const byte COLS = 5;

/* CHANGE THIS IF YOU CHANGE NUMBER_OF_BUTTONS */
char keys[ROWS * COLS] = {
    0, 1, 2, 3, 4
    5, 6, 7, 8, 9
    10, 11, 12, 13, 14
    15, 16, 17, 18, 19};
byte rowPins[ROWS] = {2, 3, 4, 5};              //connect to the row pinouts of the kpd
byte colPins[COLS] = {28, 29, 12, 13, 14};      //connect to the column pinouts of the kpd

/* Rotary Encoders */
// How many rotary encoders you have. Each requires 2 pins for input.
// ::Note:: These values affect other struct sizes
const int NUMBER_OF_ENCODERS = 2;

/*
** Automagic stuff from here (shouldn't need to touch)
*/
// Adds a delay in loop(), so that:
// a) the entire thing uses a bit less power (I hope)
// b) we don't saturate local BLE airspace (putting on 0 does!)
// If set to 0, no delay() is called
const int __loopDelayInMs = 5;

#define ROUND_UP(N, S) ((((N) + (S)-1) / (S)) * (S))
#define NUMBER_OF_ENCODER_OUTPUTS (NUMBER_OF_ENCODERS * 2)
#define TOTAL_OUTPUTS (NUMBER_OF_BUTTONS + NUMBER_OF_ENCODER_OUTPUTS)
#define NUM_HID_INPUTS (ROUND_UP(TOTAL_OUTPUTS, 8))
#define NUMBER_OF_BUTTON_BYTES (NUM_HID_INPUTS / 8)

/* TODO: How to put in a compiler warning if NUM_HID_INPUTS not a multiple of 8???? */

typedef struct __attribute__((__packed__))
{
    uint8_t buttons[NUMBER_OF_BUTTON_BYTES];
} hid_button_masher_t;

#endif

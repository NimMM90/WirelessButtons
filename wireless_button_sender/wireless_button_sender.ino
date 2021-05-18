
#include <bluefruit.h>

#include "vars.h"
#include "buttonhid.h"
#include "buttonplate.h"
#include "battery.h"

SWBButtonPlate plate;
BatteryLevelReader batteryMonitor(VBATPIN);

void setup()
{
#ifdef DEBUG
  Serial.begin(115200);
  while (!Serial)
    delay(10); // for nrf52840 with native usb

  Serial.println("488 Wheel");
  Serial.println("-------------------\n");
  Serial.printf("Number of buttons: %d\n", NUMBER_OF_BUTTONS);
  Serial.printf("Number of encoders: %d\n", NUMBER_OF_ENCODERS);
  Serial.printf("Total number of HID Inputs: %d\n", NUM_HID_INPUTS);
  Serial.printf("Num bytes for button HID struct: %d\n", NUMBER_OF_BUTTON_BYTES);
  Serial.println();
#endif

  plate.setupButtonPlate();
#ifdef DEBUG
  Serial.println("Setup battery monitor...");
#endif
  batteryMonitor.setMonitorCallback(batteryLevelChanged);
#ifdef DEBUG
  Serial.println("Setup complete");
#endif
}

void batteryLevelChanged(BatteryLevelReader *reader, int level)
{
#ifdef DEBUG_BATTERY_NOTIFICATIONS
  Serial.printf("Battery level at: %d\n", level);
#endif
  plate.notifyNewBatteryLevel(level);
}

void plateButtonPressed(int buttonNumber, KeyState state)
{
#ifdef DEBUG
  String msg = state == PRESSED ? "Pressed" : "Released";
  Serial.print(msg);
  Serial.printf(" button number %d\n", buttonNumber);
#endif
}

static int loopCounter = 0;
static int packetCounter = 0;
static int lastLoopCountTime = 0;
static int backoffAmount = 0;
void loop()
{
  unsigned long startOfLoopMillis = millis();
  bool sendNewState = false || ALWAYS_SEND_STATE;

  // check buttons
  sendNewState = plate.pollButtons();

  // check Encoders
  sendNewState |= plate.pollEncoders();

  // Monitor battery levels. Callback is fired if it's changed.
  // The level is then set to the plate (it sends it over BLE).
  // The LED is updated all the time, to assure animations!
  batteryMonitor.monitor();

  if (sendNewState)
  {
    plate.sendInputs();
    packetCounter++;
  }

  if (__loopDelayInMs > 0)
  {
    unsigned long loopTime = millis() - startOfLoopMillis;
    if (loopTime < __loopDelayInMs)
    {
      delay(__loopDelayInMs - loopTime);
    }
  }

#ifdef DEBUG_LOOPS_PER_SECOND
  loopCounter++;
  if (startOfLoopMillis - lastLoopCountTime > 1000)
  {
    Serial.printf("Loops per second: %d. Packets per second: %d\n", loopCounter, packetCounter);
    loopCounter = 0;
    packetCounter = 0;
    lastLoopCountTime = startOfLoopMillis;
  }
#endif
}

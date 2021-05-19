
#include <bluefruit.h>

// Better Encoder Support
#include <CommonBusEncoders.h>

// For the matrix keypad
#include <Key.h>
#include <Keypad.h>

#include "vars.h"
#include "buttonhid.h"
CommonBusEncoders encoders(16,15,14,4);

typedef void (*event_button_pressed_t)(int buttonNumber, KeyState state);
class SWBButtonPlate
{
public:
  SWBButtonPlate()
  {
    varclr(&_state);
  }

  void setupButtonPlate()
  {
    Bluefruit.begin();

   // off Blue LED for lowest power consumption
    Bluefruit.autoConnLed(false);

    // Set max power. Accepted values are: -40, -30, -20, -16, -12, -8, -4, 0, 4
    Bluefruit.setTxPower(4);
#ifdef PRODUCTION
    Bluefruit.setName("488GTE Style Wheel");
#else
    Bluefruit.setName("488GTE_DEBUG");
#endif

    // Configure and Start Device Information Service
    bledis.setManufacturer("NimStuffs");
    bledis.setModel("NoFrillsBLE_Buttons");
    bledis.begin();
    
    setupButtonInputs();

    // BLE HID
    hid.begin();
    battery.begin();
    startAdvertising();
  }

  void notifyNewBatteryLevel(uint8_t newLevel)
  {
    battery.write(newLevel);
    battery.notify(newLevel);
  }

  void setupButtonInputs()
  {
#ifdef DEBUG
    Serial.printf("Using a %dx%d matrix for buttons\n", ROWS, COLS);
#endif
    keypad = new Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
#ifdef DEBUG
    Serial.println();
    Serial.println("Setting up encoders ...");
#endif
    /*
      Using the switches through the matrix, so the pin isn't hooked up
      Due to this, the switch codes also don't matter
    */
    encoders.resetChronoAfter(500);
    encoders.addEncoder(1,4,7,1,100,199);
    encoders.addEncoder(2,4,11,1,200,299);
    encoders.addEncoder(3,4,30,1,300,399);
    encoders.addEncoder(4,4,27,1,400,499);
  }

  int numberOfEncoders()
  {
    return NUMBER_OF_ENCODERS;
  }

  bool pollButtons()
  {
    bool buttonStateChanged = false;
    if (keypad)
    {
      if (keypad->getKeys())
      {
        for (int i = 0; i < LIST_MAX; i++)
        {
          if (keypad->key[i].stateChanged)
          {
            switch (keypad->key[i].kstate)
            {
            case PRESSED:
            case RELEASED:
            {
              buttonStateChanged = true;
              /* I decided to use 'int' values for the keymap.
              This means I can just cast to an int, and use that directly in a call to setButtonState 
              */
              uint8_t buttonNumber = (int)keypad->key[i].kchar;
              setButtonState(buttonNumber, keypad->key[i].kstate == PRESSED ? true : false);
              buttonStateChanged = true;
            }
            default:
              break;
            }
          }
        }
      }
    }
    return buttonStateChanged;
  }

  bool pollEncoders()
  {
    int button;
    bool encoderDidChange = false;
    int code = encoders.readAll();
    if(code>0)
    {
      sawValueAtMillis = millis();
      button = getEncoderButtons(code);
      setButtonState(button ,true);
      lastButton = button;
      encoderDidChange = true;
    }
    if (sawValueAtMillis > 0)
    {
      unsigned long diff = millis() - sawValueAtMillis;
      if (diff > encoderDelay)
      {
        setButtonState( lastButton,false);
        encoderDidChange = true;
        sawValueAtMillis = 0;
      }
    }
    return encoderDidChange;
  }

  int getEncoderButtons(int code)
  {
    int button;
    switch (code)
    {
    case 100:
    {
      button = 16;
      break;
    }
    case 101:
    {
      button = 17;
      break;
    }
    case 200:
    {
      button = 18;
      break;
    }
    case 201:
    {
      button = 19;
      break;
    }
    case 300:
    {
      button = 20;
      break;
    }
    case 301:
    {
      button = 21;
      break;
    }
    case 400:
    {
      button = 22;
      break;
    }
    case 401:
    {
      button = 23;
      break;
    }
    }
    return button;
  }

  bool setButtonState(uint8_t buttonNumber, bool state)
  {
    int arrayIndex = buttonNumber / 8;
    int arrayOffset = buttonNumber % 8;

    if (arrayIndex >= 0 || arrayIndex < 3)
    {
      int existingValue = (_state.buttons[arrayIndex] >> arrayOffset) & 0x1;
      if (existingValue != state)
      {
#ifdef DEBUG_BUTTON_PRESSES &&DEBUG
        Serial.printf("Setting button %d", buttonNumber);
        Serial.printf(", array: %d offset %d", arrayIndex, arrayOffset);
        Serial.print(", to state: ");
        state ? Serial.print("ON") : Serial.print("OFF");
        Serial.println();
#endif

        if (state)
        {
          _state.buttons[arrayIndex] |= (1 << arrayOffset);
        }
        else
        {
          _state.buttons[arrayIndex] &= ~(1 << arrayOffset);
        }

        return true;
      }
    }

    return false;
  }

  void sendInputs()
  {
    hid.sendButtons(&_state);
  }

private:
  void startAdvertising(void)
  {
    // Advertising packet
    Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
    Bluefruit.Advertising.addTxPower();
//    Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_GAMEPAD);
    Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_JOYSTICK);

    // Include BLE HID services
    Bluefruit.Advertising.addService(hid);
    Bluefruit.Advertising.addService(battery);

    // There is enough room for 'Name' in the advertising packet
    // Bluefruit.Advertising.addName();
    Bluefruit.ScanResponse.addName();

    /* Start Advertising
     - Enable auto advertising if disconnected
     - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
     - Timeout for fast mode is 30 seconds
     - Start(timeout) with timeout = 0 will advertise forever (until connected)

     For recommended advertising interval
     https://developer.apple.com/library/content/qa/qa1931/_index.html
  */
    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(32, 244); // in unit of 0.625 ms
    Bluefruit.Advertising.setFastTimeout(30);   // number of seconds in fast mode
    Bluefruit.Advertising.start(0);             // 0 = Don't stop advertising after n seconds
  }

private:
  int encoderDelay = 25;
  int lastButton;  
  unsigned long sawValueAtMillis;
  BLEBas battery;
  SWBButtonHid hid; // 24 buttons over BLE
  BLEDis bledis;
  hid_button_masher_t _state;
  event_button_pressed_t _buttonPressCallback = 0;
  Keypad *keypad = 0;
};

#define VBAT_MV_PER_LSB   (0.73242188F)   // 3.0V ADC range and 12-bit ADC resolution = 3000mV/4096
#define VBAT_DIVIDER      (0.71275837F)     // 2M + 0.806M voltage divider on VBAT = (2M / (0.806M + 2M))
#define VBAT_DIVIDER_COMP (1.403F)          // Compensation factor for the VBAT divider
#define REAL_VBAT_MV_PER_LSB (VBAT_DIVIDER_COMP * VBAT_MV_PER_LSB)

class BatteryLevelReader;

typedef void (*battery_level_changed_t)(BatteryLevelReader *reader, int currntPercent);

class BatteryLevelReader
{
public:
    BatteryLevelReader(int vBatPin,
                       int checkIntervalInMs = 2000) : _vBatPin(vBatPin),
                                                     _checkIntervalInMs(checkIntervalInMs)

    {
        _lastBatteryPercent = 0;
        _lastMonitorTime = 0;
        _lastPollTime = 0;
        _levelChangedCallback = 0;
    }

    uint8_t lastBatteryPercent() { return _lastBatteryPercent; }
    void setMonitorCallback(battery_level_changed_t cb) { _levelChangedCallback = cb; }

    bool monitor()
    {
        bool batteryValueChanged = false;
        if (MONITOR_BATTERY)
        {
            unsigned long elapsedTime = millis() - _lastMonitorTime;
            if (elapsedTime < 1000)
            {
                return false;
            }

            float currentPercent = _lastBatteryPercent;

            unsigned long elapsedPollTime = millis() - _lastPollTime;
            if (elapsedPollTime > _checkIntervalInMs)
            {
                currentPercent = batteryLevelInPercent();
                _lastPollTime = millis();
            }

            if (fabs(currentPercent - _lastBatteryPercent) > 1)
            {
                _lastBatteryPercent = currentPercent;
                batteryValueChanged = true;
            }

            _lastMonitorTime = millis();
        }
        if (batteryValueChanged)
        {
            if (_levelChangedCallback != 0)
            {
                _levelChangedCallback(this, _lastBatteryPercent);
            }
        }
        return batteryValueChanged;
    }

    uint8_t batteryLevelInPercent()
    {
        float vbat_mv = readVBAT();
        uint8_t vbat_per = mvToPercent(vbat_mv);
        #ifdef DEBUG_MONITOR_BATTERY
            Serial.print("LIPO = ");
            Serial.print(vbat_mv);
            Serial.print(" mV (");
            Serial.print(vbat_per);
            Serial.println("%)");
        #endif
        return vbat_per;
    }

private:
      float readVBAT(void)
    {
        float raw;

        // Set the analog reference to 3.0V (default = 3.6V)
        analogReference(AR_INTERNAL_3_0);

        // Set the resolution to 12-bit (0..4095)
        analogReadResolution(12); // Can be 8, 10, 12 or 14

        // Let the ADC settle
        delay(1);

        // Get the raw 12-bit, 0..3000mV ADC value
        raw = analogRead(_vBatPin);

        // Set the ADC back to the default settings
        analogReference(AR_DEFAULT);
        analogReadResolution(10);

        return raw * REAL_VBAT_MV_PER_LSB;
    }
    uint8_t mvToPercent(float mvolts)
    {
        if (mvolts < 3300)
            return 0;

        if (mvolts < 3600)
        {
            mvolts -= 3300;
            return mvolts / 30;
        }

        mvolts -= 3600;
        return 10 + (mvolts * 0.15F); // thats mvolts /6.66666666
    }

    int _vBatPin;
    int _checkIntervalInMs;
    unsigned long _lastPollTime;
    unsigned long _lastMonitorTime;
    uint8_t _lastBatteryPercent = 0.0;
    battery_level_changed_t _levelChangedCallback = 0;
};

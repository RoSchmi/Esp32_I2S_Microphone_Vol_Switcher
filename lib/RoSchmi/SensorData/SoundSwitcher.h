// Copyrigth RoSchmi, 2021. License Apache 2.0
// SoundSwitcher for Esp32 on PlatformIO Arduino platform
// How it works:
// SoundSwitcher is a class to implement a two state sensor reacting on sound levels
// The sound volume is measured by an I2S micophone (SPH0645LM4 or INMP441)
// The state toggles when a changeable soundvolume level is exceeded
// A changeable hysteresis is accomplished
//
// Constructor: has two parameter, a configuaration struct and the used microphone type
// Initialization: .begin method with 4 parameters (has to be called in setup())
//     * switchThreshold
//     * hysteresis
//     * updateIntervalMs
//     * delayTimeMs

// The first two need not be explained.
// updateIntervalMs : a time interval in ms. Only when the time interval has expired
//                    a new sound level average is calculated
// delayTimeMs:       this delay time is introduced for reading the analog soundlevel
//                    after the threshold is exceeded. This parameter determins after
//                    which timeinterval the analog value is sampled (to read not just
//                    at the very beginning of the new state)
//
// soundSwitcher.SetActive() : Has to be called right after the soundSwitcher.begin function
// soundSwitcher.SetCalibrationParams() : Is optional, determins an offset and a factor
//
// soundSwitcher.feed() : This function is called frequently from the loop() of the main program
//                        It returns a struct Feedresponse.
//                        After the feed command, Feedresponse.isValid has to be checked
//                        If .isValid is true, Feedresponse.hasToggled is checked
//                        If .hasToggled is true, your reaction on the change of the
//                        state has to be performed


#include <Arduino.h>
#include <driver/i2s.h>
#include "soc/i2s_reg.h"

#ifndef _SOUND_SWITCHER_H_
#define _SOUND_SWITCHER_H_

typedef struct
    {
        bool isValid = false;
        float value = 0.0; 
    }
  AverageValue;

  typedef struct 
    {
        bool isValid = false;
        bool hasToggled = false;
        bool analogToSend = false;    
        bool state = false;
        float avValue = 0.0;
        float lowAvValue = 100000;
        float highAvValue = -100000;
    }
  FeedResponse;

typedef  enum 
{
    Percent_0 = 0,
    Percent_2 = 2,
    Percent_5 = 5,
    Percent_10 = 10,
    Percent_20 = 20
}
Hysteresis;

typedef  enum 
{
    INMP441 = 0,
    SPH0645LM4H = 1
}
MicType;

class SoundSwitcher
{
    
public:
    SoundSwitcher(i2s_pin_config_t config, MicType pMicType);
    
    void begin(uint16_t switchThreshold, Hysteresis hysteresis, uint32_t updateIntervalMs, uint32_t delayTimeMs);
    FeedResponse feed();
    AverageValue getAverage();
    void SetInactive();
    void SetActive();
    void SetCalibrationParams(float pCalibOffset = 0.0, float pCalibFactor = 1.0);  
    bool hasToggled();
    bool GetState();

private:

     i2s_pin_config_t pin_config = {
    .bck_io_num = 2,                   // BCKL
    .ws_io_num = 2,                    // LRCL
    .data_out_num = I2S_PIN_NO_CHANGE,  // not used (only for speakers)
    .data_in_num = 2                  // DOUT
};
    
    uint32_t lastFeedTimeMillis = millis();
    uint32_t lastSwitchTimeMillis = millis();
    uint32_t readDelayTimeMs = 0;
    uint32_t lastBorderNarrowTimeMillis = millis();
    size_t feedIntervalMs = 100;
    bool hasSwitched = false;
    bool analogSendIsPending = false;
    float getSoundFromMicro();
    float soundVolume;
    float threshold;
    int hysteresis;
    bool state = false;
    bool isActive = false;
    bool bufferIsFilled = false;
    uint32_t bufIdx = 0;
    const static int buflen = 10;
    float volBuffer[buflen] {0.0};
    float average = 0;
    float lowAvBorder = 10000;
    float highAvBorder = 0;
    float calibOffset = 0.0;
    float calibFactor = 1.0;

    #define BUFLEN 256

// Examples for possible configurations
// For Adafruit Huzzah Esp32
/*
static const i2s_pin_config_t pin_config_Adafruit_Huzzah_Esp32 = {
    .bck_io_num = 14,                   // BCKL
    .ws_io_num = 15,                    // LRCL
    .data_out_num = I2S_PIN_NO_CHANGE,  // not used (only for speakers)
    .data_in_num = 32                   // DOUT
};
// For some other Esp32 board
static const i2s_pin_config_t pin_config_Esp32_dev = {
    .bck_io_num = 26,                   // BCKL
    .ws_io_num = 25,                    // LRCL
    .data_out_num = I2S_PIN_NO_CHANGE,  // not used (only for speakers)
    .data_in_num = 22                   // DOUT
};
*/

};

#endif  // _SOUND_SWITCHER_H_
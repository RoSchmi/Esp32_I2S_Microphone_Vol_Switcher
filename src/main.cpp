#include <Arduino.h>
#include "SoundSwitcher.h"

// Default Esp32 stack size of 8192 byte is not enough for some applications.
// --> configure stack size dynamically from code to 16384
// https://community.platformio.org/t/esp32-stack-configuration-reloaded/20994/4
// Patch: Replace C:\Users\thisUser\.platformio\packages\framework-arduinoespressif32\cores\esp32\main.cpp
// with the file 'main.cpp' from folder 'patches' of this repository, then use the following code to configure stack size
#if !(USING_DEFAULT_ARDUINO_LOOP_STACK_SIZE)
  uint16_t USER_CONFIG_ARDUINO_LOOP_STACK_SIZE = 16384;
#endif

uint64_t loopCounter = 0;

// Possible configuration for Adafruit Huzzah Esp32
static const i2s_pin_config_t pin_config_Adafruit_Huzzah_Esp32 = {
    .bck_io_num = 14,                   // BCKL
    .ws_io_num = 15,                    // LRCL
    .data_out_num = I2S_PIN_NO_CHANGE,  // not used (only for speakers)
    .data_in_num = 32                   // DOUT
};
// Possible configuration for some Esp32 DevKitC V4
static const i2s_pin_config_t pin_config_Esp32_dev = {
    .bck_io_num = 26,                   // BCKL
    .ws_io_num = 25,                    // LRCL
    .data_out_num = I2S_PIN_NO_CHANGE,  // not used (only for speakers)
    .data_in_num = 22                   // DOUT
};

MicType usedMicType = MicType::SPH0645LM4H;
//MicType usedMicType = MicType::INMP441;

SoundSwitcher soundSwitcher(pin_config_Esp32_dev, usedMicType);
//SoundSwitcher soundSwitcher(pin_config_Adafruit_Huzzah_Esp32, usedMicType);

#define SOUNDSWITCHER_UPDATEINTERVAL 400    // in ms, update averaging buffer ever x ms
#define SOUNDSWITCHER_READ_DELAYTIME 4000   // read soundlevel x ms after toggling to display
                                            // as analog value 
#define SOUNDSWITCHER_THRESHOLD "220"       // arbitrary sound switching level

int soundSwitcherUpdateInterval = SOUNDSWITCHER_UPDATEINTERVAL;
uint32_t soundSwitcherReadDelayTime = SOUNDSWITCHER_READ_DELAYTIME;
char sSwiThresholdStr[6] = SOUNDSWITCHER_THRESHOLD;

FeedResponse feedResult;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //while (!Serial);

  delay(4000);
  Serial.println("Starting");

  soundSwitcher.begin(atoi((char *)sSwiThresholdStr), Hysteresis::Percent_10, soundSwitcherUpdateInterval, soundSwitcherReadDelayTime);
  // optional
  soundSwitcher.SetCalibrationParams(-5.0);
  soundSwitcher.SetActive();
}

void loop() {
  // put your main code here, to run repeatedly:

  if (++loopCounter % 100000 == 0)   // Make decisions to send data every 100000 th round and toggle Led to signal that App is running
  {
    delay(200);
    
    feedResult = soundSwitcher.feed();
    if (feedResult.isValid)
    {
      if(feedResult.hasToggled)
      {
         Serial.print("Has toggled: ");
         Serial.println(feedResult.avValue);
      }
      Serial.print("Volume: ");
       Serial.println(feedResult.avValue);                                        
    }                 
  }        

}
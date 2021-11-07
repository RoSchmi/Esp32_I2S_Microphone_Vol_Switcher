# Esp32_I2S_Microphone_Vol_Switcher

SoundSwitcher is a class to implement a two state sensor reacting on sound levels.
The sound volume is measured by an I2S micophone (SPH0645LM4 or INMP441)
The state toggles when a changeable arbitrary soundvolume level is exceeded
A changeable hysteresis is accomplished

Constructor: has two parameter, a configuaration struct and the used microphone type
Initialization: .begin method with 4 parameters (has to be called in setup())
     * switchThreshold
    * hysteresis
     * updateIntervalMs
    * delayTimeMs

The first two parameters need not be explained.
updateIntervalMs : a time interval in ms. Only when the time interval has expired
                   a new sound level average is calculated
delayTimeMs:       this delay time is introduced for reading the analog soundlevel
                   after the threshold is exceeded. This parameter determins after
                   which timeinterval the analog value is sampled (to read not just
                   at the very beginning of the new state)

 soundSwitcher.SetActive() : Has to be called right after the soundSwitcher.begin function
 soundSwitcher.SetCalibrationParams() : Is optional, determins an offset and a factor

 soundSwitcher.feed() : This function is called frequently from the loop() of the main program
                        It returns a struct Feedresponse.
                        After the feed command, Feedresponse.isValid has to be checked
                        If .isValid is true, Feedresponse.hasToggled is checked
                        If .hasToggled is true, your reaction on the change of the
                        state has to be performed

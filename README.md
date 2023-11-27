# Sparkfun Qwiic Alphanumeric Display Driver for RP2040

This library works with the [Sparkfun Qwiic Alphanumeric Display](https://learn.sparkfun.com/tutorials/sparkfun-qwiic-alphanumeric-display-hookup-guide). 
It is written to work with the Raspberry Pi Pico SDK.  It does not require Arduino.

# Installation

It is best to add this library to your project as a submodule. To do that use a command like the following inside of your project's repo:

    git submodule add git@github.com:james-bryant/segment-display lib/segment-display

Then add the following line to your project's `CMakeLists.txt` file:

    add_subdirectory(lib/segment-display)


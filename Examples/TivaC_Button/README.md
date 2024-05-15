# TivaC button
Button interface with support to single or multiple short and long clicks for Texas Instruments TivaC devices

**Version:** 1.0  
**Author:** Renan Duarte  
**E-mail:** duarte.renan@hotmail.com  
**Date:** 13/05/2024  

**License:**
Anyone is free to use and modify this code, but please provide credit to the author, Renan Duarte.

**Usage:**  
To use this interface, include the `Button_TivaC.hpp` header file in your project. The interface provides the following functions:

1. `Init`: Initializes the TivaC device peripherals required for button functionality.
2. `ScanEvent`: Scans the button state and detects events such as short clicks, long clicks, and long click ticks.

**Example:**
```cpp
#include "Button_TivaC.hpp"

Button button1;

int main() {
    button_config_t config;
    // Set up button configuration...

    button1.Init(&config);

    // Main loop
    while (true) {
        button_event_data_t eventData;
        if (button1.ScanEvent(&eventData)) {
            // Handle button event
        }
    }
}

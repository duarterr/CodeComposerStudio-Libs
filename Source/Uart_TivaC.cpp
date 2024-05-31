// ------------------------------------------------------------------------------------------------------- //

// UART library for TivaC devices
// Version: 1.0
// Author: Renan Duarte
// E-mail: duarte.renan@hotmail.com
// Date:   13/05/2024

// ------------------------------------------------------------------------------------------------------- //

// ------------------------------------------------------------------------------------------------------- //
// Includes
// ------------------------------------------------------------------------------------------------------- //

// Button defines and macros
#include "Uart_TivaC.hpp"

// Standard libraries
#include <stdint.h>

// TivaC device defines and macros
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

// ------------------------------------------------------------------------------------------------------- //
// Initialize the static array and counter
// ------------------------------------------------------------------------------------------------------- //

Uart* Uart::_Instance[MAX_UARTS] = {nullptr};
uint8_t Uart::_InstanceCounter = 0;

// ------------------------------------------------------------------------------------------------------- //
// Functions definitions
// ------------------------------------------------------------------------------------------------------- //

// Name:        _InitHardware
// Description: Starts device peripherals
// Arguments:   None
// Returns:     None

void Uart::_InitHardware()
{
    // Enable peripheral clocks
    SysCtlPeripheralEnable(_Config.Hardware.PeriphUART);
    SysCtlPeripheralEnable(_Config.Hardware.PeriphGPIO);

    // Wait until last peripheral is ready
    while(!SysCtlPeripheralReady (_Config.Hardware.PeriphGPIO));

    // Unlock used pins (has no effect if pin is not protected by the GPIOCR register
    GPIOUnlockPin(_Config.Hardware.BaseGPIO, _Config.Hardware.PinRX | _Config.Hardware.PinTX);

    // Configure GPIO Pins for UART mode
    GPIOPinConfigure(_Config.Hardware.PinMuxRX);
    GPIOPinConfigure(_Config.Hardware.PinMuxTX);
    GPIOPinTypeUART(_Config.Hardware.BaseGPIO, _Config.Hardware.PinRX | _Config.Hardware.PinTX);

    // Configure UART
    UARTConfigSetExpClk(_Config.Hardware.BaseUART, SysCtlClockGet(), _Config.Params.BaudRate, _Config.Params.Mode);

    // Register interrupt handler
    UARTIntRegister(_Config.Hardware.BaseUART, _IsrRxStaticCallback);

    // Enable interrupt on RX and RX timeout
    UARTIntEnable(_Config.Hardware.BaseUART, UART_INT_RX | UART_INT_RT);

    // Enable the UART module
    UARTEnable(_Config.Hardware.BaseUART);
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        _BufferPutByte
// Description: Puts a byte in the UART buffer
// Arguments:   Byte - Byte to be put
// Returns:     None

void Uart::_BufferPutByte (uint8_t Byte)
{
    // Put char in FIFO buffer - Wait until there is space
    UARTCharPut(_Config.Hardware.BaseUART, Byte);
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        _IsrRxStaticCallback
// Description: Static callback function for handling interrupts
// Arguments:   None
// Returns:     None

void Uart::_IsrRxStaticCallback()
{
    // Iterate over all instances to find the one matching the interrupt
    for (uint8_t Index = 0; Index < _InstanceCounter; Index++)
    {
        // Check if this instance triggered the interrupt and call the instance-specific handler
        if ((_Instance[Index] != nullptr) && (UARTIntStatus(_Instance[Index]->_Config.Hardware.BaseUART, true)))
            _Instance[Index]->_IsrRxHandler();
    }
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        _IsrRxHandler
// Description: UART RX interrupt service routine
// Arguments:   None
// Returns:     None

void Uart::_IsrRxHandler ()
{
    // Loop while there are characters in the receive FIFO
    while(UARTCharsAvail(_Config.Hardware.BaseUART))
    {
        // Read the next character from the UART
        char Rx = UARTCharGetNonBlocking(_Config.Hardware.BaseUART);
    }

    // Clear the asserted interrupts
    UARTIntClear(_Config.Hardware.BaseUART, UARTIntStatus(_Config.Hardware.BaseUART, true));
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        Uart
// Description: Constructor of the class with no arguments
// Arguments:   None
// Returns:     None

Uart::Uart()
{
    // Register the instance in the array
    if (_InstanceCounter < MAX_UARTS)
        _Instance[_InstanceCounter++] = this;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        Uart
// Description: Constructor of the class with uart_config_t struct as argument
// Arguments:   Config - uart_config_t struct
// Returns:     None

Uart::Uart(const uart_config_t *Config) : Uart()
{
    Init(Config);
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        Init
// Description: Starts device peripherals and variables
// Arguments:   Config - uart_config_t struct
// Returns:     None

void Uart::Init(const uart_config_t *Config)
{
    // Copy config to a private variable
    _Config = *Config;

    //  Initialize hardware
    _InitHardware();
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        SendString
// Description: Sends a string to the UART
// Arguments:   String - Pointer to the string to be sent
// Returns:     None

void Uart::SendString (const char *String)
{
    // Send all chars of the string
    while (*String)
        _BufferPutByte(*String++);
}

// ------------------------------------------------------------------------------------------------------- //

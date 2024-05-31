// ------------------------------------------------------------------------------------------------------- //

// UART library for TivaC devices
// Version: 1.0
// Author: Renan Duarte
// E-mail: duarte.renan@hotmail.com
// Date:   13/05/2024

// ------------------------------------------------------------------------------------------------------- //

#ifndef UART_TIVAC_H_
#define UART_TIVAC_H_

#ifdef __cplusplus
extern "C"
{
#endif

// ------------------------------------------------------------------------------------------------------- //
// Includes
// ------------------------------------------------------------------------------------------------------- //

// Standard libraries
#include <stdint.h>

// ------------------------------------------------------------------------------------------------------- //
// Definitions
// ------------------------------------------------------------------------------------------------------- //

#define MAX_UARTS 1                 // Maximum number of UART instances

// ------------------------------------------------------------------------------------------------------- //
// Structs
// ------------------------------------------------------------------------------------------------------- //

// Hardware configuration structure
typedef struct
{
    uint32_t PeriphUART;            // UART peripheral
    uint32_t PeriphGPIO;            // GPIO peripheral
    uint32_t BaseUART;              // SSI base
    uint32_t BaseGPIO;              // GPIO base
    uint32_t PinMuxRX;              // GPIO pin config RX
    uint32_t PinMuxTX;              // GPIO pin config TX
    uint32_t PinRX;                 // GPIO pin RX
    uint32_t PinTX;                 // GPIO pin TX
} uart_hardware_t;

// UART parameters structure
typedef struct
{
    uint32_t BaudRate;              // Baud rate (bps)
    uint32_t Mode;                  // UART config
} uart_params_t;

// Button configuration structure
typedef struct
{
    uart_hardware_t Hardware;       // Hardware struct
    uart_params_t Params;           // Parameters struct
} uart_config_t;

// ------------------------------------------------------------------------------------------------------- //
// Class prototype
// ------------------------------------------------------------------------------------------------------- //

class Uart
{
    // --------------------------------------------------------------------------------------------------- //
    // Private members
    // --------------------------------------------------------------------------------------------------- //

    private:

        // Array to store pointers to instances
        static Uart* _Instance[MAX_UARTS];

        // Counter to keep track of the number of instances
        static uint8_t _InstanceCounter;

        // UART configuration object
        uart_config_t _Config;

        // Name:        _InitHardware
        // Description: Starts device peripherals
        // Arguments:   None
        // Returns:     None
        void _InitHardware();

        // Name:        _BufferPutByte
        // Description: Puts a byte in the UART buffer
        // Arguments:   Byte - Byte to be put
        // Returns:     None
        void _BufferPutByte (uint8_t Byte);

        // Name:        _IsrRxStaticCallback
        // Description: Static callback function for handling interrupts
        // Arguments:   None
        // Returns:     None
        static void _IsrRxStaticCallback();

        // Name:        _IsrRxHandler
        // Description: UART RX interrupt service routine
        // Arguments:   None
        // Returns:     None
        void _IsrRxHandler ();

    // --------------------------------------------------------------------------------------------------- //
    // Public members
    // --------------------------------------------------------------------------------------------------- //

    public:

        // Name:        Uart
        // Description: Constructor of the class with no arguments
        // Arguments:   None
        // Returns:     None
        Uart();

        // Name:        Uart
        // Description: Constructor of the class with uart_config_t struct as argument
        // Arguments:   Config - uart_config_t struct
        // Returns:     None
        Uart(const uart_config_t *Config);

        // Name:        Init
        // Description: Starts device peripherals
        // Arguments:   Config - uart_config_t struct
        // Returns:     None
        void Init(const uart_config_t *Config);

        // Name:        SendString
        // Description: Sends a string to the UART
        // Arguments:   String - Pointer to the string to be sent
        // Returns:     None
        void SendString (const char *String);

        // Name:        ReceiveIsr
        // Description: UART RX interrupt service routine
        // Arguments:   None
        // Returns:     None
        void ReceiveIsr ();
};

// ------------------------------------------------------------------------------------------------------- //

#ifdef __cplusplus
}
#endif

#endif

// ------------------------------------------------------------------------------------------------------- //

// ------------------------------------------------------------------------------------------------------- //

// Encoder library for TivaC devices
// Version: 1.0
// Author: Renan Duarte
// E-mail: duarte.renan@hotmail.com
// Date:   14/05/2024

// ------------------------------------------------------------------------------------------------------- //

#ifndef ENCODER_TIVAC_H_
#define ENCODER_TIVAC_H_

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

#define MAX_ENCODERS 2              // Maximum number of encoder instances

// ------------------------------------------------------------------------------------------------------- //
// Structs
// ------------------------------------------------------------------------------------------------------- //

// Hardware configuration structure
typedef struct
{
    uint32_t PeriphQEI;             // QEI peripheral
    uint32_t PeriphGPIO;            // GPIO peripheral
    uint32_t BaseQEI;               // QEI base
    uint32_t BaseGPIO;              // GPIO base
    uint32_t PinMuxA;               // GPIO configuration A
    uint32_t PinMuxB;               // GPIO configuration B
    uint32_t PinA;                  // GPIO pin A
    uint32_t PinB;                  // GPIO pin B
    uint32_t Config;                // QEI module configuration
} encoder_hardware_t;

// Encoder parameters structure
typedef struct
{
    uint32_t PPR;                   // Pulses per revolution
    uint32_t ScanFreq;              // Frequency of scan
} encoder_params_t;

// Encoder configuration structure
typedef struct
{
    encoder_hardware_t Hardware;    // Hardware struct
    encoder_params_t Params;        // Parameters struct
} encoder_config_t;

// Encoder variables
typedef struct
{
   uint32_t Pos;                    // Position (pulses)
   uint32_t Vel;                    // Velocity (pulses per ScanFreq period)
   int32_t Dir;                     // Direction (1 = forward, -1 = backward)
} encoder_data_t;

// Encoder variables - Default values
#define encoder_data_t_default { \
    .Pos = 0, \
    .Vel = 0, \
    .Dir = 0, \
}

// ------------------------------------------------------------------------------------------------------- //
// Class prototype
// ------------------------------------------------------------------------------------------------------- //

class Encoder
{
    // --------------------------------------------------------------------------------------------------- //
    // Private members
    // --------------------------------------------------------------------------------------------------- //

    private:

        // Array to store pointers to instances
        static Encoder* _Instance[MAX_ENCODERS];

        // Counter to keep track of the number of instances
        static uint8_t _InstanceCounter;

        // Encoder configuration object
        encoder_config_t _Config;

        // Encoder variables
        encoder_data_t _Data = encoder_data_t_default;

        // Name:        _InitHardware
        // Description: Starts device peripherals
        // Arguments:   None
        // Returns:     None
        void _InitHardware();

        // Name:        _IsrVelStaticCallback
        // Description: Static callback function for handling interrupts
        // Arguments:   None
        // Returns:     None
        static void _IsrVelStaticCallback();

        // Name:        _IsrTimerVelHandler
        // Description: Velocity timer interrupt service routine
        // Arguments:   None
        // Returns:     None
        void _IsrTimerVelHandler ();

    // --------------------------------------------------------------------------------------------------- //
    // Public members
    // --------------------------------------------------------------------------------------------------- //

    public:

        // Name:        Encoder
        // Description: Constructor of the class with no arguments
        // Arguments:   None
        // Returns:     None
        Encoder();

        // Name:        Encoder
        // Description: Constructor of the class with encoder_config_t struct as argument
        // Arguments:   Config - encoder_config_t struct
        // Returns:     None
        Encoder(encoder_config_t *Config);

        // Name:        Init
        // Description: Starts device peripherals
        // Arguments:   Config - encoder_config_t struct
        // Returns:     None
        void Init(encoder_config_t *Config);

        // Name:        GetData
        // Description: Gets all encoder data
        // Arguments:   Buffer - encoder_data_t struct to receive data
        // Returns:     None
        void GetData (encoder_data_t *Buffer);

        // Name:        GetPos
        // Description: Gets encoder position
        // Arguments:   None
        // Returns:     Encoder position read in last scan
        uint32_t GetPos ();

        // Name:        SetPos
        // Description: Sets encoder position
        // Arguments:   New encoder position
        // Returns:     None
        void SetPos (uint32_t Pos);

        // Name:        GetVel
        // Description: Gets encoder velocity
        // Arguments:   None
        // Returns:     Encoder velocity read in last scan
        uint32_t GetVel ();

        // Name:        GetDir
        // Description: Gets encoder direction
        // Arguments:   None
        // Returns:     Encoder direction read in last scan
        int32_t GetDir ();
};

// ------------------------------------------------------------------------------------------------------- //

#ifdef __cplusplus
}
#endif

#endif

// ------------------------------------------------------------------------------------------------------- //

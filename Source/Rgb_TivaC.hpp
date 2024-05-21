// ------------------------------------------------------------------------------------------------------- //

// RGB LED library
// Version: 1.0
// Author: Renan Duarte
// E-mail: duarte.renan@hotmail.com
// Date:   20/05/2024

// ------------------------------------------------------------------------------------------------------- //

#ifndef RGB_TIVAC_H_
#define RGB_TIVAC_H_

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
// Structs
// ------------------------------------------------------------------------------------------------------- //

// Peripherals
typedef struct
{
    uint32_t PwmR;                  // PWM peripheral R
    uint32_t PwmG;                  // PWM peripheral G
    uint32_t PwmB;                  // PWM peripheral B
    uint32_t GpioR;                 // GPIO peripheral R
    uint32_t GpioG;                 // GPIO peripheral G
    uint32_t GpioB;                 // GPIO peripheral B
} rgb_periph_t;

// Bases
typedef struct
{
    uint32_t PwmR;                  // PWM base R
    uint32_t PwmG;                  // PWM base G
    uint32_t PwmB;                  // PWM base B
    uint32_t GpioR;                 // GPIO base R
    uint32_t GpioG;                 // GPIO base G
    uint32_t GpioB;                 // GPIO base B
} rgb_base_t;

// PWM generators
typedef struct
{
    uint32_t R;
    uint32_t G;
    uint32_t B;
} rgb_pwm_gen_t;

// PWM output struct
typedef struct
{
    uint32_t R;
    uint32_t G;
    uint32_t B;
} rgb_pwm_out_t;

// PWM output bit struct
typedef struct
{
    uint32_t R;
    uint32_t G;
    uint32_t B;
} rgb_pwm_out_bit_t;

// Pin mux configs
typedef struct
{
    uint32_t R;
    uint32_t G;
    uint32_t B;
} rgb_pin_mux_t;

// LED pins
typedef struct
{
    uint32_t R;
    uint32_t G;
    uint32_t B;
} rgb_pin_t;

// RGB parameters structure
typedef struct
{
    uint32_t PwmMode;               // PWM mode configuration
    uint16_t PwmFrequency;          // PWM frequency (Hz)
} rgb_params_t;

// RGB configuration structure
typedef struct
{
    rgb_periph_t Periph;            // Peripheral struct
    rgb_base_t Base;                // Base struct
    rgb_pwm_gen_t Gen;              // Generator struct
    rgb_pwm_out_t Out;              // PWM output struct
    rgb_pwm_out_bit_t OutBit;       // PWM output bit struct
    rgb_pin_mux_t PinMux;           // PinMux struct
    rgb_pin_t Pin;                  // Pin struct
    rgb_params_t Params;            // Parameters struct
} rgb_config_t;

// RGB LED color structure
typedef struct
{
    uint8_t R;  // Red value (0 to 255)
    uint8_t G;  // Green value (0 to 255)
    uint8_t B;  // Blue value (0 to 255)
} rgb_color_t;

// ------------------------------------------------------------------------------------------------------- //
// Class prototype
// ------------------------------------------------------------------------------------------------------- //

class Rgb
{
    // --------------------------------------------------------------------------------------------------- //
    // Private members
    // --------------------------------------------------------------------------------------------------- //

    private:

        // RGB configuration object
        rgb_config_t _Config;

        // RGB color
        rgb_color_t _Color = {.R = 0, .G = 0, .B = 0};

        // PWM period
        uint16_t _PwmPeriod = 0;

        // Name:        _InitHardware
        // Description: Starts device peripherals
        // Arguments:   None
        // Returns:     None
        void _InitHardware();

        // Name:        _GetPwmClock
        // Description: Gets PWM module clock
        // Arguments:   None
        // Returns:     Clock in Hz
        uint32_t _GetPwmClock();

    // --------------------------------------------------------------------------------------------------- //
    // Public members
    // --------------------------------------------------------------------------------------------------- //

    public:

        // Name:        Rgb
        // Description: Constructor of the class with no arguments
        // Arguments:   None
        // Returns:     None
        Rgb();

        // Name:        Rgb
        // Description: Constructor of the class with rgb_config_t struct as argument
        // Arguments:   Config - rgb_config_t struct
        // Returns:     None
        Rgb(rgb_config_t *Config);

        // Name:        Init
        // Description: Starts device peripherals and application logic
        // Arguments:   Config - rgb_config_t struct
        // Returns:     None
        void Init(rgb_config_t *Config);

        // Name:        SetColor
        // Description: Sets the RGB LED color
        // Arguments:   Color - New color - rgb_color_t structure (0x00 to 0xFF values)
        // Returns:     None
        void SetColor(rgb_color_t Color);

        // Name:        GetColor
        // Description: Gets the current LED color
        // Arguments:   Buffer - Pointer rgb_color_t structure where values will be saved (0x00 to 0xFF values)
        // Returns:     None
        void GetColor(rgb_color_t *Buffer);
};

// ------------------------------------------------------------------------------------------------------- //

#ifdef __cplusplus
}
#endif

#endif

// ------------------------------------------------------------------------------------------------------- //

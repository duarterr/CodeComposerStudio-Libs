// ------------------------------------------------------------------------------------------------------- //

// RGB LED library
// Version: 1.0
// Author: Renan Duarte
// E-mail: duarte.renan@hotmail.com
// Date:   20/05/2024


// Overview:

//      This library provides functionality for controlling RGB LEDs on a Tiva C microcontroller. It
//      allows the user to set the color of the RGB LED with or without fade transitions.
//      The library supports a maximum of MAX_RGB_LEDS instances.
//      The fade service runs at the same frequency as the LED PWM, so maximum fade time is 1/PwmFrequency

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
// Definitions
// ------------------------------------------------------------------------------------------------------- //

#define MAX_RGB_LEDS 1              // Maximum number of RGB LED instances

// ------------------------------------------------------------------------------------------------------- //
// Structs
// ------------------------------------------------------------------------------------------------------- //

// Peripherals
typedef struct
{
    uint32_t Pwm;                   // PWM peripheral
    uint32_t Gpio;                  // GPIO peripheral
    uint32_t Timer;                 // Timer peripheral
} rgb_periph_t;

// Bases
typedef struct
{
    uint32_t Pwm;                   // PWM base
    uint32_t Gpio;                  // GPIO base
    uint32_t Timer;                 // Timer base
} rgb_base_t;

// PWM generators
typedef struct
{
    uint32_t R;                     // PWM generator for Red
    uint32_t G;                     // PWM generator for Green
    uint32_t B;                     // PWM generator for Blue
} rgb_pwm_gen_t;

// PWM output struct
typedef struct
{
    uint32_t R;                     // PWM output for Red
    uint32_t G;                     // PWM output for Green
    uint32_t B;                     // PWM output for Blue
} rgb_pwm_out_t;

// PWM output bit struct
typedef struct
{
    uint32_t R;                     // PWM output bit for Red
    uint32_t G;                     // PWM output bit for Green
    uint32_t B;                     // PWM output bit for Blue
} rgb_pwm_out_bit_t;

// PWM interrupt struct
typedef struct
{
    uint32_t Interrupt;             // PWM interrupt
    uint32_t Gen;                   // PWM interrupt generator
} pwm_interrupt_t;

// Pin mux configs
typedef struct
{
    uint32_t R;                     // Pin mux configuration for Red
    uint32_t G;                     // Pin mux configuration for Green
    uint32_t B;                     // Pin mux configuration for Blue
} rgb_pin_mux_t;

// LED pins
typedef struct
{
    uint32_t R;                     // Red LED pin
    uint32_t G;                     // Green LED pin
    uint32_t B;                     // Blue LED pin
} rgb_pin_t;

// RGB parameters structure
typedef struct
{
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
    pwm_interrupt_t Int;            // PWM interrupt struct
    rgb_pin_mux_t PinMux;           // PinMux struct
    rgb_pin_t Pin;                  // Pin struct
    rgb_params_t Params;            // Parameters struct
} rgb_config_t;

// RGB LED color structure
typedef struct
{
    uint8_t R;                      // Red value (0 to 255)
    uint8_t G;                      // Green value (0 to 255)
    uint8_t B;                      // Blue value (0 to 255)
} rgb_color_t;

/* ------------------------------------------------------------------------------------------------------- */
// Custom RGB colors
/* ------------------------------------------------------------------------------------------------------- */

const rgb_color_t RGB_OFF = (rgb_color_t){0x00, 0x00, 0x00};
const rgb_color_t RGB_RED = (rgb_color_t){0xFF, 0x00, 0x00};
const rgb_color_t RGB_GREEN = (rgb_color_t){0x00, 0xFF, 0x00};
const rgb_color_t RGB_BLUE = (rgb_color_t){0x00, 0x00, 0xFF};
const rgb_color_t RGB_CYAN = (rgb_color_t){0x00, 0xFF, 0xFF};
const rgb_color_t RGB_MAGENTA = (rgb_color_t){0xFF, 0x00, 0xFF};
const rgb_color_t RGB_YELLOW = (rgb_color_t){0xFF, 0xFF, 0x00};
const rgb_color_t RGB_WHITE = (rgb_color_t){0xFF, 0xFF, 0xFF};

// ------------------------------------------------------------------------------------------------------- //
// Class prototype
// ------------------------------------------------------------------------------------------------------- //

class Rgb
{
    // --------------------------------------------------------------------------------------------------- //
    // Private members
    // --------------------------------------------------------------------------------------------------- //

    private:

        // Array to store pointers to instances
        static Rgb* _Instance[MAX_RGB_LEDS];

        // Counter to keep track of the number of instances
        static uint8_t _InstanceCounter;

        // RGB configuration object
        rgb_config_t _Config;

        // RGB fade variables
        rgb_color_t _CurrentColor = RGB_OFF;
        rgb_color_t _NewColor = RGB_OFF;
        uint32_t _FadeSteps = 0;
        uint32_t _StepCounter = 0;

        // PWM period
        uint16_t _PwmPeriod = 0;

        // Name:        _InitHardware
        // Description: Starts device peripherals
        // Arguments:   None
        // Returns:     None
        void _InitHardware();

        // Name:        _IsrTimerStaticCallback
        // Description: Static callback function for handling timer interrupts
        // Arguments:   None
        // Returns:     None
        static void _IsrTimerStaticCallback();

        // Name:        _IsrTimerHandler
        // Description: Timer interrupt service routine
        // Arguments:   None
        // Returns:     None
        void _IsrTimerHandler ();

        // Name:        _FadeService
        // Description: Fade service to ensure proper color transitions
        // Arguments:   None
        // Returns:     None
        void _FadeService();

        // Name:        _SetPwmFrequency
        // Description: Changes PWM modules frequency
        // Arguments:   Frequency
        // Returns:     None
        void _SetPwmFrequency (uint32_t Frequency);

        // Name:        _SetPwmDuty
        // Description: Changes PWM modules duty cycle
        // Arguments:   DutyR, DutyG, DutyB - Duty cycles - 0 to _PwmPeriod
        // Returns:     None
        void _SetPwmDuty (uint16_t DutyR, uint16_t DutyG, uint16_t DutyB);

        // Name:        _GetPwmClock
        // Description: Gets PWM module clock
        // Arguments:   None
        // Returns:     Clock in Hz
        uint32_t _GetPwmClock();

        // Name:        _ColorsAreEqual
        // Description: Check if two colors are equal
        // Arguments:   Color1 - First color - rgb_color_t value
        //              Color2 - Second color - rgb_color_t value
        // Returns:     True if equal. False otherwise
        bool _ColorsAreEqual(rgb_color_t Color1, rgb_color_t Color2);

        // Name:        _CalculateStepSkip
        // Description: Determine the number of steps to skip during the fade
        // Arguments:   NewValue - First value
        //              CurrentValue - Second value
        //              FadeSteps - Number of fade steps
        // Returns:     True if equal. False otherwise
        uint16_t _CalculateStepSkip(uint16_t NewValue, uint16_t CurrentValue, uint16_t FadeSteps);

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
        //              FadeTime - Fade time to new color (ms)
        // Returns:     None
        void SetColor(rgb_color_t Color, uint16_t FadeTime);

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

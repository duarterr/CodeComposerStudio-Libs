// ------------------------------------------------------------------------------------------------------- //

// RGB LED library
// Version: 1.0
// Author: Renan Duarte
// E-mail: duarte.renan@hotmail.com
// Date:   20/05/2024


// Overview:

//      This library provides functionality for controlling RGB LEDs on a Tiva C microcontroller. It
//      allows the user to set the color of the RGB LED with or without fade transitions.
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
#include <stdbool.h>

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

#ifndef RGB_COLORS_H
#define RGB_COLORS_H

extern const rgb_color_t RGB_OFF;
extern const rgb_color_t RGB_RED;
extern const rgb_color_t RGB_GREEN;
extern const rgb_color_t RGB_BLUE;
extern const rgb_color_t RGB_CYAN;
extern const rgb_color_t RGB_MAGENTA;
extern const rgb_color_t RGB_YELLOW;
extern const rgb_color_t RGB_WHITE;

#endif

// ------------------------------------------------------------------------------------------------------- //
// Function prototypes
// ------------------------------------------------------------------------------------------------------- //

// Name:        Rgb_Init
// Description: Starts device peripherals and application logic
// Arguments:   Config - rgb_config_t struct
// Returns:     None
void Rgb_Init(const rgb_config_t *Config);

// Name:        Rgb_SetColor
// Description: Sets the RGB LED color
// Arguments:   Color - New color - rgb_color_t structure (0x00 to 0xFF values)
//              FadeTime - Fade time to new color (ms)
// Returns:     None
void Rgb_SetColor(rgb_color_t Color, uint16_t FadeTime);

// Name:        Rgb_GetColor
// Description: Gets the current LED color
// Arguments:   Buffer - Pointer rgb_color_t structure where values will be saved (0x00 to 0xFF values)
// Returns:     None
void Rgb_GetColor(rgb_color_t *Buffer);

// Name:        Rgb_IsChangingColor
// Description: Checks if LED is in the process of changing colors
// Arguments:   None
// Returns:     True if colors are changing. False otherwise
bool Rgb_IsChangingColor (void);

// ------------------------------------------------------------------------------------------------------- //

#ifdef __cplusplus
}
#endif

#endif

// ------------------------------------------------------------------------------------------------------- //

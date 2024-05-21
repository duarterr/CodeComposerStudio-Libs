// ------------------------------------------------------------------------------------------------------- //

// RGB LED library
// Version: 1.0
// Author: Renan Duarte
// E-mail: duarte.renan@hotmail.com
// Date:   20/05/2024

// ------------------------------------------------------------------------------------------------------- //

// ------------------------------------------------------------------------------------------------------- //
// Includes
// ------------------------------------------------------------------------------------------------------- //

// RGB defines and macros
#include "Rgb_TivaC.hpp"

// Standard libraries
#include <stdint.h>

// Auxiliary functions
#include <Aux_Functions.hpp>

// TivaC device defines and macros
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"

// ------------------------------------------------------------------------------------------------------- //
// Functions definitions
// ------------------------------------------------------------------------------------------------------- //

// Name:        _InitHardware
// Description: Starts device peripherals
// Arguments:   None
// Returns:     None

void Rgb::_InitHardware()
{
    // Enable peripheral clocks
    SysCtlPeripheralEnable(_Config.Periph.PwmR);
    SysCtlPeripheralEnable(_Config.Periph.PwmG);
    SysCtlPeripheralEnable(_Config.Periph.PwmB);
    SysCtlPeripheralEnable(_Config.Periph.GpioR);
    SysCtlPeripheralEnable(_Config.Periph.GpioG);
    SysCtlPeripheralEnable(_Config.Periph.GpioB);

    // Power up delay
    SysCtlDelay(10);

    // Unlock used pins (has no effect if pin is not protected by the GPIOCR register
    GPIOUnlockPin(_Config.Base.GpioR, _Config.Pin.R);
    GPIOUnlockPin(_Config.Base.GpioG, _Config.Pin.G);
    GPIOUnlockPin(_Config.Base.GpioB, _Config.Pin.B);

    // Configure pins as PWM outputs
    GPIOPinTypePWM (_Config.Base.GpioR, _Config.Pin.R);
    GPIOPinTypePWM (_Config.Base.GpioG, _Config.Pin.G);
    GPIOPinTypePWM (_Config.Base.GpioB, _Config.Pin.B);
    GPIOPinConfigure (_Config.PinMux.R);
    GPIOPinConfigure (_Config.PinMux.G);
    GPIOPinConfigure (_Config.PinMux.B);

    // Configure PWM options
    PWMGenConfigure (_Config.Base.PwmR, _Config.Gen.R, _Config.Params.PwmMode);
    PWMGenConfigure (_Config.Base.PwmG, _Config.Gen.G, _Config.Params.PwmMode);
    PWMGenConfigure (_Config.Base.PwmB, _Config.Gen.B, _Config.Params.PwmMode);

    // Set the period (expressed in clock ticks)
    _PwmPeriod = (SysCtlClockGet()/_Config.Params.PwmFrequency) - 1;
    PWMGenPeriodSet (_Config.Base.PwmR, _Config.Gen.R, _PwmPeriod);
    PWMGenPeriodSet (_Config.Base.PwmG, _Config.Gen.G, _PwmPeriod);
    PWMGenPeriodSet (_Config.Base.PwmB, _Config.Gen.B, _PwmPeriod);

    // Set initial color - Off
    SetColor (_Color);

    // Turn on the output pins
    PWMOutputState (_Config.Base.PwmR, _Config.OutBit.R , true);
    PWMOutputState (_Config.Base.PwmG, _Config.OutBit.G , true);
    PWMOutputState (_Config.Base.PwmB, _Config.OutBit.B , true);

    // Enable the PWM generators
    PWMGenEnable (_Config.Base.PwmR, _Config.Gen.R);
    PWMGenEnable (_Config.Base.PwmG, _Config.Gen.G);
    PWMGenEnable (_Config.Base.PwmB, _Config.Gen.B);
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        _GetPwmClock
// Description: Gets PWM module clock
// Arguments:   None
// Returns:     Clock in Hz

uint32_t Rgb::_GetPwmClock()
{
    uint8_t ClockShifts = 0;

    switch (SysCtlPWMClockGet())
    {
        case SYSCTL_PWMDIV_1:
            ClockShifts = 0;
            break;

        case SYSCTL_PWMDIV_2:
            ClockShifts = 1;
            break;

        case SYSCTL_PWMDIV_4:
            ClockShifts = 2;
            break;

        case SYSCTL_PWMDIV_8:
            ClockShifts = 3;
            break;

        case SYSCTL_PWMDIV_16:
            ClockShifts = 4;
            break;

        case SYSCTL_PWMDIV_32:
            ClockShifts = 5;
            break;

        case SYSCTL_PWMDIV_64:
            ClockShifts = 6;
            break;

        default:
            break;
    }

    return SysCtlClockGet() >> ClockShifts;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        Rgb
// Description: Constructor of the class with no arguments
// Arguments:   None
// Returns:     None

Rgb::Rgb()
{

}

// ------------------------------------------------------------------------------------------------------- //

// Name:        Rgb
// Description: Constructor of the class with rgb_config_t struct as argument
// Arguments:   Config - rgb_config_t struct
// Returns:     None

Rgb::Rgb(rgb_config_t *Config) : Rgb()
{
    Init(Config);
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        Init
// Description: Starts device peripherals and application state machine
// Arguments:   Config - rgb_config_t struct
// Returns:     None

void Rgb::Init(rgb_config_t *Config)
{
    // Get rgb_config_t object parameters and store in a "private" variable
    memcpy(&_Config, Config, sizeof(rgb_config_t));

    //  Initialize hardware
    _InitHardware();
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        SetColor
// Description: Sets the RGB LED color
// Arguments:   Color - New color - rgb_color_t structure (0x00 to 0xFF values)
// Returns:     None

void Rgb::SetColor(rgb_color_t Color)
{
    uint16_t DutyR = (uint16_t)Aux::Map(Color.R, 0, 255, 1, _PwmPeriod);
    uint16_t DutyG = (uint16_t)Aux::Map(Color.G, 0, 255, 1, _PwmPeriod);
    uint16_t DutyB = (uint16_t)Aux::Map(Color.B, 0, 255, 1, _PwmPeriod);

    PWMPulseWidthSet (_Config.Base.PwmR, _Config.Out.R , DutyR);
    PWMPulseWidthSet (_Config.Base.PwmG, _Config.Out.G , DutyG);
    PWMPulseWidthSet (_Config.Base.PwmB, _Config.Out.B , DutyB);
}

// Name:        GetColor
// Description: Gets the current LED color
// Arguments:   Buffer - Pointer rgb_color_t structure where values will be saved (0x00 to 0xFF values)
// Returns:     None

void Rgb::GetColor(rgb_color_t *Buffer)
{
    Buffer->R = _Color.R;
    Buffer->G = _Color.G;
    Buffer->B = _Color.B;
}

// ------------------------------------------------------------------------------------------------------- //

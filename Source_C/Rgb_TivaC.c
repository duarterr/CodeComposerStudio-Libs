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

// ------------------------------------------------------------------------------------------------------- //
// Includes
// ------------------------------------------------------------------------------------------------------- //

// RGB defines and macros
#include "Rgb_TivaC.h"

// Standard libraries
#include <stdint.h>
#include <stdbool.h>

// Auxiliary functions
#include <Aux_Functions.h>

// TivaC device defines and macros
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

// ------------------------------------------------------------------------------------------------------- //
// Custom RGB colors
// ------------------------------------------------------------------------------------------------------- //

const rgb_color_t RGB_OFF = {0x00, 0x00, 0x00};
const rgb_color_t RGB_RED = {0xFF, 0x00, 0x00};
const rgb_color_t RGB_GREEN = {0x00, 0xFF, 0x00};
const rgb_color_t RGB_BLUE = {0x00, 0x00, 0xFF};
const rgb_color_t RGB_CYAN = {0x00, 0xFF, 0xFF};
const rgb_color_t RGB_MAGENTA = {0xFF, 0x00, 0xFF};
const rgb_color_t RGB_YELLOW = {0xFF, 0xFF, 0x00};
const rgb_color_t RGB_WHITE = {0xFF, 0xFF, 0xFF};

// ------------------------------------------------------------------------------------------------------- //
// "Private" variables
// ------------------------------------------------------------------------------------------------------- //

// RGB configuration object
static rgb_config_t _Config;

// RGB fade variables
static rgb_color_t _CurrentColor = {0};
static rgb_color_t _NewColor = {0};
static uint32_t _FadeSteps = 0;
static uint32_t _StepCounter = 0;

// PWM period
static uint16_t _PwmPeriod = 0;

// ------------------------------------------------------------------------------------------------------- //
// Functions prototypes - "Private" functions
// ------------------------------------------------------------------------------------------------------- //

// Name:        _InitHardware
// Description: Starts device peripherals
// Arguments:   None
// Returns:     None
static void _InitHardware(void);

// Name:        _IsrTimerHandler
// Description: Timer interrupt service routine
// Arguments:   None
// Returns:     None
static void _IsrTimerHandler (void);

// Name:        _FadeService
// Description: Fade service to ensure proper color transitions
// Arguments:   None
// Returns:     None
static void _FadeService(void);

// Name:        _SetPwmFrequency
// Description: Changes PWM modules frequency
// Arguments:   Frequency
// Returns:     None
static void _SetPwmFrequency (uint32_t Frequency);

// Name:        _SetPwmDuty
// Description: Changes PWM modules duty cycle
// Arguments:   DutyR, DutyG, DutyB - Duty cycles - 0 to _PwmPeriod
// Returns:     None
static void _SetPwmDuty (uint16_t DutyR, uint16_t DutyG, uint16_t DutyB);

// Name:        _GetPwmClock
// Description: Gets PWM module clock
// Arguments:   None
// Returns:     Clock in Hz
static uint32_t _GetPwmClock(void);

// Name:        _ColorsAreEqual
// Description: Check if two colors are equal
// Arguments:   Color1 - First color - rgb_color_t value
//              Color2 - Second color - rgb_color_t value
// Returns:     True if equal. False otherwise
static bool _ColorsAreEqual(rgb_color_t Color1, rgb_color_t Color2);

// Name:        _CalculateStepSkip
// Description: Determine the number of steps to skip during the fade
// Arguments:   NewValue - First value
//              CurrentValue - Second value
//              FadeSteps - Number of fade steps
// Returns:     True if equal. False otherwise
static uint16_t _CalculateStepSkip(uint16_t NewValue, uint16_t CurrentValue, uint16_t FadeSteps);

// ------------------------------------------------------------------------------------------------------- //
// Functions definitions
// ------------------------------------------------------------------------------------------------------- //

// Name:        _InitHardware
// Description: Starts device peripherals
// Arguments:   None
// Returns:     None

static void _InitHardware(void)
{
    // Enable peripheral clocks
    SysCtlPeripheralEnable(_Config.Periph.Pwm);
    SysCtlPeripheralEnable(_Config.Periph.Gpio);
    SysCtlPeripheralEnable(_Config.Periph.Timer);

    // Wait until last peripheral is ready
    while(!SysCtlPeripheralReady (_Config.Periph.Timer));

    // Unlock used pins (has no effect if pin is not protected by the GPIOCR register
    GPIOUnlockPin(_Config.Base.Gpio, _Config.Pin.R | _Config.Pin.G | _Config.Pin.B);

    // Configure pins as PWM outputs
    GPIOPinTypePWM (_Config.Base.Gpio, _Config.Pin.R | _Config.Pin.G | _Config.Pin.B);
    GPIOPinConfigure (_Config.PinMux.R);
    GPIOPinConfigure (_Config.PinMux.G);
    GPIOPinConfigure (_Config.PinMux.B);

    // Configure PWM options
    PWMGenConfigure (_Config.Base.Pwm, _Config.Gen.R, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_DBG_RUN);
    PWMGenConfigure (_Config.Base.Pwm, _Config.Gen.G, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_DBG_RUN);
    PWMGenConfigure (_Config.Base.Pwm, _Config.Gen.B, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_DBG_RUN);

    // Set the PWM frequency
    _SetPwmFrequency (_Config.Params.PwmFrequency);

    // Turn on the output pins
    PWMOutputState (_Config.Base.Pwm, _Config.OutBit.R | _Config.OutBit.G | _Config.OutBit.B , true);

    // Enable the PWM generators
    PWMGenEnable (_Config.Base.Pwm, _Config.Gen.R);
    PWMGenEnable (_Config.Base.Pwm, _Config.Gen.G);
    PWMGenEnable (_Config.Base.Pwm, _Config.Gen.B);

    // Configure timer mode
    TimerConfigure(_Config.Base.Timer, TIMER_CFG_PERIODIC);

    // Set timer period
    uint32_t timerPeriod = (SysCtlClockGet()/_Config.Params.PwmFrequency) - 1;
    TimerLoadSet(_Config.Base.Timer, TIMER_A, timerPeriod);

    // Register interrupt handler
    TimerIntRegister (_Config.Base.Timer, TIMER_A, _IsrTimerHandler);

    // Enable interrupt on timer timeout
    TimerIntEnable(_Config.Base.Timer, TIMER_TIMA_TIMEOUT);

    // Set initial color - Off
    Rgb_SetColor (_NewColor, 0);
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        _IsrTimerHandler
// Description: Timer interrupt service routine
// Arguments:   None
// Returns:     None

static void _IsrTimerHandler (void)
{
    // Clear interrupt flag
    TimerIntClear (_Config.Base.Timer, TIMER_TIMA_TIMEOUT);

    // Fade to desired color
    _FadeService();
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        _FadeService
// Description: Fade service to ensure proper color transitions
// Arguments:   None
// Returns:     None

static void _FadeService(void)
{
    static bool _IsFading = false;
    static uint16_t _StepSkipR = 0;
    static uint16_t _StepSkipG = 0;
    static uint16_t _StepSkipB = 0;

    // Make sure PWM frequency is right (may change if PWM clock is divided externally)
    _SetPwmFrequency(_Config.Params.PwmFrequency);

    // Fade to new color
    if (_FadeSteps != 0 && !_ColorsAreEqual(_CurrentColor, _NewColor))
    {
        // Not already fading
        if (!_IsFading)
        {
            // Reset variables
            _StepCounter = 0;
            _IsFading = true;

            // Calculate number of steps to skip for each color component
            _StepSkipR = _CalculateStepSkip(_NewColor.R, _CurrentColor.R, _FadeSteps);
            _StepSkipG = _CalculateStepSkip(_NewColor.G, _CurrentColor.G, _FadeSteps);
            _StepSkipB = _CalculateStepSkip(_NewColor.B, _CurrentColor.B, _FadeSteps);
        }

        // Increase step counter
        _StepCounter++;

        // Change color components
        if ((_StepCounter % _StepSkipR) == 0)
            _CurrentColor.R += (_NewColor.R > _CurrentColor.R ? 1 : -1);

        if ((_StepCounter % _StepSkipG) == 0)
            _CurrentColor.G += (_NewColor.G > _CurrentColor.G ? 1 : -1);

        if ((_StepCounter % _StepSkipB) == 0)
            _CurrentColor.B += (_NewColor.B > _CurrentColor.B ? 1 : -1);

        // Fade is complete
        if (_StepCounter >= _FadeSteps)
        {
            _CurrentColor = _NewColor;
            _IsFading = false;
        }
    }

    // No fade
    else
    {
        _CurrentColor = _NewColor;
        _IsFading = false;
        TimerDisable(_Config.Base.Timer, TIMER_A);
    }

    // Define and apply duty cycles
    uint16_t DutyR = (uint16_t) (Aux_Map(_CurrentColor.R, 0, 255, 1, _PwmPeriod));
    uint16_t DutyG = (uint16_t) (Aux_Map(_CurrentColor.G, 0, 255, 1, _PwmPeriod));
    uint16_t DutyB = (uint16_t) (Aux_Map(_CurrentColor.B, 0, 255, 1, _PwmPeriod));
    _SetPwmDuty(DutyR, DutyG, DutyB);
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        _SetPwmFrequency
// Description: Changes PWM modules frequency
// Arguments:   Frequency
// Returns:     None

static void _SetPwmFrequency (uint32_t Frequency)
{
    // Set the period (expressed in clock ticks)
    _PwmPeriod = (_GetPwmClock()/Frequency) - 1;
    PWMGenPeriodSet (_Config.Base.Pwm, _Config.Gen.R, _PwmPeriod);
    PWMGenPeriodSet (_Config.Base.Pwm, _Config.Gen.G, _PwmPeriod);
    PWMGenPeriodSet (_Config.Base.Pwm, _Config.Gen.B, _PwmPeriod);
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        _SetPwmDuty
// Description: Changes PWM modules duty cycle
// Arguments:   DutyR, DutyG, DutyB - Duty cycles - 0 to _PwmPeriod
// Returns:     None

static void _SetPwmDuty (uint16_t DutyR, uint16_t DutyG, uint16_t DutyB)
{
    PWMPulseWidthSet (_Config.Base.Pwm, _Config.Out.R , DutyR);
    PWMPulseWidthSet (_Config.Base.Pwm, _Config.Out.G , DutyG);
    PWMPulseWidthSet (_Config.Base.Pwm, _Config.Out.B , DutyB);
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        _GetPwmClock
// Description: Gets PWM module clock
// Arguments:   None
// Returns:     Clock in Hz

static uint32_t _GetPwmClock()
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

// Name:        _ColorsAreEqual
// Description: Check if two colors are equal
// Arguments:   Color1 - First color - rgb_color_t value
//              Color2 - Second color - rgb_color_t value
// Returns:     True if equal. False otherwise

static bool _ColorsAreEqual(rgb_color_t Color1, rgb_color_t Color2)
{
    return (Color1.R == Color2.R && Color1.G == Color2.G && Color1.B == Color2.B);
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        _CalculateStepSkip
// Description: Determine the number of steps to skip during the fade
// Arguments:   NewValue - First value
//              CurrentValue - Second value
//              FadeSteps - Number of fade steps
// Returns:     True if equal. False otherwise

static uint16_t _CalculateStepSkip(uint16_t NewValue, uint16_t CurrentValue, uint16_t FadeSteps)
{
    uint16_t Diff = abs((int16_t)NewValue - CurrentValue);

    if (Diff == 0)
        return 0;

    else if (Diff >= FadeSteps)
        return 1;

    else
        return (float)FadeSteps / Diff + 0.5;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        Rgb_Init
// Description: Starts device peripherals and application logic
// Arguments:   Config - rgb_config_t struct
// Returns:     None

void Rgb_Init(const rgb_config_t *Config)
{
    // Copy config to a private variable
    _Config = *Config;

    //  Initialize hardware
    _InitHardware();
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        Rgb_SetColor
// Description: Sets the RGB LED color
// Arguments:   Color - New color - rgb_color_t structure (0x00 to 0xFF values)
//              FadeTime - Fade time to new color (ms)
// Returns:     None

void Rgb_SetColor(rgb_color_t Color, uint16_t FadeTime)
{
    _StepCounter = 0;
    _NewColor = Color;
    _FadeSteps = (FadeTime != 0) * (FadeTime*1000) / _Config.Params.PwmFrequency;

    // Limit fade steps
    if (_FadeSteps > _Config.Params.PwmFrequency)
        _FadeSteps = _Config.Params.PwmFrequency;

    TimerEnable(_Config.Base.Timer, TIMER_A);
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        Rgb_GetColor
// Description: Gets the current LED color
// Arguments:   Buffer - Pointer rgb_color_t structure where values will be saved (0x00 to 0xFF values)
// Returns:     None

void Rgb_GetColor(rgb_color_t *Buffer)
{
    // Copy the current color to the provided buffer
    if (Buffer != 0)
        *Buffer = _CurrentColor;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        Rgb_IsChangingColor
// Description: Checks if LED is in the process of changing colors
// Arguments:   None
// Returns:     True if colors are changing. False otherwise

bool Rgb_IsChangingColor (void)
{
    return !_ColorsAreEqual(_CurrentColor, _NewColor);
}

// ------------------------------------------------------------------------------------------------------- //

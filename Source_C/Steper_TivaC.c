// ------------------------------------------------------------------------------------------------------- //

// Stepper library
// Version: 1.0
// Author: Renan Duarte
// E-mail: duarte.renan@hotmail.com
// Date:   23/05/2024

// Overview:

//      This library is designed to provide robust and precise control over a stepper motor that is
//      responsible for linear movements. It includes a comprehensive set of features to manage the
//      motor's velocity, acceleration, and directional control, ensuring smooth and accurate movements
//      for applications requiring linear motion.

// Key Features:

// Velocity Control:
//      The library allows for precise control over the stepper motor's velocity. It ensures smooth
//      transitions between different speeds, whether accelerating or decelerating, by dynamically
//      adjusting the PWM frequency.

// Acceleration Management:
//      It includes functionality to set and adjust the acceleration and deceleration rates. The library
//      calculates the necessary changes to the PWM frequency to achieve the desired acceleration,
//      ensuring the motor moves smoothly without sudden changes in speed.

// Direction Control:
//      The library provides mechanisms to control the motor's direction. It can dynamically change the
//      motor's direction based on the target velocity, ensuring the motor moves forward or backward as
//      needed.

// Limit Switches Integration:
//      The library checks the status of limit switches to prevent the motor from moving beyond the
//      predefined limits, protecting both the motor and the mechanical system.

// Stall Detection:
//      The library includes a stall detection feature based on a given encoder reading (external). If the
//      motor is enabled but there is no movement detected (indicating a possible stall), it can trigger
//      appropriate actions to handle the stall condition. This function must be called externally every
//      time a new encoder reading is available.

// ------------------------------------------------------------------------------------------------------- //

// ------------------------------------------------------------------------------------------------------- //
// Includes
// ------------------------------------------------------------------------------------------------------- //

// Stepper defines and macros
#include "Stepper_TivaC.h"

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
// "Private" variables
// ------------------------------------------------------------------------------------------------------- //

// Velocity delta to keep constant acceleration
static float _DeltaVel = 0;

// Stepper configuration object
static stepper_config_t _Config;

// Stepper-related variables
static stepper_status_t _Status = stepper_status_t_default;

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

// Name:        _IsrLimHandler
// Description: PWM interrupt service routine
// Arguments:   None
// Returns:     None
static void _IsrLimHandler (void);

// Name:        _GetPwmClock
// Description: Gets PWM module clock
// Arguments:   None
// Returns:     Clock in Hz
static uint32_t _GetPwmClock(void);

// Name:        _SetDirection
// Description: Sets stepper direction pin
// Arguments:   NewDirection - True to go forward, false otherwise
// Returns:     None
static void _SetDirection (bool NewDirection);

// Name:        _SetEnable
// Description: Sets stepper enable pin
// Arguments:   NewEnable - True to enable stepper, false otherwise
// Returns:     None
static void _SetEnable (bool NewEnable);

// Name:        _StartPwm
// Description: Starts stepper PWM generator
// Arguments:   None
// Returns:     None
static void _StartPwm (void);

// Name:        _StopPwm
// Description: Stops stepper PWM generator
// Arguments:   None
// Returns:     None
static void _StopPwm (void);

// Name:        _SetPwmFreq
// Description: Updates the frequency and duty cycle of the PWM generator
// Arguments:   NewFreq - Frequency to be set (Hz)
// Returns:     None
static void _SetPwmFreq (uint32_t NewFreq);

// Name:        _SetVel
// Description: Updates the stepper velocity
// Arguments:   NewVel - New velocity (m/s)
// Returns:     None
static void _SetVel (float NewVel);

// Name:        _CanMove
// Description: Checks if movement is possible in a given direction
// Arguments:   Direction - True to forward direction, false otherwise
// Returns:     None
static bool _CanMove(bool Direction);

// Name:        _CalculateVel
// Description: Defines stepper velocity based on target values and current state
// Arguments:   None
// Returns:     None
static void _CalculateVel (void);

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
    SysCtlPeripheralEnable(_Config.Pwm.Periph);
    SysCtlPeripheralEnable(_Config.Step.Periph);
    SysCtlPeripheralEnable(_Config.Dir.Periph);
    SysCtlPeripheralEnable(_Config.En.Periph);
    SysCtlPeripheralEnable(_Config.LimStart.Periph);
    SysCtlPeripheralEnable(_Config.LimEnd.Periph);
    SysCtlPeripheralEnable(_Config.Timer.Periph);

    // Wait until last peripheral is ready
    while(!SysCtlPeripheralReady (_Config.Timer.Periph));

    // Unlock used pins (has no effect if pin is not protected by the GPIOCR register
    GPIOUnlockPin(_Config.Step.Base, _Config.Step.Pin);
    GPIOUnlockPin(_Config.Dir.Base, _Config.Dir.Pin);
    GPIOUnlockPin(_Config.En.Base, _Config.En.Pin);
    GPIOUnlockPin(_Config.LimStart.Base, _Config.LimStart.Pin);
    GPIOUnlockPin(_Config.LimEnd.Base, _Config.LimEnd.Pin);

    // Configure DIR and EN pins as outputs
    GPIOPinTypeGPIOOutput (_Config.Dir.Base, _Config.Dir.Pin);
    GPIOPinTypeGPIOOutput (_Config.En.Base, _Config.En.Pin);

    // Configure pins - 8mA drive with slew rate control and push-pull mode
    GPIOPadConfigSet (_Config.Dir.Base, _Config.Dir.Pin, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);
    GPIOPadConfigSet (_Config.En.Base, _Config.En.Pin, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);

    // Set initial output states
    _SetDirection (_Status.Dir);
    _SetEnable(_Status.Enabled);

    // Configure step pin as PWM output
    GPIOPinTypePWM (_Config.Step.Base, _Config.Step.Pin);
    GPIOPinConfigure (_Config.Step.PinMux);

    // Define initial PWM clock
    _SetPwmFreq (_Config.Params.VelMax * _Config.Params.Kv);

    // Configure PWM options
    PWMGenConfigure (_Config.Pwm.Base, _Config.Pwm.Gen, PWM_GEN_MODE_DOWN);

    // Configure timer mode
    TimerConfigure(_Config.Timer.Base, TIMER_CFG_PERIODIC);

    // Set timer period
    uint32_t timerPeriod = (SysCtlClockGet()/_Config.Params.VelUpdateFrequency) - 1;
    TimerLoadSet(_Config.Timer.Base, TIMER_A, timerPeriod);

    // Register interrupt handler
    TimerIntRegister (_Config.Timer.Base, TIMER_A, _IsrTimerHandler);

    // Enable interrupt on timer timeout
    TimerIntEnable(_Config.Timer.Base, TIMER_TIMA_TIMEOUT);

    // Configure limit switch pins as inputs
    GPIOPinTypeGPIOInput (_Config.LimStart.Base, _Config.LimStart.Pin);
    GPIOPinTypeGPIOInput (_Config.LimEnd.Base, _Config.LimEnd.Pin);

    // Enable pull-up resistor
    GPIOPadConfigSet (_Config.LimStart.Base, _Config.LimStart.Pin, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOPadConfigSet (_Config.LimEnd.Base, _Config.LimEnd.Pin, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // Interrupt trigger on rising edge
    GPIOIntTypeSet (_Config.LimStart.Base, _Config.LimStart.Pin, GPIO_RISING_EDGE);
    GPIOIntTypeSet (_Config.LimEnd.Base, _Config.LimEnd.Pin, GPIO_RISING_EDGE);

    // Register interrupt handler
    GPIOIntRegister(_Config.LimStart.Base, _IsrLimHandler);
    GPIOIntRegister(_Config.LimEnd.Base, _IsrLimHandler);

    // Enable interrupt on limit switches
    GPIOIntEnable (_Config.LimStart.Base, _Config.LimStart.Pin);
    GPIOIntEnable (_Config.LimEnd.Base, _Config.LimEnd.Pin);
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        _IsrLimHandler
// Description: PWM interrupt service routine
// Arguments:   None
// Returns:     None

static void _IsrLimHandler (void)
{
    if (GPIOIntStatus (_Config.LimStart.Base, true) == _Config.LimStart.Pin)
    {
        // Is moving backwards. Not possible  anymore
        if ((_Status.Enabled) && (_Status.Dir == 0))
            Stepper_Stop();

        // Clear the asserted interrupts
        GPIOIntClear (_Config.LimStart.Base, _Config.LimStart.Pin);
    }

    if (GPIOIntStatus (_Config.LimEnd.Base, true) == _Config.LimEnd.Pin)
    {
        // Is moving forwards. Not possible  anymore
        if ((_Status.Enabled) && (_Status.Dir == 1))
            Stepper_Stop();

        // Clear the asserted interrupts
        GPIOIntClear (_Config.LimEnd.Base, _Config.LimEnd.Pin);
    }
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        _IsrTimerHandler
// Description: Timer interrupt service routine
// Arguments:   None
// Returns:     None

static void _IsrTimerHandler (void)
{
    // Clear interrupt flag
    TimerIntClear (_Config.Timer.Base, TIMER_TIMA_TIMEOUT);

    // Velocity update routine
    _CalculateVel();
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        _GetPwmClock
// Description: Gets PWM module clock
// Arguments:   None
// Returns:     Clock in Hz

static uint32_t _GetPwmClock(void)
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

// Name:        _SetDirection
// Description: Sets stepper direction pin
// Arguments:   NewDirection - True to go forward, false otherwise
// Returns:     None

static void _SetDirection (bool NewDirection)
{
    _Status.Dir = NewDirection;
    GPIOPinWrite (_Config.Dir.Base, _Config.Dir.Pin, NewDirection * 0xFF);
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        _SetEnable
// Description: Sets stepper enable pin
// Arguments:   NewEnable - True to enable stepper, false otherwise
// Returns:     None

static void _SetEnable (bool NewEnable)
{
    _Status.Enabled = NewEnable;
    GPIOPinWrite (_Config.En.Base, _Config.En.Pin, !NewEnable * 0xFF);
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        _StartPwm
// Description: Starts stepper PWM generator
// Arguments:   None
// Returns:     None

static void _StartPwm (void)
{
    // Turn on the output pin
    PWMOutputState (_Config.Pwm.Base, _Config.Pwm.OutBit, true);

    // Start PWM
    PWMGenEnable (_Config.Pwm.Base, _Config.Pwm.Gen);
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        _StopPwm
// Description: Stops stepper PWM generator
// Arguments:   None
// Returns:     None

static void _StopPwm (void)
{
    // Stop PWM
    PWMGenDisable (_Config.Pwm.Base, _Config.Pwm.Gen);

    // Clear interrupt flag
    PWMGenIntClear(_Config.Pwm.Base, _Config.Pwm.Gen, PWM_INT_CNT_ZERO);

    // Turn off the output pin
    PWMOutputState (_Config.Pwm.Base, _Config.Pwm.OutBit, false);

    // Reset value
    _Status.PwmFrequency = 0;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        _SetPwmFreq
// Description: Updates the frequency and duty cycle of the PWM generator
// Arguments:   NewFreq - Frequency to be set (Hz)
// Returns:     None

static void _SetPwmFreq (uint32_t NewFreq)
{
    bool ClockChanged = false;

    // Check if PWM clock needs adjustment
    if ((NewFreq > 3000) && (_Config.Params.PwmClock != SysCtlClockGet()))
    {
        // Desired PWM frequency is high enough for high speed PWM clock
        SysCtlPWMClockSet(SYSCTL_PWMDIV_1);
        ClockChanged = true;
    }

    else if ((NewFreq < 2000) && (_Config.Params.PwmClock == SysCtlClockGet()))
    {
        // Desired PWM frequency is too low for current PWM clock
        SysCtlPWMClockSet(SYSCTL_PWMDIV_64);
        ClockChanged = true;
    }

    // Update PWM variables if it has changed
    if (ClockChanged)
    {
        _Config.Params.PwmClock = _GetPwmClock();
        _Config.Params.PwmDz = (_GetPwmClock() >> 16) + 1;
        _Config.Params.VelMin = (float)_Config.Params.PwmDz / _Config.Params.Kv;
    }

    // Set value
    _Status.PwmFrequency = NewFreq;

    // Disable motor without resetting the status flag if inside dead zone
    if ((NewFreq < _Config.Params.PwmDz) && (_Status.Enabled))
    {
        GPIOPinWrite (_Config.En.Base, _Config.En.Pin, 0xFF);
        return;
    }

    // Make sure motor is enabled otherwise
    else if (_Status.Enabled)
        GPIOPinWrite (_Config.En.Base, _Config.En.Pin, 0x00);

    // Define PWM period (expressed in clock ticks)
    _Config.Params.PwmPeriod = (_Config.Params.PwmClock/_Status.PwmFrequency) - 1;

    // Change PWM frequency and duty cycle (only if necessary)
    if (_Config.Params.PwmPeriod != PWMGenPeriodGet (_Config.Pwm.Base, _Config.Pwm.Gen))
    {
        PWMGenPeriodSet (_Config.Pwm.Base, _Config.Pwm.Gen, _Config.Params.PwmPeriod);
        PWMPulseWidthSet (_Config.Pwm.Base, _Config.Pwm.Out, _Config.Params.PwmPeriod >> 1);
    }
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        _SetVel
// Description: Updates the stepper velocity
// Arguments:   NewVel - New velocity (m/s)
// Returns:     None

static void _SetVel (float NewVel)
{
    if (NewVel == 0)
    {
        Stepper_Stop();
        return;
    }

    // Set value
    if ((Aux_FastFabs(NewVel - _Status.TargetVel) * _Config.Params.Kv) < 1)
        _Status.CurrentVel = _Status.TargetVel;
    else
        _Status.CurrentVel = NewVel;

    // Change PWM frequency
    _SetPwmFreq (_Config.Params.Kv * Aux_FastFabs(NewVel));
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        _CanMove
// Description: Checks if movement is possible in a given direction
// Arguments:   Direction - True to forward direction, false otherwise
// Returns:     None

static bool _CanMove(bool Direction)
{
    // One of the limit switches is pressed and stepper is trying to move in the same direction
    return !((GPIOPinRead(_Config.LimStart.Base, _Config.LimStart.Pin) && (Direction == 0))
            || (GPIOPinRead(_Config.LimEnd.Base, _Config.LimEnd.Pin) && (Direction == 1)));
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        _CalculateVel
// Description: Defines stepper velocity based on target values and current state
// Arguments:   None
// Returns:     None

static void _CalculateVel (void)
{
    // Target reached
    if (_Status.CurrentVel == _Status.TargetVel)
    {
        // Stop velocity control timer
        TimerDisable(_Config.Timer.Base, TIMER_A);

        // No need to go further
        return;
    }

    // New velocity
    float NewVel = _Status.CurrentVel;

    // Acceleration is off
    if (_Status.CurrentAcc < 0)
        NewVel = _Status.TargetVel;

    // Acceleration is on
    else if (_Status.CurrentAcc > 0)
    {
        // Calculate the new velocity
        if (!_Status.Enabled)
            NewVel = (_Status.TargetVel < 0 ? -_Config.Params.VelMin : _Config.Params.VelMin);
        else if (Aux_FastFabs(_Status.CurrentVel - _Status.TargetVel) < _DeltaVel)
            NewVel = _Status.TargetVel;
        else if (_Status.CurrentVel < _Status.TargetVel)
            NewVel = _Status.CurrentVel + _DeltaVel;
        else if (_Status.CurrentVel > _Status.TargetVel)
            NewVel = _Status.CurrentVel - _DeltaVel;
    }

    // _DeltaVel caused a sign inversion in NewVel, motor was stopped or acceleration is off. Redefine direction
    if (((*(int32_t*)&NewVel ^ *(int32_t*)&_Status.CurrentVel) < 0) || (!_Status.Enabled) || (_Status.CurrentAcc < 0))
        _SetDirection (_Status.TargetVel < 0 ? 0 : 1);

    // Apply new velocity
    _SetVel (NewVel);
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        Init
// Description: Starts device peripherals and application logic
// Arguments:   Config - rgb_config_t struct
// Returns:     None

void Stepper_Init(const stepper_config_t *Config)
{
    // Copy config to a private variable
    _Config = *Config;

    //  Initialize hardware
    _InitHardware();
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        GetStatus
// Description: Gets current status of stepper motor
// Arguments:   Buffer - stepper_status_t struct to receive the data
// Returns:     None

void Stepper_GetStatus(stepper_status_t *Buffer)
{
    if (Buffer != 0)
        *Buffer = _Status;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        GetEnabled
// Description: Gets the current enabled status
// Arguments:   None
// Returns:     bool - Current enabled status

bool Stepper_GetEnabled(void)
{
    return _Status.Enabled;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        GetDir
// Description: Gets the current direction
// Arguments:   None
// Returns:     bool - Current direction (1 = forward, 0 = backward)

bool Stepper_GetDir(void)
{
    return _Status.Dir;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        GetTargetVel
// Description: Gets the target velocity
// Arguments:   None
// Returns:     float - Target velocity (m/s)

float Stepper_GetTargetVel(void)
{
    return _Status.TargetVel;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        GetCurrentVel
// Description: Gets the current velocity
// Arguments:   None
// Returns:     float - Current velocity (m/s)

float Stepper_GetCurrentVel(void)
{
    return _Status.CurrentVel;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        GetCurrentAcc
// Description: Gets the current acceleration
// Arguments:   None
// Returns:     float - Current acceleration (m/s^2)

float Stepper_GetCurrentAcc(void)
{
    return _Status.CurrentAcc;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        GetPwmFrequency
// Description: Gets the current PWM frequency
// Arguments:   None
// Returns:     uint32_t - Current PWM frequency (Hz)

uint32_t Stepper_GetPwmFrequency(void)
{
    return _Status.PwmFrequency;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        Stop
// Description: Stops stepper
// Arguments:   None
// Returns:     None

void Stepper_Stop (void)
{
    // Reset enable pin
    _SetEnable(false);

    // Stop PWM
    _StopPwm ();

    // Reset velocity and acceleration
    _Status.CurrentVel = 0;
    _Status.CurrentAcc = 0;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        Move
// Description: Move stepper at FinalVelocity with or without velocity ramp
//              Direction is determined based on current status and final velocity value
// Arguments:   FinalVelocity - Target velocity in m/s
//              Acceleration - Acceleration value in m/s^2 (-1 to instant change in velocity)
// Returns:     True is movement is possible. False otherwise

bool Stepper_Move (float FinalVelocity, float Acceleration)
{
    // Get velocity sign (1 or -1)
    int8_t VelocitySign = FinalVelocity > 0 ? 1 : -1;

    // Get velocity absolute value
    float VelocityAbs = FinalVelocity*VelocitySign;

    // Velocity saturation
    if (VelocityAbs > _Config.Params.VelMax)
        VelocityAbs = _Config.Params.VelMax;

    // Acceleration saturation
    if (Acceleration > _Config.Params.AccMax)
        Acceleration = _Config.Params.AccMax;

    // Set target velocity and acceleration
    _Status.TargetVel = VelocityAbs * VelocitySign;
    _Status.CurrentAcc = Acceleration;

    // Define required velocity delta
    _DeltaVel = _Status.CurrentAcc / _Config.Params.VelUpdateFrequency;

    // Stepper is stopped and need to move
    if ((!_Status.Enabled) && (VelocityAbs != 0))
    {
        // Define initial conditions
        _CalculateVel();

        // Can move in desired direction
        if (_CanMove(_Status.Dir))
        {
            // Enable stepper
            _SetEnable(true);

            // Start PWM
            _StartPwm ();
        }

        // Cannot move. Reset variables
        else
            Stepper_Stop();
    }

    // Start velocity control timer
    if (_Status.CurrentVel != _Status.TargetVel)
        TimerEnable(_Config.Timer.Base, TIMER_A);

    // Return
    return _Status.Enabled;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        CheckForStall
// Description: Checks if the stepper motor is stalled by comparing the current and last encoder values
// Arguments:   EncoderValue - The current value of the encoder
// Returns:     bool - Returns true if the motor is stalled, otherwise false

bool Stepper_CheckForStall (uint32_t EncoderValue)
{
    bool Stall = false;

    if (_Status.Enabled)
    {
        static uint32_t LastEncoderValue = 0;

        // Cart moved too little and stepper has PWM
        if ((LastEncoderValue == EncoderValue) && (_Status.CurrentVel != 0))
            Stall = true;

        LastEncoderValue = EncoderValue;
    }

    return Stall;
}

// ------------------------------------------------------------------------------------------------------- //

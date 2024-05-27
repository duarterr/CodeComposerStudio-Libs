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

#ifndef STEPPER_TIVAC_H_
#define STEPPER_TIVAC_H_

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

#define MAX_STEPPERS 1              // Maximum number of encoder instances

// ------------------------------------------------------------------------------------------------------- //
// Structs
// ------------------------------------------------------------------------------------------------------- //

// PWM generator struct
typedef struct
{
    uint32_t Periph;                // Peripheral
    uint32_t Base;                  // Base
    uint32_t Gen;                   // Generator
    uint32_t Out;                   // Output
    uint32_t OutBit;                // Output bit
} stepper_pwm_t;

// Timer struct (used for the acceleration control)
typedef struct
{
    uint32_t Periph;                // Peripheral
    uint32_t Base;                  // Base
} stepper_timer_t;

// GPIO struct
typedef struct
{
    uint32_t Periph;                // Peripheral
    uint32_t Base;                  // Base
    uint32_t Pin;                   // Pin
    uint32_t PinMux;                // Pin mux config
} stepper_gpio_t;

// Stepper parameters structure
typedef struct
{
    float VelMax;                   // Maximum velocity (m/s)
    float AccMax;                   // Maximum acceleration (m/s^2)
    float Kv;                       // Relation between PPS and m/s
    uint16_t VelUpdateFrequency;    // Frequency at which the velocity calculation will be performed
    float VelMin;                   // Minimum achievable velocity (m/s) - SET INTERNALLY - DO NOT CHANGE
    uint32_t PwmDz;                 // PWM dead-zone (minimum possible frequency) - SET INTERNALLY - DO NOT CHANGE
    uint32_t PwmPeriod;             // Current PWM period (clock ticks) - SET INTERNALLY - DO NOT CHANGE
    uint32_t PwmClock;              // Current PWM clock (Hz) - SET INTERNALLY - DO NOT CHANGE
} stepper_params_t;

// Stepper configuration structure
typedef struct
{
    stepper_pwm_t Pwm;              // PWM generator struct
    stepper_gpio_t Step;            // GPIO struct - PWM pin
    stepper_gpio_t Dir;             // GPIO struct - Direction pin
    stepper_gpio_t En;              // GPIO struct - Enable pin
    stepper_gpio_t LimStart;        // GPIO struct - Limit switch - Axis start
    stepper_gpio_t LimEnd;          // GPIO struct - Limit switch - Axis end
    stepper_timer_t Timer;          // Timer stuct
    stepper_params_t Params;        // Parameters struct
} stepper_config_t;

// Stepper status variables
typedef struct
{
    bool Enabled;                   // Current enabled status
    bool Dir;                       // Current direction (1 = forward, 0 = backward)
    float TargetVel;                // Target velocity (m/s)
    float CurrentVel;               // Current velocity (m/s)
    float CurrentAcc;               // Current acceleration (m/s^2)
    uint32_t PwmFrequency;          // Current PWM frequency (Hz)
} stepper_status_t;

// Stepper status variables - Default values
#define stepper_status_t_default { \
    .Enabled = false, \
    .Dir = false, \
    .TargetVel = 0, \
    .CurrentVel = 0, \
    .CurrentAcc = 0, \
    .PwmFrequency = 0, \
}

// ------------------------------------------------------------------------------------------------------- //
// Class prototype
// ------------------------------------------------------------------------------------------------------- //

class Stepper
{
    // --------------------------------------------------------------------------------------------------- //
    // Private members
    // --------------------------------------------------------------------------------------------------- //

    private:

        // Array to store pointers to instances
        static Stepper* _Instance[MAX_STEPPERS];

        // Counter to keep track of the number of instances
        static uint8_t _InstanceCounter;

        // Velocity delta to keep constant acceleration
        float _DeltaVel = 0;

        // Stepper configuration object
        stepper_config_t _Config;

        // Stepper-related variables
        stepper_status_t _Status = stepper_status_t_default;

        // Name:        _InitHardware
        // Description: Starts device peripherals
        // Arguments:   None
        // Returns:     None
        void _InitHardware();

        // Name:        _IsrLimStaticCallback
        // Description: Static callback function for handling limit switches interrupts
        // Arguments:   None
        // Returns:     None
        static void _IsrLimStaticCallback();

        // Name:        _IsrLimHandler
        // Description: PWM interrupt service routine
        // Arguments:   None
        // Returns:     None
        void _IsrLimHandler ();

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

        // Name:        _GetPwmClock
        // Description: Gets PWM module clock
        // Arguments:   None
        // Returns:     Clock in Hz
        uint32_t _GetPwmClock();

        // Name:        _SetDirection
        // Description: Sets stepper direction pin
        // Arguments:   NewDirection - True to go forward, false otherwise
        // Returns:     None
        void _SetDirection (bool NewDirection);

        // Name:        _SetEnable
        // Description: Sets stepper enable pin
        // Arguments:   NewEnable - True to enable stepper, false otherwise
        // Returns:     None
        void _SetEnable (bool NewEnable);

        // Name:        _StartPwm
        // Description: Starts stepper PWM generator
        // Arguments:   None
        // Returns:     None
        void _StartPwm ();

        // Name:        _StopPwm
        // Description: Stops stepper PWM generator
        // Arguments:   None
        // Returns:     None
        void _StopPwm ();

        // Name:        _SetPwmFreq
        // Description: Updates the frequency and duty cycle of the PWM generator
        // Arguments:   NewFreq - Frequency to be set (Hz)
        // Returns:     None
        void _SetPwmFreq (uint32_t NewFreq);

        // Name:        _CalculateVel
        // Description: Defines stepper velocity based on target values and current state
        // Arguments:   None
        // Returns:     None
        void _CalculateVel ();

        // Name:        _SetVel
        // Description: Updates the stepper velocity
        // Arguments:   NewVel - New velocity (m/s)
        // Returns:     None
        void _SetVel (float NewVel);

        // Name:        _CanMove
        // Description: Checks if movement is possible in a given direction
        // Arguments:   Direction - True to forward direction, false otherwise
        // Returns:     None
        bool _CanMove(bool Direction);

    // --------------------------------------------------------------------------------------------------- //
    // Public members
    // --------------------------------------------------------------------------------------------------- //

    public:

        // Name:        Stepper
        // Description: Constructor of the class with no arguments
        // Arguments:   None
        // Returns:     None
        Stepper();

        // Name:        Stepper
        // Description: Constructor of the class with stepper_config_t struct as argument
        // Arguments:   Config - stepper_config_t struct
        // Returns:     None
        Stepper(stepper_config_t *Config);

        // Name:        Init
        // Description: Starts device peripherals and application logic
        // Arguments:   Config - stepper_config_t struct
        // Returns:     None
        void Init(stepper_config_t *Config);

        // Name:        GetStatus
        // Description: Gets current status of stepper motor
        // Arguments:   Buffer - stepper_status_t struct to receive the data
        // Returns:     None
        void GetStatus(stepper_status_t *Buffer);

        // Name:        GetEnabled
        // Description: Gets the current enabled status
        // Arguments:   None
        // Returns:     bool - Current enabled status
        bool GetEnabled() const;

        // Name:        GetDir
        // Description: Gets the current direction
        // Arguments:   None
        // Returns:     bool - Current direction (1 = forward, 0 = backward)
        bool GetDir() const;

        // Name:        GetTargetVel
        // Description: Gets the target velocity
        // Arguments:   None
        // Returns:     float - Target velocity (m/s)
        float GetTargetVel() const;

        // Name:        GetCurrentVel
        // Description: Gets the current velocity
        // Arguments:   None
        // Returns:     float - Current velocity (m/s)
        float GetCurrentVel() const;

        // Name:        GetCurrentAcc
        // Description: Gets the current acceleration
        // Arguments:   None
        // Returns:     float - Current acceleration (m/s^2)
        float GetCurrentAcc() const;

        // Name:        GetPwmFrequency
        // Description: Gets the current PWM frequency
        // Arguments:   None
        // Returns:     uint32_t - Current PWM frequency (Hz)
        uint32_t GetPwmFrequency() const;

        // Name:        Stop
        // Description: Stops stepper
        // Arguments:   None
        // Returns:     None
        void Stop ();

        // Name:        Move
        // Description: Move stepper at FinalVelocity with or without velocity ramp
        //              Direction is determined based on current status and final velocity value
        // Arguments:   FinalVelocity - Target velocity in m/s
        //              Acceleration - Acceleration value in m/s^2 (-1 to instant change in velocity)
        // Returns:     True is movement is possible. False otherwise
        bool Move (float FinalVelocity, float Acceleration);

        // Name:        CheckForStall
        // Description: Checks if the stepper motor is stalled by comparing the current and last encoder values
        // Arguments:   EncoderValue - The current value of the encoder
        // Returns:     bool - Returns true if the motor is stalled, otherwise false
        bool CheckForStall (uint32_t EncoderValue);
};

// ------------------------------------------------------------------------------------------------------- //

#ifdef __cplusplus
}
#endif

#endif

// ------------------------------------------------------------------------------------------------------- //

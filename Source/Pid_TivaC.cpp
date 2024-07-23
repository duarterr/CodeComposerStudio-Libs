// ------------------------------------------------------------------------------------------------------- //

// PID controller library
// Version: 1.0
// Author: Renan Duarte
// E-mail: duarte.renan@hotmail.com
// Date:   23/07/2024

// ------------------------------------------------------------------------------------------------------- //

// ------------------------------------------------------------------------------------------------------- //
// Includes
// ------------------------------------------------------------------------------------------------------- //

// PID defines and macros
#include "PID_TivaC.hpp"

// Standard libraries
#include <stdint.h>

// ------------------------------------------------------------------------------------------------------- //
// Functions definitions
// ------------------------------------------------------------------------------------------------------- //

// Name:        Pid
// Description: Constructor of the class with no arguments
// Arguments:   None
// Returns:     None

Pid::Pid()
{
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        Pid
// Description: Constructor of the class with gains, reference and limits
// Arguments:   Kp - Proportional gain
//              Ki - Integral gain
//              Kd - Derivative gain
//              Ref - Reference value
//              Ut_min - Minimum output value
//              Ut_max - Maximum output value
// Returns:     None

Pid::Pid(const float Kp, const float Ki, const float Kd, const float Ref, const float Ut_min, const float Ut_max)
{    
    SetGains(Kp, Ki, Kd);
    SetReference(Ref);
    SetLimits (Ut_min, Ut_max);
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        SetGains
// Description: Sets the controller gains
// Arguments:   Kp - Proportional gain
//              Ki - Integral gain
//              Kd - Derivative gain
// Returns:     None

void Pid::SetGains(const float Kp, const float Ki, const float Kd)
{
    _Data.Kp = Kp;
    _Data.Ki = Ki;
    _Data.Kd = Kd;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        GetGains
// Description: Gets the controller gains
// Arguments:   Buffer - Buffer to receive the values
// Returns:     None

void Pid::GetGains(float *Buffer)
{
    Buffer[0] = _Data.Kp;
    Buffer[1] = _Data.Ki;
    Buffer[2] = _Data.Kd;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        SetReference
// Description: Sets the reference
// Arguments:   NewReference - The reference value
// Returns:     None

void Pid::SetReference(float NewReference)
{
    _Data.Ref = NewReference;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        GetReference
// Description: Gets the reference 
// Arguments:   None
// Returns:     The reference value

float Pid::GetReference()
{
    return _Data.Ref;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        SetLimits
// Description: Sets the controller output limits
// Arguments:   Ut_min - Minimum output value
//              Ut_max - Maximum output value
// Returns:     None

void Pid::SetLimits(const float Ut_min, const float Ut_max)
{
    _Data.Ut_min = Ut_min;
    _Data.Ut_max = Ut_max;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        GetLimits
// Description: Gets the controller output limits
// Arguments:   Buffer - Buffer to receive the values
// Returns:     None

void Pid::GetLimits(float *Buffer)
{
    Buffer[0] = _Data.Ut_min;
    Buffer[1] = _Data.Ut_max;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        Compute
// Description: Computes the control action
// Arguments:   Y - Current system output
// Returns:     The new control action

float Pid::Compute(float Y)
{
    // Error - Sample k
    _Data.E_now = _Data.Ref - Y;

    // Integral error - Anti windup
    if (_Data.Saturated == false)
        _Data.E_int += _Data.E_now;

    // Derivative error - Derivative on measurement
    _Data.E_der = _Data.Y_lst - Y;

    // Rotate buffer
    _Data.Y_lst = Y;

    // Proportional portion - Sample k + 1
    _Data.Up_nxt = _Data.E_now * _Data.Kp;

    // Integral portion - Sample k + 1
    _Data.Ui_nxt = _Data.E_int * _Data.Ki;

    // Derivative portion - Sample k + 1
    _Data.Ud_nxt = _Data.E_der * _Data.Kd;

    // Total control action - Sample k + 1
    _Data.Ut_nxt = (_Data.Up_nxt + _Data.Ui_nxt + _Data.Ud_nxt);

    // Limiter - Maximum output exceeded
    if (_Data.Ut_nxt >= _Data.Ut_max)
    {
        _Data.Ut_nxt = _Data.Ut_max;
        _Data.Saturated = true;
    }

    // Limiter - Minimum output exceeded
    else if (_Data.Ut_nxt <= _Data.Ut_min)
    {
        _Data.Ut_nxt = _Data.Ut_min;
        _Data.Saturated = true;
    }

    // Limiter - Between limits
    else
        _Data.Saturated = false;  

    // Return calculated value
    return _Data.Ut_nxt;  
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        Reset
// Description: Resets the controller to its initial state
// Arguments:   None
// Returns:     None

void Pid::Reset()
{
    _Data = pid_t_default;
}

// ------------------------------------------------------------------------------------------------------- //

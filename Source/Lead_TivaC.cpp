// ------------------------------------------------------------------------------------------------------- //

// Lead controller library
// Version: 1.0
// Author: Renan Duarte
// E-mail: duarte.renan@hotmail.com
// Date:   23/07/2024

// ------------------------------------------------------------------------------------------------------- //

// ------------------------------------------------------------------------------------------------------- //
// Includes
// ------------------------------------------------------------------------------------------------------- //

// Lead defines and macros
#include "Lead_TivaC.hpp"

// Standard libraries
#include <stdint.h>

// ------------------------------------------------------------------------------------------------------- //
// Functions definitions
// ------------------------------------------------------------------------------------------------------- //

// Name:        Lead
// Description: Constructor of the class with no arguments
// Arguments:   None
// Returns:     None

Lead::Lead()
{
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        Lead
// Description: Constructor of the class with gains, reference and limits
// Arguments:   A - A gain
//              B - B gain
//              C - C gain
//              Ref - Reference value
//              Ut_min - Minimum output value
//              Ut_max - Maximum output value
// Returns:     None

Lead::Lead(const float A, const float B, const float C, const float Ref, const float Ut_min, const float Ut_max)
{    
    SetGains(A, B, C);
    SetReference(Ref);
    SetLimits (Ut_min, Ut_max);
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        SetGains
// Description: Sets the controller gains
// Arguments:   A - A gain
//              B - B gain
//              C - C gain
// Returns:     None

void Lead::SetGains(const float A, const float B, const float C)
{
    _Data.A = A;
    _Data.B = B;
    _Data.C = C;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        GetGains
// Description: Gets the controller gains
// Arguments:   Buffer - Buffer to receive the values
// Returns:     None

void Lead::GetGains(float *Buffer)
{
    Buffer[0] = _Data.A;
    Buffer[1] = _Data.B;
    Buffer[2] = _Data.C;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        SetReference
// Description: Sets the reference
// Arguments:   NewReference - The reference value
// Returns:     None

void Lead::SetReference(float NewReference)
{
    _Data.Ref = NewReference;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        GetReference
// Description: Gets the reference 
// Arguments:   None
// Returns:     The reference value

float Lead::GetReference()
{
    return _Data.Ref;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        SetLimits
// Description: Sets the controller output limits
// Arguments:   Ut_min - Minimum output value
//              Ut_max - Maximum output value
// Returns:     None

void Lead::SetLimits(const float Ut_min, const float Ut_max)
{
    _Data.Ut_min = Ut_min;
    _Data.Ut_max = Ut_max;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        GetLimits
// Description: Gets the controller output limits
// Arguments:   Buffer - Buffer to receive the values
// Returns:     None

void Lead::GetLimits(float *Buffer)
{
    Buffer[0] = _Data.Ut_min;
    Buffer[1] = _Data.Ut_max;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        Compute
// Description: Computes the control action
// Arguments:   Y - Current system output
// Returns:     The new control action

float Lead::Compute(float Y)
{
    // Error - Sample k
    _Data.E_now = _Data.Ref - Y;

    // Total control action - Sample k + 1
    _Data.Ut_nxt = _Data.A*_Data.Ut_now + _Data.B*_Data.E_now + _Data.C*_Data.E_lst;

    // Rotate buffers
    _Data.Ut_now = _Data.Ut_nxt;
    _Data.E_lst = _Data.E_now;

    // Limiter - Maximum output exceeded
    if (_Data.Ut_nxt >= _Data.Ut_max)
    {
        _Data.Ut_nxt = _Data.Ut_max;
    }

    // Limiter - Minimum output exceeded
    else if (_Data.Ut_nxt <= _Data.Ut_min)
    {
        _Data.Ut_nxt = _Data.Ut_min;
    }

    // Return calculated value
    return _Data.Ut_nxt;  
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        Reset
// Description: Resets the controller to its initial state
// Arguments:   None
// Returns:     None

void Lead::Reset()
{
    _Data = lead_t_default;
}

// ------------------------------------------------------------------------------------------------------- //

// ------------------------------------------------------------------------------------------------------- //

// LQR controller library
// Version: 1.0
// Author: Renan Duarte
// E-mail: duarte.renan@hotmail.com
// Date:   31/05/2024

// ------------------------------------------------------------------------------------------------------- //

// ------------------------------------------------------------------------------------------------------- //
// Includes
// ------------------------------------------------------------------------------------------------------- //

// LQR defines and macros
#include "Lqr_TivaC.hpp"

// Standard libraries
#include <stdint.h>

// ------------------------------------------------------------------------------------------------------- //
// Functions definitions
// ------------------------------------------------------------------------------------------------------- //

// Name:        Lqr
// Description: Constructor of the class with no arguments
// Arguments:   None
// Returns:     None

Lqr::Lqr()
{
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        Lqr
// Description: Constructor of the class with gains, references and limit arguments
// Arguments:   Gains - The gain vector
//              Refs - The reference vector
//              Size - Number of states
//              Ut_min - Minimum output value
//              Ut_max - Maximum output value
// Returns:     None

Lqr::Lqr(const float* Gains, const float* Refs, uint8_t Size, const float Ut_min, const float Ut_max)
{
    Init(Gains, Refs, Size, Ut_min, Ut_max);
}


// ------------------------------------------------------------------------------------------------------- //

// Name:        Init
// Description: Initialises the LQR with gains, references and limit arguments
// Arguments:   Gains - The gain vector
//              Refs - The reference vector
//              Size - Number of states
//              Ut_min - Minimum output value
//              Ut_max - Maximum output value
// Returns:     None

void Lqr::Init(const float* Gains, const float* Refs, uint8_t Size, const float Ut_min, const float Ut_max)
{
    if (Size <= MAX_LQR_STATES)
    {
        _StateCount = Size;

        for (uint8_t Idx = 0; Idx < _StateCount; Idx++)
        {
            SetGain(Idx, Gains[Idx]);
            SetReference(Idx, Refs[Idx]);
        }
    }

    SetLimits(Ut_min, Ut_max);
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        SetGain
// Description: Sets the gain associated to one state
// Arguments:   StateIndex - Index of the state associated with the new value
//              NewGain - The gain value
// Returns:     None

void Lqr::SetGain(uint8_t StateIndex, float NewGain)
{
    if (StateIndex < _StateCount)
        _Data.K[StateIndex] = NewGain;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        SetReference
// Description: Sets the reference associated to one state
// Arguments:   StateIndex - Index of the state associated with the new value
//              NewReference - The reference value
// Returns:     None

void Lqr::SetReference(uint8_t StateIndex, float NewReference)
{
    if (StateIndex < _StateCount)
        _Data.Ref[StateIndex] = NewReference;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        GetReference
// Description: Gets the reference associated to one state
// Arguments:   StateIndex - Index of the state associated with the new value
// Returns:     The reference value

float Lqr::GetReference(uint8_t StateIndex)
{
    if (StateIndex < _StateCount)
        return _Data.Ref[StateIndex];

    return 0;
}


// ------------------------------------------------------------------------------------------------------- //

// Name:        SetState
// Description: Sets the value of one state
// Arguments:   StateIndex - Index of the state
//              NewState - The state value
// Returns:     None

void Lqr::SetState(uint8_t StateIndex, float NewState)
{
    if (StateIndex < _StateCount)
        _Data.State[StateIndex] = NewState;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        GetState
// Description: Gets the value of one state
// Arguments:   StateIndex - Index of the state
// Returns:     The state value

float Lqr::GetState(uint8_t StateIndex)
{
    if (StateIndex < _StateCount)
        return _Data.State[StateIndex];

    return 0;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        SetLimits
// Description: Sets the controller output limits
// Arguments:   Ut_min - Minimum output value
//              Ut_max - Maximum output value
// Returns:     None

void Lqr::SetLimits(const float Ut_min, const float Ut_max)
{
    _Data.Ut_min = Ut_min;
    _Data.Ut_max = Ut_max;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        GetLimits
// Description: Gets the controller output limits
// Arguments:   Buffer - Buffer to receive the values
// Returns:     None

void Lqr::GetLimits(float *Buffer)
{
    Buffer[0] = _Data.Ut_min;
    Buffer[1] = _Data.Ut_max;
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        Compute
// Description: Computes the control action
// Arguments:   None
// Returns:     The new control action

float Lqr::Compute()
{
    // Reset control action value value
    _Data.Ut_nxt = 0;

    // Calculate state error and control action
    for (uint8_t Idx = 0; Idx < _StateCount; Idx++)
    {
        _Data.E[Idx] = _Data.Ref[Idx] - _Data.State[Idx];
        _Data.Ut_nxt += _Data.K[Idx] * _Data.E[Idx];
    }

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

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
// Description: Constructor of the class with gains and references arguments
// Arguments:   Gains - The gain vector
//              Refs - The reference vector
//              Size - Number of states
// Returns:     None

Lqr::Lqr(const float* Gains, const float* Refs, uint8_t Size) : Lqr()
{
    Init(Gains, Refs, Size);
}

// ------------------------------------------------------------------------------------------------------- //

// Name:        Init
// Description: Initializes the LQR with the gain and reference vectors
// Arguments:   Gains - The gain vector
//              Refs - The reference vector
//              Size - Number of states
// Returns:     None

void Lqr::Init(const float* Gains, const float* Refs, uint8_t Size)
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

// Name:        Compute
// Description: Sets the reference associated to one state
// Arguments:   StateIndex - Index of the state associated with the new value
//              NewReference - The reference value
// Returns:     None

float Lqr::Compute()
{
    _Data.U = 0;

    // Calculate state error and control action
    for (uint8_t Idx = 0; Idx < _StateCount; Idx++)
    {
        _Data.E[Idx] = _Data.Ref[Idx] - _Data.State[Idx];
        _Data.U += _Data.K[Idx] * _Data.E[Idx];
    }

    return _Data.U;
}

// ------------------------------------------------------------------------------------------------------- //

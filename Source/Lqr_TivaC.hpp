// ------------------------------------------------------------------------------------------------------- //

// RGB LED library
// Version: 1.0
// Author: Renan Duarte
// E-mail: duarte.renan@hotmail.com
// Date:   31/05/2024

// Overview:

//      This library provides functionality for controlling RGB LEDs on a Tiva C microcontroller. It
//      allows the user to set the color of the RGB LED with or without fade transitions.
//      The library supports a maximum of MAX_RGB_LEDS instances.
//      The fade service runs at the same frequency as the LED PWM, so maximum fade time is 1/PwmFrequency

// ------------------------------------------------------------------------------------------------------- //

#ifndef LQR_TIVAC_H_
#define LQR_TIVAC_H_

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

#define MAX_LQR_STATES 10               // Define the maximum number of states

// ------------------------------------------------------------------------------------------------------- //
// Structs
// ------------------------------------------------------------------------------------------------------- //

// LQR controller variables
typedef struct
{
    float K[MAX_LQR_STATES];            // Gain matrix
    float Ref[MAX_LQR_STATES];          // Setpoint
    float State[MAX_LQR_STATES];        // State values (sample k)
    float E[MAX_LQR_STATES];            // Error (sample k)
    float U;                            // Control action (sample k + 1)
} lqr_t;

// LQR controller variables - Default values
#define lqr_t_default { \
    .K = {0}, \
    .Ref = {0}, \
    .State = {0}, \
    .E = {0}, \
    .U = 0, \
}

// ------------------------------------------------------------------------------------------------------- //
// Class prototype
// ------------------------------------------------------------------------------------------------------- //

class Lqr
{
    // --------------------------------------------------------------------------------------------------- //
    // Private members
    // --------------------------------------------------------------------------------------------------- //

    private:
        
        // LQR data
        lqr_t _Data = lqr_t_default;

        // Number of states
        uint8_t _StateCount = 0;

    // --------------------------------------------------------------------------------------------------- //
    // Public members
    // --------------------------------------------------------------------------------------------------- //

    public:

        // Name:        Lqr
        // Description: Constructor of the class with no arguments
        // Arguments:   None
        // Returns:     None
        Lqr();

        // Name:        Lqr
        // Description: Constructor of the class with gains and references arguments
        // Arguments:   Gains - The gain vector
        //              Refs - The reference vector
        //              Size - Number of states
        // Returns:     None
        Lqr(const float* Gains, const float* Refs, uint8_t Size);

        // Name:        Init
        // Description: Initializes the LQR with the gain and reference vectors
        // Arguments:   Gains - The gain vector
        //              Refs - The reference vector
        //              Size - Number of states
        // Returns:     None
        void Init(const float* Gains, const float* Refs, uint8_t Size);

        // Name:        SetGain
        // Description: Sets the gain associated to one state
        // Arguments:   StateIndex - Index of the state associated with the new value
        //              NewGain - The gain value
        // Returns:     None
        void SetGain(uint8_t StateIndex, float NewGain);

        // Name:        SetReference
        // Description: Sets the reference associated to one state
        // Arguments:   StateIndex - Index of the state associated with the new value
        //              NewReference - The reference value
        // Returns:     None
        void SetReference(uint8_t StateIndex, float NewReference);

        // Name:        GetReference
        // Description: Gets the reference associated to one state
        // Arguments:   StateIndex - Index of the state associated with the new value
        // Returns:     The reference value
        float GetReference(uint8_t StateIndex);

        // Name:        SetState
        // Description: Sets the value of one state
        // Arguments:   StateIndex - Index of the state
        //              NewState - The state value
        // Returns:     None
        void SetState(uint8_t StateIndex, float NewState);

        // Name:        GetState
        // Description: Gets the value of one state
        // Arguments:   StateIndex - Index of the state
        // Returns:     The state value
        float GetState(uint8_t StateIndex);

        // Name:        Compute
        // Description: Sets the reference associated to one state
        // Arguments:   StateIndex - Index of the state associated with the new value
        //              NewReference - The reference value
        // Returns:     None
        float Compute();
};

// ------------------------------------------------------------------------------------------------------- //

#ifdef __cplusplus
}
#endif

#endif

// ------------------------------------------------------------------------------------------------------- //

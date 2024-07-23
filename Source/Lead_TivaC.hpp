// ------------------------------------------------------------------------------------------------------- //

// Lead controller library
// Version: 1.0
// Author: Renan Duarte
// E-mail: duarte.renan@hotmail.com
// Date:   23/07/2024

// ------------------------------------------------------------------------------------------------------- //

#ifndef LEAD_TIVAC_H_
#define LEAD_TIVAC_H_

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

// ------------------------------------------------------------------------------------------------------- //
// Structs
// ------------------------------------------------------------------------------------------------------- //

// Lead controller variables
typedef struct
{
    float Ref;                  // Setpoint
    float E_now;                // Error (sample k)
    float E_lst;                // Error (sample k - 1)
    float Ut_nxt;               // Control action - Total (sample k + 1)
    float Ut_now;               // Control action - Total (sample k)
    float A;                    // Gain A
    float B;                    // Gain B
    float C;                    // Gain C
    float Ut_min;               // Minimum output value
    float Ut_max;               // Maximum output value
} lead_t;

// Lead controller variables - Default values
#define lead_t_default { \
    .Ref = 0, \
    .E_now = 0, \
    .E_lst = 0, \
    .Ut_nxt = 0, \
    .Ut_now = 0, \
    .A = 0, \
    .B = 0, \
    .C = 0, \
    .Ut_min = 0, \
    .Ut_max = 0, \
}

// ------------------------------------------------------------------------------------------------------- //
// Class prototype
// ------------------------------------------------------------------------------------------------------- //

class Lead
{
    // --------------------------------------------------------------------------------------------------- //
    // Private members
    // --------------------------------------------------------------------------------------------------- //

    private:
        
        // Lead data
        lead_t _Data = lead_t_default;

    // --------------------------------------------------------------------------------------------------- //
    // Public members
    // --------------------------------------------------------------------------------------------------- //

    public:

        // Name:        Lead
        // Description: Constructor of the class with no arguments
        // Arguments:   None
        // Returns:     None
        Lead();

        // Name:        Lead
        // Description: Constructor of the class with gains, reference and limits
        // Arguments:   A - A gain
        //              B - B gain
        //              C - C gain
        //              Ref - Reference value
        //              Ut_min - Minimum output value
        //              Ut_max - Maximum output value
        // Returns:     None
        Lead(const float A, const float B, const float C, const float Ref, const float Ut_min, const float Ut_max);

        // Name:        SetGains
        // Description: Sets the controller gains
        // Arguments:   A - A gain
        //              B - B gain
        //              C - C gain
        // Returns:     None
        void SetGains(const float A, const float B, const float C);

        // Name:        GetGains
        // Description: Gets the controller gains
        // Arguments:   Buffer - Buffer to receive the values
        // Returns:     None
        void GetGains(float *Buffer);

        // Name:        SetReference
        // Description: Sets the reference
        // Arguments:   NewReference - The reference value
        // Returns:     None
        void SetReference(float NewReference);

        // Name:        GetReference
        // Description: Gets the reference 
        // Arguments:   None
        // Returns:     The reference value
        float GetReference();

        // Name:        SetLimits
        // Description: Sets the controller output limits
        // Arguments:   Ut_min - Minimum output value
        //              Ut_max - Maximum output value
        // Returns:     None
        void SetLimits(const float Ut_min, const float Ut_max);

        // Name:        GetLimits
        // Description: Gets the controller output limits
        // Arguments:   Buffer - Buffer to receive the values
        // Returns:     None
        void GetLimits(float *Buffer);

        // Name:        Compute
        // Description: Computes the control action
        // Arguments:   Y - Current system output
        // Returns:     The new control action
        float Compute(float Y);

        // Name:        Reset
        // Description: Resets the controller to its initial state
        // Arguments:   None
        // Returns:     None
        void Reset();
};

// ------------------------------------------------------------------------------------------------------- //

#ifdef __cplusplus
}
#endif

#endif

// ------------------------------------------------------------------------------------------------------- //

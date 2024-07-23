// ------------------------------------------------------------------------------------------------------- //

// PID controller library
// Version: 1.0
// Author: Renan Duarte
// E-mail: duarte.renan@hotmail.com
// Date:   31/05/2024

// ------------------------------------------------------------------------------------------------------- //

#ifndef PID_TIVAC_H_
#define PID_TIVAC_H_

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

// PID controller variables
typedef struct
{
    float Ref;                  // Setpoint
    float E_now;                // Error (sample k)
    float Y_lst;                // Output (sample k - 1) -> Derivative on measurement
    float E_int;                // Error (integral)
    float E_der;                // Error (derivative) -> Derivative on measurement
    float Up_nxt;               // Control action - Proportional portion (sample k + 1)
    float Ui_nxt;               // Control action - Integral portion (sample k + 1)
    float Ud_nxt;               // Control action - Derivative portion (sample k + 1)
    float Ut_nxt;               // Control action - Total (sample k + 1)
    bool Saturated;             // Saturation flag
    float Kp;                   // Proportional gain
    float Ki;                   // Integral gain
    float Kd;                   // Derivative gain
    float Ut_min;               // Minimum output value
    float Ut_max;               // Maximum output value
} pid_t;

// PID controller variables - Default values
#define pid_t_default { \
    .Ref = 0, \
    .E_now = 0, \
    .Y_lst = 0, \
    .E_int = 0, \
    .E_der = 0, \
    .Ui_nxt = 0, \
    .Up_nxt = 0, \
    .Ud_nxt = 0, \
    .Ut_nxt = 0, \
    .Saturated = false, \
    .Kp = 0, \
    .Ki = 0, \
    .Kd = 0, \
    .Ut_min = 0, \
    .Ut_max = 0, \
}

// ------------------------------------------------------------------------------------------------------- //
// Class prototype
// ------------------------------------------------------------------------------------------------------- //

class Pid
{
    // --------------------------------------------------------------------------------------------------- //
    // Private members
    // --------------------------------------------------------------------------------------------------- //

    private:
        
        // PID data
        pid_t _Data = pid_t_default;

    // --------------------------------------------------------------------------------------------------- //
    // Public members
    // --------------------------------------------------------------------------------------------------- //

    public:

        // Name:        Pid
        // Description: Constructor of the class with no arguments
        // Arguments:   None
        // Returns:     None
        Pid();

        // Name:        Pid
        // Description: Constructor of the class with gains, reference and limits
        // Arguments:   Kp - Proportional gain
        //              Ki - Integral gain
        //              Kd - Derivative gain
        //              Ref - Reference value
        //              Ut_min - Minimum output value
        //              Ut_max - Maximum output value
        // Returns:     None
        Pid(const float Kp, const float Ki, const float Kd, const float Ref, const float Ut_min, const float Ut_max);

        // Name:        SetGains
        // Description: Sets the controller gains
        // Arguments:   Kp - Proportional gain
        //              Ki - Integral gain
        //              Kd - Derivative gain
        // Returns:     None
        void SetGains(const float Kp, const float Ki, const float Kd);

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

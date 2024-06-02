// ------------------------------------------------------------------------------------------------------- //

// Auxiliary functions
// Version: 1.0
// Author: Renan Duarte
// E-mail: duarte.renan@hotmail.com
// Date:   14/05/2024

// ------------------------------------------------------------------------------------------------------- //

#ifndef AUX_FUNCTIONS_H_
#define AUX_FUNCTIONS_H_

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
// Class prototype
// ------------------------------------------------------------------------------------------------------- //

class Aux
{
    // --------------------------------------------------------------------------------------------------- //
    // Public members
    // --------------------------------------------------------------------------------------------------- //

    public:

        // Name:        Map
        // Description: Maps a value from one range to another
        // Arguments:   Value - Value to be mapped
        //              InMin - Minimum value of the input range
        //              InMax - Maximum value of the input range
        //              OutMin - Minimum value of the output range
        //              OutMax - Maximum value of the input range
        // Returns:     Scaled value
        static float Map(float Value, float InMin, float InMax, float OutMin, float OutMax);

        // Name:        Min
        // Description: Returns the smaller of two floating-point numbers
        // Arguments:   a - The first floating-point number
        //              b - The second floating-point number
        // Returns:     The smaller of the two floating-point numbers
        static float Min(float a, float b);

        // Name:        Max
        // Description: Returns the larger of two floating-point numbers
        // Arguments:   a - The first floating-point number
        //              b - The second floating-point number
        // Returns:     The larger of the two floating-point numbers
        static float Max(float a, float b);

        // Name:        Strcpy
        // Description: Copy one string to another
        // Arguments:   Destination - Destination string
        //              Source - Source string
        // Returns:     Pointer to the beginning of the destination string
        static char* Strcpy(char* Destination, const char* Source);

        // Name:        StrReverse
        // Description: Reverse all characters in the string between the given pointers
        // Arguments:   Begin - Pointer to the beginning of the string
        //              End - Pointer to the end of the string
        // Returns:     None
        static void StrReverse(char* Begin, char* End);

        // Name:        L2Str
        // Description: Converts a given integer to a string representation
        //              If the integer is negative, the string includes a negative sign
        // Arguments:   Number - The integer to be converted (int32_t format)
        //              String - Pointer to the output buffer where the string representation will be stored
        // Returns:     The number of characters written to the string buffer
        static uint8_t L2Str(int32_t Number, char* String);

        // Name:        F2Str
        // Description: Converts a float number to a string with a specified number of decimal places
        //              Number will be rounded at the last specified decimal place
        // Arguments:   Number - The float number to be converted
        //              String - Pointer to the output buffer where the string representation will be stored
        //              DecPlaces - The number of decimal places in the resulting string
        // Returns:     The number of characters written to the string buffer
        static uint8_t F2Str(float Number, char* String, uint8_t DecPlaces);

        // Name:        FastFabs
        // Description: Computes the absolute value of a float number using a ternary operator for fast execution
        // Arguments:   x - The float number for which the absolute value is to be computed
        // Returns:     The absolute value of the input number
        static float FastFabs(float x);

        // Name:        Mean
        // Description: Computes the mean (average) of an array of unsigned 32-bit integers
        // Arguments:   Array - The array of unsigned 32-bit integers
        //              Size  - The number of elements in the array
        // Returns:     The mean (average) value of the array elements as a float
        static float Mean(uint32_t Array[], int Size);

        // Name:        LinearInterpolation
        // Description: Calculates the slope and offset of a linear interpolation  for a given set of X-Y data
        //              using the least squares method
        // Arguments:   ArrayX - Array of unsigned 32-bit integers representing the X points
        //              ArrayY - Array of unsigned 32-bit integers representing the Y points
        //              Size - The number of elements in the arrays
        //              Slope - Pointer to a float where the calculated slope will be stored
        //              Offset  - Pointer to a float where the calculated offset will be stored
        // Returns:     None (the results are stored in the provided Slope and Offset pointers)
        static void LinearInterpolation(uint32_t ArrayX[], uint32_t ArrayY[], uint8_t Size, float *Slope, float *Offset);
};

// ------------------------------------------------------------------------------------------------------- //

#ifdef __cplusplus
}
#endif

#endif

// ------------------------------------------------------------------------------------------------------- //

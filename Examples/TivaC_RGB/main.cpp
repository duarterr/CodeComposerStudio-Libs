/* -------------------------------------------------------------------------------------------------------------------- */
// Tiva C Base project
/* -------------------------------------------------------------------------------------------------------------------- */
/*                                                                                                                      */
/* Author:      Renan R. Duarte                                                                                         */
/* Date:        13/05/2024                                                                                              */
/* Device:      CORTEX M4F TM4C123GH6PM Tiva C Launchpad                                                                */
/*                                                                                                                      */
/* -------------------------------------------------------------------------------------------------------------------- */
/*                                                                                                                      */
/* Predefined Symbols:                                                                                                  */
/*                                                                                                                      */
/*              PART_TM4C123GH6PM - To use pin_map.h                                                                    */
/*              TARGET_IS_TM4C129_RA0 - To use the include files in ROM of the Tiva device                              */
/*                                                                                                                      */
/* -------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------- */
/* Includes                                                                                                             */
/* -------------------------------------------------------------------------------------------------------------------- */

#include <stdint.h>
#include <stdbool.h>
#include "stdio.h"
#include "stdlib.h"

// TivaC hardware related libraries
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_timer.h"

// TivaC driver libraries
#include "driverlib/adc.h"
#include "driverlib/aes.h"
#include "driverlib/can.h"
#include "driverlib/comp.h"
#include "driverlib/crc.h"
#include "driverlib/des.h"
#include "driverlib/eeprom.h"
#include "driverlib/emac.h"
#include "driverlib/epi.h"
#include "driverlib/flash.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/hibernate.h"
#include "driverlib/i2c.h"
#include "driverlib/interrupt.h"
#include "driverlib/lcd.h"
#include "driverlib/mpu.h"
#include "driverlib/onewire.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/qei.h"
#include "driverlib/rom.h"
#include "driverlib/shamd5.h"
#include "driverlib/ssi.h"
#include "driverlib/sw_crc.h"
#include "driverlib/sysctl.h"
#include "driverlib/sysexc.h"
#include "driverlib/systick.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/udma.h"
#include "driverlib/usb.h"
#include "driverlib/watchdog.h"

// RGB LED functions
#include "Rgb_TivaC.hpp"

/* -------------------------------------------------------------------------------------------------------------------- */
/* Definitions                                                                                                          */
/* -------------------------------------------------------------------------------------------------------------------- */

#define RGB_PWM_PERIPH SYSCTL_PERIPH_PWM1;
#define RGB_GPIO_PERIPH SYSCTL_PERIPH_GPIOF;
#define RGB_PWM_BASE PWM1_BASE;
#define RGB_GPIO_BASE GPIO_PORTF_BASE;
#define RGB_PWM_R_GEN    PWM_GEN_2
#define RGB_PWM_G_GEN    PWM_GEN_3
#define RGB_PWM_B_GEN    PWM_GEN_3
#define RGB_PWM_R_OUT    PWM_OUT_5
#define RGB_PWM_G_OUT    PWM_OUT_7
#define RGB_PWM_B_OUT    PWM_OUT_6
#define RGB_PWM_R_OUT_BIT    PWM_OUT_5_BIT
#define RGB_PWM_G_OUT_BIT    PWM_OUT_7_BIT
#define RGB_PWM_B_OUT_BIT    PWM_OUT_6_BIT
#define RGB_PIN_R_CFG    GPIO_PF1_M1PWM5
#define RGB_PIN_G_CFG     GPIO_PF3_M1PWM7
#define RGB_PIN_B_CFG     GPIO_PF2_M1PWM6
#define RGB_R_PIN     GPIO_PIN_1
#define RGB_G_PIN    GPIO_PIN_3
#define RGB_B_PIN    GPIO_PIN_2
#define RGB_PWM_INT    INT_PWM1_2
#define RGB_PWM_INT_GEN    PWM_INT_GEN_2
#define RGB_PWM_CFG    PWM_GEN_MODE_DOWN// | PWM_GEN_MODE_DBG_RUN
#define RGB_PWM_FREQ    1000


/* -------------------------------------------------------------------------------------------------------------------- */
/* Variables                                                                                                            */
/* -------------------------------------------------------------------------------------------------------------------- */

// RGB LED object - From Rgb_TivaC class
Rgb Led;

// CPU clock - Hz
uint32_t cpuClock = 0;

/* -------------------------------------------------------------------------------------------------------------------- */
/* Main function                                                                                                        */
/* -------------------------------------------------------------------------------------------------------------------- */

void main (void)
{
    /* ---------------------------------------------------------------------------------------------------------------- */
    // CPU settings
    /* ---------------------------------------------------------------------------------------------------------------- */

    // Configure clock - 80 MHz - 200 MHz (PLL) / 2.5
    SysCtlClockSet (SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    // Get CPU clock
    cpuClock = SysCtlClockGet();

    // Master interrupt enable API for all interrupts
    IntMasterEnable();

    /* ---------------------------------------------------------------------------------------------------------------- */
    // RGB LED configuration
    /* ---------------------------------------------------------------------------------------------------------------- */

    // Define RGB LED configuration parameters
    rgb_config_t Led_Config;
    Led_Config.Periph.Pwm = RGB_PWM_PERIPH;
    Led_Config.Periph.Gpio = RGB_GPIO_PERIPH;
    Led_Config.Base.Pwm = RGB_PWM_BASE;
    Led_Config.Base.Gpio = RGB_GPIO_BASE;
    Led_Config.Gen.R = RGB_PWM_R_GEN;
    Led_Config.Gen.G = RGB_PWM_G_GEN;
    Led_Config.Gen.B = RGB_PWM_B_GEN;
    Led_Config.Out.R = RGB_PWM_R_OUT;
    Led_Config.Out.G = RGB_PWM_G_OUT;
    Led_Config.Out.B = RGB_PWM_B_OUT;
    Led_Config.OutBit.R = RGB_PWM_R_OUT_BIT;
    Led_Config.OutBit.G = RGB_PWM_G_OUT_BIT;
    Led_Config.OutBit.B = RGB_PWM_B_OUT_BIT;
    Led_Config.PinMux.R = RGB_PIN_R_CFG;
    Led_Config.PinMux.G = RGB_PIN_G_CFG;
    Led_Config.PinMux.B = RGB_PIN_B_CFG;
    Led_Config.Pin.R = RGB_R_PIN;
    Led_Config.Pin.G = RGB_G_PIN;
    Led_Config.Pin.B = RGB_B_PIN;
    Led_Config.Int.Interrupt = RGB_PWM_INT;
    Led_Config.Int.Gen = RGB_PWM_INT_GEN;
    Led_Config.Int.Callback = [](){Led.PwmIsr();};
    Led_Config.Params.PwmMode = RGB_PWM_CFG;
    Led_Config.Params.PwmFrequency = RGB_PWM_FREQ;

    // Initialize RGB LED
    Led.Init (&Led_Config);

    /* ---------------------------------------------------------------------------------------------------------------- */
    // Main loop
    /* ---------------------------------------------------------------------------------------------------------------- */

    while (1)
    {
        rgb_color_t NewColor = {.R = 255, .G = 0, .B = 0};
        Led.SetColor (NewColor, 1000);
        SysCtlDelay(20000000);

        NewColor = {.R = 0, .G = 255, .B = 0};
        Led.SetColor (NewColor, 1000);
        SysCtlDelay(20000000);

        NewColor = {.R = 0, .G = 0, .B = 255};
        Led.SetColor (NewColor, 1000);
        SysCtlDelay(20000000);
    }
}

/* -------------------------------------------------------------------------------------------------------------------- */

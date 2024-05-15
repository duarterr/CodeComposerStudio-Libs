/* -------------------------------------------------------------------------------------------------------------------- */
// Tiva C LCD
/* -------------------------------------------------------------------------------------------------------------------- */
/*                                                                                                                      */
/* Author:      Renan R. Duarte                                                                                         */
/* Date:        07/07/2017                                                                                              */
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

#include <Button_TivaC.hpp>

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

/* -------------------------------------------------------------------------------------------------------------------- */
/* Definitions                                                                                                          */
/* -------------------------------------------------------------------------------------------------------------------- */

// Timer defines
#define TIMER_FREQUENCY             1000

// Button defines
#define BUTTON_GPIO_PERIPH          SYSCTL_PERIPH_GPIOF
#define BUTTON_GPIO_BASE            GPIO_PORTF_BASE
#define BUTTON_PIN_1                GPIO_PIN_4
#define BUTTON_PIN_2                GPIO_PIN_0
#define BUTTON_SCAN_INTERVAL        1000/TIMER_FREQUENCY
#define BUTTON_DEAD_TIME            10
#define BUTTON_WINDOW               250
#define BUTTON_LONG_PRESS_TIMEOUT   1000

/* -------------------------------------------------------------------------------------------------------------------- */
/* Variables                                                                                                            */
/* -------------------------------------------------------------------------------------------------------------------- */

// CPU clock - Hz
uint32_t cpuClock = 0;

// Buttons
Button Button1, Button2;

// Watch these variables
int8_t SingleClick = 0;
int8_t DoubleClick = 0;
int8_t SingleLClick = 0;
int8_t DoubleLClick = 0;
int8_t Tick = 0;

/* -------------------------------------------------------------------------------------------------------------------- */
// SysTick interrupt service routine
/* -------------------------------------------------------------------------------------------------------------------- */

void IsrSysTick (void)
{
    button_event_data_t Event_Data;
    bool EventFlag = Button1.ScanEvent (&Event_Data);

    if (EventFlag)
    {
        // Button short clicks
        if (Event_Data.EventCode == BUTTON_SHORT_CLICK)
        {
            // Single click
            if (Event_Data.Counter == 1)
                SingleClick++;

            // Double click
            else if (Event_Data.Counter == 2)
                DoubleClick++;
        }

        // Button long clicks
        else if (Event_Data.EventCode == BUTTON_LONG_CLICK)
        {
            // Simple long click
            if (Event_Data.Counter == 1)
                SingleLClick++;

            // Double long click
            else if (Event_Data.Counter == 2)
                DoubleLClick++;
        }

        // Button long click tick event
        else if (Event_Data.EventCode == BUTTON_LONG_CLICK_TICK)
            Tick++;
    }

    EventFlag = Button2.ScanEvent (&Event_Data);

    if (EventFlag)
    {
        // Button short clicks
        if (Event_Data.EventCode == BUTTON_SHORT_CLICK)
        {
            // Single click
            if (Event_Data.Counter == 1)
                SingleClick--;

            // Double click
            else if (Event_Data.Counter == 2)
                DoubleClick--;
        }

        // Button long clicks
        else if (Event_Data.EventCode == BUTTON_LONG_CLICK)
        {
            // Simple long click
            if (Event_Data.Counter == 1)
                SingleLClick--;

            // Double long click
            else if (Event_Data.Counter == 2)
                DoubleLClick--;
        }

        // Button long click tick event
        else if (Event_Data.EventCode == BUTTON_LONG_CLICK_TICK)
            Tick--;
    }
}

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
    // Configure buttons
    /* ---------------------------------------------------------------------------------------------------------------- */

    // Define button 1 parameters
    button_config_t Button_Config;
    Button_Config.Hardware.Periph = BUTTON_GPIO_PERIPH;
    Button_Config.Hardware.Base = BUTTON_GPIO_BASE;
    Button_Config.Hardware.Pin = BUTTON_PIN_1;
    Button_Config.Params.Interval = BUTTON_SCAN_INTERVAL;
    Button_Config.Params.DeadTime = BUTTON_DEAD_TIME;
    Button_Config.Params.Window = BUTTON_WINDOW;
    Button_Config.Params.LongClickTimeout = BUTTON_LONG_PRESS_TIMEOUT;

    // Initialize button 1
    Button1.Init(&Button_Config);

    // Define button 2 parameters
    Button_Config.Hardware.Pin = BUTTON_PIN_2;

    // Initialize button 2
    Button2.Init(&Button_Config);

    /* ---------------------------------------------------------------------------------------------------------------- */
    // Configure Sistick timer
    /* ---------------------------------------------------------------------------------------------------------------- */

    // Set timer period
    SysTickPeriodSet ((cpuClock/TIMER_FREQUENCY) - 1);

    // Register interrupt handler
    SysTickIntRegister (&IsrSysTick);

    // Enable timer interrupt
    SysTickIntEnable ();

    // Start timer
    SysTickEnable ();

    /* ---------------------------------------------------------------------------------------------------------------- */
    // Main loop
    /* ---------------------------------------------------------------------------------------------------------------- */

    while (1)
    {
    }
}

/* -------------------------------------------------------------------------------------------------------------------- */

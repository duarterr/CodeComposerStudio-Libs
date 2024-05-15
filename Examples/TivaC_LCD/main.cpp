/* -------------------------------------------------------------------------------------------------------------------- */
// Tiva C 5110 LCD
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
/*              TARGetIS_TM4C129_RA0 - To use the include files in ROM of the Tiva device                              */
/*                                                                                                                      */
/* -------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------- */
/* Includes                                                                                                             */
/* -------------------------------------------------------------------------------------------------------------------- */

// LCD Functions
#include <Lcd_TivaC.hpp>

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

// LCD
#define LCD_SSI_PERIPH                      SYSCTL_PERIPH_SSI0
#define LCD_SCLK_PERIPH                     SYSCTL_PERIPH_GPIOA
#define LCD_DN_PERIPH                       SYSCTL_PERIPH_GPIOA
#define LCD_SCE_PERIPH                      SYSCTL_PERIPH_GPIOA
#define LCD_DC_PERIPH                       SYSCTL_PERIPH_GPIOA
#define LCD_BKL_PERIPH                      SYSCTL_PERIPH_GPIOA
#define LCD_SSI_BASE                        SSI0_BASE
#define LCD_SCLK_BASE                       GPIO_PORTA_BASE
#define LCD_DN_BASE                         GPIO_PORTA_BASE
#define LCD_SCE_BASE                        GPIO_PORTA_BASE
#define LCD_DC_BASE                         GPIO_PORTA_BASE
#define LCD_BKL_BASE                        GPIO_PORTA_BASE
#define LCD_SCLK_CFG                        GPIO_PA2_SSI0CLK
#define LCD_DN_CFG                          GPIO_PA5_SSI0TX
#define LCD_SCLK_PIN                        GPIO_PIN_2
#define LCD_DN_PIN                          GPIO_PIN_5
#define LCD_SCE_PIN                         GPIO_PIN_4
#define LCD_DC_PIN                          GPIO_PIN_3
#define LCD_BKL_PIN                         GPIO_PIN_6

/* -------------------------------------------------------------------------------------------------------------------- */
/* Variables                                                                                                            */
/* -------------------------------------------------------------------------------------------------------------------- */

// Lcd object - From Lcd_TivaC class
Lcd Display;

// General purpose counter
unsigned char counter = 0;

// Bitmap logo
unsigned char blockGedre [504] =
{
    0x00, 0xC0, 0xF0, 0xF8, 0xFC, 0x7C, 0x3E, 0x1E, 0x0F, 0x0F, 0x0F, 0x0F, 0x1E, 0x1E, 0x3E, 0x7C,
    0xF8, 0xF0, 0xE0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF8, 0xFC, 0x7E, 0x1E,
    0x1E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF0, 0xF8,
    0x78, 0x78, 0x78, 0x78, 0x78, 0xF8, 0xF0, 0xF0, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFE, 0xFE, 0xFC, 0x00, 0xFF, 0xFF,
    0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xE0, 0xE0, 0xF0, 0xF8, 0x78, 0x78, 0x78, 0x78, 0x78,
    0x78, 0xF0, 0xF0, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x0F, 0x3F, 0x7F, 0xFE, 0xF8, 0xF0, 0xE0,
    0xE0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFE, 0xFF, 0xFF,
    0xE7, 0xC1, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC3, 0xEF, 0xFF, 0xFF, 0xFE,
    0x00, 0x80, 0x80, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xFF, 0xFF, 0xFF,
    0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFC, 0xFF, 0xFF, 0xFF, 0xC3, 0xC1, 0xC0, 0xC0,
    0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC1, 0xC7, 0xFF, 0xFF, 0xFF, 0xF8, 0x60, 0xF0, 0xF0, 0xE0,
    0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFF, 0xFF, 0xFF, 0xFF,
    0x1F, 0x3F, 0xFF, 0xFF, 0xFB, 0xC1, 0x81, 0x81, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0xE1, 0xF9, 0xFF, 0xFF, 0x1F, 0x07, 0x07, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x01, 0x03, 0x01, 0x00, 0x00, 0x1F, 0x7F, 0xFF, 0xFF,
    0xE1, 0xC1, 0x81, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
    0x00, 0x07, 0x0F, 0x3F, 0x3F, 0x7C, 0xF8, 0xF0, 0xF0, 0xE0, 0xE0, 0xF0, 0xF0, 0xF0, 0x78, 0x7C,
    0x3F, 0x1F, 0x0F, 0x03, 0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x07, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x03, 0x03, 0x07, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    0x0F, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0F, 0x1F, 0x3E, 0x3C,
    0x3C, 0x78, 0x78, 0x78, 0x78, 0x3C, 0x3C, 0x3E, 0x1F, 0x0F, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

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

    /* ---------------------------------------------------------------------------------------------------------------- */
    // LCD configuration
    /* ---------------------------------------------------------------------------------------------------------------- */

    // Define LCD configuration parameters
    lcd_config_t Lcd_Config;
    Lcd_Config.Periph.Ssi = LCD_SSI_PERIPH;
    Lcd_Config.Periph.Sclk = LCD_SCLK_PERIPH;
    Lcd_Config.Periph.Dn = LCD_DN_PERIPH;
    Lcd_Config.Periph.Sce = LCD_SCE_PERIPH;
    Lcd_Config.Periph.Dc = LCD_DC_PERIPH;
    Lcd_Config.Periph.Bkl = LCD_BKL_PERIPH;
    Lcd_Config.Base.Ssi = LCD_SSI_BASE;
    Lcd_Config.Base.Sclk = LCD_SCLK_BASE;
    Lcd_Config.Base.Dn = LCD_DN_BASE;
    Lcd_Config.Base.Sce = LCD_SCE_BASE;
    Lcd_Config.Base.Dc = LCD_DC_BASE;
    Lcd_Config.Base.Bkl = LCD_BKL_BASE;
    Lcd_Config.PinMux.Sclk = LCD_SCLK_CFG;
    Lcd_Config.PinMux.Dn = LCD_DN_CFG;
    Lcd_Config.Pin.Sclk = LCD_SCLK_PIN;
    Lcd_Config.Pin.Dn = LCD_DN_PIN;
    Lcd_Config.Pin.Sce = LCD_SCE_PIN;
    Lcd_Config.Pin.Dc = LCD_DC_PIN;
    Lcd_Config.Pin.Bkl = LCD_BKL_PIN;

    // Initialize LCD display
    Display.Init (&Lcd_Config);

    /* ---------------------------------------------------------------------------------------------------------------- */
    // Main loop
    /* ---------------------------------------------------------------------------------------------------------------- */

    while (1)
    {
        // Clear Display
        Display.ClearAll ();

        Display.DrawFilledRectangle(0,0,83,5, LCD_PIXEL_XOR);
        Display.Goto (0, 0);

        // Send String
        Display.WriteChar ('a', LCD_FONT_DEFAULT, LCD_PIXEL_ON);
        Display.WriteChar ('b', LCD_FONT_SMALL, LCD_PIXEL_OFF);

        // Next Line
        Display.DrawFilledRectangle(0,8,83,12, LCD_PIXEL_XOR);
        Display.Goto (1, 0);

        // Send String
        Display.WriteString ("str", LCD_FONT_SMALL, LCD_PIXEL_ON);
        Display.WriteString ("ing", LCD_FONT_DEFAULT, LCD_PIXEL_OFF);
        Display.WriteString ("SRTING", LCD_FONT_DEFAULT, LCD_PIXEL_XOR);

        Display.Goto (Display.GetBank ()+ 1, 0);
        Display.WriteInt (123456, LCD_FONT_DEFAULT, LCD_PIXEL_XOR);

        // Next Line
        Display.Goto (Display.GetBank ()+ 1, 0);

        Display.WriteFloat (1234.56, 2, LCD_FONT_SMALL, LCD_PIXEL_XOR);

        Display.DrawRectangle(0,0,83,47, LCD_PIXEL_ON);

        Display.Commit();

        // Delay
        SysCtlDelay (10000000);

        /* ------------------------------------------------------------------------------------------------------------ */

        // Clear Display
        Display.ClearAll ();

        Display.DrawFilledRectangle(0,0,83,5, LCD_PIXEL_XOR);
        Display.Goto (0, 0);

        Display.WriteIntBig (12, LCD_PIXEL_ON);
        Display.WriteIntBig (34, LCD_PIXEL_OFF);
        Display.WriteIntBig (56, LCD_PIXEL_XOR);

        Display.Commit();

        // Delay
        SysCtlDelay (10000000);

        // Clear Display
        Display.ClearAll ();

        Display.DrawFilledRectangle(0,0,83,5, LCD_PIXEL_XOR);
        Display.Goto (0, 0);

        Display.WriteFloatBig (12.34, 2, LCD_PIXEL_ON);

        Display.DrawFilledRectangle(0,25,83,30, LCD_PIXEL_XOR);
        Display.Goto (2, 0);
        Display.WriteFloatBig (12.34, 2, LCD_PIXEL_OFF);

        Display.DrawFilledRectangle(0,40,83,47, LCD_PIXEL_XOR);
        Display.Goto (4, 0);
        Display.WriteFloatBig (12.34, 2, LCD_PIXEL_XOR);

        Display.Commit();

        // Delay
        SysCtlDelay (10000000);

        /* ------------------------------------------------------------------------------------------------------------ */

        // Clear Display
        Display.ClearAll ();

        // Send Block
        Display.DrawBitmap (blockGedre, 504);

        Display.Commit();

        // Delay
        SysCtlDelay (10000000);

        /* ------------------------------------------------------------------------------------------------------------ */

        // Clear Display
        Display.ClearBank (1);
        Display.ClearRange (0, 2, 42);

        Display.Commit();

        // Delay
        SysCtlDelay (10000000);

        /* ------------------------------------------------------------------------------------------------------------ */

        // Clear Display
        Display.ClearAll ();

        Display.DrawPixel (10, 10, LCD_PIXEL_ON);

        Display.DrawLine (0, 0, 83, 47, LCD_PIXEL_ON);

        Display.Commit();

        // Delay
        SysCtlDelay (10000000);

        Display.ClearAll();

        Display.DrawRectangle (0, 0, PCD8544_COLUMNS - 1, PCD8544_ROWS - 1, LCD_PIXEL_ON);

        Display.DrawFilledRectangle (5, 10, 78, 37, LCD_PIXEL_XOR);

        Display.DrawCircle (42, 24, 20, LCD_PIXEL_XOR);

        Display.Commit();

        // Delay
        SysCtlDelay (10000000);

        /* ------------------------------------------------------------------------------------------------------------ */

        Display.Powerdown (LCD_PD_OFF);

        // Delay
        SysCtlDelay (10000000);

        Display.Powerdown (LCD_PD_ON);

        // Delay
        SysCtlDelay (10000000);

        /* ------------------------------------------------------------------------------------------------------------ */

        if (counter == 1)
        {
            counter = 0;

            if (Display.Invert (LCD_INV_GET) == LCD_INV_OFF)
                Display.Invert (LCD_INV_ON);
            else
                Display.Invert (LCD_INV_OFF);

            /* ------------------------------------------------------------------------------------------------------------ */

            if (Display.Backlight (LCD_BKL_GET) == LCD_BKL_OFF)
                Display.Backlight (LCD_BKL_ON);
            else
                Display.Backlight (LCD_BKL_OFF);
        }

        else
            counter++;
    }
}

/* -------------------------------------------------------------------------------------------------------------------- */

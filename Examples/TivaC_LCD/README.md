# TivaC 5110 LCD
Nokia 5110 LCD library for Texas Instruments TivaC devices

**Version:** 2.0  
**Author:** Renan Duarte  
**E-mail:** duarte.renan@hotmail.com  
**Date:** 13/05/2024  

**License:**
Anyone is free to use and modify this code, but please provide credit to the author, Renan Duarte.

**Usage:**  
To use this library, include the `Lcd_TivaC.hpp` header file in your project. 

The library provides the following functions:

## Initialization

### `Init`
- **Description:** Starts the device peripherals, configures the LCD controller, and clears display RAM.
- **Arguments:** `Config` - `lcd_config_t` struct
- **Returns:** None

## Display Control

### `Commit`
- **Description:** Copies the local buffer to the LCD RAM.
- **Arguments:** None
- **Returns:** None

### `ClearRange`
- **Description:** Clears a range of columns and sets the cursor at the start of the range.
- **Arguments:** 
  - `Bank` - Starting bank of the range
  - `Column` - Starting column of the range
  - `Length` - Number of columns to be cleared
- **Returns:** None

### `ClearBank`
- **Description:** Clears a bank of the LCD and sets the cursor at column 0 of this bank.
- **Arguments:** `Bank` - Bank to be cleared
- **Returns:** None

### `ClearAll`
- **Description:** Clears the LCD and sets the cursor at bank 0, column 0.
- **Arguments:** None
- **Returns:** None

### `Goto`
- **Description:** Sets the LCD cursor position (bank and column).
- **Arguments:** 
  - `Bank` - Desired bank
  - `Column` - Desired column
- **Returns:** None

### `GetBank`
- **Description:** Gets the current bank of the LCD cursor.
- **Arguments:** None
- **Returns:** Cursor bank

### `GetColumn`
- **Description:** Gets the current column of the LCD cursor.
- **Arguments:** None
- **Returns:** Cursor column

## Power and Backlight Control

### `Powerdown`
- **Description:** Sets or gets LCD powerdown mode.
- **Arguments:** `Active` - `lcd_powerdown_t` value. Use `LCD_PD_GET` to get the current powerdown status.
- **Returns:** Current powerdown status - `lcd_powerdown_t` value

### `Backlight`
- **Description:** Sets or gets the backlight status.
- **Arguments:** `Bkl` - `lcd_backlight_t` value. Use `LCD_BKL_GET` to get the backlight status.
- **Returns:** Backlight status - `lcd_backlight_t` value

### `Invert`
- **Description:** Sets or gets inverted mode status.
- **Arguments:** `Inv` - `lcd_inv_t` value. Use `LCD_INV_GET` to get the inverted status.
- **Returns:** Inverted mode status - `lcd_inv_t` value

## Writing Text and Graphics

### `WriteChar`
- **Description:** Writes a 6x8 px char on the display starting at the current cursor position.
- **Arguments:** 
  - `Char` - Char to be sent
  - `Font` - `lcd_font_t` value
  - `Mode` - Pixel mode - `lcd_pixel_mode_t` value
- **Returns:** None

### `WriteString`
- **Description:** Writes a string on the display starting at the current cursor position using 6x8 px chars.
- **Arguments:** 
  - `String` - Pointer to the string to be sent
  - `Font` - `lcd_font_t` value
  - `Mode` - Pixel mode - `lcd_pixel_mode_t` value
- **Returns:** None

### `WriteInt`
- **Description:** Writes a signed integer number (type long) on the display starting at the current cursor position using 6x8 px chars.
- **Arguments:** 
  - `Number` - Number to be sent
  - `Font` - `lcd_font_t` value
  - `Mode` - Pixel mode - `lcd_pixel_mode_t` value
- **Returns:** None

### `WriteFloat`
- **Description:** Writes a signed float number (single precision) on the display starting at the current cursor position using 6x8 px chars.
- **Arguments:** 
  - `Number` - Number to be sent
  - `DecPlaces` - Number of decimal places to be shown
  - `Font` - `lcd_font_t` value
  - `Mode` - Pixel mode - `lcd_pixel_mode_t` value
- **Returns:** None

### `WriteCharBig`
- **Description:** Writes a 10x16 char on the display starting at the current cursor position.
- **Arguments:** 
  - `Char` - Char to be sent
  - `Mode` - Pixel mode - `lcd_pixel_mode_t` value
- **Returns:** None

### `WriteIntBig`
- **Description:** Writes a signed integer number (type long) on the display starting at the current cursor position using 10x16 px chars.
- **Arguments:** 
  - `Number` - Number to be sent
  - `Mode` - Pixel mode - `lcd_pixel_mode_t` value
- **Returns:** None

### `WriteFloatBig`
- **Description:** Writes a signed float number (single precision) on the display starting at the current cursor position using 10x16 px chars.
- **Arguments:** 
  - `Number` - Number to be sent
  - `DecPlaces` - Number of decimal places to be shown
  - `Mode` - Pixel mode - `lcd_pixel_mode_t` value
- **Returns:** None

## Drawing Graphics

### `DrawBitmap`
- **Description:** Draws a bitmap on the display starting at the current cursor position.
- **Arguments:** 
  - `Bitmap` - Pointer to the array
  - `Length` - Length of the array (0 to PCD8544_MAXBYTES bytes)
- **Returns:** None

### `DrawPixel`
- **Description:** Draws a single pixel on the display.
- **Arguments:** 
  - `X` - Column of the pixel (0 to PCD8544_COLUMNS)
  - `Y` - Row of the pixel (0 to PCD8544_ROWS)
  - `Mode` - Pixel mode - `lcd_pixel_mode_t` value
- **Returns:** None

### `DrawLine`
- **Description:** Draws a line between two points on the display using DDA algorithm.
- **Arguments:** 
  - `Xi`, `Yi` - Absolute pixel coordinates for line origin
  - `Xf`, `Yf` - Absolute pixel coordinates for line end
  - `Mode` - Pixel mode - `lcd_pixel_mode_t` value
- **Returns:** None

### `DrawRectangle`
- **Description:** Draws a rectangle between two diagonal points on the display.
- **Arguments:** 
  - `Xi`, `Yi` - Absolute pixel coordinates for the first point
  - `Xf`, `Yf` - Absolute pixel coordinates for end point
  - `Mode` - Pixel mode - `lcd_pixel_mode_t` value
- **Returns:** None

### `DrawFilledRectangle`
- **Description:** Draws a filled rectangle between two diagonal points on the display.
- **Arguments:** 
  - `Xi`, `Yi` - Absolute pixel coordinates for the first point
  - `Xf`, `Yf` - Absolute pixel coordinates for end point
  - `Mode` - Pixel mode - `lcd_pixel_mode_t` value
- **Returns:** None

### `DrawCircle`
- **Description:** Draw a circle using Bresenham algorithm.
- **Arguments:** 
  - `Xc`, `Yc` - Absolute pixel coordinates for the center of the circle
  - `Radius` - Radius of the circle in pixels
  - `Mode` - Pixel mode - Can be either `LCD_PIXEL_OFF`, `LCD_PIXEL_ON`, or `LCD_PIXEL_XOR`
- **Returns:** None

## Example:
```cpp
#include "Lcd_TivaC.hpp"

// Lcd object - From Lcd_TivaC class
Lcd Display;

int main() {
    lcd_config_t config;
    // Set up LCD configuration...

    Display.Init(&config);

    // Main loop
    while (true) {
        Display.ClearAll ();
        Display.WriteString ("Hello", LCD_FONT_SMALL, LCD_PIXEL_ON);
        Display.Commit();
    }
}

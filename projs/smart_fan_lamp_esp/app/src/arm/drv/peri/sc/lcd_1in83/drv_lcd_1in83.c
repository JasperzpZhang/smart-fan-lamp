/*****************************************************************************
* | File        :   LCD_1in83.c
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :   Used to shield the underlying layers of each master and enhance portability
*----------------
* | This version:   V1.0
* | Date        :   2024-03-05
* | Info        :   Basic version
 *
 ******************************************************************************/
#include "drv/peri/sc/lcd_1in83/drv_lcd_1in83.h"
#include "drv/peri/sc/lcd_1in83/drv_lcd_1in83_cfg.h"

#include <stdio.h>
#include <stdlib.h> //itoa()

LCD_1IN83_ATTRIBUTES LCD_1IN83;

/******************************************************************************
function :  Hardware reset
parameter:
******************************************************************************/
static void
LCD_1IN83_Reset(void) {
    LCD_1IN83_RST_1;
    DEV_DELAY_MS(100);
    LCD_1IN83_RST_0;
    DEV_DELAY_MS(100);
    LCD_1IN83_RST_1;
    DEV_DELAY_MS(120);
}

/******************************************************************************
function :  send command
parameter:
     Reg : Command register
******************************************************************************/
static void
lcd_1in83_send_cmd(uint8_t Reg) {
    LCD_1IN83_DC_0;
    LCD_1IN83_CS_0;
    DEV_SPI_WRITE(Reg);
    LCD_1IN83_CS_1;
}

/******************************************************************************
function :  send data
parameter:
    Data : Write data
******************************************************************************/
static void
lcd_1in83_send_data_8bit(uint8_t Data) {
    LCD_1IN83_DC_1;
    LCD_1IN83_CS_0;
    DEV_SPI_WRITE(Data);
    LCD_1IN83_CS_1;
}

/******************************************************************************
function :  send data
parameter:
    Data : Write data
******************************************************************************/
static void
LCD_1IN83_SendData_16Bit(uint16_t Data) {
    LCD_1IN83_DC_1;
    LCD_1IN83_CS_0;
    DEV_SPI_WRITE((Data >> 8) & 0xFF);
    DEV_SPI_WRITE(Data & 0xFF);
    LCD_1IN83_CS_1;
}

/******************************************************************************
function :  Initialize the lcd register
parameter:
******************************************************************************/
static void
lcd_1in83_initReg(void) {

    lcd_1in83_send_cmd(0x11);
    DEV_DELAY_MS(120);

    lcd_1in83_send_cmd(0xB2);
    lcd_1in83_send_data_8bit(0x0C);
    lcd_1in83_send_data_8bit(0x0C);
    lcd_1in83_send_data_8bit(0x00);
    lcd_1in83_send_data_8bit(0x33);
    lcd_1in83_send_data_8bit(0x33);

    lcd_1in83_send_cmd(0x35);
    lcd_1in83_send_data_8bit(0x00);

    lcd_1in83_send_cmd(0x36);
    lcd_1in83_send_data_8bit(0x00);

    lcd_1in83_send_cmd(0x3A);
    lcd_1in83_send_data_8bit(0x05);

    lcd_1in83_send_cmd(0xB7);
    lcd_1in83_send_data_8bit(0x66); //VGH=14.5V VGL=-11.38V

    lcd_1in83_send_cmd(0xBB);
    lcd_1in83_send_data_8bit(0x22);

    lcd_1in83_send_cmd(0xC0);
    lcd_1in83_send_data_8bit(0x2C);

    lcd_1in83_send_cmd(0xC2);
    lcd_1in83_send_data_8bit(0x01);

    lcd_1in83_send_cmd(0xC3);
    lcd_1in83_send_data_8bit(0x19); //4.8V

    lcd_1in83_send_cmd(0xC6);
    lcd_1in83_send_data_8bit(0x0F);

    lcd_1in83_send_cmd(0xD0);
    lcd_1in83_send_data_8bit(0xA7);

    lcd_1in83_send_cmd(0xD0);
    lcd_1in83_send_data_8bit(0xA4);
    lcd_1in83_send_data_8bit(0xA1);

    lcd_1in83_send_cmd(0xD6);
    lcd_1in83_send_data_8bit(0xA1);

    lcd_1in83_send_cmd(0xE0);
    lcd_1in83_send_data_8bit(0xF0);
    lcd_1in83_send_data_8bit(0x00);
    lcd_1in83_send_data_8bit(0x04);
    lcd_1in83_send_data_8bit(0x03);
    lcd_1in83_send_data_8bit(0x03);
    lcd_1in83_send_data_8bit(0x10);
    lcd_1in83_send_data_8bit(0x2C);
    lcd_1in83_send_data_8bit(0x43);
    lcd_1in83_send_data_8bit(0x43);
    lcd_1in83_send_data_8bit(0x38);
    lcd_1in83_send_data_8bit(0x14);
    lcd_1in83_send_data_8bit(0x13);
    lcd_1in83_send_data_8bit(0x2A);
    lcd_1in83_send_data_8bit(0x30);

    lcd_1in83_send_cmd(0xE1);
    lcd_1in83_send_data_8bit(0xF0);
    lcd_1in83_send_data_8bit(0x08);
    lcd_1in83_send_data_8bit(0x0B);
    lcd_1in83_send_data_8bit(0x0E);
    lcd_1in83_send_data_8bit(0x0D);
    lcd_1in83_send_data_8bit(0x08);
    lcd_1in83_send_data_8bit(0x2B);
    lcd_1in83_send_data_8bit(0x33);
    lcd_1in83_send_data_8bit(0x43);
    lcd_1in83_send_data_8bit(0x3A);
    lcd_1in83_send_data_8bit(0x15);
    lcd_1in83_send_data_8bit(0x15);
    lcd_1in83_send_data_8bit(0x2C);
    lcd_1in83_send_data_8bit(0x32);

    lcd_1in83_send_cmd(0x21);

    lcd_1in83_send_cmd(0x29);

    lcd_1in83_send_cmd(0x2C);
}

/********************************************************************************
function:   Set the resolution and scanning method of the screen
parameter:
        Scan_dir:   Scan direction
********************************************************************************/
static void
LCD_1IN83_SetAttributes(uint8_t Scan_dir) {
    // Get the screen scan direction
    LCD_1IN83.SCAN_DIR = Scan_dir;
    uint8_t MemoryAccessReg = 0X08;

    // Get GRAM and LCD width and height
    if (Scan_dir == HORIZONTAL) {
        LCD_1IN83.HEIGHT = LCD_1IN83_WIDTH;
        LCD_1IN83.WIDTH = LCD_1IN83_HEIGHT;
        MemoryAccessReg = 0X78;
    } else {
        LCD_1IN83.HEIGHT = LCD_1IN83_HEIGHT;
        LCD_1IN83.WIDTH = LCD_1IN83_WIDTH;
        MemoryAccessReg = 0X08;
    }

    // Set the read / write scan direction of the frame memory
    lcd_1in83_send_cmd(0x36);                  // MX, MY, RGB mode
    lcd_1in83_send_data_8bit(MemoryAccessReg); // 0x08 set RGB
}

/********************************************************************************
function :  Initialize the lcd
parameter:
********************************************************************************/
void
lcd_1in83_init(uint8_t Scan_dir) {
    // Hardware reset
    LCD_1IN83_Reset();

    // Set the resolution and scanning method of the screen
    LCD_1IN83_SetAttributes(Scan_dir);

    // Set the initialization register
    lcd_1in83_initReg();
}

/********************************************************************************
function:   Sets the start position and size of the display area
parameter:
        Xstart  :   X direction Start coordinates
        Ystart  :   Y direction Start coordinates
        Xend    :   X direction end coordinates
        Yend    :   Y direction end coordinates
********************************************************************************/
void
lcd_1in83_set_windows(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend) {
    if (LCD_1IN83.SCAN_DIR == VERTICAL) {
        // set the X coordinates
        lcd_1in83_send_cmd(0x2A);
        lcd_1in83_send_data_8bit(Xstart >> 8);
        lcd_1in83_send_data_8bit(Xstart);
        lcd_1in83_send_data_8bit((Xend - 1) >> 8);
        lcd_1in83_send_data_8bit(Xend - 1);

        // set the Y coordinates
        // lcd_1in83_send_cmd(0x2B);
        // lcd_1in83_send_data_8bit((Ystart + 20) >> 8);
        // lcd_1in83_send_data_8bit(Ystart + 20);
        // lcd_1in83_send_data_8bit((Yend + 20 - 1) >> 8);
        // lcd_1in83_send_data_8bit(Yend + 20 - 1);

        // lcd_1in83_send_cmd(0x2B);
        lcd_1in83_send_data_8bit((Ystart) >> 8);
        lcd_1in83_send_data_8bit(Ystart);
        lcd_1in83_send_data_8bit((Yend - 1) >> 8);
        lcd_1in83_send_data_8bit(Yend - 1);

    } else {
        // set the X coordinates
        // lcd_1in83_send_cmd(0x2A);
        // lcd_1in83_send_data_8bit((Xstart + 20) >> 8);
        // lcd_1in83_send_data_8bit(Xstart + 20);
        // lcd_1in83_send_data_8bit((Xend + 20 - 1) >> 8);
        // lcd_1in83_send_data_8bit(Xend + 20 - 1);

        lcd_1in83_send_cmd(0x2A);
        lcd_1in83_send_data_8bit((Xstart) >> 8);
        lcd_1in83_send_data_8bit(Xstart);
        lcd_1in83_send_data_8bit((Xend - 1) >> 8);
        lcd_1in83_send_data_8bit(Xend - 1);

        // set the Y coordinates
        // lcd_1in83_send_cmd(0x2B);
        // lcd_1in83_send_data_8bit(Ystart >> 8);
        // lcd_1in83_send_data_8bit(Ystart);
        // lcd_1in83_send_data_8bit((Yend - 1) >> 8);
        // lcd_1in83_send_data_8bit(Yend - 1);

        lcd_1in83_send_cmd(0x2B);
        lcd_1in83_send_data_8bit((Ystart + 20) >> 8);
        lcd_1in83_send_data_8bit(Ystart + 20);
        lcd_1in83_send_data_8bit((Yend + 20 - 1) >> 8);
        lcd_1in83_send_data_8bit(Yend + 20 - 1);
    }

    lcd_1in83_send_cmd(0X2C);
}

void
lcd_1in83_set_windows_point(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend) {
    if (LCD_1IN83.SCAN_DIR == VERTICAL) {
        // set the X coordinates
        lcd_1in83_send_cmd(0x2A);
        lcd_1in83_send_data_8bit(Xstart >> 8);
        lcd_1in83_send_data_8bit(Xstart);
        lcd_1in83_send_data_8bit((Xend) >> 8);
        lcd_1in83_send_data_8bit(Xend);

        // set the Y coordinates
        lcd_1in83_send_cmd(0x2B);
        lcd_1in83_send_data_8bit((Ystart) >> 8);
        lcd_1in83_send_data_8bit(Ystart);
        lcd_1in83_send_data_8bit((Yend) >> 8);
        lcd_1in83_send_data_8bit(Yend);

        //        lcd_1in83_send_cmd(0x2B);
        //        lcd_1in83_send_data_8bit((Ystart + 20) >> 8);
        //        lcd_1in83_send_data_8bit(Ystart + 20);
        //        lcd_1in83_send_data_8bit((Yend + 20) >> 8);
        //        lcd_1in83_send_data_8bit(Yend + 20);

    } else {
        // set the X coordinates
        lcd_1in83_send_cmd(0x2A);
        //        lcd_1in83_send_data_8bit((Xstart + 20) >> 8);
        //        lcd_1in83_send_data_8bit(Xstart + 20);
        //        lcd_1in83_send_data_8bit((Xend + 20) >> 8);
        //        lcd_1in83_send_data_8bit(Xend + 20);

        lcd_1in83_send_data_8bit((Xstart) >> 8);
        lcd_1in83_send_data_8bit(Xstart);
        lcd_1in83_send_data_8bit((Xend) >> 8);
        lcd_1in83_send_data_8bit(Xend);

        // set the Y coordinates
        lcd_1in83_send_cmd(0x2B);
        lcd_1in83_send_data_8bit(Ystart >> 8);
        lcd_1in83_send_data_8bit(Ystart);
        lcd_1in83_send_data_8bit((Yend) >> 8);
        lcd_1in83_send_data_8bit(Yend);
    }

    lcd_1in83_send_cmd(0X2C);
}

/******************************************************************************
function :  Clear screen
parameter:
******************************************************************************/
void
lcd_1in83_clear(uint16_t Color) {

    uint16_t i, j;

    lcd_1in83_set_windows(0, 0, LCD_1IN83.WIDTH, LCD_1IN83.HEIGHT);
    DEV_Digital_Write(DEV_DC_PIN, 1);

    for (i = 0; i < LCD_1IN83.HEIGHT; i++) {
        for (j = 0; j < LCD_1IN83.WIDTH; j++) {
            DEV_SPI_WRITE((Color >> 8) & 0xff);
            DEV_SPI_WRITE(Color);
        }
    }
}

/******************************************************************************
function :  Sends the image buffer in RAM to displays
parameter:
******************************************************************************/
void
lcd_1in83_display(uint16_t* Image) {
    uint16_t i, j;

    lcd_1in83_set_windows(0, 0, LCD_1IN83.WIDTH, LCD_1IN83.HEIGHT);
    DEV_Digital_Write(DEV_DC_PIN, 1);

    for (i = 0; i < LCD_1IN83_WIDTH; i++) {
        for (j = 0; j < LCD_1IN83_HEIGHT; j++) {
            DEV_SPI_WRITE((*(Image + i * LCD_1IN83_HEIGHT + j) >> 8) & 0xff);
            DEV_SPI_WRITE(*(Image + i * LCD_1IN83_WIDTH + j));
        }
    }
}

void
lcd_1in83_display_windows(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend, uint16_t* Image) {
    // display
    uint32_t Addr = 0;
    uint16_t i, j;

    lcd_1in83_set_windows(Xstart, Ystart, Xend, Yend);
    LCD_1IN83_DC_1;

    for (i = Ystart; i < Yend - 1; i++) {
        Addr = Xstart + i * LCD_1IN83_WIDTH;
        for (j = Xstart; j < Xend - 1; j++) {
            DEV_SPI_WRITE((*(Image + Addr + j) >> 8) & 0xff);
            DEV_SPI_WRITE(*(Image + Addr + j));
        }
    }
}

void
lcd_1in83_draw_point(uint16_t X, uint16_t Y, uint16_t Color) {
    lcd_1in83_set_windows_point(X, Y, X, Y);
    LCD_1IN83_SendData_16Bit(Color);
}

void
lcd_1in83_set_backlight(uint16_t Value) {
    DEV_SET_PWM(Value);
}

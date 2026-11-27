#ifndef LCD_H
#define LCD_H

#include <lpc21xx.h>
#include "carelink_config.h"

/* _____________16x2 LCD driver: RS P0.16, EN P0.17, RW GND, D4-D7 P0.20-P0.23 _______*/
#define LCD_RS          (1u << 16)
#define LCD_EN          (1u << 17)
#define LCD_DATA_SHIFT  20
#define LCD_DATA_MASK   (0xFu << LCD_DATA_SHIFT)
#define LCD_MASK        (LCD_RS | LCD_EN | LCD_DATA_MASK)

#define DELAY_COUNT     6000   

void delay(unsigned int ms)
{
    volatile unsigned int i, j;
    for (i = 0; i < ms; i++)
    {
        rx_service();
        for (j = 0; j < DELAY_COUNT; j++);
    }
}

static void lcd_nibble(unsigned char n)
{
    IOCLR0 = LCD_DATA_MASK;
    IOSET0 = ((unsigned int)(n & 0x0F)) << LCD_DATA_SHIFT;
    IOSET0 = LCD_EN;
    delay(1);
    IOCLR0 = LCD_EN;
    delay(1);
}

void cmd(unsigned char c)
{
    IOCLR0 = LCD_RS;
    lcd_nibble(c >> 4);
    lcd_nibble(c);
    delay(2);
}

void lcd_data(unsigned char d)
{
    IOSET0 = LCD_RS;
    lcd_nibble(d >> 4);
    lcd_nibble(d);
    delay(1);
}

void lcd_init(void)
{
    IODIR0 |= LCD_MASK;
    IOCLR0 = LCD_MASK;
    delay(20);

    IOCLR0 = LCD_RS;
    lcd_nibble(0x03); delay(5);
    lcd_nibble(0x03); delay(1);
    lcd_nibble(0x03); delay(1);
    lcd_nibble(0x02);       

    cmd(0x28);               
    cmd(0x0C);               
    cmd(0x06);             
    cmd(0x01);              
    delay(2);
}
char lcd_now[2][17];

void lcd_line(unsigned char line, char *text)
{
    char pad[17];
    char *now = lcd_now[line == 0x80 ? 0 : 1];
    int i = 0, same = 1;
    while (text[i] && i < 16) { pad[i] = text[i]; i++; }
    while (i < 16) pad[i++] = ' ';
    pad[16] = 0;
    for (i = 0; i < 16; i++)
        if (now[i] != pad[i]) same = 0;
    if (same) return;
    cmd(line);
    for (i = 0; i < 16; i++)
    {
        lcd_data(pad[i]);
        now[i] = pad[i];
    }
    now[16] = 0;
}
void lcd_resync(void)
{
    IOCLR0 = LCD_RS;
    lcd_nibble(0x03); delay(5);
    lcd_nibble(0x03); delay(1);
    lcd_nibble(0x03); delay(1);
    lcd_nibble(0x02);
    cmd(0x28);
    cmd(0x0C);
    cmd(0x06);
    lcd_now[0][0] = 0;          /* forget what is shown, so both lines are rewritten */
    lcd_now[1][0] = 0;
}

void show(char *l1, char *l2)
{
    lcd_line(0x80, l1);
    lcd_line(0xC0, l2);
}

#endif

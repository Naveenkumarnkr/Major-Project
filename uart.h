#ifndef UART_H
#define UART_H

#include <lpc21xx.h>
#include "carelink_config.h"

/*____________UART0 9600 baud____________*/

#define RX_SIZE 128
char rx_buf[RX_SIZE];
volatile unsigned int rx_head, rx_tail;

void uart_init(void)
{
    PINSEL0 |= 0x05;                  
    U0LCR = 0x83;
    U0DLL = 0x86;                     
    U0DLM = 0x01;
    U0LCR = 0x03;
    U0FCR = 0x07;                    
}
void rx_service(void)
{
    while (U0LSR & 0x01)
    {
        char c = U0RBR;
        unsigned int next = (rx_head + 1) % RX_SIZE;
        if (next != rx_tail)          /* full: drop the character */
        {
            rx_buf[rx_head] = c;
            rx_head = next;
        }
    }
}

void uart_tx(char c)
{
    while ((U0LSR & (1 << 5)) == 0);
    U0THR = c;
}

void uart_string(char *s)
{
    while (*s)
        uart_tx(*s++);
}
char line[16];
int  line_len, line_bad;

int get_line(void)
{
    rx_service();
    while (rx_tail != rx_head)
    {
        char c = rx_buf[rx_tail];
        rx_tail = (rx_tail + 1) % RX_SIZE;
        if (c == '\r') continue;
        if (c == '\n')
        {
            int ok = !line_bad && line_len > 0;
            line[line_len] = 0;
            line_len = 0;
            line_bad = 0;
            if (ok) return 1;
            continue;
        }
        if (line_len < 15) line[line_len++] = c;
        else line_bad = 1;
    }
    return 0;
}

#endif

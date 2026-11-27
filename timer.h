#ifndef TIMER_H
#define TIMER_H

#include <lpc21xx.h>
#include "carelink_config.h"

/* ---------- Timers: Timer0 for waits, Timer1 as a millisecond clock ---------- */

volatile int btn_seen;                /* set when the button is seen pressed during any wait */

void clock_init(void)
{
    VPBDIV = 0x01;                    /* PCLK = CCLK = 60 MHz */
    T1TCR = 0x02;                     /* Timer1: free-running ms clock */
    T1PR  = 59999;
    T1TCR = 0x01;
}

unsigned long now_ms(void)
{
    return T1TC;
}

void wait_ms(unsigned int ms)
{
    unsigned long guard = (unsigned long)ms * 6000UL + 1000UL;   /* never hang */
    T0TCR = 0x02;
    T0PR  = 59999;                    /* 1 count per ms */
    T0TCR = 0x01;
    while (T0TC < ms && --guard)
    {
        rx_service();
        if (BTN_PRESSED())
            btn_seen = 1;
    }
    T0TCR = 0x00;
}

#endif

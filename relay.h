#ifndef RELAY_H
#define RELAY_H

#include <lpc21xx.h>
#include "carelink_config.h"
void relay_set(int n, int on)         
{
    unsigned int bit = 1u << (7 + n);
    if (RELAY_ACTIVE_LOW ? !on : on)
        IOSET0 = bit;
    else
        IOCLR0 = bit;
}

void relays_init(void)                
{
    int n;
    for (n = 1; n <= 4; n++)
        relay_set(n, 0);
    IODIR0 |= RELAY_MASK;
}

#endif

#ifndef CARELINK_CONFIG_H
#define CARELINK_CONFIG_H

#include <lpc21xx.h>
#define RELAY_ACTIVE_LOW  1
#define USE_CALL_BUTTON   1             
#define CALL_BTN_PIN      15            

#define TITLE_PAGE_MS     10000        
#define ESP_SILENT_MS     25000        
#define MSG_SHOW_MS       3000         
/* _____________Pins __________*/
#define BTN_PIN     (1u << CALL_BTN_PIN)   
#define BTN_PRESSED()  (USE_CALL_BUTTON && (IOPIN0 & BTN_PIN) == 0)
#define RELAY_MASK  0x00000F00u       
void rx_service(void);
unsigned long now_ms(void);

#endif

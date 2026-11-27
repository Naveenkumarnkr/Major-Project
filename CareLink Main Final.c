#include <lpc21xx.h>
#include "carelink_config.h"
#include "uart.h"
#include "lcd.h"
#include "timer.h"
#include "relay.h"
char *put_str(char *d, char *s)
{
    while (*s) *d++ = *s++;
    *d = 0;
    return d;
}

int str_eq(char *a, char *b)
{
    while (*a && *a == *b) { a++; b++; }
    return *a == *b;
}

/*_____________ CareLink_____________*/

int  state[5];                        
int  net;                             
int  esp_heard;                       
int  offline_call;                    
unsigned long last_heard;             
unsigned long msg_until;              
char msg[17];

char *dev_name[5] = { "", "Bed light", "Fan", "Night lamp", "Nurse call" };
void send_states(void)
{
    char s[8];
    s[0] = 'S'; s[1] = '=';
    s[2] = (char)('0' + state[1]);
    s[3] = (char)('0' + state[2]);
    s[4] = (char)('0' + state[3]);
    s[5] = (char)('0' + state[4]);
    s[6] = '\n'; s[7] = 0;
    uart_string(s);
}

void show_msg(char *text)
{
    int i = 0;
    while (text[i] && i < 16) { msg[i] = text[i]; i++; }
    msg[i] = 0;
    msg_until = now_ms() + MSG_SHOW_MS;
}
void show_screen(void)
{
    char l1[17];
    l1[0] = 'L'; l1[1] = (char)('0' + state[1]); l1[2] = ' ';
    l1[3] = 'F'; l1[4] = (char)('0' + state[2]); l1[5] = ' ';
    l1[6] = 'N'; l1[7] = (char)('0' + state[3]); l1[8] = ' ';
    l1[9] = 'B'; l1[10] = (char)('0' + state[4]);
    if (state[4]) put_str(l1 + 11, " CALL");
    else          l1[11] = 0;
    lcd_line(0x80, l1);

    if (msg[0] && (long)(msg_until - now_ms()) > 0)
        lcd_line(0xC0, msg);
    else if (state[4])
        lcd_line(0xC0, "NURSE CALL!");
    else if (!esp_heard || now_ms() - last_heard > ESP_SILENT_MS)
        lcd_line(0xC0, "ESP-01 no reply");
    else if (!net)
        lcd_line(0xC0, "Cloud connecting");
    else
        lcd_line(0xC0, "Cloud online");
}
void handle_line(void)
{
    int n, on;
    char text[17], *p;

    if (str_eq(line, "NET=1")) { net = 1; return; }
    if (str_eq(line, "NET=0")) { net = 0; return; }
    if (line[0] == 'R' && line[1] >= '1' && line[1] <= '4' && line[2] == '=' &&
        (line[3] == '0' || line[3] == '1') && line[4] == 0)
    {
        n  = line[1] - '0';
        on = line[3] - '0';
        if (n == 4 && !on && offline_call)
        {
            offline_call = 0;
            send_states();
            return;
        }
        if (n == 4 && on) offline_call = 0;

        if (state[n] != on)
        {
            state[n] = on;
            relay_set(n, on);
            p = put_str(text, dev_name[n]);
            put_str(p, on ? " ON" : " OFF");
            show_msg(text);
        }
        send_states();                
    }
    
}
void service_button(void)
{
    if (btn_seen || BTN_PRESSED())
    {
        btn_seen = 0;
        if (!state[4])
        {
            state[4] = 1;
            relay_set(4, 1);
            if (!net) offline_call = 1;
            send_states();
            show_screen();
        }
    }
}

void show_title(void)
{
    cmd(0x01);
    delay(2);
    show("   CareLink:", " IoT-Based Smart");
    wait_ms(TITLE_PAGE_MS * 35 / 100);
    show(" Hospital Ward", "and Patient Room");
    wait_ms(TITLE_PAGE_MS * 35 / 100);
    show("   Controller", "ThingSpeak cloud");
    wait_ms(TITLE_PAGE_MS - 2 * (TITLE_PAGE_MS * 35 / 100));
}

/* _______________Main_______________*/

int main(void)
{
    unsigned long last_hello, last_resync;

    relays_init();
    IODIR0 &= ~BTN_PIN;               
    clock_init();
    uart_init();
    lcd_init();

    show_title();
    btn_seen = 0;                     
    rx_service();
    rx_tail = rx_head;
    line_len = 0;
    line_bad = 0;
    uart_string("\nHELLO\n");
    last_hello = now_ms();
    last_resync = now_ms();

    while (1)
    {
        service_button();

        while (get_line())
        {
            if (!esp_heard) show_msg("ESP-01 found");
            esp_heard = 1;
            last_heard = now_ms();
            handle_line();
        }
        if ((!esp_heard || now_ms() - last_heard > ESP_SILENT_MS) &&
            now_ms() - last_hello > 5000)
        {
            uart_string("HELLO\n");
            last_hello = now_ms();
        }

        if (now_ms() - last_resync > 10000)   
        {
            lcd_resync();
            last_resync = now_ms();
        }

        show_screen();
        wait_ms(20);
    }
}

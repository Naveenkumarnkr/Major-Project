CareLink: IoT-Based Smart Hospital Ward and Patient Room Controller

CareLink lets nurses switch a patient room's bed light, fan and night lamp from a web dashboard, and lets the patient call the nurse with a bedside button. An LPC2129 (ARM7) microcontroller controls the room through relays and a 16x2 LCD, and an ESP8266 (ESP-01) Wi-Fi module connects it to the ThingSpeak IoT cloud.
The project was first simulated in Proteus 8 and then built and tested on a real LPC2129 development board.
Features
Remote control of 4 loads (bed light, fan, night lamp, nurse-call lamp/buzzer) from ON/OFF buttons on the ThingSpeak channel page
Nurse call button on the board: the alarm turns on at once, even without internet, and the call appears on the cloud dashboard
Live status on a 16x2 LCD (L1 F0 N1 B0) and on ThingSpeak lamp widgets
History charts of every ON/OFF change (ThingSpeak field charts)
Reliability: automatic LCD re-sync after power dips, clear error messages on the LCD, automatic Wi-Fi reconnect, watchdog restart (AT version)
How it works
mermaid
flowchart LR
    A[Nurse: ON/OFF button<br/>ThingSpeak plugin] -->|BED_ON, CALL_OFF...| B[(ThingSpeak<br/>TalkBack queue)]
    B -->|every 3 s| C[ESP8266<br/>ESP-01]
    C -->|UART 9600: R1=1| D[LPC2129<br/>ARM7]
    D --> E[4 relays<br/>bed light, fan,<br/>night lamp, nurse call]
    D --> F[16x2 LCD]
    G[SW1 nurse call<br/>button] --> D
    D -->|UART: S=1001| C
    C -->|field1..field4| H[(ThingSpeak<br/>channel)]
    H --> I[Lamps and charts<br/>on the dashboard]

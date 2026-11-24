CareLink: IoT-Based Smart Hospital Ward and Patient Room Controller

CareLink lets nurses switch a patient room's bed light, fan and night lamp from a web dashboard, and lets the patient call the nurse with a bedside button. An LPC2129 (ARM7) microcontroller controls the room through relays and a 16x2 LCD, and an ESP8266 (ESP-01) Wi-Fi module connects it to the ThingSpeak IoT cloud.
The project was first simulated in Proteus 8 and then built and tested on a real LPC2129 development board.
Features
Remote control of 4 loads (bed light, fan, night lamp, nurse-call lamp/buzzer) from ON/OFF buttons on the ThingSpeak channel page
Nurse call button on the board: the alarm turns on at once, even without internet, and the call appears on the cloud dashboard
Live status on a 16x2 LCD (L1 F0 N1 B0) and on ThingSpeak lamp widgets
History charts of every ON/OFF change (ThingSpeak field charts)
Reliability: automatic LCD re-sync after power dips, clear error messages on the LCD, automatic Wi-Fi reconnect, watchdog restart (AT version)
## How it works
<img width="281" height="584" alt="image" src="https://github.com/user-attachments/assets/bac9e8f0-e748-49b2-9ad7-3c3d8ace24b8" />

The nurse clicks a button on the channel page. A ThingSpeak plugin adds a command (for example BEDLAMP_ON) to the TalkBack command queue.
The ESP8266 asks TalkBack for the next command every 3 seconds and sends a short line to the LPC2129 over UART (R1=1).
The LPC2129 switches the relay, updates the LCD, and replies with the real state of all four outputs (S=1001).
The ESP8266 writes the states to the channel fields, so the lamps and charts always show what is really happening in the room.
When the patient presses SW1, the LPC2129 turns the nurse call on immediately and reports it; the nurse turns it off from the dashboard.

<img width="843" height="635" alt="Screenshot 2026-11-24 203844" src="https://github.com/user-attachments/assets/30dfc8e9-7ff3-4279-a053-126aaedd387c" />

<img width="842" height="530" alt="Screenshot 2026-11-24 203944" src="https://github.com/user-attachments/assets/fd69155b-e89b-4434-a792-de0fec275857" />

<img width="836" height="431" alt="image" src="https://github.com/user-attachments/assets/9914587b-f30f-4665-9f6f-4af6cb9fce5f" />

<img width="842" height="467" alt="Screenshot 2026-11-24 204154" src="https://github.com/user-attachments/assets/f7c9f644-5f4c-4b32-9f24-64a415d6fc6e" />

<img width="836" height="431" alt="Screenshot 2026-11-24 204320" src="https://github.com/user-attachments/assets/76deb855-60aa-4fa3-9cef-a6e540aa30f6" />

Setup
1. ThingSpeak
Create a channel with 4 fields: Bed light, Fan, Night lamp, Nurse call. Note the Write API Key.
Add a Lamp Indicator widget for each field (condition: greater than 0).
Apps → TalkBacks → New TalkBack. Note the TalkBack ID and API Key.
Apps → Plugins → New → Custom. Paste thingspeak/carelink_plugin_board.html into the HTML box, fill in your TalkBack ID and key, and add it to the channel.

ESP8266 (Version 1)
Arduino IDE → Preferences → Additional Boards URL: http://arduino.esp8266.com/stable/package_esp8266com_index.json, then install esp8266 in Boards Manager.
Open esp8266/carelink_esp8266_ts/carelink_esp8266_ts.ino and fill in carelink_esp_config.h (Wi-Fi name, password, keys).
Board Generic ESP8266 Module, Flash Size 1MB, Reset Method no dtr (aka ck).
Put the ESP-01 in the programmer with GPIO0 held to GND (PROG mode), then Upload.

On Linux, add your user to the dialout group (sudo usermod -a -G dialout $USER) and log in again, or the upload fails with "cannot access /dev/ttyUSB0".

3. LPC2129
Keil: new project for LPC2129, add Startup.s and lpc2129/main.c only (the headers are included by main.c).
Options: tick Use MicroLIB and Create HEX File. Build.
Flash with Flash Magic over UART0. Afterwards unplug the PC serial cable and remove the MAX3232 Rx0/Tx0 jumpers, because they share P0.0/P0.1 with the ESP-01.

<img width="861" height="455" alt="Screenshot 2026-11-24 204506" src="https://github.com/user-attachments/assets/a1f62853-4d9b-4188-8c6d-02fab9c782ea" />

Future improvements
MQTT with TLS for instant, encrypted control
LM35 room temperature with automatic fan (code ready in the AT version, USE_TEMP)
Phone/e-mail alert on nurse call (ThingSpeak React)
One channel per room and a central ward dashboard

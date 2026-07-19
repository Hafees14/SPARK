# SPARK — Gas & Fire Safety Monitor

Makerspace Product Designing Course Project — built for a hackathon demo.

## What it does
SPARK is an early-warning safety device that detects gas leaks and fire risk conditions in real time, alerts locally, and can trigger an automatic safety response.

- **MQ-2 gas sensor** continuously monitors for flammable gas/smoke
- **DHT11** tracks ambient temperature and humidity
- **OLED display** shows live readings on the device
- **Buzzer + LEDs** give an immediate local alarm when gas crosses a safe threshold
- **Relay output** can trigger an external safety action (exhaust fan, solenoid valve, power cutoff)
- **ESP32** hosts a self-contained WiFi access point with a live web dashboard — no internet or router required, connect directly to the device's own hotspot

## Hardware
Custom PCB (140×70mm) designed around an ESP32 DevKit, housed in a 3D-printed two-part enclosure.

| File | Purpose |
|---|---|
| `hardware/Copy_of_Lakshan_140x70.pdf` | PCB layout reference |
| `hardware/Copy_of_Lakshan.json` | EasyEDA PCB source |
| `hardware/Copy_of_Lakshan_2026-07-18.dxf` | Enclosure panel cutout drawing |
| `hardware/Copy_of_Lakshan_up.STL` / `.SLDPRT` | Enclosure top |
| `hardware/Copy_of_Lakshan_Down.STL` / `.SLDPRT` | Enclosure bottom |

## Firmware
| File | Purpose |
|---|---|
| `firmware/bringup.ino` | Component-by-component test sketch — run first to verify every solder joint before real logic |
| `firmware/ap_dashboard.ino` | Main firmware — ESP32 hosts its own WiFi hotspot and serves a live dashboard (gas/temp/humidity, alarm status, manual relay control) |

## Getting started
1. Flash `firmware/bringup.ino` first and confirm LEDs, buzzer, relay, OLED, and sensors all respond correctly over Serial.
2. Update pin numbers in both sketches to match your schematic's net labels (LED1, LED2, SW1–4, RELAY, BUZZER, MQ2, DHT11).
3. Flash `firmware/ap_dashboard.ino`.
4. Connect to the WiFi network `GasSafetyMonitor` (password in the sketch) from your phone or laptop.
5. Open `http://192.168.4.1` in a browser to view live readings.

## Team
Built by [Hafees](https://github.com/Hafees14) and team.

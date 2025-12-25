# Arduino Internet Time Clock

An Arduino Uno drives a 16×2 I²C LCD and keeps accurate time without an RTC by requesting NTP data over Wi‑Fi. Connectivity is provided by an ESP‑01 (ESP8266) running AT firmware and controlled through the WiFiEsp library.

## Features

- No dedicated RTC—time is fetched from pool.ntp.org every hour via NTP.
- 16×2 I²C LCD displays current time/date with a configurable UTC offset.
- WiFiEsp + SoftwareSerial allows the Uno to use an ESP‑01 while keeping hardware serial free for logging.
- `wifi_credentials.h` isolates SSID/password for easy updates.

## Hardware

| Component | Notes |
|-----------|-------|
| Arduino Uno (5 V board) | Drives LCD + SoftwareSerial |
| ESP‑01 (ESP8266) with AT firmware | Use stable AT firmware (e.g., v1.5.x) at 9600 baud |
| 16×2 LCD with PCF8574 I²C backpack | Default address 0x27 (run I²C scanner to confirm) |
| Stable 3.3 V regulator (≥300 mA) | Powers the ESP‑01; don’t use the Uno’s 3.3 V pin |
| Level shifter / resistor divider | Drops Arduino TX (5 V) down to ESP RX (3.3 V) |
| Dupont wires + 10 kΩ pull-ups | Pull CH_PD, GPIO0, GPIO2 high |

### Wiring

**ESP‑01 ↔ Uno**

| ESP Pin | Uno / Notes |
|---------|-------------|
| VCC, EN/CH_PD | Regulated 3.3 V (tie EN high via 10 kΩ) |
| GND | Common ground |
| TX | D2 (ESP_RX_PIN) |
| RX | D3 (ESP_TX_PIN) through divider (e.g., 1 kΩ/2 kΩ) |
| GPIO0, GPIO2 | 3.3 V via 10 kΩ pull-ups (GPIO0 low only when flashing firmware) |
| RST | 3.3 V via 10 kΩ (optional button to GND) |

**LCD (I²C)**
- VCC → 5 V
- GND → GND
- SDA → A4
- SCL → A5

## Software Setup

1. **Install libraries (Sketch → Include Library → Manage Libraries…)**
   - WiFiEsp, NTPClient, Time (TimeLib), LiquidCrystal_I2C (NewLiquidCrystal).
2. **Prep the ESP‑01**
   - Connect via USB‑serial adapter, ensure AT firmware responds.
   - Lock baud to 9600: `AT+UART_DEF=9600,8,1,0,0`.
   - Verify it can join your AP with `AT+CWJAP="SSID","PASSWORD"`.
3. **Configure Wi‑Fi credentials**
   - Edit `1_internet_time_clock/wifi_credentials.h`.
4. **Upload**
   - Disconnect ESP‑01 from pins 0/1 while flashing.
   - Select *Board → Arduino Uno* and the correct COM port, then upload.
   - Reconnect ESP‑01 to D2/D3 afterwards.
5. **Monitor**
   - Open Serial Monitor at 9600 baud to view WiFiEsp logs (`Connecting to ...`, `Connected to WiFi`).
   - LCD shows `TIME`/`DATE` once NTP data is received. Adjust `UTC_OFFSET_SECONDS` to match your timezone.

## Troubleshooting

- **LCD only shows a blinking block:** run the I²C scanner to confirm the address, check SDA/SCL wiring, and adjust contrast.
- **`WiFiEsp >>> TIMEOUT` or “ESP-01 not responding”:** ensure a solid 3.3 V supply, correct TX/RX wiring, proper pull-ups, and that AT firmware responds at 9600 baud.
- **Fails to connect to Wi‑Fi:** double-check SSID/password, keep ESP close to the router, ensure STA mode (`AT+CWMODE=1`), and consider channel changes if your router uses channels 12/13.
- **Time offset incorrect:** update `UTC_OFFSET_SECONDS` in the sketch (seconds relative to UTC). Add or remove 3600 seconds when daylight saving changes.

## Photos

![Working_output](https://github.com/user-attachments/assets/ce161c0a-93d2-4812-be8f-0155774bc5f1)
![Serial_monitor](https://github.com/user-attachments/assets/bb5a078c-82f4-4711-88d9-5e16c6461d19)

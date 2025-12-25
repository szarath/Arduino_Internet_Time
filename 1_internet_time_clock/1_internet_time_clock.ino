#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <WiFiEsp.h>
#include <WiFiEspUdp.h>
#include <NTPClient.h>
#include <TimeLib.h>
#include "wifi_credentials.h" // Include the WiFi credentials from the separate file

constexpr uint8_t ESP_RX_PIN = 2; // Arduino pin receiving data from ESP-01 TX
constexpr uint8_t ESP_TX_PIN = 3; // Arduino pin sending data to ESP-01 RX
constexpr long ESP_BAUD = 9600;   // Ensure the ESP-01 AT firmware is set to the same baud rate
constexpr long UTC_OFFSET_SECONDS = 2L * 60L * 60L; // UTC+2 (adjust for DST if needed)

SoftwareSerial espSerial(ESP_RX_PIN, ESP_TX_PIN);
WiFiEspUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", UTC_OFFSET_SECONDS, 60UL * 60UL * 1000UL); // Update hourly
// Constructor mapping typical PCF8574 backpack pins: En=2, Rw=1, Rs=0, D4..D7=4..7
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7);

char Time[] = "TIME:00:00:00";
char Date[] = "DATE:00/00/2000";
byte last_second, second_, minute_, hour_, day_, month_;
int year_;

void setup() {
  Serial.begin(9600); // Start the Serial communication to send messages to the computer
  espSerial.begin(ESP_BAUD); // Start communication with ESP-01
  WiFi.init(&espSerial);     // Initialize WiFi library with ESP-01 serial

  lcd.begin(16, 2); // Initialize I2C LCD module
  lcd.setBacklightPin(3, POSITIVE); // Backlight pin on typical PCF8574 backpacks
  lcd.setBacklight(HIGH); // Turn backlight ON
  lcd.setCursor(0, 0);
  lcd.print(Time);
  lcd.setCursor(0, 1);
  lcd.print(Date);

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("ESP-01 not responding. Check wiring/power.");
    while (true) {
      delay(1000);
    }
  }

  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, password) != WL_CONNECTED) {
    Serial.print(".");
    delay(2000);
  }
  Serial.println("\nConnected to WiFi");

  timeClient.begin(); // Initialize NTP client
  timeClient.forceUpdate(); // Fetch initial time as soon as possible
}

void loop() {
  timeClient.update();
  unsigned long unix_epoch = timeClient.getEpochTime(); // Get Unix epoch time from the NTP server

  second_ = second(unix_epoch);
  if (last_second != second_) {
    minute_ = minute(unix_epoch);
    hour_ = hour(unix_epoch);
    day_ = day(unix_epoch);
    month_ = month(unix_epoch);
    year_ = year(unix_epoch);

    Time[12] = second_ % 10 + 48;
    Time[11] = second_ / 10 + 48;
    Time[9] = minute_ % 10 + 48;
    Time[8] = minute_ / 10 + 48;
    Time[6] = hour_ % 10 + 48;
    Time[5] = hour_ / 10 + 48;

    Date[5] = day_ / 10 + 48;
    Date[6] = day_ % 10 + 48;
    Date[8] = month_ / 10 + 48;
    Date[9] = month_ % 10 + 48;
    Date[13] = (year_ / 10) % 10 + 48;
    Date[14] = year_ % 10 % 10 + 48;

    // Send time and date to serial monitor
    Serial.println(Time);
    Serial.println(Date);

    // Display time and date on the 16x2 LCD
    lcd.setCursor(0, 0);
    lcd.print(Time);
    lcd.setCursor(0, 1);
    lcd.print(Date);

    last_second = second_;
  }

  delay(200);
}

/*
  Bring-up firmware — Gas/Fire Safety Monitor (ESP32)
  Run this FIRST before your real logic. It exercises every
  component individually so you can catch bad solder joints fast.

  >>> UPDATE ALL PIN NUMBERS to match your actual schematic <<<
  Cross-check against the net labels in your PCB file:
  LED1, LED2, SW1-4, RELAY, BUZZER, MQ2 (analog), DHT11 (data)
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// ---- PIN MAP (placeholder — confirm against schematic) ----
#define PIN_LED1     2
#define PIN_LED2     4
#define PIN_SW1      13
#define PIN_SW2      14
#define PIN_BUZZER   25
#define PIN_RELAY    26
#define PIN_MQ2      34   // analog input
#define PIN_DHT11    27

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
DHT dht(PIN_DHT11, DHT11);

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("=== Bring-up start ===");

  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_SW1, INPUT_PULLUP);
  pinMode(PIN_SW2, INPUT_PULLUP);

  dht.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init FAILED - check wiring/address (try 0x3D too)");
  } else {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Bring-up OK");
    display.display();
    Serial.println("OLED init OK");
  }

  Serial.println("Testing LEDs...");
  for (int i = 0; i < 3; i++) {
    digitalWrite(PIN_LED1, HIGH); delay(200);
    digitalWrite(PIN_LED1, LOW);
    digitalWrite(PIN_LED2, HIGH); delay(200);
    digitalWrite(PIN_LED2, LOW);
  }

  Serial.println("Testing buzzer (short beep)...");
  digitalWrite(PIN_BUZZER, HIGH); delay(150);
  digitalWrite(PIN_BUZZER, LOW);

  Serial.println("Testing relay (click on/off, unloaded)...");
  digitalWrite(PIN_RELAY, HIGH); delay(500);
  digitalWrite(PIN_RELAY, LOW);

  Serial.println("=== Bring-up complete. Entering monitor loop. ===");
}

void loop() {
  int gas = analogRead(PIN_MQ2);
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  bool sw1 = digitalRead(PIN_SW1) == LOW;
  bool sw2 = digitalRead(PIN_SW2) == LOW;

  Serial.printf("Gas: %d | Temp: %.1fC | Hum: %.1f%% | SW1:%d SW2:%d\n",
                gas, temp, hum, sw1, sw2);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.printf("Gas: %d\n", gas);
  display.printf("T:%.1fC H:%.1f%%\n", temp, hum);
  display.display();

  // Simple threshold demo — tune this value on-site with a lighter/gas source
  if (gas > 1800) {
    digitalWrite(PIN_LED1, HIGH);
    digitalWrite(PIN_BUZZER, HIGH);
    digitalWrite(PIN_RELAY, HIGH); // e.g. trip exhaust fan
  } else {
    digitalWrite(PIN_LED1, LOW);
    digitalWrite(PIN_BUZZER, LOW);
    digitalWrite(PIN_RELAY, LOW);
  }

  delay(1000);
}

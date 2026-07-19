/*
  Gas/Fire Safety Monitor — Web Dashboard (ESP32 Access Point mode)

  ESP32 creates its OWN WiFi hotspot. No internet/router needed.
  Connect your phone/laptop directly to the ESP32's WiFi, then open
  the fixed IP address in a browser.

  >>> UPDATE: pin numbers to match your actual schematic <<<
*/

#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// ---- Access Point credentials (this device's own hotspot) ----
const char* AP_SSID = "GasSafetyMonitor";
const char* AP_PASS = "hackathon2026"; // must be 8+ characters

// ---- PIN MAP (placeholder — confirm against schematic) ----
#define PIN_LED1     2
#define PIN_LED2     4
#define PIN_SW1      13
#define PIN_SW2      14
#define PIN_BUZZER   25
#define PIN_RELAY    26
#define PIN_MQ2      34   // analog input
#define PIN_DHT11    27

#define GAS_THRESHOLD 1800

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
DHT dht(PIN_DHT11, DHT11);
WebServer server(80);

int gasValue = 0;
float tempValue = 0;
float humValue = 0;
bool alarmActive = false;
bool relayManualOverride = false;

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>";
  html += "<meta http-equiv='refresh' content='2'>";
  html += "<title>Gas Safety Monitor</title>";
  html += "<style>";
  html += "body{font-family:sans-serif;background:#111;color:#eee;text-align:center;padding:20px;}";
  html += ".card{background:#222;border-radius:12px;padding:20px;margin:10px auto;max-width:400px;}";
  html += ".value{font-size:2.5em;font-weight:bold;}";
  html += ".alarm{background:#c0392b;}";
  html += ".safe{background:#27ae60;}";
  html += "button{padding:12px 24px;font-size:1em;border-radius:8px;border:none;margin-top:10px;}";
  html += "</style></head><body>";
  html += "<h1>Gas &amp; Fire Safety Monitor</h1>";

  html += "<div class='card " + String(alarmActive ? "alarm" : "safe") + "'>";
  html += "<div>Status</div><div class='value'>" + String(alarmActive ? "ALERT" : "SAFE") + "</div></div>";

  html += "<div class='card'><div>Gas Level</div><div class='value'>" + String(gasValue) + "</div></div>";
  html += "<div class='card'><div>Temperature</div><div class='value'>" + String(tempValue, 1) + " &deg;C</div></div>";
  html += "<div class='card'><div>Humidity</div><div class='value'>" + String(humValue, 1) + " %</div></div>";

  html += "<div class='card'><div>Relay (manual override)</div>";
  html += "<form action='/relay' method='POST'>";
  html += "<button name='state' value='1'>Turn ON</button> ";
  html += "<button name='state' value='0'>Turn OFF</button>";
  html += "</form></div>";

  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleRelay() {
  if (server.hasArg("state")) {
    relayManualOverride = server.arg("state") == "1";
    digitalWrite(PIN_RELAY, relayManualOverride ? HIGH : LOW);
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_SW1, INPUT_PULLUP);
  pinMode(PIN_SW2, INPUT_PULLUP);

  dht.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init FAILED - check wiring/address");
  }

  WiFi.softAP(AP_SSID, AP_PASS);
  IPAddress ip = WiFi.softAPIP();
  Serial.println("Access Point started");
  Serial.print("WiFi name: "); Serial.println(AP_SSID);
  Serial.print("WiFi password: "); Serial.println(AP_PASS);
  Serial.print("Open this in a browser: http://"); Serial.println(ip);

  server.on("/", handleRoot);
  server.on("/relay", HTTP_POST, handleRelay);
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();

  static unsigned long lastRead = 0;
  if (millis() - lastRead > 1000) {
    lastRead = millis();

    gasValue = analogRead(PIN_MQ2);
    tempValue = dht.readTemperature();
    humValue = dht.readHumidity();
    alarmActive = gasValue > GAS_THRESHOLD;

    digitalWrite(PIN_LED1, alarmActive ? HIGH : LOW);
    digitalWrite(PIN_BUZZER, alarmActive ? HIGH : LOW);

    if (!relayManualOverride) {
      digitalWrite(PIN_RELAY, alarmActive ? HIGH : LOW);
    }

    if (display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.printf("Gas: %d\n", gasValue);
      display.printf("T:%.1fC H:%.1f%%\n", tempValue, humValue);
      display.printf("%s\n", alarmActive ? "ALARM!" : "Safe");
      display.printf("AP:%s", AP_SSID);
      display.display();
    }

    Serial.printf("Gas: %d | Temp: %.1fC | Hum: %.1f%% | Alarm: %d\n",
                  gasValue, tempValue, humValue, alarmActive);
  }
}

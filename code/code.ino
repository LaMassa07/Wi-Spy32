#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <WebServer.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "config.h"



Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void configPortal();
void connectWifi();
void showRSSI();
void showInfo();

WebServer server(WS_PORT);
String wifiSSID = "";
String wifiPASS = "";

uint8_t WiFiState = APMODE;
uint8_t rssiState = SHOWRSSI;
int network_n = 0;
int rssi = 0;

unsigned long lastUpdate = 0;
const unsigned long interval = 500;
int posX = 0;
bool first = true;


void setup() {
  pinMode(BUTTON, INPUT_PULLUP);

  if(!oled.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 ALLOCATION FAILED!");
    for(;;);
  }

  Serial.begin(BAUDRATE);

  configPortal();
}


void loop() {
  switch(WiFiState){
    case APMODE: server.handleClient(); break;
    case STAMODE: 
      switch(rssiState){
        case SHOWRSSI: showRSSI(); break;
        case SHOWINFO: showInfo(); break;
      }
      break;
  }
}


void showRSSI(){
  unsigned long now = millis();

  if(now - lastUpdate >= interval){
    lastUpdate = now;

    int rssi = WiFi.RSSI();
    int posY = map(constrain(rssi, -100, -40), -100, -40, Y0, 11);

    oled.fillRect(80, 0, 48, 32, SSD1306_BLACK);

    oled.setTextSize(2);
    oled.setCursor(80, 4);
    oled.print(rssi);

    oled.setTextSize(1);
    oled.setCursor(108, 22);
    oled.print("dB");
    oled.setCursor(8, 2);
    oled.println(wifiSSID.substring(0, 25));

    oled.drawLine(posX, Y0, posX, posY, SSD1306_WHITE);

    oled.display();

    posX++;

    if(posX >= MAX_RSSI_COLUMN){
      posX = 0;
      oled.clearDisplay();
    }
  }
  if(!digitalRead(BUTTON)){
    rssiState = SHOWINFO;
    first = true;
    delay(200);
  }
}


void showInfo(){
  if(first){
    //SSID, PASSWORD, BSSID, ENCRYPTION
    oled.clearDisplay();
    oled.drawBitmap(100, 4, INFO, BITMAP_WIDTH, BITMAP_HEIGHT, 1);
    oled.setTextSize(1);
    oled.setTextColor(SSD1306_WHITE);
    oled.setCursor(4,0);
    oled.println(wifiSSID.substring(0, 25));
    oled.setCursor(4,8);
    oled.println(wifiPASS.substring(0, 25));

    uint8_t* bssid = WiFi.BSSID();
    char bssidStr[18]; // 17 caratteri + terminatore \0
    sprintf(bssidStr, "%02X:%02X:%02X:%02X:%02X:%02X",
            bssid[0], bssid[1], bssid[2],
            bssid[3], bssid[4], bssid[5]);

    oled.setCursor(4, 16);
    oled.println(bssidStr);
    oled.display();
    first = false;
  }
  if(!digitalRead(BUTTON)){
    rssiState = SHOWRSSI;
    delay(200);
    oled.clearDisplay();
  }
}






//-------------WIFI_FUNCTIONS-------------//
void handleRoot() {
  network_n = WiFi.scanNetworks();
  
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 Setup</title>
  <style>
    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
    }
    body {
      font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
      background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
      min-height: 100vh;
      display: flex;
      align-items: center;
      justify-content: center;
      padding: 20px;
    }
    .card {
      background: white;
      border-radius: 16px;
      padding: 32px;
      box-shadow: 0 20px 60px rgba(0,0,0,0.3);
      width: 100%;
      max-width: 380px;
    }
    h1 {
      color: #2d3748;
      font-size: 24px;
      margin-bottom: 8px;
      font-weight: 600;
    }
    .subtitle {
      color: #718096;
      font-size: 14px;
      margin-bottom: 28px;
    }
    label {
      display: block;
      color: #4a5568;
      font-size: 13px;
      font-weight: 500;
      margin-bottom: 6px;
      text-transform: uppercase;
      letter-spacing: 0.5px;
    }
    select, input {
      width: 100%;
      padding: 12px 14px;
      border: 2px solid #e2e8f0;
      border-radius: 8px;
      font-size: 15px;
      margin-bottom: 20px;
      transition: all 0.2s;
      background: white;
    }
    select:focus, input:focus {
      outline: none;
      border-color: #667eea;
      box-shadow: 0 0 0 3px rgba(102,126,234,0.1);
    }
    select {
      cursor: pointer;
      appearance: none;
      background-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' width='12' height='12' viewBox='0 0 12 12'%3E%3Cpath fill='%234a5568' d='M6 9L1 4h10z'/%3E%3C/svg%3E");
      background-repeat: no-repeat;
      background-position: right 14px center;
      padding-right: 40px;
    }
    button {
      width: 100%;
      padding: 14px;
      background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
      color: white;
      border: none;
      border-radius: 8px;
      font-size: 16px;
      font-weight: 600;
      cursor: pointer;
      transition: transform 0.2s, box-shadow 0.2s;
      margin-top: 8px;
    }
    button:hover {
      transform: translateY(-2px);
      box-shadow: 0 8px 20px rgba(102,126,234,0.4);
    }
    button:active {
      transform: translateY(0);
    }
  </style>
</head>
<body>
  <div class="card">
    <h1>WiFi Setup</h1>
    <p class="subtitle">Configure your connection</p>
    
    <form action="/save" method="POST">
      <label>WiFi Network</label>
      <select name="ssid" required>
        <option value="">Select a network</option>
)rawliteral";

  // Add scanned networks dynamically
  for (int i = 0; i < network_n; i++) {
    String ssid = WiFi.SSID(i);
    html += "        <option value=\"" + ssid + "\">" + ssid + "</option>\n";
  }

  html += R"rawliteral(
      </select>

      <label>Password <span style="color:#a0aec0; font-weight:400;">(optional)</span></label>
      <input type="password" name="password" placeholder="Leave empty if open">

      <button type="submit">Connect</button>
    </form>
  </div>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}


void handleSave() {
  wifiSSID = server.arg("ssid");
  wifiPASS = server.arg("password");

  server.send(200, "text/html", "<h2>Saved! <br> Disconnecting...</h2>");

  delay(500);
  oled.clearDisplay();

  server.stop();
  WiFi.softAPdisconnect(true);

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSSID.c_str(), wifiPASS.c_str());

  WiFiState = STAMODE;
}


void configPortal(){
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(16,4);
  oled.println(F("CONNECT TO:"));
  oled.drawBitmap(100, 4, WIFI_LOGO, BITMAP_WIDTH, BITMAP_HEIGHT, 1);
  oled.setCursor(22, 16);
  oled.println(ssid);
  oled.setCursor(22, 24);
  oled.println(password);
  oled.display();

  while(WiFi.softAPgetStationNum() == 0){
    delay(10);
  }

  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(16,4);
  oled.println(F("NAVIGATE TO:"));
  oled.drawBitmap(100, 4, WIFI_LOGO, BITMAP_WIDTH, BITMAP_HEIGHT, 1);
  oled.setCursor(16, 16);
  oled.println(WiFi.softAPIP());
  oled.display();

  server.on("/", handleRoot);
  server.on("/save", HTTP_POST, handleSave);

  server.begin();
}
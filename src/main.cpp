#include <Arduino.h>
#include <ArduinoJson.h>
#include "FS.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>

#include "config/Config.h"
#include "controllers/configops.h"
#include "common/StringConstants.h"
#include "controllers/wifiops.h"
#include "controllers/smartops.h"
#include "operations/capabilities.h"

extern "C" {
  #include <user_interface.h>
}

using namespace MyHomeNew;
ESP8266WebServer server(80);

uint8_t chr2Nibble(char ch) {
  if (ch >= '0' && ch <= '9')
      return ch - '0';
  if (ch >= 'A' && ch <= 'F')
      return (ch - 'A') + 10;
  return 0;
}

void setupMacAddress() {
  const char* APMacStr = Config::getInstance()->getValue(CONFIG_AP_MAC);
  const char* STMacStr = Config::getInstance()->getValue(CONFIG_ST_MAC);

  uint8_t APMac[6];
  uint8_t STAMac[6];

  for(uint8_t i = 0; i < 6; i++) {
    uint8_t strIdx = i * 2;
    APMac[i] = (chr2Nibble(APMacStr[strIdx]) << 4) + chr2Nibble(APMacStr[strIdx + 1]);
    STAMac[i] = (chr2Nibble(STMacStr[strIdx]) << 4) + chr2Nibble(STMacStr[strIdx + 1]);
  }
  
  WiFi.mode(WIFI_AP_STA);
  yield();
  wifi_set_macaddr(SOFTAP_IF, APMac);
  yield();
  wifi_set_macaddr(STATION_IF, STAMac);
  yield();
}

void WiFiManagerSetup(String hostname) {
  WiFiManager wifiManager;
  wifiManager.autoConnect(hostname.c_str(), Config::getInstance()->getValue(CONFIG_ST_PASSWORD));
}

void setup() {
  Serial.begin(115200);
  SPIFFS.begin();
  setupMacAddress();
  String hostname = "myhomenew-" + String(ESP.getChipId(), HEX);
  WiFi.hostname(hostname);
  Capabilities::setOutputMode();

  // Setup spiffs & config
  Serial.println("");
  Serial.println(FStr(MountingMsg));
  if (!SPIFFS.begin()) {
    Serial.println(FStr(MountingFailed));
    return;
  }
  Serial.print("This node AP mac: "); Serial.println(WiFi.softAPmacAddress());
  Serial.print("This node AP (STA) mac: "); Serial.println(WiFi.macAddress());

  if(WiFi.SSID() != "") {
    WiFi.begin();

    if(WiFi.waitForConnectResult() != WL_CONNECTED) {
      WiFiManagerSetup(hostname);
    }
  }
  else {
    WiFiManagerSetup(hostname);
  }
  Serial.print("Connected to:"); Serial.println(WiFi.SSID());

  yield();

  if (!MDNS.begin(hostname)) {
    Serial.println("Error setting up MDNS responder!");
  } else {
    Serial.println("MDNS begin success");
  }

  yield();
  
  MDNS.addService("http", "tcp", 80);
  MDNS.addServiceTxt("http", "tcp", "type", Config::getInstance() ->getValue(CONFIG_TYPE));
  Serial.println("MDNS service started!");
  Serial.print("Hostname:"); Serial.println(hostname);

  server.addHandler(new ConfigOps());
  server.addHandler(new WiFiOps());
  server.addHandler(new SmartOps());
  server.begin();
  Serial.println("HTTP server started");

  WiFi.softAPdisconnect();
}

void loop() {
  MDNS.update();
  server.handleClient();
  delay(100);
}

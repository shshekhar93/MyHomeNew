#define ARDUINOJSON_ENABLE_PROGMEM 1

#include <Arduino.h>
#include "LittleFS.h"
#include "IPAddress.h"
#include <ESP8266WiFi.h>

#include "config/Config.h"
#include "common/StringConstants.h"
#include "controllers/WebSocketHanlder.h"
#include "operations/capabilities.h"
#include "operations/wifi.h"
#include "common/CryptoUtils.h"

extern "C" {
  #include <user_interface.h>
}

using namespace MyHomeNew;

WebSocketHandler* client = WebSocketHandler::getInstance();

void setup() {
  wifi_set_sleep_type(MODEM_SLEEP_T);
  Serial.begin(115200);
  LittleFS.begin();
  WiFiSetup::setupMacAddress();
  String hostname = "myhomenew-" + String(ESP.getChipId(), HEX);
  WiFi.hostname(hostname);
  Capabilities::setPinMode();

  // Setup littlefs & config
  Serial.println("");
  Serial.println(FStr(MountingMsg));
  if (!LittleFS.begin()) {
    Serial.println(FStr(MountingFailed));
    return;
  }
  Serial.print("This node AP mac: "); Serial.println(WiFi.softAPmacAddress());
  Serial.print("This node STA mac: "); Serial.println(WiFi.macAddress());

  if(WiFi.SSID() != "") {
    WiFi.begin();

    if(WiFi.waitForConnectResult() != WL_CONNECTED) {
      WiFiSetup::setupWithWiFiManager(hostname);
    }
  }
  else {
    WiFiSetup::setupWithWiFiManager(hostname);
  }
  Serial.print("Connected to:"); Serial.println(WiFi.SSID());

  yield();
  Serial.print("Hostname:"); Serial.println(hostname);

  client->connect();
  WiFi.softAPdisconnect();
}

void loop() {
  client->loop();
  WiFiSetup::loop();
  delay(100);
}

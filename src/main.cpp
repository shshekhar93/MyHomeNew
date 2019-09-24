#include <Arduino.h>
#include "FS.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "config/Config.h"
#include "controllers/configops.h"
#include "common/StringConstants.h"
#include "controllers/wifiops.h"
#include "controllers/smartops.h"
#include "operations/capabilities.h"
#include "operations/wifi.h"

extern "C" {
  #include <user_interface.h>
}

using namespace MyHomeNew;
ESP8266WebServer server(80);

void setup() {
  wifi_set_sleep_type(MODEM_SLEEP_T);
  Serial.begin(115200);
  SPIFFS.begin();
  WiFiSetup::setupMacAddress();
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
      WiFiSetup::setupWithWiFiManager(hostname);
    }
  }
  else {
    WiFiSetup::setupWithWiFiManager(hostname);
  }
  Serial.print("Connected to:"); Serial.println(WiFi.SSID());

  yield();
  Serial.print("Hostname:"); Serial.println(hostname);

  server.addHandler(new ConfigOps());
  server.addHandler(new WiFiOps());
  server.addHandler(new SmartOps());
  server.begin();
  Serial.println("HTTP server started");

  WiFi.softAPdisconnect();
}

void loop() {
  server.handleClient();
  delay(100);
}

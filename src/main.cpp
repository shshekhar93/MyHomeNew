#include <Arduino.h>
#include <ArduinoJson.h>
#include "FS.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

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
uint8_t APMac[6];
uint8_t STAMac[6];

// bool loadConfig() {
//   Config* _config = Config::getInstance();
//   Serial.print("serverName: ");
//   Serial.println(_config->getValue("name"));
//   return true;
// }

uint8_t hexCharToByte(char ch) {
  if (ch >= '0' && ch <= '9')
      return ch - '0';
  if (ch >= 'A' && ch <= 'F')
      return ch - 'A' + 10;
  if (ch >= 'a' && ch <= 'f')
      return ch - 'a' + 10;
  return 0;
}

void setupMacAddress() {
  // String apMacStr(FStr(APMacAddress));
  // String staMacStr(FStr(STAMacAddress));
  // int i = 0;
  // for(i = 0; i < 6; i++) {
  //   //char buf[100];
  //   APMac[i] = (hexCharToByte(apMacStr[i*2]) * 16) + hexCharToByte(apMacStr[(i * 2) + 1]);
  //   //sprintf(buf, "AP[%d] (%c%c): %d", i, apMacStr[i*2], apMacStr[(i * 2) + 1], APMac[i]);
  //   //Serial.println(buf);
  //   STAMac[i] = (hexCharToByte(staMacStr[i*2]) * 16) + hexCharToByte(staMacStr[(i * 2) + 1]);
  //   //sprintf(buf, "ST[%d] (%c%c): %d", i, staMacStr[i*2], staMacStr[(i * 2) + 1], STAMac[i]);
  //   //Serial.println(buf);
  // }

  uint8_t APMac[6] = {0xCA, 0x43, 0x96, 0x00, 0x00, 0x00};
  uint8_t STAMac[6] = {0xDA, 0x43, 0x96, 0x00, 0x00, 0x00};
  
  WiFi.mode(WIFI_AP_STA);
  yield();
  wifi_set_macaddr(SOFTAP_IF, APMac);
  yield();
  wifi_set_macaddr(STATION_IF, STAMac);
  yield();
}

void setup() {
  Serial.begin(115200);
  SPIFFS.begin();
  setupMacAddress();
  WiFi.hostname("myhomenew" + String(ESP.getChipId(), HEX));
  Capabilities::setOutputMode();

  // Setup spiffs & config
  Serial.println("");
  Serial.println(FStr(MountingMsg));
  if (!SPIFFS.begin()) {
    Serial.println(FStr(MountingFailed));
    return;
  }
  Serial.print("This node AP mac: "); Serial.println(WiFi.softAPmacAddress());
  Serial.print("This node AP (STA) mac: "); Serial.println(WiFi.softAPmacAddress());

  String ssid = WiFi.SSID();
  if(ssid == "") {
    WiFiOps::startAP();
  }
  else {
    char* host = new char[WiFi.hostname().length()];
    strcpy(host, WiFi.hostname().c_str());
    
    if (!MDNS.begin(host)) {
      Serial.println("Error setting up MDNS responder!");
    } else {
      Serial.println("MDNS begin success");
      yield();
    }
    
    MDNS.addService("http", "tcp", 80);
    Serial.println("MDNS service started!");

    Serial.print("Connecting to SSID: "); Serial.println(ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(); 
    
    // if not return WL_CONNECTED, retry for some time, then just start AP.
    if(WiFi.waitForConnectResult() != WL_CONNECTED) {
      WiFiOps::startAP();
    }
    else {
      Serial.print("hostname: ");
      Serial.println(WiFi.hostname());
    }
  }

  server.addHandler(new ConfigOps());
  server.addHandler(new WiFiOps());
  server.addHandler(new SmartOps());
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  delay(100);
}

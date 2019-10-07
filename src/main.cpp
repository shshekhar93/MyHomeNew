#include <Arduino.h>
#include "FS.h"
#include "IPAddress.h"
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>

#include "config/Config.h"
#include "controllers/configops.h"
#include "common/StringConstants.h"
#include "controllers/wifiops.h"
#include "controllers/smartops.h"
#include "operations/capabilities.h"
#include "operations/wifi.h"
#include "common/CryptoUtils.h"

extern "C" {
  #include <user_interface.h>
}

using namespace MyHomeNew;

WebSocketsClient client;

void handleEvent (String jsonStr) {
  
}

void onWSEvent(WStype_t type, uint8_t * payload, size_t length) {
  char* input;
  String key;
  switch (type)
  {
  case WStype_ERROR:
    Serial.println("an error occured!");
    break;
  case WStype_CONNECTED:
    Serial.println("connected");
    break;
  case WStype_DISCONNECTED:
    Serial.println("disconnected");
    break;
  case WStype_TEXT:
    input = new char[length + 1];
    memcpy(input, payload, length);
    input[length] = '\0';
    key = Config::getInstance()->getValue(CONFIG_AES_KEY);
    handleEvent(decrypt(input, key.c_str()));
    delete input;
    break;
  default:
    break;
  }
}

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

  String password = encrypt(hostname.c_str(), Config::getInstance()->getValue(CONFIG_AES_KEY));
  String auth = "Authorization: " + String(Config::getInstance()->getValue(CONFIG_USER)) + ":" + password;
  client.setExtraHeaders(auth.c_str());
  client.begin("192.168.2.5", 8090, "/v1/ws", "myhomenew-device");
  client.onEvent(onWSEvent);

  WiFi.softAPdisconnect();
}

void loop() {
  client.loop();
  delay(100);
}

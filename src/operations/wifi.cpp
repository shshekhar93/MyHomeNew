#include "wifi.h"
#include "../config/Config.h"
#include "../common/CryptoUtils.h"
#include <ESP8266WiFi.h>
#include <WiFiManager.h>

extern "C" {
  #include <user_interface.h>
}

void MyHomeNew::WiFiSetup::forgetWiFiCredsAndRestart() {
  WiFi.disconnect(true);
  delay(100);
  ESP.restart();
}

void MyHomeNew::WiFiSetup::setupMacAddress() {
  const char* APMacStr = Config::getInstance()->getValue(CONFIG_AP_MAC);
  const char* STMacStr = Config::getInstance()->getValue(CONFIG_ST_MAC);

  uint8_t APMac[6];
  uint8_t STAMac[6];
  hexStrToByteArr(APMacStr, APMac);
  hexStrToByteArr(STMacStr, STAMac);
  
  WiFi.mode(WIFI_AP_STA);
  yield();
  wifi_set_macaddr(SOFTAP_IF, APMac);
  yield();
  wifi_set_macaddr(STATION_IF, STAMac);
  yield();
}

void MyHomeNew::WiFiSetup::setupWithWiFiManager(String hostname) {
  WiFiManager wifiManager;
  WiFiManagerParameter userIdParam("user", "Registered username", "", 32);
  WiFiManagerParameter passParam("pass", "Temporary password", "", 32);
  WiFiManagerParameter hostParam("host", "Hub address (optional)", "", 32);

  wifiManager.addParameter(&userIdParam);
  wifiManager.addParameter(&passParam);
  wifiManager.addParameter(&hostParam);
  wifiManager.setConfigPortalTimeout(300);
  wifiManager.autoConnect(hostname.c_str(), Config::getInstance()->getValue(CONFIG_ST_PASSWORD));
  if(WiFi.status() != WL_CONNECTED) {
    ESP.restart();
  }

  const char* user = userIdParam.getValue();
  const char* pass = passParam.getValue();
  const char* host = hostParam.getValue();

  Config* _config = Config::getInstance();
  if(user[0] == '\0') {
    user = _config->getValue(CONFIG_USER);
  }
  if(pass[0] == '\0') {
    pass = _config->getValue(CONFIG_AES_KEY);
  }
  if(host[0] == '\0'){
    host = _config->getValue(CONFIG_HOST);
  }

  if(user[0] == '\0' || pass[0] == '\0' || host[0] == '\0') {
    // User did not provide a mandatory parameter.
    forgetWiFiCredsAndRestart();
  }

  // Save in config.json
  _config->setValue(CONFIG_HOST, host);
  _config->setValue(CONFIG_AES_KEY, pass);
  _config->setValue(CONFIG_USER, user);
  if(!_config->save()) {
    forgetWiFiCredsAndRestart();
  }

  // Save in backup.json
  if(!Config::getInstance()->save()) {
    forgetWiFiCredsAndRestart();
  }
}

#include "wifiops.h"
#include "../config/Config.h"

const String MyHomeNew::WiFiOps::s_path = "/v1/wifi";

bool MyHomeNew::WiFiOps::canHandle(HTTPMethod method, String uri) {
  return uri == s_path;
}

bool MyHomeNew::WiFiOps::handleDisallowedMethods(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) {
  server.send(405);
  return true;
}

bool MyHomeNew::WiFiOps::handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) {
  if(requestMethod == HTTP_GET) {
    return handleGet(server, requestMethod, requestUri);
  }
  return handleDisallowedMethods(server, requestMethod, requestUri);
}

bool MyHomeNew::WiFiOps::handleGet(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) {
  String command = server.arg("cmd");
  bool success = false;
  if(command == "") {
    server.send(400);
  }
  if(command == "cnct") {
    success = connect(false);
  }
  else if(command == "recnct") {
    success = connect(true);
  }
  else if(command == "ap") {
    WiFi.disconnect(true);
    yield();
    delay(200); // Wait for 200 msecs.
    startAP();
  }
  else if(command == "ls") {
    
  }
  server.send((success ? 204 : 400));
  return true;
}

bool MyHomeNew::WiFiOps::connect(bool refresh) {
  String ssid = Config::getInstance()->getValue(CONFIG_SSID);
  String pass = Config::getInstance()->getValue(CONFIG_PASSWORD);
  if(ssid == "") {
    return false;
  }

  WiFi.softAPdisconnect();
  if(!WiFi.isConnected()) {
    WiFi.hostname("myhomenew" + String(ESP.getChipId(), HEX));
    WiFi.begin(ssid.c_str(), pass.c_str());
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
  }
  else if(refresh) {
    return WiFi.reconnect();
  }
  return true;
}

bool MyHomeNew::WiFiOps::startAP() {
  WiFi.mode(WIFI_AP);
  yield();
  String ssidStr = "MyHomeNew-" + String(ESP.getChipId(), HEX);
  Serial.print("ssid: ");
  Serial.println(ssidStr);
  bool success = WiFi.softAP(ssidStr.c_str(), "11111111");
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
  return success;
}

bool MyHomeNew::WiFiOps::canUpload(String uri) {
  return false;
}

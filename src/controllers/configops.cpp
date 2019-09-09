#include "configops.h"
#include "../config/Config.h"
#include <ArduinoJson.h>

const String MyHomeNew::ConfigOps::s_path = "/v1/config";

bool MyHomeNew::ConfigOps::canHandle(HTTPMethod method, String uri) {
  return uri == s_path;
}
bool MyHomeNew::ConfigOps::canUpload(String uri) {
  return false;
}
bool MyHomeNew::ConfigOps::handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) {
  if(requestMethod == HTTP_GET) {
    return handleGet(server, requestMethod, requestUri);
  }

  if(requestMethod == HTTP_POST) {
    return handlePost(server, requestMethod, requestUri);
  }
  return handleDisallowedMethods(server, requestMethod, requestUri);
}

bool MyHomeNew::ConfigOps::handleDisallowedMethods(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) {
  server.setContentLength(0);
  server.send(405);
  return true;
}

bool MyHomeNew::ConfigOps::handlePost(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& reqBody = jsonBuffer.parseObject(server.arg("plain"));
  String ssid = reqBody["ssid"];
  String password = reqBody["password"];
  String stPassword = reqBody[F("st_password")];

  // Save ssid and password if provided.
  if(ssid != "") {
    Serial.print("ssid: ");
    Serial.println(ssid);

    if(password == "") {
      Serial.println("Setting empty password");
    }
    else {
      Serial.print("Setting password len ");
      Serial.println(password.length());
    }

    Config::getInstance()
      ->setValue(CONFIG_SSID, ssid.c_str())
      ->setValue(CONFIG_PASSWORD, password.c_str());
  }

  if(stPassword != "") {
    Config::getInstance()
      ->setValue(CONFIG_ST_PASSWORD, stPassword.c_str());
  }

  Config::getInstance()->save();

  server.setContentLength(0);
  server.send(204);
  return true;
}

bool MyHomeNew::ConfigOps::handleGet(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) {
  Config* _config = Config::getInstance();
  String endQuote = String("\"");
  String resp = "{";
  resp += String("\"ssid\":\"") + _config->getValue(CONFIG_SSID) + endQuote;
  resp += ",\"password_len\":" + String(strlen(_config->getValue(CONFIG_PASSWORD)));
  resp += ",\"st_password_len\":" + String(strlen(_config->getValue(CONFIG_ST_PASSWORD)));
  resp += String(",\"type\":\"") + _config->getValue(CONFIG_TYPE) + endQuote;
  for(uint8_t i = 0; i < 4; i++) {
    resp += ",\"lead" + String(i) + "\":" + String((unsigned int)_config->getLeadVal((ConfigKeys)(CONFIG_LEAD1 + i)));
  }
  resp += "}";

  server.send(200, "application/json", resp);
  return true;
}

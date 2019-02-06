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
  if(ssid != "") {
    Serial.print("ssid: ");
    Serial.println(ssid);
  }

  if(password == "") {
    Serial.println("Setting empty password");
  }
  else {
    Serial.print("Setting password len ");
    Serial.println(password.length());
  }

  Config::getInstance()
    ->setValue(CONFIG_SSID, ssid.c_str())
    ->setValue(CONFIG_PASSWORD, password.c_str())
    ->save();

  Serial.println("setting content length 0");
  server.setContentLength(0);
  Serial.println("sending 204");
  server.send(204);
  return true;
}

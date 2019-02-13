#include "smartops.h"
#include "../common/utils.h"
#include "../operations/capabilities.h"

const String MyHomeNew::SmartOps::s_path = "/v1/ops";

bool MyHomeNew::SmartOps::canHandle(HTTPMethod method, String uri) {
    return uri.indexOf(s_path) == 0;
}

bool MyHomeNew::SmartOps::canUpload(String uri) {
  return false;
}

bool MyHomeNew::SmartOps::handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) {
  if(requestMethod == HTTP_POST) {
    return handlePost(server, requestMethod, requestUri);
  }
  if(requestMethod == HTTP_GET) {
    return handleGet(server, requestMethod, requestUri);
  }

  return handleDisallowedMethods(server, requestMethod, requestUri);
}

bool MyHomeNew::SmartOps::handleDisallowedMethods(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) {
  server.setContentLength(0);
  server.send(405);
  return true;
}

bool MyHomeNew::SmartOps::handleGet(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) {
  String device = server.arg("dev");
  String state = server.arg("state");

  if(!Utils::isInt(device)) {
    server.send(400);
    return true;
  }

  // Get mode
  if(state == "") {
    String curState = Capabilities::getState(device.toInt()) ? "on" : "off";
    String resp = "{";
    resp += "\"dev\":\"" + device + "\",";
    resp += "\"state\":\"" + curState + "\"";
    resp += "}";
    server.send(200, "application/json", resp);
    return true;
  }

  // set mode
  Capabilities::setState(device.toInt(), state == "on");
  server.send(202);

  return true;
}

bool MyHomeNew::SmartOps::handlePost(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) {
  return true;
}

#include "WebSocketHanlder.h"
#include "common/CryptoUtils.h"
#include "config/Config.h"
#include "string.h"
#include "operations/capabilities.h"

MyHomeNew::WebSocketHandler* MyHomeNew::WebSocketHandler::s_instance = NULL;
String MyHomeNew::WebSocketHandler::s_failResp = "{\"status\":\"FAIL\"}";
String MyHomeNew::WebSocketHandler::s_okResp = "{\"status\":\"OK\"}";
uint32_t MyHomeNew::WebSocketHandler::m_lastDiscTime = 0;

MyHomeNew::WebSocketHandler::WebSocketHandler() {
  m_respStr = "";
  m_flags = 0;
  m_cyclesTracker = -1;
  m_client = new WebSocketsClient();
  m_lastDiscTime = 0;
}

MyHomeNew::WebSocketHandler::~WebSocketHandler() {
  if(m_client) {
    delete m_client;
  }
}

MyHomeNew::WebSocketHandler* MyHomeNew::WebSocketHandler::getInstance() {
  if(!s_instance) {
    s_instance = new WebSocketHandler();
  }
  return s_instance;
}

void MyHomeNew::WebSocketHandler::onWSEvent(WStype_t type, uint8_t * payload, size_t length) {
  char* input;
  String key;
  switch (type)
  {
  case WStype_ERROR:
    Serial.println("WSC_ERR");
    break;
  case WStype_CONNECTED:
    Serial.println("WSC_CON");
    m_lastDiscTime = 0;
    break;
  case WStype_DISCONNECTED:
    Serial.println("WSC_DIS");

    if(m_lastDiscTime == 0){
      m_lastDiscTime = millis();
    }
    else {
      // We have not been able to connect for last 5 mins.
      // Simply restart.
      if((millis() - m_lastDiscTime) > 300000ul) {
        ESP.restart();
      }
    }

    break;
  case WStype_TEXT:
    input = new char[length + 1];
    memcpy(input, payload, length);
    input[length] = '\0';
    key = Config::getInstance()->getValue(CONFIG_AES_KEY);
    WebSocketHandler::getInstance()->handleEvent(decrypt(input, key.c_str()));
    delete input;
    break;
  default:
    break;
  }
}

void MyHomeNew::WebSocketHandler::handleEvent(const String& jsonStr) {
  Config* _config = Config::getInstance();
  StaticJsonBuffer<512> jsonBuffer;
  JsonObject& obj = jsonBuffer.parseObject(jsonStr);
  if(!obj.success()) {
    sendEncrypted(s_failResp);
    return ;
  }

  String action = obj.get<String>("action");
  String data = obj.get<String>("data");

  // Update the key!
  if(action == "update-key") {
    Serial.println("SET_KEY");
    _config->setValue(CONFIG_AES_KEY, data.c_str());
    // Update the header in case we get disconnected.
    m_client->setExtraHeaders(getHeaders().c_str());
    m_flags |= SHOULD_SAVE_CONFIG;
    sendEncrypted(s_okResp);
    m_cyclesTracker += 2;
    return;
  }

  // Update the username!
  if(action == "update-username") {
    Serial.println("SET_USR");
    _config->setValue(CONFIG_USER, data.c_str());
    m_flags |= SHOULD_SAVE_CONFIG;
    sendEncrypted(s_okResp);
    m_cyclesTracker += 2;
    return;
  }

  // Set state
  if(action == "set-state") {
    Serial.println("SET_STE");
    uint8_t eqIdx = data.indexOf('=');
    uint8_t devId = data.substring(0, eqIdx).toInt();
    uint8_t brightness = data.substring(eqIdx + 1).toInt();
    Capabilities::setState(devId, brightness);
    m_flags |= SHOULD_SAVE_CONFIG;
    sendEncrypted(s_okResp);
    m_cyclesTracker += 2;
    return;
  }

  // Get state
  if(action == "get-state") {
    Serial.println("GET_STE");
    String state = s_okResp.substring(0, s_okResp.length() - 1) + 
      String(",\"type\":\"") + _config->getValue(CONFIG_TYPE) + "\"";
    for(uint8_t i = 0; i < 4; i++) {
      state += ",\"lead" + String(i) + "\":" + String((unsigned int)_config->getLeadVal((ConfigKeys)(CONFIG_LEAD1 + i)));
    }
    state += "}";
    sendEncrypted(state);
    return;
  }
}

void MyHomeNew::WebSocketHandler::sendEncrypted() {
  if(m_respStr == "") {
    Serial.println("BUF_EMY");
    return;
  }

  String encrypted = encrypt(m_respStr.c_str(), Config::getInstance()->getValue(CONFIG_AES_KEY));
  m_client->sendTXT(encrypted);
  m_respStr = "";
}

void MyHomeNew::WebSocketHandler::sendEncrypted(const String& resp) {
  if(m_respStr != "") { // first clear old buffer
    Serial.println("OLD_SND");
    sendEncrypted();
  }
  m_respStr = resp;
  m_cyclesTracker += 4;
}

String MyHomeNew::WebSocketHandler::getHeaders() {
  Config* _config = Config::getInstance();
  String hostname = "myhomenew-" + String(ESP.getChipId(), HEX);
  String password = encrypt(hostname.c_str(), _config->getValue(CONFIG_AES_KEY));
  String auth = "Authorization: " + String(_config->getValue(CONFIG_USER)) + ":" + password;
  return auth;
}

void MyHomeNew::WebSocketHandler::connect() {
  m_client->setExtraHeaders(getHeaders().c_str());
  m_client->begin(Config::getInstance()->getValue(CONFIG_HOST), 8020, "/v1/ws", "myhomenew-device");
  m_client->onEvent(onWSEvent);
  m_client->enableHeartbeat(15000, 4000, 3);
  m_client->setReconnectInterval(5000);
}

void MyHomeNew::WebSocketHandler::loop() {
  if(m_cyclesTracker == 2) {
    Serial.println("REL_SND");
    sendEncrypted();
  }

  if(m_cyclesTracker == 0 && (m_flags & SHOULD_SAVE_CONFIG)) {
    Serial.println("CNF_SAV");
    Config::getInstance()->save();
    m_flags &= ~SHOULD_SAVE_CONFIG;
  }

  if(m_cyclesTracker >= 0) {
    --m_cyclesTracker;
  }

  m_client->loop();
}

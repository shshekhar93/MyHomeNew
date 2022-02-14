#include "WString.h"
#include "WebSocketHanlder.h"
#include "common/CryptoUtils.h"
#include "config/Config.h"
#include "operations/updater.h"
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
  m_lastFrameNum = 0;
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
  WebSocketHandler* _instance = WebSocketHandler::getInstance();
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
    
    // Regenerate session key and update headers.
    _instance->updateHeaders();

    if(m_lastDiscTime == 0){
      m_lastDiscTime = millis();
    }
    else {
      delay(1000); // delay for some time.
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
    _instance->handleEvent(decrypt(input, _instance->m_sessKey));
    delete input;
    break;
  default:
    break;
  }
}

/**
 * This assumes we are trying to send a JSON object.
 * Isn't really a problem right now since we only send
 * objects. Refactor this, if that changes in future.
 */
String addFrameNumToJSON(String resp, int frameNum) {
  return resp.substring(0, resp.length() - 1) +
    ",\"frame-num\":" + String(frameNum) + "}";
}

void MyHomeNew::WebSocketHandler::handleEvent(const String& jsonStr) {
  Config* _config = Config::getInstance();
  StaticJsonDocument<512> obj;
  auto error = deserializeJson(obj, jsonStr);
  if(error) {
    sendEncrypted(s_failResp);
    return ;
  }

  String action = obj["action"];
  String data = obj["data"];
  int frameNum = 0; 
  
  if(obj["frame-num"].is<int>()) {
    frameNum = obj["frame-num"].as<int>();
  }

  if(frameNum == 0 || frameNum <= m_lastFrameNum) {
    return sendEncrypted(s_failResp);
  }

  // Ideally we should also check if frameNum is about to overflow and restart.
  // However, 2 billion seems like a high enough number to be safe.
  m_lastFrameNum = frameNum;

  String sucessWithFrameNum = addFrameNumToJSON(s_okResp, frameNum);
  String failureWithFrameNum = addFrameNumToJSON(s_failResp, frameNum);

  if(action == "confirm-session") {
    Serial.println(F("CNF_KEY"));
    String storedSessKey = byteArrToHexStr(m_sessKey, 16);
    if(storedSessKey == data) {
      Serial.println(F("CNF_KEY_SCS"));
      m_flags |= SESSION_KEY_VERIFIED;
      sendEncrypted(sucessWithFrameNum);
    }
    else {
      sendEncrypted(failureWithFrameNum);
    }
    return;
  }

  // The server hasn't proven itself yet. don't process anything else.
  if(!(m_flags | SESSION_KEY_VERIFIED)) {
    return sendEncrypted(failureWithFrameNum);
  }

  // Update the key!
  if(action == "update-key") {
    Serial.println(F("SET_KEY"));
    _config->setValue(CONFIG_AES_KEY, data.c_str());
    m_flags |= SHOULD_SAVE_CONFIG;
    sendEncrypted(sucessWithFrameNum);
    m_cyclesTracker += 2;
    return;
  }

  // Update the username!
  if(action == "update-username") {
    Serial.println(F("SET_USR"));
    _config->setValue(CONFIG_USER, data.c_str());
    m_flags |= SHOULD_SAVE_CONFIG;
    sendEncrypted(sucessWithFrameNum);
    m_cyclesTracker += 2;
    return;
  }

  // Set state
  if(action == "set-state") {
    Serial.println(F("SET_STE"));
    uint8_t eqIdx = data.indexOf('=');
    uint8_t devId = data.substring(0, eqIdx).toInt();
    uint8_t brightness = data.substring(eqIdx + 1).toInt();
    Capabilities::setState(devId, brightness);
    m_flags |= SHOULD_SAVE_CONFIG;
    sendEncrypted(sucessWithFrameNum);
    m_cyclesTracker += 2;
    return;
  }

  // Get state
  if(action == "get-state") {
    Serial.println(F("GET_STE"));
    String state = s_okResp.substring(0, s_okResp.length() - 1) + 
      String(",\"frame-num\":") + String(frameNum) +
      String(",\"type\":\"") + _config->getValue(CONFIG_TYPE) + "\"" +
      String(",\"version\":\"") + Updater::getFullVersion() + "\"";
    for(uint8_t i = 0; i < 4; i++) {
      state += ",\"lead" + String(i) + "\":" + String((unsigned int)_config->getLeadVal((ConfigKeys)(CONFIG_LEAD1 + i)));
    }
    state += "}";
    sendEncrypted(state);
    return;
  }

  // Update Firmware
  if(action == "firmware-update") {
    Serial.println(F("Firmware update req"));
    Updater::update(data.c_str());
  }

  // Update FS
  if(action == "spiffs-update") {
    Serial.println(F("SPIFFS update req"));
    Updater::updateFS(data.c_str());
  }
}

void MyHomeNew::WebSocketHandler::sendEncrypted() {
  if(m_respStr == "") {
    Serial.println("BUF_EMY");
    return;
  }

  String encrypted = encrypt(m_respStr.c_str(), m_sessKey);
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

void MyHomeNew::WebSocketHandler::updateHeaders() {
  Config* _config = Config::getInstance();

  m_flags &= ~SESSION_KEY_VERIFIED;
  m_lastFrameNum = 0;
  generateIV(m_sessKey);
  String sessionKeyStr = byteArrToHexStr(m_sessKey, 16);

  String payload = "myhomenew-" + String(ESP.getChipId(), HEX) + "|" + sessionKeyStr;
  String password = encrypt(payload.c_str(), _config->getValue(CONFIG_AES_KEY));
  String auth = "Authorization: " + String(_config->getValue(CONFIG_USER)) + ":" + password;
  m_client->setExtraHeaders(auth.c_str());
}

void MyHomeNew::WebSocketHandler::connect() {
  updateHeaders();
  m_client->begin(Config::getInstance()->getValue(CONFIG_HOST), 80, "/v1/ws", "myhomenew-device");
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

#include "Config.h"
#include "../common/StringConstants.h"
#include "FS.h"
#include "../common/utils.h"

MyHomeNew::Config* MyHomeNew::Config::s_intance = NULL;

MyHomeNew::Config::Config() {
  strcpy(m_ssid, "");
  strcpy(m_password, "");
  strcpy(m_stPassword, "");

  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println(FStr(ConfigLoadFailed));
    return;
  }

  StaticJsonBuffer<1024> jsonBuffer;
  JsonObject& config = jsonBuffer.parseObject(configFile);

  if (!config.success()) {
    Serial.println(FStr(ConfigParseFailed));
    return;
  }

  strcpy(m_ssid, config.get<String>("ssid").c_str());
  strcpy(m_password, config.get<String>("pass").c_str());
  strcpy(m_stPassword, config.get<String>("st_pass").c_str());
  
  for(uint8_t i = 0; i < 4; i++) {
    String key = "lead" + (i + 1);
    String val = config.get<String>(key);
    if(!Utils::isInt(val)) {
      m_leads[i] = 0;
      continue;
    }
    m_leads[i] = val.toInt();
  }

  configFile.close();
}

MyHomeNew::Config* MyHomeNew::Config::getInstance() {
  if(s_intance == NULL) {
    s_intance = new Config();
  }
  return s_intance;
}

const char* MyHomeNew::Config::getValue(ConfigKeys key) {
  switch(key) {
    case CONFIG_SSID:
      return m_ssid;
    case CONFIG_PASSWORD:
      return m_password;
    case CONFIG_ST_PASSWORD:
      return m_stPassword;
    default:
      break;
  }
  return p_empty;
}

uint8_t MyHomeNew::Config::getLeadVal(ConfigKeys key) {
  switch (key) {
    case CONFIG_LEAD1:
      return m_leads[0];
    case CONFIG_LEAD2:
      return m_leads[1];
    case CONFIG_LEAD3:
      return m_leads[2];
    case CONFIG_LEAD4:
      return m_leads[3];
    default:
      break;
  }
  return 0;
}

MyHomeNew::Config* MyHomeNew::Config::setValue(ConfigKeys key, const char* value) {
  switch(key) {
    case CONFIG_SSID:
      strcpy(m_ssid, value);
      break;
    case CONFIG_PASSWORD:
      strcpy(m_password, value);
      break;
    case CONFIG_ST_PASSWORD:
      strcpy(m_stPassword, value);
      break;
    default:
      break;
  }
  return this;
}

MyHomeNew::Config* MyHomeNew::Config::setLeadVal(ConfigKeys key, uint8_t val) {
  switch (key)
  {
    case CONFIG_LEAD1:
      m_leads[0] = val;
      break;
    case CONFIG_LEAD2:
      m_leads[1] = val;
      break;
    case CONFIG_LEAD3:
      m_leads[2] = val;
      break;
    case CONFIG_LEAD4:
      m_leads[3] = val;
      break;
    default:
      break;
  }
  return this;
}

bool MyHomeNew::Config::save() {
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println(FStr(ConfigLoadFailed));
    return false;
  }

  StaticJsonBuffer<1024> jsonBuffer;
  JsonObject& obj = jsonBuffer.createObject();
  obj["ssid"] = String(m_ssid);
  obj["pass"] = String(m_password);
  obj["st_pass"] = String(m_stPassword);

  for(uint8_t i = 0; i < 4; i++) {
    String key = "lead" + (i + 1);
    obj[key] = String((unsigned int)m_leads[i]);
  }

  obj.printTo(configFile);
  configFile.close();
  return true;
}

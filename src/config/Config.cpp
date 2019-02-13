#include "Config.h"
#include "../common/StringConstants.h"
#include "FS.h"

MyHomeNew::Config* MyHomeNew::Config::s_intance = NULL;

MyHomeNew::Config::Config() {
  strcpy(m_ssid, "");
  strcpy(m_password, "");
  strcpy(m_stPassword, "");
  m_leads = 0;

  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println(FStr(ConfigLoadFailed));
    return;
  }

  StaticJsonBuffer<1024> jsonBuffer;
  JsonObject& config = jsonBuffer.parseObject(configFile);

  if (!config.success()) {
    Serial.println(FStr(ConfigParseFailed));
  }

  strcpy(m_ssid, config.get<String>("ssid").c_str());
  strcpy(m_password, config.get<String>("pass").c_str());
  strcpy(m_stPassword, config.get<String>("st_pass").c_str());
  m_leads = 0;
  
  for(uint8_t i = 0; i < 4; i++) {
    String key = "lead" + (i + 1);
    String val = config.get<String>(key);
    if(val == p_on) {
      m_leads |= (1 << i);
    } else {
      m_leads &= ~(1 << i);
    }
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
    case CONFIG_LEAD1:
      return (m_leads & 0x01) ? p_on : p_off;
    case CONFIG_LEAD2:
      return (m_leads & 0x02) ? p_on : p_off;
    case CONFIG_LEAD3:
      return (m_leads & 0x04) ? p_on : p_off;
    case CONFIG_LEAD4:
      return (m_leads & 0x08) ? p_on : p_off;
  }
  return p_empty;
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
    case CONFIG_LEAD1:
      setOrClearLead(0x01, value);
      break;
    case CONFIG_LEAD2:
      setOrClearLead(0x02, value);
      break;
    case CONFIG_LEAD3:
      setOrClearLead(0x04, value);
      break;
    case CONFIG_LEAD4:
      setOrClearLead(0x08, value);
      break;
  }
  return this;
}

void MyHomeNew::Config::setOrClearLead(uint8_t mask, const char* val) {
  if(!strcmp(val, p_on)) {
    m_leads |= mask;
  }
  else {
    m_leads &= ~mask;
  }
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
    String value = (m_leads & (1 << i)) ? p_on : p_off;
    obj[key] = String(value);
  }

  obj.printTo(configFile);
  configFile.close();
  return true;
}

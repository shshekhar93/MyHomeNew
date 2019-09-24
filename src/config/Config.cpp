#include "Config.h"
#include "../common/StringConstants.h"
#include "FS.h"
#include "../common/utils.h"

MyHomeNew::Config* MyHomeNew::Config::s_intance = NULL;

MyHomeNew::Config::Config() {
  is_configLoaded = false;
  strcpy(m_ssid, p_empty);
  strcpy(m_password, p_empty);
  strcpy(m_stPassword, p_empty);
  strcpy(m_host, p_empty);
  strcpy(m_aesKey, p_empty);
  strcpy(m_user, p_empty);
  m_isActiveStateLow = false;

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
  strcpy(m_apMac, config.get<String>("ap_mac").c_str());
  strcpy(m_stMac, config.get<String>("st_mac").c_str());
  strcpy(m_type, config.get<String>("type").c_str());

  strcpy(m_user, config.get<String>("user").c_str());
  strcpy(m_aesKey, config.get<String>("aes_key").c_str());
  strcpy(m_host, config.get<String>("host").c_str());

  m_isActiveStateLow = config.get<String>("active_state") == "low";
  
  for(uint8_t i = 0; i < 4; i++) {
    String key = "lead" + i;
    String val = config.get<String>(key);
    if(!Utils::isInt(val)) {
      m_leads[i] = 0;
      continue;
    }
    m_leads[i] = val.toInt();
  }

  configFile.close();
  is_configLoaded = true;
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
    case CONFIG_AP_MAC:
      return m_apMac;
    case CONFIG_ST_MAC:
      return m_stMac;
    case CONFIG_TYPE:
      return m_type;
    case CONFIG_HOST:
      return m_host;
    case CONFIG_AES_KEY:
      return m_aesKey;
    case CONFIG_USER:
      return m_user;
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
    case CONFIG_HOST:
      strcpy(m_host, value);
      break;
    case CONFIG_AES_KEY:
      strcpy(m_aesKey, value);
      break;
    case CONFIG_USER:
      strcpy(m_user, value);
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

bool MyHomeNew::Config::isActiveStateLow() {
  return m_isActiveStateLow;
}

bool MyHomeNew::Config::save() {
  if(is_configLoaded == false) {
    return false;
  }
  
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
  obj["active_state"] = String(m_isActiveStateLow ? "low" : "high");
  obj["ap_mac"] = String(m_apMac);
  obj["st_mac"] = String(m_stMac);
  obj["type"] = String(m_type);

  obj["user"] = String(m_user);
  obj["aes_key"] = String(m_aesKey);
  obj["host"] = String(m_host);

  for(uint8_t i = 0; i < 4; i++) {
    String key = "lead" + i;
    obj[key] = String((unsigned int)m_leads[i]);
  }

  obj.printTo(configFile);
  configFile.close();
  return true;
}

bool MyHomeNew::Config::saveBackupConfig(const char* user, const char* aesKey, const char* host) {
  File backupFileR = SPIFFS.open("/backup.json", "r");
  if (!backupFileR) {
    return false;
  }

  StaticJsonBuffer<1024> jsonBuffer;
  JsonObject& backup = jsonBuffer.parseObject(backupFileR);
  backupFileR.close();

  if(!backup.success()) {
    // ignore for now!
    return false;
  }

  backup["user"] = user;
  backup["aes_key"] = aesKey;
  backup["host"] = host;

  File backupFileW = SPIFFS.open("/backup.json", "r");
  if (!backupFileW) {
    return false;
  }
  backup.printTo(backupFileW);
  backupFileW.close();
  return true;
}

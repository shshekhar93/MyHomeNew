#include "Config.h"
#include "FS.h"
#include "../common/utils.h"

MyHomeNew::Config* MyHomeNew::Config::s_intance = NULL;

String STATE_FILENAME = "/state.json";
String LEAD_PREFIX = "lead";
String READ_MODE = "r";
String WRITE_MODE = "w";

String SETTINGS_FILENAME = "/settings.json";
String BACKUP_FILENAME = "/backup.json";
String ACTIVE_STATE_KEY = "active_state";

bool MyHomeNew::Config::loadState() {
  File stateFile = SPIFFS.open(STATE_FILENAME, READ_MODE.c_str());
  if(!stateFile) {
    Serial.println(STATE_FILENAME);
    Serial.println(FStr(FileOpenFailed));
    return false;
  }
  StaticJsonDocument<1024> state;
  auto error = deserializeJson(state, stateFile);
  if(error) {
    Serial.println(STATE_FILENAME);
    Serial.println(ConfigParseFailed);
    stateFile.close();
    return false;
  }

  for(uint8_t i = 0; i < 4; i++) {
    String key = LEAD_PREFIX + i;
    String val = state[key];
    if(!Utils::isInt(val)) {
      m_leads[i] = 0;
      continue;
    }
    m_leads[i] = val.toInt();
  }
  stateFile.close();
  return true;
}

bool MyHomeNew::Config::loadSettings(String filename) {
  if(filename == "") {
    filename = SETTINGS_FILENAME;
  }

  File settingsFile = SPIFFS.open(filename, READ_MODE.c_str());
  if(!settingsFile) {
    Serial.println(SETTINGS_FILENAME);
    Serial.println(FStr(FileOpenFailed));
    if(filename != BACKUP_FILENAME) {
      return loadSettings(BACKUP_FILENAME);
    }
    return false;
  }
  StaticJsonDocument<1024> settings;
  auto error = deserializeJson(settings, settingsFile);
  if(error) {
    Serial.println(SETTINGS_FILENAME);
    Serial.println(ConfigParseFailed);
    settingsFile.close();
    if(filename != BACKUP_FILENAME) {
      return loadSettings(BACKUP_FILENAME);
    }
    return false;
  }
  strcpy(m_stPassword, settings[FStr(ConfKeyStPass)].as<String>().c_str());
  strcpy(m_apMac, settings[FStr(ConfKeyApMac)].as<String>().c_str());
  strcpy(m_stMac, settings[FStr(ConfKeyStMac)].as<String>().c_str());
  strcpy(m_type, settings[FStr(ConfKeyType)].as<String>().c_str());
  strcpy(m_host, settings[FStr(ConfKeyHost)].as<String>().c_str());
  strcpy(m_aesKey, settings[FStr(ConfKeyAESKey)].as<String>().c_str());
  strcpy(m_user, settings[FStr(ConfKeyUser)].as<String>().c_str());
  m_isActiveStateLow = settings["active_state"].as<String>() == "low";

  settingsFile.close();
  return true;
}

MyHomeNew::Config::Config() {
  is_stateLoaded = false;
  is_settingsLoaded = false;
  strcpy(m_apMac, EMPTY_STR);
  strcpy(m_stMac, EMPTY_STR);
  strcpy(m_type, EMPTY_STR);
  strcpy(m_stPassword, EMPTY_STR);
  strcpy(m_host, EMPTY_STR);
  strcpy(m_aesKey, EMPTY_STR);
  strcpy(m_user, EMPTY_STR);
  m_isActiveStateLow = false;

  is_settingsLoaded = loadSettings();
  is_stateLoaded = loadState();
}

MyHomeNew::Config* MyHomeNew::Config::getInstance() {
  if(s_intance == NULL) {
    s_intance = new Config();
  }
  return s_intance;
}

const char* MyHomeNew::Config::getValue(ConfigKeys key) {
  switch(key) {
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
  return EMPTY_STR;
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
  is_settingsDirty = true;
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
  is_stateDirty = true;
  return this;
}

bool MyHomeNew::Config::isActiveStateLow() {
  return m_isActiveStateLow;
}

bool MyHomeNew::Config::saveState() {
  File stateFile = SPIFFS.open(STATE_FILENAME, WRITE_MODE.c_str());
  if(!stateFile) {
    Serial.println(STATE_FILENAME);
    Serial.println(FStr(FileOpenFailed));
    return false;
  }
  StaticJsonDocument<1024> jsonDoc;
  JsonObject state = jsonDoc.to<JsonObject>();
  for(uint8_t i = 0; i < 4; i++) {
    String key = LEAD_PREFIX + i;
    state[key] = String((unsigned int)m_leads[i]);
  }
  serializeJson(state, stateFile);
  stateFile.close();
  return true;
}

bool MyHomeNew::Config::saveSettings() {
  StaticJsonDocument<1024> jsonDoc;
  JsonObject settingsObject = jsonDoc.to<JsonObject>();

  settingsObject[FStr(ConfKeyApMac)]= m_apMac;
  settingsObject[FStr(ConfKeyStMac)] = m_stMac;
  settingsObject[FStr(ConfKeyType)] = m_type;
  settingsObject[FStr(ConfKeyActiveState)] = String(m_isActiveStateLow ? "low" : "high");

  settingsObject[FStr(ConfKeyStPass)] = m_stPassword;
  settingsObject[FStr(ConfKeyHost)] = m_host;
  settingsObject[FStr(ConfKeyUser)] = m_user;
  settingsObject[FStr(ConfKeyAESKey)] = m_aesKey;

  bool retVal = false;
  File settingsFile = SPIFFS.open(SETTINGS_FILENAME, WRITE_MODE.c_str());
  if(settingsFile) {
    serializeJson(settingsObject, settingsFile);
    settingsFile.close();
    retVal = true;
  }
  File backupFile = SPIFFS.open(BACKUP_FILENAME, WRITE_MODE.c_str());
  if(backupFile) {
    serializeJson(settingsObject, backupFile);
    backupFile.close();
    retVal = retVal && true;
  }
  return retVal;
}

bool MyHomeNew::Config::save() {
  bool retVal = true;
  if(is_stateDirty) {
    is_stateDirty = false;
    
    if(is_stateLoaded) {
      retVal = saveState();
    } else {
      retVal = false;
      Serial.println(F("STA_NLD"));
    }
  }
  
  if(is_settingsDirty) {
    is_settingsDirty = false;
    if(is_settingsLoaded) {
      retVal = retVal && saveSettings();
    }
    else {
      retVal = false;
      Serial.println(F("SET_NLD"));
    }
  }
  
  return retVal;
}

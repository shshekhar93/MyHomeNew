#include "Config.h"
#include "../common/StringConstants.h"
#include "FS.h"

MyHomeNew::Config* MyHomeNew::Config::s_intance = NULL;

MyHomeNew::Config::Config() {
  // File configFile = SPIFFS.open("/config.json", "r");
  // if (!configFile) {
  //   Serial.println(FStr(ConfigLoadFailed));
  //   return;
  // }

  // size_t size = configFile.size();
  // if (size > 1024) {
  //   Serial.println("Config file size is too large");
  //   return;
  // }
  // char* buf = new char[size + 1];

  // configFile.readBytes(buf, size);
  // buf[size] = '\0';

  // StaticJsonBuffer<200> jsonBuffer;
  // m_config = &jsonBuffer.parseObject(buf);

  // if (!m_config->success()) {
  //   Serial.println(FStr(ConfigParseFailed));
  //   m_config = NULL;
  // }
  // configFile.close();
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
  }
  return "";
}

MyHomeNew::Config* MyHomeNew::Config::setValue(ConfigKeys key, const char* value) {
  switch(key) {
    case CONFIG_SSID:
      m_ssid = new char[strlen(value)];
      strcpy(m_ssid, value);
      break;
    case CONFIG_PASSWORD:
      m_password = new char[strlen(value)];
      strcpy(m_password, value);
      break;
  }
  return this;
}

bool MyHomeNew::Config::save() {
  // File configFile = SPIFFS.open("/config.json", "w+");
  // if (!configFile) {
  //   Serial.println(FStr(ConfigLoadFailed));
  //   return false;
  // }

  // m_config->printTo(configFile);
  // configFile.close();
  // return true;
  return false;
}

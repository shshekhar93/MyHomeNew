#include <cstddef>
#include <ArduinoJson.h>
#include "common/StringConstants.h"

namespace MyHomeNew {
  enum ConfigKeys {
    CONFIG_AP_MAC,
    CONFIG_ST_MAC,
    CONFIG_TYPE,
    CONFIG_ST_PASSWORD,
    CONFIG_USER,
    CONFIG_AES_KEY,
    CONFIG_HOST,
    CONFIG_LEAD1,
    CONFIG_LEAD2,
    CONFIG_LEAD3,
    CONFIG_LEAD4
  };

  class Config {
    private:
      Config();
      bool is_stateLoaded;
      bool is_settingsLoaded;
      bool is_stateDirty;
      bool is_settingsDirty;

      bool m_isActiveStateLow;
      char m_apMac[13];
      char m_stMac[13];
      char m_type[32];

      char m_user[33];
      char m_aesKey[33];
      char m_host[33];
      char m_stPassword[33];
      
      uint8_t m_leads[8];

      bool loadState();
      bool loadSettings(String filename = "");
      bool saveState();
      bool saveSettings();

      static Config* s_intance;

    public:
      static Config* getInstance();
      const char* getValue(ConfigKeys key);
      uint8_t getLeadVal(ConfigKeys);
      Config* setValue(ConfigKeys key, const char* value);
      Config* setLeadVal(ConfigKeys, uint8_t);
      bool isActiveStateLow();
      bool save();
  };
}

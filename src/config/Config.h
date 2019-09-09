#include <cstddef>
#include <ArduinoJson.h>

namespace MyHomeNew {
  enum ConfigKeys {
    CONFIG_AP_MAC,
    CONFIG_ST_MAC,
    CONFIG_TYPE,
    CONFIG_SSID,
    CONFIG_PASSWORD,
    CONFIG_ST_PASSWORD,
    CONFIG_LEAD1,
    CONFIG_LEAD2,
    CONFIG_LEAD3,
    CONFIG_LEAD4
  };

  class Config {
    private:
      Config();
      char m_apMac[12];
      char m_stMac[12];
      char m_type[32];
      char m_ssid[32];
      char m_password[32];
      char m_stPassword[32];
      bool m_isActiveStateLow;
      uint8_t m_leads[8];

      const char p_empty[1] = {'\0'};

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

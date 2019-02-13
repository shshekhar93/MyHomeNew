#include <cstddef>
#include <ArduinoJson.h>

namespace MyHomeNew {
  enum ConfigKeys {
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
      char m_ssid[32];
      char m_password[32];
      char m_stPassword[32];
      uint8_t m_leads;

      const char p_on[3] = {'o', 'n', '\0'};
      const char p_off[4] = {'o', 'f', 'f', '\0'};
      const char p_empty[1] = {'\0'};

      static Config* s_intance;

      void setOrClearLead(uint8_t mask, const char*);

    public:
      static Config* getInstance();
      const char* getValue(ConfigKeys key);
      Config* setValue(ConfigKeys key, const char* value);
      bool save();
  };
}

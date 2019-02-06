#include <cstddef>
#include <ArduinoJson.h>

namespace MyHomeNew {
  enum ConfigKeys {
    CONFIG_SSID,
    CONFIG_PASSWORD
  };

  class Config {
    private:
      Config();
      char* m_ssid;
      char* m_password;
      static Config* s_intance;

    public:
      static Config* getInstance();
      const char* getValue(ConfigKeys key);
      Config* setValue(ConfigKeys key, const char* value);
      bool save();
  };
}

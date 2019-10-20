#include "Arduino.h"

namespace MyHomeNew {
  class WiFiSetup {
    private:
      static uint32_t s_btnPressStart;
      static void forgetWiFiCredsAndRestart();

    public:
      static void setupWithWiFiManager(String hostname);
      static void setupMacAddress();
      static void loop();
  };
}
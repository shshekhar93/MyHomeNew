#include "Arduino.h"

namespace MyHomeNew {
  class WiFiSetup {
    private: 
      static void forgetWiFiCredsAndRestart();

    public:
      static void setupWithWiFiManager(String hostname);
      static void setupMacAddress();
  };
}
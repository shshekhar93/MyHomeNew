#include <WString.h>

namespace MyHomeNew {
  static const char APMacAddress[] PROGMEM = "CA4396000000";
  static const char STAMacAddress[] PROGMEM = "DA4396000000";

  // Info messages
  static const char MountingMsg[] PROGMEM = "Mounting FS...";


  // Error messages
  static const char MountingFailed[] PROGMEM = "Failed to mount file system";
  static const char ConfigLoadFailed[] PROGMEM = "Failed to open config file";
  static const char ConfigParseFailed[] PROGMEM = "Failed to parse config file";

  const __FlashStringHelper* FStr(const char*);
}

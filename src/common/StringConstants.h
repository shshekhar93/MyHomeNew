#include <WString.h>

#ifndef __MYHOMENEW_STRING_CONST__
#define __MYHOMENEW_STRING_CONST__

namespace MyHomeNew {
  const char EMPTY_STR[1] = {'\0'};

  static const char APMacAddress[] PROGMEM = "CA4396000000";
  static const char STAMacAddress[] PROGMEM = "DA4396000000";

  // Info messages
  static const char MountingMsg[] PROGMEM = "Mounting FS...";


  // Error messages
  static const char MountingFailed[] PROGMEM = "Failed to mount file system";
  static const char FileOpenFailed[] PROGMEM = "OPN_ERR";
  static const char ConfigParseFailed[] PROGMEM = "PAR_ERR";

  // Config keys
  static const char ConfKeyApMac[] PROGMEM = "ap_mac";
  static const char ConfKeyStMac[] PROGMEM = "st_mac";
  static const char ConfKeyStPass[] PROGMEM = "st_pass";
  static const char ConfKeyUser[] PROGMEM = "user";
  static const char ConfKeyAESKey[] PROGMEM = "aes_key";
  static const char ConfKeyHost[] PROGMEM = "host";
  static const char ConfKeyType[] PROGMEM = "type";
  static const char ConfKeyActiveState[] PROGMEM = "active_state";

  const __FlashStringHelper* FStr(const char*);
}

#endif

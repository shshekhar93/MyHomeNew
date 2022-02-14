#ifndef __MYHOMENEW_UPDATER__
#define __MYHOMENEW_UPDATER__
#include "WString.h"
#include "ESP8266httpUpdate.h"

namespace MyHomeNew {
  class Updater {
    private:
    static void handleUpdateResult(HTTPUpdateResult);
    public:
    static const char m_softVersion[6];
    static const char m_hardVersion[6];

    static String getFullVersion();
    static void update(const char*);
    static void updateFS(const char*);
  };
}

#endif

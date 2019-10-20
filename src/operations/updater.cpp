#include "updater.h"
#include "config/Config.h"
#include "WiFiClient.h"

const char MyHomeNew::Updater::m_softVersion[6] = "1.0.1";
const char MyHomeNew::Updater::m_hardVersion[6] = "E8266";

String MyHomeNew::Updater::getFullVersion() {
  return String(m_hardVersion) + "-" + String(m_softVersion);
}

void MyHomeNew::Updater::handleUpdateResult(HTTPUpdateResult updateResp) {
  switch (updateResp)
  {
    case HTTP_UPDATE_OK:
      Serial.println("UPDATE_INSTALLED");
      ESP.restart();
      break;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("NO_UPDATE_AVLBL");
      break;
    case HTTP_UPDATE_FAILED:
      Serial.println(F("UPDATE_FAILED"));
      Serial.println(ESPhttpUpdate.getLastErrorString());
      break;
    default:
      Serial.println(F("UNSUP_RET_CODE"));
      break;
  }
}

void MyHomeNew::Updater::update(const char* url) {
  WiFiClient wifiClient;
  Config* _config = Config::getInstance();

  ESPhttpUpdate.rebootOnUpdate(true);
  HTTPUpdateResult updateResp = ESPhttpUpdate.update(
    wifiClient,
    _config->getValue(CONFIG_HOST),
    80,
    url,
    getFullVersion()
  );

  handleUpdateResult(updateResp);
}



void MyHomeNew::Updater::updateSpiffs(const char* url) {
  WiFiClient wifiClient;
  Config* _config = Config::getInstance();
  ESPhttpUpdate.rebootOnUpdate(true);
  HTTPUpdateResult updateResp = ESPhttpUpdate.updateSpiffs(
    wifiClient,
    String(_config->getValue(CONFIG_HOST)) + String(80) + String(url),
    getFullVersion()
  );

  handleUpdateResult(updateResp);
}

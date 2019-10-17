#include "WebSocketsClient.h"

namespace MyHomeNew {
  enum WSFlagDefs {
    SHOULD_SAVE_CONFIG = 0x01,
    SHOULD_SEND_WS = 0x02,
    SESSION_KEY_VERIFIED = 0x04
  };

  class WebSocketHandler {
    static String s_failResp;
    static String s_okResp;

    static WebSocketHandler* s_instance;
    uint8_t m_flags;
    int8_t m_cyclesTracker;
    String m_respStr;
    WebSocketsClient* m_client;
    uint8_t m_sessKey[16];
    WebSocketHandler();
    void updateHeaders();

    static void onWSEvent(WStype_t type, uint8_t * payload, size_t length);
    static uint32_t m_lastDiscTime;

    public:
    ~WebSocketHandler();
    static WebSocketHandler* getInstance();

    void connect();
    void loop();
    void handleEvent(const String&);
    void sendEncrypted();
    void sendEncrypted(const String&);
  };
}
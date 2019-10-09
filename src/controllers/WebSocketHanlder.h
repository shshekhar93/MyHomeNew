#include "WebSocketsClient.h"

namespace MyHomeNew {
  enum WSFlagDefs {
    SHOULD_SAVE_CONFIG = 0x01,
    SHOULD_SEND_WS = 0x02
  };

  class WebSocketHandler {
    static String s_failResp;
    static String s_okResp;
    static WebSocketHandler* s_instance;
    uint8_t m_flags;
    int8_t m_cyclesTracker;
    String m_respStr;
    WebSocketsClient* m_client;
    WebSocketHandler();

    static void onWSEvent(WStype_t type, uint8_t * payload, size_t length);

    public:
    ~WebSocketHandler();
    static WebSocketHandler* getInstance();

    void connect(const String&);
    void loop();
    void handleEvent(const String&);
    void sendEncrypted();
    void sendEncrypted(const String&);
  };
}
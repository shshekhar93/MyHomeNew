#include <ESP8266WebServer.h>

namespace MyHomeNew {
  class WiFiOps : public RequestHandler {
    private:
      static const String s_path;

      bool handleDisallowedMethods(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri);
      bool handleGet(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri);

    public:
      bool canHandle(HTTPMethod method, String uri);
      bool canUpload(String uri);
      bool handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri);

      static bool connect(bool);
      static bool startAP();
  };
}

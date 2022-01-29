#include <web_server_handler.h>

// Local web server instance
AsyncWebServer wsrvh_ws(WSRVH_PORT);

void wsrvh_route_root(AsyncWebServerRequest *req)
{
  req->send(200, "text/html", WSRVH_ROOT_WELCOME);
}

void wsrvh_init()
{
  // Set up routes
  wsrvh_ws.on("/", HTTP_GET, wsrvh_route_root);

  // Start serving requests
  wsrvh_ws.begin();
}

void wsrvh_register_handler(AsyncWebHandler *handler)
{
  wsrvh_ws.addHandler(handler);
}
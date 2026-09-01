#include "EventLoop.h"
#include "http/HttpRequest.h"
#include "http/HttpResponse.h"
#include "http/HttpServer.h"
#include "util/Logger.h"

int main() {
  initLogger();
  EventLoop loop;
  InetAddr listenAddr(8888);
  HttpServer server(&loop, listenAddr, "httpserver");
  server.setThreadNum(5);
  server.setHttpCallback([](const HttpRequest &req, HttpResponse *resp) {
    LOG_INFO("recieve: {} {} {} ", req.getMethodStr(), req.getUrl(),
             req.getVersionStr());
    resp->setVersion(HttpResponse::VersionE::HTTP11);
    resp->setStateCode(200);
    resp->setStateMessage("OK");
  });
  server.start();
  loop.loop();
  return 0;
}
#include "EventLoop.h"
#include "http/HttpRequest.h"
#include "http/HttpResponse.h"
#include "http/HttpServer.h"
#include "util/Logger.h"

#include <fstream>
#include <streambuf>
#include <string>



int main() {
  string index_html_cache{};
  initLogger();
  EventLoop loop;
  InetAddr listenAddr(8888);
  HttpServer server(&loop, listenAddr, "httpserver");
  server.setThreadNum(4);
  server.setHttpCallback([&index_html_cache](const HttpRequest &req, HttpResponse *resp) {
    LOG_INFO("recieve: {} {} {} ", req.getMethodStr(), req.getUrl(),
             req.getVersionStr());
    string url = req.getUrl();
    HttpRequest::MethodE method = req.getMethod();
    if (method == HttpRequest::MethodE::GET && url == "/static/index.html") {
      if (index_html_cache.empty()) {
        std::ifstream file("../static/index.html", std::ios::binary);
        index_html_cache = std::string(std::istreambuf_iterator<char>(file),
                                       std::istreambuf_iterator<char>());
      }
      resp->setBody(index_html_cache);
      resp->insertHeader("Connection", "keep-alive");
      resp->insertHeader("Content-Type", "text/html; charset=utf-8");
      resp->insertHeader("Content-Length",
                         std::to_string(resp->getBodyLength()));
      resp->setVersion(HttpResponse::VersionE::HTTP11);
      resp->setStateCode(200);
      resp->setStateMessage("OK");
    } else {
      resp->setVersion(HttpResponse::VersionE::HTTP11);
      resp->setStateCode(400);
      resp->setStateMessage("Bad Request");
    }
  });
  server.start();
  loop.loop();
  return 0;
}
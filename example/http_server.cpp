#include "EventLoop.h"
#include "http/HttpRequest.h"
#include "http/HttpResponse.h"
#include "http/HttpServer.h"
#include "util/Logger.h"

#include <fstream>
#include <streambuf>
#include <string>

int main() {
  initLogger();
  EventLoop loop;
  InetAddr listenAddr(8888);
  HttpServer server(&loop, listenAddr, "httpserver");
  server.setThreadNum(5);
  server.setHttpCallback([](const HttpRequest &req, HttpResponse *resp) {
    LOG_INFO("recieve: {} {} {} ", req.getMethodStr(), req.getUrl(),
             req.getVersionStr());
    string url = req.getUrl();
    HttpRequest::MethodE method = req.getMethod();
    if (method == HttpRequest::MethodE::GET && url == "/static/index.html") {
      string file_path = ".." + url;
      std::ifstream file(file_path, std::ios::binary);
      if (!file.is_open()) {
        LOG_FATAL("{} 文件不存在", url);
      }
      resp->setBody(string(std::istreambuf_iterator<char>(file),
                           std::istreambuf_iterator<char>()));
      resp->insertHeader("Content-Type", "text/html; charset=utf-8");
      resp->insertHeader("Content-Length",
                         std::to_string(resp->getBodyLength()));
    }
    resp->setVersion(HttpResponse::VersionE::HTTP11);
    resp->setStateCode(200);
    resp->setStateMessage("OK");
  });
  server.start();
  loop.loop();
  return 0;
}
#pragma once

#include "InetAddr.h"
#include "TcpConnection.h"
#include "TcpServer.h"
#include "http/HttpContext.h"

class HttpRequest;
class HttpResponse;

class HttpServer {
  using HttpCallback = std::function<void(const HttpRequest &, HttpResponse *)>;

public:
  //禁止copy
  HttpServer(const HttpServer &) = delete;
  HttpServer &operator=(const HttpServer &) = delete;

  HttpServer(EventLoop *loop, const InetAddr &listenAddr, const string &nameArg,
             TcpServer::Option option = TcpServer::Option::kNoReusePort);
  ~HttpServer() = default;

  void start(){server_.start();};
  void setHttpCallback(const HttpCallback &cb) { httpCallback_ = cb; }
  void setThreadNum(int num){server_.setThreadNum(num);}

private:
  void onConnection(const TcpConnectionPtr &conn);
  void onMessage(const TcpConnectionPtr &conn, Buffer *buffer);
  void onRequest(const TcpConnectionPtr &conn, const HttpRequest &req);

  void processHttpContext(const TcpConnectionPtr &conn, HttpContext &ctx,
                          Buffer *buffer);

  TcpServer server_;
  HttpCallback httpCallback_;
};
#include "http/HttpServer.h"
#include "EventLoop.h"
#include "TcpConnection.h"
#include "http/HttpContext.h"
#include "http/HttpRequest.h"
#include "http/HttpResponse.h"
#include "util/Logger.h"
#include <any>

HttpServer::HttpServer(EventLoop *loop, const InetAddr &listenAddr,
                       const string &nameArg, TcpServer::Option option)
    : server_(loop, listenAddr, nameArg, option) {
  server_.setConnectionCallback(std::bind(&HttpServer::onConnection, this, _1));
  server_.setMessageCallback(std::bind(&HttpServer::onMessage, this, _1, _2));
}

void HttpServer::onConnection(const TcpConnectionPtr &conn) {
  LOG_DEBUG("onConnection");
  assert(conn->connected());
  conn->setContext(HttpContext());
}

void HttpServer::onMessage(const TcpConnectionPtr &conn, Buffer *buffer) {
  ContextVariant &context = conn->getContext();
  std::visit([this, &conn,
              buffer](auto &ctx) { processHttpContext(conn, ctx, buffer); },
             context);
}

void HttpServer::processHttpContext(const TcpConnectionPtr &conn,
                                    HttpContext &ctx, Buffer *buffer) {
  int res = ctx.parse(buffer);
  if (res == 0) {
    const HttpRequest &req = ctx.getRequest();
    auto connection = req.getHeader("Connection");
    bool close = true;
    if (connection) {
      close = *connection == "close" ||
              (req.getVersion() == HttpRequest::VersionE::HTTP10 &&
               *connection != "Keep-Alive");
    }
    HttpResponse res(close);
    httpCallback_(req, &res);
    Buffer buf;
    res.appendToBuffer(&buf);
    conn->send(&buf);
    ctx.reset();
    if (res.closeConnection()) {
      conn->shutdown();
    }
  } else if (res == 1) {
    LOG_DEBUG("HtppServer::onMessage: Waiting Request");
  } else {
    LOG_DEBUG("HtppServer::onMessage: Bad Request");
    conn->send("HTTP/1.1 400 Bad Request/r/n/r/n");
    conn->shutdown();
  }
}

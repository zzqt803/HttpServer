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
    : server_(loop, listenAddr, nameArg, option), workersPool_(4) {
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
    bool close = false;
    if (connection) {
      close = *connection == "close" ||
              (req.getVersion() == HttpRequest::VersionE::HTTP10 &&
               *connection != "Keep-Alive");
    }
    workersPool_.enqueue([this, conn, req, close]() {
      HttpResponse resp(close);
      httpCallback_(req, &resp); // 在工作线程执行
      Buffer buf;
      resp.appendToBuffer(&buf);

      // 关键：跨线程发送，必须通过 runInLoop 回到 IO 线程
      conn->getLoop()->runInLoop([conn, buf = std::move(buf), close]()mutable {
        conn->send(&buf);
        if (close) {
          conn->shutdown();
        }
      });
    });
    ctx.reset();
  } else if (res == 1) {
    LOG_DEBUG("HtppServer::onMessage: Waiting Request");
  } else {
    LOG_DEBUG("HtppServer::onMessage: Bad Request");
    conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
    conn->shutdown();
  }
}

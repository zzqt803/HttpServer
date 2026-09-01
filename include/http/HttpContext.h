#pragma once

#include "Buffer.h"
#include "http/HttpRequest.h"

class HttpContext {
public:
  //移动构造，移动赋值
  HttpContext(HttpContext &&) = default;
  HttpContext &operator=(HttpContext &&) = default;

  //禁止copy
  HttpContext(const HttpContext &) = delete;
  HttpContext &operator=(const HttpContext &) = delete;

  //默认构造函数
  HttpContext() = default;

  //从buf中解析请求
  // 0 = 解析成功； -1 = 解析遇到错误； 1=解析不完全
  int parse(Buffer *);

  const HttpRequest &getRequest() const { return request_; }

  void reset() {
    request_ = HttpRequest{};
    state_ = kExpectRequestLine;
  }

private:
  enum ParseStateE {
    kExpectRequestLine, // 等待解析 "GET /index.html HTTP/1.1"
    kExpectHeaders,     // 循环解析 "Host: ..." 直到空行
    kExpectBody,        // 根据 Content-Length 读 body
    kGotAll
  };

  HttpRequest request_{};
  ParseStateE state_{kExpectRequestLine};
};
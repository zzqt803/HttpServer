#include "http/HttpContext.h"
#include "util/Logger.h"

int HttpContext::parse(Buffer *buf) {
  // LOG_DEBUG("HttpContext::parse: start parse");
  bool stop = false;
  while (!stop) {
    const char *crlf{nullptr};
    string line{};
    switch (state_) {
    case kExpectRequestLine: {
      // LOG_DEBUG("解析请求行");
      crlf = buf->findCRLF();
      if (!crlf) {
        // LOG_DEBUG("无crlf");
        return 1;
      }
      line = string(buf->peek(), crlf - buf->peek());
      buf->retrieve(crlf - buf->peek() + 2);
      int space1 = line.find(" ");
      int space2 = line.rfind(" ");
      string method = line.substr(0, space1);
      string url = line.substr(space1 + 1, space2 - space1 - 1);
      string version = line.substr(space2 + 1);
      if (!request_.setMethod(method)) {
        return -1;
      }
      if (!request_.setVersion(version)) {
        return -1;
      }
      request_.setUrl(url);
      state_ = kExpectHeaders;
      break;
    }
    case kExpectHeaders: {
      // LOG_DEBUG("解析请求头");
      crlf = buf->findCRLF();
      //空行
      if (crlf == buf->peek()) {
        buf->retrieve(2);
        state_ = kExpectBody;
        break;
      }
      if (!crlf) {
        // LOG_DEBUG("剩余 {} 字节", size);
        return 1;
      }
      //正常
      line = string(buf->peek(), crlf - buf->peek());
      buf->retrieve(crlf - buf->peek() + 2);
      int colon = line.find(":");
      string key = line.substr(0, colon);
      string value{};
      if (colon + 2 <= static_cast<int>(line.size())) {
        value = line.substr(colon + 2);
      }
      request_.insertHeader(key, value);
      break;
    }
    case kExpectBody: {
      // LOG_DEBUG("解析请求体");
      auto te = request_.getHeader("Transfer-Encoding");
      if (te.has_value() && te.value() == "chunked") {
        // Http1.1分块解析，暂不实现
        return -1;
      }

      auto lenStr = request_.getHeader("Content-Length");
      if (lenStr.has_value()) {
        int len = std::stoul(lenStr.value());
        if (len > 0) {
          request_.setBody(buf->retrieveAsString(len));
        }
      }
      state_ = kGotAll;
      break;
    }
    case kGotAll: {
      LOG_DEBUG("解析完毕");
      return 0;
    }
    default:
      return -1;
    }
  }
  return -1;
}
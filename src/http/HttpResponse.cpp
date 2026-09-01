#include "http/HttpResponse.h"

#include "Buffer.h"
#include <string>

HttpResponse::HttpResponse(bool close) : close_(close) {}

void HttpResponse::appendToBuffer(Buffer *buf) {
  //响应行
  string resStr = (version_ == VersionE::HTTP11 ? "HTTP/1.1" : "HTTP/1.0");
  resStr+=" ";
  resStr += std::to_string(stateCode_)+" ";
  resStr += stateMessage_ + "\r\n";

  //响应头
  for (auto &[key, value] : headers_) {
    string keyValueStr = key+": "+value+"\r\n";
    resStr+=keyValueStr;
  }
  resStr += "\r\n";

  //响应体
  resStr += body_;
  buf->append(resStr);
}
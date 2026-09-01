#pragma once

#include <string>
#include <sys/types.h>
#include <unordered_map>

using std::string;
class Buffer;
class HttpResponse {
public:
  enum class VersionE { HTTP10, HTTP11 };
  HttpResponse(bool close = false);

  void appendToBuffer(Buffer *);
  bool closeConnection() const { return close_; }

  void setVersion(VersionE v) { version_ = v; }
  void setStateCode(u_int16_t st) { stateCode_ = st; }
  void setStateMessage(const string &m) { stateMessage_ = m; }

  void insertHeader(const string &key, const string &value) {
    headers_[key] = value;
  }
  void setBody(const string &b) { body_ = b; }

private:
  bool close_{false};

  VersionE version_{VersionE::HTTP11};
  u_int16_t stateCode_{0};
  string stateMessage_{};

  std::unordered_map<string, string> headers_{};

  string body_{};
};
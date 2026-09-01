#pragma once
#include <optional>
#include <string>
#include <unordered_map>

using std::string;
class HttpRequest {
public:
  enum class MethodE { GET, POST, PUT, DELETE, PATCH, OPTIONS, HEAD };
  enum class VersionE { HTTP10, HTTP11 };

  std::optional<string> getHeader(const string &key) const {
    auto it = headers_.find(key);
    if (it == headers_.end()) {
      return std::nullopt;
    } else {
      return it->second;
    }
  }

  void insertHeader(const string &key, const string &value) {
    headers_[key] = value;
  }

  MethodE getMethod() const { return method_; }
  VersionE getVersion() const { return version_; }
  string getMethodStr() const;
  string getVersionStr() const;
  const string &getUrl() const { return url_; }
  const string &getBody() const { return body_; };

  bool setMethod(const string &method);
  bool setVersion(const string &version);
  void setUrl(const string &url) { url_ = url; }
  void setBody(const string &body) { body_ = body; }

private:
  //请求行
  MethodE method_{MethodE::GET};
  string url_{"/"};
  VersionE version_{VersionE::HTTP11};

  //请求头
  std::unordered_map<string, string> headers_{};

  //请求体
  string body_{};
};
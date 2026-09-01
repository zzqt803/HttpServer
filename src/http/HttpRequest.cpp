#include "http/HttpRequest.h"

bool HttpRequest::setMethod(const string &method) {
  if (method == "GET") {
    method_ = MethodE::GET;
  } else if (method == "POST") {
    method_ = MethodE::POST;
  } else if (method == "PUT") {
    method_ = MethodE::PUT;
  } else if (method == "DELETE") {
    method_ = MethodE::DELETE;
  } else if (method == "OPTIONS") {
    method_ = MethodE::OPTIONS;
  } else if (method == "PATCH") {
    method_ = MethodE::PATCH;
  } else if (method == "HEAD") {
    method_ = MethodE::HEAD;
  } else {
    return false;
  }
  return true;
}
bool HttpRequest::setVersion(const string &version) {
  if (version == "HTTP/1.0") {
    version_ = VersionE::HTTP10;
  } else if (version == "HTTP/1.1") {
    version_ = VersionE::HTTP11;
  } else {
    return false;
  }
  return true;
}

string HttpRequest::getMethodStr() const {
  switch (method_) {
  case MethodE::GET:
    return "GET";
  case MethodE::POST:
    return "POST";
  case MethodE::PUT:
    return "PUT";
  case MethodE::DELETE:
    return "DELETE";
  case MethodE::PATCH:
    return "PATCH";
  case MethodE::HEAD:
    return "HEAD";
  case MethodE::OPTIONS:
    return "OPTIONS";
  default:
    return "ERROR-METHOD";
  }
}
string HttpRequest::getVersionStr() const {
  switch (version_) {
  case VersionE::HTTP10:
    return "HTTP/1.0";
  case VersionE::HTTP11:
    return "HTTP/1.1";
  default:
    return "ERROR-VERSION";
  }
}
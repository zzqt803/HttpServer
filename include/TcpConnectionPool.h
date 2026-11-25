#pragma once

#include "TcpConnection.h"
#include <mutex>

class TcpConnectionPool {
public:
  void *acquire() {
    // std::lock_guard<std::mutex> lock(mutex_);
    if (freeList_.empty())
      return ::operator new(sizeof(TcpConnection));
    void *p = freeList_.back();
    freeList_.pop_back();
    return p;
  }
  void release(void *p) {
    // std::lock_guard<std::mutex> lock(mutex_);
    freeList_.push_back(p); // 注意：调用前对象已析构
  }

private:
  std::vector<void *> freeList_;
  std::mutex mutex_;
};
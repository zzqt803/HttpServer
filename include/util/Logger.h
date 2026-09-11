#pragma once

#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include <cerrno>
#include <cstdlib>
#include <cstring>

// 普通日志宏封装
#define LOG_TRACE(...) spdlog::trace(__VA_ARGS__)
#define LOG_DEBUG(...) spdlog::debug(__VA_ARGS__)
#define LOG_INFO(...) spdlog::info(__VA_ARGS__)
#define LOG_WARN(...) spdlog::warn(__VA_ARGS__)
#define LOG_ERROR(...) spdlog::error(__VA_ARGS__)

// 致命错误宏：打印 -> 立即刷盘 -> 挂掉程序
#define LOG_FATAL(...)                                                         \
  do {                                                                         \
    spdlog::critical(__VA_ARGS__);                                             \
    spdlog::default_logger()->flush();                                         \
    std::abort();                                                              \
  } while (0)

// 带 errno 系统错误信息的宏（Muduo 特色）
#define LOG_SYSERR(fmt, ...)                                                   \
  spdlog::error(fmt " [errno: {}, error: {}]", ##__VA_ARGS__, errno,           \
                std::strerror(errno))

// 带有 errno 信息的致命系统错误宏
#define LOG_SYSFATAL(fmt, ...)                                                 \
  do {                                                                         \
    spdlog::critical(fmt " [errno: {}, error: {}]", ##__VA_ARGS__, errno,      \
                     std::strerror(errno));                                    \
    spdlog::default_logger()->flush();                                         \
    std::abort();                                                              \
  } while (0)

// 全局日志初始化
inline void initLogger() {
  spdlog::init_thread_pool(8192, 1);

  // 创建异步 logger
  auto logger = spdlog::basic_logger_mt<spdlog::async_factory>(
      "async_file_logger", "logs/async.log");
  spdlog::set_default_logger(logger);
  // 设置日志输出格式：[时间] [日志级别] [线程ID] 内容
  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v");
  spdlog::set_level(spdlog::level::warn);

  // 关键：设置自动刷新间隔（每 3 秒刷盘一次，避免每条日志都 flush）
  logger->flush_on(spdlog::level::warn);
  spdlog::flush_every(std::chrono::seconds(3));

}
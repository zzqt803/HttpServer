#pragma once

#include "Callbacks.h"
#include "Channel.h"
#include "util/Timestamp.h"
#include <atomic>
#include <set>
#include <vector>

class EventLoop;
class Timer;
class TimerId;

class TimerQueue {
  TimerQueue(const TimerQueue &) = delete;
  TimerQueue &operator=(const TimerQueue &) = delete;

public:
  explicit TimerQueue(EventLoop *loop);
  ~TimerQueue();

  TimerId addTimer(TimerCallback cb, Timestamp when, double interval);

  void cancel(TimerId timerId);

private:
  typedef std::pair<Timestamp, Timer *> Entry;
  typedef std::set<Entry> TimerList;
  typedef std::pair<Timer *, int64_t> ActiveTimer;
  typedef std::set<ActiveTimer> ActiveTimerSet;

  void addTimerInLoop(Timer *timer);
  void cancelInLoop(TimerId timerId);
  // 到时调用回调
  void handleRead();
  // 过期的
  std::vector<Entry> getExpired(Timestamp now);
  void reset(const std::vector<Entry> &expired, Timestamp now);

  bool insert(Timer *timer);

  EventLoop *loop_;
  const int timerfd_;
  Channel timerfdChannel_;
  // 根据结束时间排列
  TimerList timers_;

  // for cancel()
  ActiveTimerSet activeTimers_;
  std::atomic<bool> callingExpiredTimers_;
  ActiveTimerSet cancelingTimers_;
};
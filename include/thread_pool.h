#pragma once
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <queue>
#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <future>

template<typename T>
class ts_queue {
public:
  ts_queue() = default;
  ts_queue(const ts_queue&) = delete;
  ts_queue(const ts_queue&&) = delete;
  ~ts_queue() = default;

  void push(const T& t) {
    std::lock_guard lock(mtx_);
    q_.emplace(t);
  }

  bool pop(T& t) {
    std::lock_guard lock(mtx_);
    if (q_.empty()) {
      return false;
    }
    t = std::move(q_.front());
    q_.pop();
    return true;
  }

  bool empty() {
    std::lock_guard lock(mtx_);
    return q_.empty();
  }

private:
  std::queue<T> q_;
  std::mutex mtx_;
};


class thread_pool {
public:
  thread_pool() = default;
  thread_pool(const thread_pool&) = delete;
  thread_pool(thread_pool&&) = delete;
  ~thread_pool() { close(); }

  void start(std::size_t n) {
    open_ = true;
    for (int i = 0; i < n; i++) {
      threads_.push_back(std::thread{std::bind(&thread_pool::work, this)});
    }
  }

  void close() {
    while (!q_.empty()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    open_ = false;
    cv_.notify_all();
    for (auto &thd : threads_) {
      if (thd.joinable()) {
        thd.join();
      }
    }
    threads_.clear();
  }
  
  template<typename F, typename... Args>
  auto submit(F&& f, Args&&... args) {
    using return_t = decltype(f(args...));
    std::function<return_t()> task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    auto task_ptr = std::make_shared<std::packaged_task<return_t()>>(task);
    q_.push([task_ptr]() { (*task_ptr)(); });
    cv_.notify_one();
    return task_ptr->get_future();
  }

private:
  ts_queue<std::function<void()>> q_;
  std::vector<std::thread> threads_;
  std::mutex mtx_;
  std::condition_variable cv_;
  bool open_{false};

  void work() {
    std::function<void()> func;
    bool get;
    while (open_) {
      {
        std::unique_lock lock(mtx_);
        cv_.wait(lock, [this]() { return !open_ || !q_.empty(); });
        if (!open_) {
          return;
        }
        get = q_.pop(func);
      }
      if (get) {
        func();
      }
    }
  }
};

#endif

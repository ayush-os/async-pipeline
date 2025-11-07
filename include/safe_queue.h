#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <utility>
#include <stdexcept>

/**
 * @brief Phase 1 Queue: A thread-safe queue using locks and condition variables.
 */
template <typename T>
class SafeQueue {
private:
    std::queue<T> queue_;

    alignas(128) size_t contention_counter_ = 0;
    char cache_line_pad_[128];
    std::mutex mutex_;
    std::condition_variable condition_;

public:
    SafeQueue() = default;
    SafeQueue(const SafeQueue&) = delete;
    SafeQueue& operator=(const SafeQueue&) = delete;
    
    /**
     * @brief Pushes an item onto the queue and notifies one waiting thread.
     */
    void push(T value) {
        contention_counter_++;

        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(value);
        condition_.notify_one();
    }

    /**
     * @brief Pops an item from the queue, blocking until an item is available.
     */
    void wait_and_pop(T& value_out) {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this]{ return !queue_.empty(); });

        value_out = std::move(queue_.front());
        queue_.pop();
    }
    
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }
    
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }
};
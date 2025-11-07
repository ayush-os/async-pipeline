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
        // TODO: [Step 1] Implement push() using std::lock_guard and condition_.notify_one().
        // The lock guard must ensure the push is atomic with respect to other push/pop operations.
    }

    /**
     * @brief Pops an item from the queue, blocking until an item is available.
     */
    void wait_and_pop(T& value_out) {
        // TODO: [Step 1] Implement wait_and_pop() using std::unique_lock and condition_.wait() 
        // with a predicate (a lambda function). This is essential for correctness (handling spurious wakeups).
        // Remember to use std::move to efficiently transfer the element out.
    }

    /**
     * @brief Non-blocking attempt to pop an item.
     * @return true if an item was popped, false otherwise.
     */
    bool try_pop(T& value_out) {
        // Optional: Implement non-blocking pop if needed for debugging/analysis
        return false; 
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
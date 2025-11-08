#pragma once

#include <atomic>
#include <memory>
#include <stdexcept>

static constexpr size_t DEFAULT_CAPACITY = 1024;

/**
 * @brief Phase 3 Queue: A lock-free queue for one producer and one consumer.
 * Uses only atomic operations and memory ordering constraints.
 */
template <typename T>
class SPSCLockFreeQueue
{
public:
    SPSCLockFreeQueue(size_t capacity = DEFAULT_CAPACITY)
        : capacity_(capacity),
          buffer_(std::make_unique<T[]>(capacity)),
          head_(0),
          tail_(0)
    {
    }

    SPSCLockFreeQueue(const SPSCLockFreeQueue &) = delete;
    SPSCLockFreeQueue &operator=(const SPSCLockFreeQueue &) = delete;

    bool push(const T &value)
    {
        // can use & instead of % here for more perf because capacity is a power of 2
        size_t next_head = (head_ + 1) & (capacity_ - 1);
        if (tail_.load(std::memory_order_relaxed) == next_head)
        {
            return false; // q full cannot push
        }

        buffer_[head_] = value;
        head_.store(next_head, std::memory_order_release);

        return true;
    }

    bool pop(T &value_out)
    {
        size_t cur_tail = tail_.load(std::memory_order_acquire);
        if (cur_tail == head_)
        {
            return false; // q empty nothing to pop
        }

        value_out = buffer_[cur_tail];
        tail_.store((cur_tail + 1) & (capacity_ - 1), std::memory_order_relaxed);

        return true;
    }

private:
    const size_t capacity_;

    std::unique_ptr<T[]> buffer_;

    // atomics here ensure synchronization (producer only updates head_ once it writes the val and
    // consumer only reads new head value once all other producer writes are done)
    // and visibility (force in cache as opposed to private thread execution context)
    std::atomic<size_t> head_;
    std::atomic<size_t> tail_;
};

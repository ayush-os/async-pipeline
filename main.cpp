#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

#include "safe_queue.h"

// --- Data Structures ---

struct DataItem {
    int id;

    bool operator==(const DataItem& other) const {
        return id == other.id;
    }
};

const DataItem SENTINEL = {-1};
const int NUM_ITEMS = 1000000;
const int NUM_PRODUCERS = 4;

SafeQueue<DataItem> pipeline_queue;

// --- Performance Utilities ---

class ScopedTimer {
public:
    ScopedTimer(const std::string& name) : name_(name), start_time_(std::chrono::high_resolution_clock::now()) {}

    ~ScopedTimer() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time_);
        printf("%s time: %lld ms\n", name_.c_str(), (long long)duration.count());
    }
private:
    std::string name_;
    std::chrono::high_resolution_clock::time_point start_time_;
};

// --- Pipeline Stages ---

/**
 * @brief Produces DataItems and pushes them into the queue.
 * Signals completion using the SENTINEL.
 */
void producer_stage(int thread_id) {
    printf("Producer %d started\n", thread_id);

    for (int i = thread_id; i < NUM_ITEMS; i += NUM_PRODUCERS) {
        pipeline_queue.push({i});
    }

    if (thread_id == NUM_PRODUCERS - 1) {
        pipeline_queue.push(SENTINEL);
    }

    printf("Producer %d finished and sent sentinel.\n", thread_id);
}

/**
 * @brief Consumes DataItems from the queue, processing them until the SENTINEL is received.
 */
void consumer_stage() {
    printf("Consumer started.\n");
    DataItem item;
    size_t processed_count = 0;

    while (true) {
        pipeline_queue.wait_and_pop(item);

        if (item == SENTINEL) {
            break;
        }

        processed_count++;
    }
    
    printf("Consumer finished. Processed %zu items.\n", processed_count);
}

// --- Main Driver ---

int main() {
    printf("--- Asynchronous Pipeline Benchmark (2-Stage SafeQueue) ---\n");

    { 
        ScopedTimer overall_timer("Total Pipeline Execution");

        std::thread producers[NUM_PRODUCERS];

        for (int i = 0; i < NUM_PRODUCERS; i++) {
            producers[i] = std::thread(producer_stage, i);
        }
        std::thread consumer(consumer_stage);

        for (int i = 0; i < NUM_PRODUCERS; i++) {
            producers[i].join();
        }
        consumer.join();
    }

    printf("Pipeline finished successfully.\n");

    return 0;
}
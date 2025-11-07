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
void producer_stage(int num_items) {
    printf("Producer started. Total items: %d\n", num_items);
    
    for (int i = 0; i < num_items; i++) {
        pipeline_queue.push({i});
    }

    pipeline_queue.push(SENTINEL);
    
    printf("Producer finished and sent sentinel.\n");
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
    const int NUM_ITEMS = 1000000; 

    printf("--- Asynchronous Pipeline Benchmark (2-Stage SafeQueue) ---\n");
    
    { 
        ScopedTimer overall_timer("Total Pipeline Execution");

        std::thread producer(producer_stage, NUM_ITEMS);
        std::thread consumer(consumer_stage);

        producer.join();
        consumer.join();
    }
    
    printf("Pipeline finished successfully.\n");

    return 0;
}
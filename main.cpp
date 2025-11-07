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
        std::cout << name_ << " time: " << duration.count() << " ms" << std::endl;
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
    std::cout << "Producer started. Total items: " << num_items << std::endl;
    // TODO
    std::cout << "Producer finished and sent sentinel." << std::endl;
}

/**
 * @brief Consumes DataItems from the queue, processing them until the SENTINEL is received.
 */
void consumer_stage() {
    std::cout << "Consumer started." << std::endl;
    // TODO
    std::cout << "Consumer finished." << std::endl;
}

// --- Main Driver ---

int main() {
    const int NUM_ITEMS = 1000000; 

    std::cout << "--- Asynchronous Pipeline Benchmark (2-Stage SafeQueue) ---" << std::endl;
    
    { 
        ScopedTimer overall_timer("Total Pipeline Execution");

        // TODO
    }
    
    std::cout << "Pipeline finished successfully." << std::endl;

    return 0;
}
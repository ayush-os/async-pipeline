#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

#include "safe_queue.h"

// --- Data Structures ---

struct DataItem {
    int id;
    int result = 0;

    bool operator==(const DataItem& other) const {
        return id == other.id;
    }
};

const DataItem SENTINEL = {-1};
const int NUM_ITEMS = 1000000;

SafeQueue<DataItem> pipeline_queue_one;
SafeQueue<DataItem> pipeline_queue_two;
SafeQueue<DataItem> pipeline_queue_three;

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

void loader_stage() {
    printf("loader started\n");

    for (int i = 0; i < NUM_ITEMS; i++) {
        pipeline_queue_one.push({i});
    }
    pipeline_queue_one.push(SENTINEL);

    printf("loader finished and sent sentinel.\n");
}

void parser_stage() {
    printf("parser started\n");
    DataItem item;

    while (true) {
        pipeline_queue_one.wait_and_pop(item);

        if (item == SENTINEL) {
            break;
        }

        item.result = static_cast<int>(std::sqrt(item.id));

        pipeline_queue_two.push(item);
    }
    pipeline_queue_two.push(SENTINEL);

    printf("parser finished and sent sentinel.\n");
}

void transformer_stage() {
    printf("transformer started.\n");
    DataItem item;

    while (true) {
        pipeline_queue_two.wait_and_pop(item);

        if (item == SENTINEL) {
            break;
        }

        item.result *= 7;

        pipeline_queue_three.push(item);
    }
    pipeline_queue_three.push(SENTINEL);

    printf("transformer finished and sent sentinel\n");
}

void output_stage() {
    printf("output started.\n");
    DataItem item;
    size_t processed_count = 0;

    while (true) {
        pipeline_queue_three.wait_and_pop(item);

        if (item == SENTINEL) {
            break;
        }

        processed_count++;
    }
    
    printf("output finished. Processed %zu items.\n", processed_count);
}

// --- Main Driver ---

int main() {
    printf("--- Asynchronous Pipeline Benchmark (2-Stage SafeQueue) ---\n");

    { 
        ScopedTimer overall_timer("Total Pipeline Execution");

        std::thread loader(loader_stage);
        std::thread parser(parser_stage);
        std::thread transformer(transformer_stage);
        std::thread output(output_stage);

        loader.join();
        parser.join();
        transformer.join();
        output.join();
    }

    printf("Pipeline finished successfully.\n");

    return 0;
}
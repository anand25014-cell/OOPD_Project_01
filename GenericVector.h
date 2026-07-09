#ifndef CELLULAR_CORE_H
#define CELLULAR_CORE_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "GenericVector.h"
#include "basicIO.h"

// MessageBatch - simple container for tower's aggregated messages
struct MessageBatch {
    int tower_id;
    int message_count;
    MessageBatch() : tower_id(0), message_count(0) {}
    MessageBatch(int t, int m) : tower_id(t), message_count(m) {}
};

class CellularCore {
public:
    CellularCore(int max_msgs, int overhead_percent);
    ~CellularCore();

    void enqueue_batch(const MessageBatch& b);
    void start_workers(int n);
    void stop_workers();

    static int cores_needed_for_load(long total_msgs, int overhead_percent, int core_capacity);

    int get_overhead() const { return overhead_per_100; }
    int get_capacity() const { return max_messages_per_core; }

    // public stats and queue for inspection
    std::atomic<int> current_processed;
    std::atomic<int> accepted_total;
    std::atomic<int> rejected_total;

    GenericVector<MessageBatch> queue;
    std::mutex qmutex;
    std::condition_variable qcv;

private:
    // ORDER: ints first, then atomics (to suppress -Wreorder warnings)
    int overhead_per_100;
    int max_messages_per_core;

    GenericVector<std::thread*> workers;
    bool stop_flag;

    void worker_thread();
};

#endif

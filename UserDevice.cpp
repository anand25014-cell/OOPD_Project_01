#include "CellularCore.h"
#include "basicIO.h"

CellularCore::CellularCore(int max_msgs, int overhead_percent)
    : overhead_per_100(overhead_percent),
      max_messages_per_core(max_msgs),
      current_processed(0),
      accepted_total(0),
      rejected_total(0),
      stop_flag(false)
{
#ifdef DEBUG
    io.outputstring("[DEBUG] CellularCore: created max_msgs="); io.outputint(max_msgs);
    io.outputstring(" overhead="); io.outputint(overhead_percent); io.terminate();
#endif
}

CellularCore::~CellularCore() {
#ifdef DEBUG
    io.outputstring("[DEBUG] CellularCore: destructor, stopping workers"); io.terminate();
#endif
    stop_workers();
}

void CellularCore::enqueue_batch(const MessageBatch& b) {
    std::lock_guard<std::mutex> lk(qmutex);
    queue.push_back(b);
#ifdef DEBUG
    io.outputstring("[DEBUG] enqueue_batch: tower="); io.outputint(b.tower_id);
    io.outputstring(" msgs="); io.outputint(b.message_count); io.terminate();
#endif
    qcv.notify_one();
}

void CellularCore::worker_thread() {
#ifdef DEBUG
    io.outputstring("[DEBUG] worker_thread: started"); io.terminate();
#endif
    while (true) {
        MessageBatch batch;
        {
            std::unique_lock<std::mutex> lk(qmutex);
            qcv.wait(lk, [&](){ return queue.size() > 0 || stop_flag; });
            if (queue.size() == 0 && stop_flag) {
#ifdef DEBUG
                io.outputstring("[DEBUG] worker_thread: exit (stop_flag)"); io.terminate();
#endif
                return;
            }
            batch = queue[0];
            for (unsigned int i = 1; i < queue.size(); ++i) queue[i-1] = queue[i];
            queue.pop_back();
        }

#ifdef DEBUG
        io.outputstring("[DEBUG] worker_thread: processing tower="); io.outputint(batch.tower_id);
        io.outputstring(" msgs="); io.outputint(batch.message_count); io.terminate();
#endif

        int effective = batch.message_count + (batch.message_count * overhead_per_100) / 100;
        int prev = current_processed.fetch_add(effective);
        if (prev + effective <= max_messages_per_core) {
            accepted_total.fetch_add(batch.message_count);
#ifdef DEBUG
            io.outputstring("[DEBUG] worker_thread: accepted tower="); io.outputint(batch.tower_id);
            io.outputstring(" msgs="); io.outputint(batch.message_count);
            io.outputstring(" effective="); io.outputint(effective); io.terminate();
#endif
        } else {
            current_processed.fetch_sub(effective);
            rejected_total.fetch_add(batch.message_count);
#ifdef DEBUG
            io.outputstring("[DEBUG] worker_thread: rejected tower="); io.outputint(batch.tower_id);
            io.outputstring(" msgs="); io.outputint(batch.message_count);
            io.outputstring(" effective="); io.outputint(effective); io.terminate();
#endif
        }
    }
}

void CellularCore::start_workers(int n) {
    if (workers.size() > 0) return;
    for (int i = 0; i < n; ++i) {
        std::thread* t = new std::thread(&CellularCore::worker_thread, this);
        workers.push_back(t);
#ifdef DEBUG
        io.outputstring("[DEBUG] start_workers: started index="); io.outputint(i); io.terminate();
#endif
    }
}

void CellularCore::stop_workers() {
    {
        std::lock_guard<std::mutex> lk(qmutex);
        stop_flag = true;
        qcv.notify_all();
    }
#ifdef DEBUG
    io.outputstring("[DEBUG] stop_workers: stopping..."); io.terminate();
#endif
    for (unsigned int i = 0; i < workers.size(); ++i) {
        if (workers[i]) {
            workers[i]->join();
#ifdef DEBUG
            io.outputstring("[DEBUG] stop_workers: joined index="); io.outputint(i); io.terminate();
#endif
            delete workers[i];
        }
    }
    workers.clear();
    stop_flag = false;
    queue.clear();
#ifdef DEBUG
    io.outputstring("[DEBUG] stop_workers: done, queue cleared"); io.terminate();
#endif
}

int CellularCore::cores_needed_for_load(long total_user_messages,
                                        int overhead_percent,
                                        int core_capacity)
{
    long effective = total_user_messages +
                     (total_user_messages * overhead_percent) / 100;

    if (core_capacity <= 0)
        return 1; // avoid division by zero

    int cores = (int)((effective + core_capacity - 1) / core_capacity);
    return cores;
}

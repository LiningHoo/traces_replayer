#pragma once

#include <string>

#include "trace_replayer.hpp"
#include "../operator/loader/loader.hpp"
#include "../operator/loader/msr_loader.hpp"
#include "../utils/delay_queue/delay_queue.hpp"

class msr_replayer: public trace_replayer {
private:
    static const int DEFAULT_BATCH_SIZE = 100;

    int batch_size = DEFAULT_BATCH_SIZE;
    std::string trace_file;
    loader *m_loader;
    delay_queue task_queue;
    std::vector<std::thread*> workers;
public:
    msr_replayer(std::string _trace_file, int _batch_size, int worker_num):
        trace_file(_trace_file), batch_size(_batch_size) {
        m_loader = new msr_loader(trace_file);
        workers.resize(worker_num);
    }

    ~msr_replayer() {
        delete m_loader;
        for (auto& worker : workers) {
            delete worker;
        }
    }

    void play() {
        std::thread task_producer([&](){
            while(true) {
                std::vector<delay_task*> tasks = m_loader->load_batch_task(batch_size);
                for (const auto& task : tasks) {
                    task_queue.push(task);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
        task_producer.detach();

        // std::mutex mu;

        for (auto& worker : workers) {
            worker = new std::thread([&](){
                while(true) {
                    // std::lock_guard<std::mutex> lock(mu);
                    msr_task *task = (msr_task*)task_queue.get_first();
                    task->exec();
                    delete task;
                }
            });
        }

        for (const auto& worker : workers) {
            worker->join();
        }
    }
};
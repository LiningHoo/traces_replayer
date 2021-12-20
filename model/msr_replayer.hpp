#pragma once

#include <string>
#include <fcntl.h>
#include <exception>
#include <atomic>

#include "./trace_replayer.hpp"
#include "./device.hpp"
#include "../operator/loader/loader.hpp"
#include "../operator/loader/msr_loader.hpp"
#include "../utils/delay_queue/delay_queue.hpp"

class msr_replayer: public trace_replayer {
private:
    static const int DEFAULT_BATCH_SIZE = 100;
    static std::atomic_long count;

    int batch_size = DEFAULT_BATCH_SIZE;
    std::string trace_file;
    device *m_dev;
    loader *m_loader;
    delay_queue task_queue;
    std::vector<std::thread*> workers;

    class open_failed_exception: public std::exception {
        const char* what() {
            return "file open failed";
        }
    };
public:
    msr_replayer(std::string _trace_file, std::string _dev_name, int64_t _dev_size, int _batch_size, int worker_num):
      trace_file(_trace_file), batch_size(_batch_size) {
        int fd = open(_dev_name.c_str(), O_RDWR | O_CREAT);
        if (fd <= 0) {
            throw new open_failed_exception;
        }
        m_dev = new device(fd, _dev_size);
        
        m_loader = new msr_loader(trace_file);
        workers.resize(worker_num);
    }

    ~msr_replayer() {
        if (m_loader) {
            delete m_loader;
        }
        if (m_dev) {
            delete m_dev;
        }
        for (auto& worker : workers) {
            delete worker;
        }
    }

    void play() {
        std::thread task_producer([&](){
            int diff = 0;
            double reload_factor = 0.3;
            while(true) {
                std::vector<delay_task*> tasks = m_loader->load_batch_task(batch_size-diff);
                for (const auto& task : tasks) {
                    task_queue.push(task);
                }
                diff = task_queue.wait_down_util(batch_size * reload_factor);
            }
        });
        task_producer.detach();


        std::thread monitor([&](){
            while(true) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                int64_t qps = count;
                std::cout << "trace replay qps: " + std::to_string(qps) + "\n";
                count -= qps;
            }
        });

        monitor.detach();

        for (auto& worker : workers) {
            worker = new std::thread([&](){
                while(true) {
                    msr_task *task = (msr_task*)task_queue.get_first();
                    bool success = task->exec(m_dev);
                    if (success) {
                        ++count;
                    }
                    delete task;
                }
            });
        }

        for (const auto& worker : workers) {
            worker->join();
        }
    }
};

std::atomic_long msr_replayer::count(0);
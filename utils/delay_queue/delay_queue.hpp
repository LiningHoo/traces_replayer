#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <unistd.h>
#include <thread>
#include <sys/select.h>

#include "delay_task.hpp"
#include "consts.hpp"
#include "../channel/channel.hpp"

struct ExpireTimeComparator {
    bool operator()(delay_task*& task1, delay_task*& task2) {
        return task1->get_exec_time() > task2->get_exec_time();
    }
};

class delay_queue {
private:
    std::mutex mu, door_lock;
    std::priority_queue<delay_task*, std::vector<delay_task*>, ExpireTimeComparator> task_queue;
    channel newest;
    std::condition_variable cond;
public:
    ~delay_queue() {
        newest.close_chan();
    }

    void push(delay_task *task) {
        mu.lock();
        bool is_top = task_queue.empty() || task_queue.top()->get_exec_time() > task->get_exec_time();
        task_queue.push(task);
        if (is_top) {
            newest.write_to_chan(NEWEST_ELEM, sizeof(NEWEST_ELEM));
        }
        mu.unlock();
    }

    delay_task* get_first() {
        std::lock_guard<std::mutex> lock(door_lock);
    fetch_first:
        mu.lock();
        if (task_queue.empty()) {
            mu.unlock();
            char* bytes = newest.read_from_chan(sizeof(NEWEST_ELEM));
            delete[] bytes;
            goto fetch_first;
        }

        delay_task* current_first_task = task_queue.top();
        if (current_first_task->is_expire()) {
            task_queue.pop();
            cond.notify_all();
            mu.unlock();
            return current_first_task;
        } else {
            mu.unlock();
            channel expire_chan; 
            std::thread timer([&](){
                int64_t left_time = current_first_task->left_time();
                std::this_thread::sleep_for(std::chrono::nanoseconds(left_time));
                expire_chan.write_to_chan(EXPIRE, sizeof(EXPIRE));
            });
            timer.detach();

            fd_set event_fds;
            FD_SET(expire_chan.get_read_fd(), &event_fds);
            FD_SET(newest.get_read_fd(), &event_fds);
            int fd = select(1024, &event_fds, nullptr, nullptr, nullptr);
            char* bytes;
            if (fd == newest.get_read_fd()) {
                bytes = newest.read_from_chan(sizeof(NEWEST_ELEM));
            } else {
                bytes = expire_chan.read_from_chan(sizeof(EXPIRE));
            }
            delete[] bytes;
            expire_chan.close_chan();

            goto fetch_first;
        }
    }

    int wait_down_util(int size) {
        std::unique_lock<std::mutex> lock(mu);
        cond.wait(lock, [&]()->bool {
            return task_queue.size() <= size;
        });
        return task_queue.size();
    }
};
#include <iostream>
#include <thread>
#include <chrono>

#include "delay_task.hpp"
#include "delay_queue.hpp"

class mock_delay_task: public delay_task {
private:
    int id;
    int64_t exec_time;
public:
    mock_delay_task(int _id, int64_t _exec_time): id(_id), exec_time(_exec_time) {

    }

    bool is_expire() {
        return get_current_time() > exec_time;
    }

    int64_t get_exec_time() {
        return exec_time;
    }

    int64_t left_time() {
        return exec_time - get_current_time();
    }

    std::string to_string() {
        return std::string("mock delay task: ") + std::to_string(id);
    }
private:
    static int64_t get_current_time() {
        return std::chrono::high_resolution_clock::now().time_since_epoch().count();
    }
};

int main() {
    delay_queue queue;

    int64_t now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    const int64_t SECOND = 1e9;
    std::thread producer([&](){
        for (int i=0; i<10; ++i) {
            auto task = new mock_delay_task(i, now+SECOND*i);
            queue.push(task);
            // std::cout << task->get_exec_time() << std::endl;
        }
    });
    auto consume = [&](int id){
        for (;;) {
            mock_delay_task *task = (mock_delay_task*)queue.get_first();
            std::cout << "consumer " + std::to_string(id) +  ": " + task->to_string() << std::endl;
            delete task;
        }
    };
    std::thread consumer1(consume, 1), consumer2(consume, 2);

    producer.join();
    consumer1.join();
    consumer2.join();
}
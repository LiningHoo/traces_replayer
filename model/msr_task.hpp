#pragma once

#include <string>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

#include "./device.hpp"
#include "../utils/delay_queue/delay_task.hpp"

typedef std::string io_type;

const io_type IO_WRITE = "Write";
const io_type IO_READ = "Read";

class msr_task: public delay_task {
private:
    int64_t exec_time;
    io_type m_io_type;
    int64_t offset;
    int64_t len;

public:
    msr_task(int64_t _exec_time, io_type _io_type, int64_t _offset, int64_t _len):
        exec_time(_exec_time), m_io_type(_io_type), offset(_offset), len(_len) {}

    bool is_expire() {
        return get_current_time() > exec_time;
    }

    int64_t get_exec_time() {
        return exec_time;
    }

    int64_t left_time() {
        int64_t diff_time = exec_time - get_current_time();
        return std::max<int64_t>(0, diff_time);
    }

    std::string to_string() {
        return std::string("msr task: ") + std::to_string(exec_time) + " " +  
             m_io_type + " " + std::to_string(offset) + " " + std::to_string(len);
    }

    void exec() {
        int64_t now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        // if (now - exec_time > 1e6) {
        //     std::cout << "incorrect!!!\n";
        // }
        // std::cout << "time diff: " + std::to_string(now - exec_time) + "\n";
        std::cout << to_string() + "\n";
    }

    bool exec(device *dev) {
        // int64_t now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        // if (now - exec_time > 1e6)
        //     std::cout << "time diff: " + std::to_string(now - exec_time) + "\n";
        int64_t length = std::min(dev->get_size(), len);
        char *buffer = new char[length];
        for (int i=0; i<length; ++i) {
            buffer[i] = '0';
        }
        int64_t position = std::min(offset % dev->get_size(), dev->get_size() - length);
        lseek(dev->get_fd(), position, SEEK_SET);
        bool sucess = false;
        if (m_io_type == IO_WRITE) {
            int ret = write(dev->get_fd(), buffer, length);
            sucess = ret > 0;
        }
        else if (m_io_type == IO_READ) {
            int ret = read(dev->get_fd(), buffer, length);
            sucess = ret > 0;
        }
        delete[] buffer;

        return sucess;
    }
private:
    static int64_t get_current_time() {
        return std::chrono::high_resolution_clock::now().time_since_epoch().count();
    }
};
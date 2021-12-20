#pragma once

#include <string>
#include <chrono>
#include <fstream>
#include <vector>

#include "loader.hpp"
#include "../../model/msr_task.hpp"
#include "../../utils/csv/csv.hpp"

class msr_loader: public loader {
private:
    static const int COLUMN_NUM = 7;

    int64_t base_time = 0;
    std::string filename;
    io::CSVReader<COLUMN_NUM> *source;
public:
    msr_loader(std::string _filename): filename(_filename) {
        source = new io::CSVReader<COLUMN_NUM>(filename);
        base_time = get_base_time();
    }

    ~msr_loader() {
        if (source)
            delete source;
    }

    delay_task* load_one_task() {
        int64_t timestamp, offset, len, resp_time;
        int volume;
        std::string host, io_type;
        bool eof = !source->read_row(timestamp, host, volume, io_type, offset, len, resp_time);
        if (eof)
            return nullptr;
        return new msr_task(base_time+timestamp, io_type, offset, len);
    }

    std::vector<delay_task*> load_batch_task(int batch_size) {
        std::vector<delay_task*> ret;
        for (int i=0; i<batch_size; ++i) {
            delay_task* task = load_one_task();
            if (task == nullptr) {
                return ret;
            }
            ret.emplace_back(task);
        }
        return ret;
    }
private:
    // maybe a little trick to get base time
    int64_t get_base_time() {
        std::ifstream file(filename);
        int64_t traces_start_time, now;
        file >> traces_start_time;
        now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        return now - traces_start_time;
    }
};
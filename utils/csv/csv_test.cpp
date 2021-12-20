#include <iostream>
#include <string>

#include "csv.hpp"

int main() {
    io::CSVReader<7> in("../../resources/sample.csv");
    // in.read_header(io::ignore_extra_column, "vendor", "size", "speed");
    int64_t timestamp, offset, len, resp_time;
    int volume;
    std::string host, io_type;
    while(in.read_row(timestamp, host, volume, io_type, offset, len, resp_time)){
        // do stuff with the data
        std::cout << timestamp << " " << io_type << " " << offset << " " << len << std::endl;
    }
}
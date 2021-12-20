
#include <string>
#include <cmath>

#include "./model/trace_replayer.hpp"
#include "./model/msr_replayer.hpp"

int main() {
    // const std::string trace_file = "./resources/main_test.csv";
    const std::string trace_file = "/mnt/d/Downloads/msr/MSR-Cambridge/proj_0.csv/CAM-02-SRV-lvm0.csv";
    const int64_t dev_size = pow(2, 30);
    trace_replayer *replayer = new msr_replayer(trace_file, "./test_dev", dev_size, 1000, 100);
    replayer->play();
    delete replayer;
}
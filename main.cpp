
#include <string>

#include "./model/trace_replayer.hpp"
#include "./model/msr_replayer.hpp"

int main() {
    // const std::string trace_file = "./resources/main_test.csv";
    const std::string trace_file = "/mnt/d/Downloads/msr/MSR-Cambridge/proj_0.csv/CAM-02-SRV-lvm0.csv";
    trace_replayer *replayer = new msr_replayer(trace_file, "./test_dev", 4096, 1000, 100);
    replayer->play();
    delete replayer;
}
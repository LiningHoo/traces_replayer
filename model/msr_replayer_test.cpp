#include <string>

#include "msr_replayer.hpp"
#include "trace_replayer.hpp"

const std::string csv_path = "/mnt/d/Downloads/msr/MSR-Cambridge/proj_0.csv/CAM-02-SRV-lvm0.csv";

int main() {
    trace_replayer *player = new msr_replayer(csv_path, "/dev/null", 4096, 100, 10);
    player->play();
    delete player;
}
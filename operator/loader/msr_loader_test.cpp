#include <iostream>

#include <vector>

#include "msr_loader.hpp"

int main() {
    msr_loader loader("../../resources/sample.csv");
    std::vector<delay_task*> tasks = loader.load_batch_task(100);
    for (const auto& task : tasks) {
        std::cout << ((msr_task*)task)->to_string() << std::endl;
        delete task;
    }
}
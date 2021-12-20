#pragma once

#include <vector>

#include "../../utils/delay_queue/delay_task.hpp"

class loader {
public:
    virtual delay_task* load_one_task() = 0;
    virtual std::vector<delay_task*> load_batch_task(int batch_size) = 0;
    virtual ~loader() {}
};
#pragma once

#include <inttypes.h>

#include "../../model/device.hpp"

class delay_task {
public:
    virtual int64_t get_exec_time() = 0;
    virtual bool is_expire() = 0;
    virtual int64_t left_time() = 0;
    virtual void exec() = 0;
    virtual bool exec(device *dev) = 0;
};
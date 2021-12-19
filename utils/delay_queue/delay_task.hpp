#pragma once

#include <inttypes.h>

class delay_task {
public:
    virtual int64_t get_exec_time() = 0;
    virtual bool is_expire() = 0;
    virtual int64_t left_time() = 0;
    virtual void exec() = 0;
};
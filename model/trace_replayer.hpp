#pragma once

#include "../operator/loader/loader.hpp"

class trace_replayer {
public:
    virtual void play() = 0;
    virtual ~trace_replayer() {}
};
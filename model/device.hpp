#pragma once

#include <inttypes.h>

class device {
private:
    int fd;
    int64_t size;

public:
    device(int _fd, int64_t _size):
      fd(_fd), size(_size) {}

    int get_fd() {
        return fd;
    }

    int64_t get_size() {
        return size;
    }
};
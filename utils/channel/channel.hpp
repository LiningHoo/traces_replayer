#pragma once

#include <unistd.h>
#include <exception>

class channel {
private:
    int read_fd;
    int write_fd;
public:
    channel() {
        int fds[2];
        if(pipe(fds) == 0) {
            read_fd = fds[0], write_fd = fds[1];
        } else {
            throw new create_pipe_error;
        }
    }

    bool write_to_chan(char *bytes, int size) {
        if(write(write_fd, bytes, size))
            return false;
        return true;
    }

    char* read_from_chan(int size) {
        char *buffer = new char[size];
        read(read_fd, buffer, size);
        return buffer;
    }

    void close_chan() {
        close(read_fd);
        close(write_fd);
    }

    int get_read_fd() {
        return read_fd;
    }

private:
    class create_pipe_error: public std::exception {
    public:
        const char* what() {
            return "create pipe line error";
        }
    };
};
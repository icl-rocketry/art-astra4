#pragma once

#include <Filesys.h>
// All of this will be replaced by libriccore after astra 4 so no point making it good code
// Just in case it isn't imma write some comments

class Logger {
public:
    Logger(File32 log_file) : _log_file(log_file) {}


    void log_line(String item) {
        _log_file.write(item.c_str());
        _log_file.write('\n');
    }

    // Guess what? Since this code will be thrown away anyways, I might as well have some fun with it
    // Bon appetit
    template <typename T>
    void log(T item) {
        char* buffer = static_cast<char*>(&item);
        _log_file.write(buffer, sizeof(T));
    }

    template <typename First, typename Second, typename... Rest>
    void log(First f, Second s, Rest... rest) {
        log(f);
        log(s, rest...);
    }

private:
    File32 _log_file;
};
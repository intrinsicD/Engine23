//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_FILEWATCHER_H
#define ENGINE23_FILEWATCHER_H

#include <unordered_map>
#include <vector>
#include <functional>
#include <chrono>
#include <string>

namespace Bcg {
    struct FileWatcher {
        std::unordered_map<std::string, std::vector<std::function<void()>>> items;
        std::unordered_map<std::string, std::chrono::file_clock::time_point> last_write_times;

        void add(std::string filepath, std::function<void()> callback);

        void remove(std::string filepath);

        void clear();

        void check();
    };
}

#endif //ENGINE23_FILEWATCHER_H

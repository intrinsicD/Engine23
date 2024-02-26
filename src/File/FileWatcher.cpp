//
// Created by alex on 26.02.24.
//

#include "FileWatcher.h"
#include <filesystem>

namespace Bcg{
    void FileWatcher::add(const std::string& filepath, const std::function<void()>& callback) {
        last_write_times[filepath] = std::filesystem::last_write_time(filepath);
        items[filepath].emplace_back(callback);
    }

    void FileWatcher::remove(const std::string& filepath) {
        items.erase(filepath);
    }

    void FileWatcher::clear() {
        items.clear();
    }

    void FileWatcher::check() {
        for (auto &[filepath, callbacks]: items) {
            auto &last_write_time = last_write_times[filepath];
            auto current_write_time = std::filesystem::last_write_time(filepath);
            //compare write times and call callbacks if necessary
            if ((current_write_time - last_write_time).count()) {
                last_write_time = current_write_time;
                for (auto &callback: callbacks) {
                    callback();
                }
            }
        }
    }
}
//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_COMMANDDOUBLEBUFFER_H
#define ENGINE23_COMMANDDOUBLEBUFFER_H

#include "CommandBuffer.h"
#include <mutex>

namespace Bcg {
    struct CommandDoubleBuffer {
        CommandDoubleBuffer(CommandBuffer *current, CommandBuffer *next) : p_current(current), p_next(next) {}

        CommandBuffer *p_current;
        CommandBuffer *p_next;
        std::mutex currentMutex;
        std::mutex nextMutex;

        void enqueue_current(std::shared_ptr<Command> command) {
            std::unique_lock<std::mutex> lock(currentMutex);
            p_current->emplace_back(std::move(command));
        }

        void enqueue_next(std::shared_ptr<Command> command) {
            std::unique_lock<std::mutex> lock(nextMutex);
            p_next->emplace_back(std::move(command));
        }

        void clear_and_swap() {
            std::unique_lock<std::mutex> lock_curr(currentMutex);
            std::unique_lock<std::mutex> lock_next(nextMutex);
            p_current->clear_and_swap();
            std::swap(p_current, p_next);
        }
    };
}
#endif //ENGINE23_COMMANDDOUBLEBUFFER_H

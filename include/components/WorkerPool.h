//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_WORKERPOOL_H
#define ENGINE23_WORKERPOOL_H

#include <vector>
#include <memory>
#include <queue>
#include <thread>
#include <condition_variable>

namespace Bcg {
    struct AbstractCommand;

    struct WorkerPool {
        std::vector<std::thread> workers;
        std::queue<std::shared_ptr<AbstractCommand>> tasks;

        std::mutex queueMutex;
        std::condition_variable condition;
        bool stop = false;
    };
}
#endif //ENGINE23_WORKERPOOL_H

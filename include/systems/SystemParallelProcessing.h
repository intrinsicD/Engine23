//
// Created by alex on 29.11.23.
//

#ifndef ENGINE23_SYSTEMPARALLELPROCESSING_H
#define ENGINE23_SYSTEMPARALLELPROCESSING_H

#include "EngineFwd.h"

namespace Bcg {
    class SystemParallelProcessing {
    public:
        SystemParallelProcessing() = default;

        ~SystemParallelProcessing() = default;

        static std::string name();

        static void start(int num_threads);

        static void stop();

        static void resize(int num_threads);

        static void enqueue(std::shared_ptr<Command> command);

        static void enqueue(std::vector<std::shared_ptr<Command>> commands);

    protected:
        friend Engine;

        static void pre_init();

        static void init();

        static void remove();
    };
}

#endif //ENGINE23_SYSTEMPARALLELPROCESSING_H

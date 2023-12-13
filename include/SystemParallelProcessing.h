//
// Created by alex on 29.11.23.
//

#ifndef ENGINE23_SYSTEMPARALLELPROCESSING_H
#define ENGINE23_SYSTEMPARALLELPROCESSING_H

#include "System.h"

namespace Bcg {
    class SystemParallelProcessing : protected System {
    public:
        SystemParallelProcessing();

        ~SystemParallelProcessing() override = default;

        static void start(int num_threads);

        static void stop();

        static void resize(int num_threads);

        static void enqueue(std::shared_ptr<Command> command);

        static void enqueue(std::vector<std::shared_ptr<Command>> commands);

    protected:
        friend Engine;

        void pre_init() override;

        void init() override;

        void remove() override;
    };
}

#endif //ENGINE23_SYSTEMPARALLELPROCESSING_H

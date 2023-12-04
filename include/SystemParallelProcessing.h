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

        void start(int num_threads);

        void stop();

        void resize(int num_threads);

        void enqueue(std::shared_ptr<Command> command);

        void enqueue(std::vector<std::shared_ptr<Command>> commands);

    protected:
        friend Engine;

        void pre_init_system() override;

        void init_system() override;

        void remove_system() override;
    };
}

#endif //ENGINE23_SYSTEMPARALLELPROCESSING_H
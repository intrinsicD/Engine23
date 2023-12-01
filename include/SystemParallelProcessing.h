//
// Created by alex on 29.11.23.
//

#ifndef ENGINE23_SYSTEMPARALLELPROCESSING_H
#define ENGINE23_SYSTEMPARALLELPROCESSING_H

#include "Events.h"
#include "EngineFwd.h"
#include <memory>
#include <vector>

namespace Bcg::System::ParallelProcessing{
    void pre_init_system();

    void init_system();

    void remove_system();

    void start(int num_threads);

    void stop();

    void resize(int num_threads);

    void enqueue_parallel(std::shared_ptr<Command> command);

    void enqueue_parallel(std::vector<std::shared_ptr<Command>> commands);

    void on_startup_engine(const Events::Startup<Engine> &event);

    void on_shutdown_engine(const Events::Shutdown<Engine> &event);

    void on_begin_frame(const Events::Begin<Frame> &event);
}

#endif //ENGINE23_SYSTEMPARALLELPROCESSING_H

//
// Created by alex on 22.11.23.
//

#ifndef ENGINE23_FACTORIES_H
#define ENGINE23_FACTORIES_H

#include "Managers.h"
#include "Systems.h"

namespace Bcg {
    class Factory {
    public:
        Factory() = default;

        virtual ~Factory() = default;
    };

    class ManagerFactory : public Factory {
    public:
        ManagerFactory() = default;

        ~ManagerFactory() override = default;

        static CommandBufferManager *create_or_get_command_buffer_manager();

        static WorkerPoolManager *create_or_get_worker_pool_manager();
    };
}

#endif //ENGINE23_FACTORIES_H

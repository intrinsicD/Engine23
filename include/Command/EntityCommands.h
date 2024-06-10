//
// Created by alex on 10.06.24.
//

#ifndef ENGINE23_ENTITYCOMMANDS_H
#define ENGINE23_ENTITYCOMMANDS_H

#include "Commands.h"

namespace Bcg::Command::Entity{
    struct Create : public AbstractCommand {
        Create();

        ~Create() override = default;

        int execute() override;
    };

    struct Destroy : public AbstractCommand {
        Destroy();

        ~Destroy() override = default;

        int execute() override;
    };
}

#endif //ENGINE23_ENTITYCOMMANDS_H

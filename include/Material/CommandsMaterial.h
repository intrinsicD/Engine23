//
// Created by alex on 18.06.24.
//

#ifndef ENGINE23_COMMANDSMATERIAL_H
#define ENGINE23_COMMANDSMATERIAL_H

#include "Commands.h"

namespace Bcg{
    struct InitMaterials : public AbstractCommand{
        InitMaterials(entt::entity entity_id);

        ~InitMaterials() override = default;

        int execute() override;

    private:
        entt::entity entity_id;
    };
}

#endif //ENGINE23_COMMANDSMATERIAL_H

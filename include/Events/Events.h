//
// Created by alex on 24.11.23.
//

#ifndef ENGINE23_EVENTS_H
#define ENGINE23_EVENTS_H

#include "entt/entity/entity.hpp"

namespace Bcg::Events{
    template<typename T>
    struct Startup{

    };

    template<typename T>
    struct Shutdown{

    };

    template<typename T>
    struct Update{
        entt::entity entity_id = entt::null;
    };

    template<typename T>
    struct Render{

    };

    template<typename T>
    struct Begin{

    };

    template<typename T>
    struct End{

    };

    template<typename T>
    struct Notify{

    };

    template<typename T>
    struct Change{

    };

    template<typename T>
    struct Load{
        entt::entity entity_id = entt::null;
        std::string filepath;
    };

    template<typename T>
    struct Save{
        entt::entity entity_id = entt::null;
        std::string filepath;
    };

    template<typename T>
    struct Create{
        T *return_value;
    };

    template<typename T>
    struct Destroy{
        T *value;
    };
}

#endif //ENGINE23_EVENTS_H

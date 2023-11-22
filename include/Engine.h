//
// Created by alex on 22.11.23.
//

#ifndef ENGINE23_ENGINE_H
#define ENGINE23_ENGINE_H

#include "entt/entt.hpp"

namespace Bcg {
    class Engine {
    public:
        Engine();

        virtual ~Engine();

        virtual void run();

        static Engine *get_instance();

        entt::registry state;
        entt::dispatcher dispatcher;
        bool is_running = false;
    };

    //Main way to have access to the engines state
    static auto &get_state() {
        return Engine::get_instance()->state;
    }

    //Main way to have access to the engines state context
    static auto &get_state_ctx() {
        return get_state().ctx();
    }
}


#endif //ENGINE23_ENGINE_H

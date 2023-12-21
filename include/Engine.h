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

        //Main way to have access to the engine
        static Engine *Instance() {
            return entt::locator<Engine *>::value();
        }

        //Main way to have access to the engines state
        static auto &State() {
            return Instance()->state;
        }

        //Main way to have access to the engines state context
        static auto &Context() {
            return State().ctx();
        }

        entt::registry state;
        entt::dispatcher dispatcher;
        bool is_running = false;
    };
}


#endif //ENGINE23_ENGINE_H

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

        static auto &Dispatcher() {
            return Instance()->dispatcher;
        }

        std::function<void()> window_close_callback;
        std::function<void()> window_size_callback;
        std::function<void()> mouse_button_callback;
        std::function<void()> cursor_pos_callback;
        std::function<void()> scroll_callback;
        std::function<void()> key_callback;
        std::function<void()> drop_callback;
        std::function<void()> user_callback;

        entt::registry state;
        entt::dispatcher dispatcher;
        bool is_running = false;
    };
}


#endif //ENGINE23_ENGINE_H

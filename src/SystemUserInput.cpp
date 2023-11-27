//
// Created by alex on 27.11.23.
//

#include "SystemUserInput.h"
#include "Engine.h"
#include "Commands.h"
#include "Components.h"

namespace Bcg::System::UserInput{
    void add_system(){
        if (!Engine::Context().find<Input>()) {
            Engine::Context().emplace<Input>();
            Log::Info("SystemUserInput: Added Input Component to Context");
        }
        Log::Info("SystemUserInput: Added");
    }

    void remove_system(){
        if (Engine::Context().find<Input>()) {
            Engine::Context().erase<Input>();
            Log::Info("SystemUserInput: Removed Input Component from Context");
        }
        Log::Info("SystemUserInput: Removed");
    }

    void on_startup_engine(const Events::Startup<Engine> &event);

    void on_shutdown_engine(const Events::Shutdown<Engine> &event);
}
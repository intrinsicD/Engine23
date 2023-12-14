//
// Created by alex on 05.12.23.
//

#include "SystemBuffers.h"
#include "Engine.h"
#include "Commands.h"

namespace Bcg {
    std::string SystemBuffers::name() {
        return "SystemBuffers";
    }

    void SystemBuffers::pre_init() {

    }

    void SystemBuffers::init() {
        Log::Info(name() + ": Initialized").enqueue();
    }

    void SystemBuffers::remove() {
        Log::Info(name() + ": Removed").enqueue();
    }
}
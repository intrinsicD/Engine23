//
// Created by alex on 05.12.23.
//

#include "SystemBuffers.h"
#include "Engine.h"
#include "Commands.h"

namespace Bcg {
    SystemBuffers::SystemBuffers() : System("SystemBuffers") {

    }

    void SystemBuffers::pre_init() {

    }

    void SystemBuffers::init() {
        Log::Info(m_name + ": Initialized").enqueue();
    }

    void SystemBuffers::remove() {
        Log::Info(m_name + ": Removed").enqueue();
    }
}
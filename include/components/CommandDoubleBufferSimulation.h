//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_COMMANDDOUBLEBUFFERSIMULATION_H
#define ENGINE23_COMMANDDOUBLEBUFFERSIMULATION_H

#include "CommandDoubleBuffer.h"

namespace Bcg {
    struct SimulationCommandDoubleBuffer : public CommandDoubleBuffer {
        SimulationCommandDoubleBuffer(CommandBuffer *current, CommandBuffer *next) : CommandDoubleBuffer(current,
                                                                                                         next) {}
    };
}

#endif //ENGINE23_COMMANDDOUBLEBUFFERSIMULATION_H

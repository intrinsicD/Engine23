//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_COMMANDDOUBLEBUFFERRENDER_H
#define ENGINE23_COMMANDDOUBLEBUFFERRENDER_H

#include "CommandDoubleBuffer.h"

namespace Bcg {
    struct RenderCommandDoubleBuffer : public CommandDoubleBuffer {
        RenderCommandDoubleBuffer(CommandBuffer *current, CommandBuffer *next) : CommandDoubleBuffer(current, next) {}
    };
}
#endif //ENGINE23_COMMANDDOUBLEBUFFERRENDER_H

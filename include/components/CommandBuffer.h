//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_COMMANDBUFFER_H
#define ENGINE23_COMMANDBUFFER_H

#include <vector>
#include <memory>

namespace Bcg {
    struct Command;

    struct CommandBuffer : public std::vector<std::shared_ptr<AbstractCommand>> {
        using std::vector<std::shared_ptr<AbstractCommand>>::vector;

        void clear_and_swap() {
            std::vector<std::shared_ptr<AbstractCommand>>().swap(*this);
        }
    };
}

#endif //ENGINE23_COMMANDBUFFER_H

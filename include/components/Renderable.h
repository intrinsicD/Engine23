//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_RENDERABLE_H
#define ENGINE23_RENDERABLE_H

#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

namespace Bcg {
    struct Renderable {
        virtual void update_local_uniforms(unsigned int shader_id) const = 0;

        virtual void draw() const = 0;
    };

    struct RenderBatch {
        std::string name;

        virtual void update_global_uniforms(unsigned int shader_id) const = 0;

        std::vector<std::shared_ptr<Renderable>> renderables;
    };

    struct RenderBatches {
        std::unordered_map<unsigned int, std::shared_ptr<RenderBatch>> batches;
    };
}

#endif //ENGINE23_RENDERABLE_H

//
// Created by alex on 20.03.24.
//

#ifndef ENGINE23_DEFAULTMESHRENDERPASS_H
#define ENGINE23_DEFAULTMESHRENDERPASS_H

#include "RenderPass.h"

namespace Bcg {
    class DefaultMeshRenderPass : public RenderPass {
    public:
        DefaultMeshRenderPass();

        ~DefaultMeshRenderPass() override = default;

        void execute() const override;
    };
}

#endif //ENGINE23_DEFAULTMESHRENDERPASS_H

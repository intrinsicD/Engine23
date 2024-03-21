//
// Created by alex on 20.03.24.
//

#include "DefaultMeshRenderPass.h"
#include "Engine.h"
#include "Camera.h"

namespace Bcg {
    void DefaultMeshRenderPass::execute() const {
        auto &backend = Engine::Context().get<RenderingBackend>();

        auto &camera = Engine::Context().get<Camera>();
        auto &program = backend.get_program("DefaultMeshRenderPass");
        backend.use(program);
        backend.update(camera);

        for (auto entity_id : m_entities) {
            auto &transform = Engine::State().get<Transform>(entity_id);
            auto &material = Engine::State().get<DefaultMeshRenderPass::Material>(entity_id);
            auto &renderable = Engine::State().get<Renderable::Mesh>(entity_id);

            backend.update(transform);
            backend.update(material);
            backend.render(renderable);
        }
    }
}
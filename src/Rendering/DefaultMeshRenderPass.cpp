//
// Created by alex on 20.03.24.
//

#include "DefaultMeshRenderPass.h"
#include "Engine.h"
#include "Camera.h"
#include "Transform.h"

namespace Bcg {
    void DefaultMeshRenderPass::execute() const {
        auto &camera = Engine::Context().get<Camera>();
        auto &programs = Engine::Context().get<OpenGL::ShaderPrograms>();
        auto program = programs["learn_opengl"];

        program.use();
        program.set_mat4("view", camera.get_view().data());
        program.set_mat4("projection", camera.get_projection().data());

        for (auto entity_id : m_entities) {
            auto &transform = Engine::State().get<Transform>(entity_id);
            auto &material = Engine::State().get<DefaultMeshRenderPass::Material>(entity_id);
            auto &renderable = Engine::State().get<OpenGL::RenderableTriangles>(entity_id);

            program.set_vec3("our_color", renderable.our_color);
            program.set_mat4("model", transform.model.matrix().data());
            renderable.vao.bind();
            renderable.draw();
            renderable.vao.release();
        }
    }
}
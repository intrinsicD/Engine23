//
// Created by alex on 28.02.24.
//

#include "GLMeshRenderPass.h"
#include "Engine.h"
#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"
#include "AABB.h"
#include "MeshVertexNormalsAreaAngleHybrid.h"
#include "PropertyEigenMap.h"
#include "OpenGLUtils.h"

namespace Bcg{
    //Currently the render pass is rendering the mesh
    void GLMeshRenderPass::prepare(){

    }

    void GLMeshRenderPass::render(){
        auto view = Engine::State().view<OpenGL::RenderableTriangles, Transform<float>>();
        auto &camera = Engine::Context().get<Camera>();
        //bind shader_program
        //update ubos (camera, etc...)

        auto &programs = Engine::Context().get<OpenGL::ShaderPrograms>();
        auto program = programs["learn_opengl"];

        program.use();
        program.set_mat4("view", camera.get_view().data());
        program.set_mat4("projection", camera.get_projection().data());

        for(auto entity_id : view){
            auto &transform = view.get<Transform<float>>(entity_id);
            auto &renderable = view.get<OpenGL::RenderableTriangles>(entity_id);
            program.set_vec3("our_color", renderable.our_color);
            program.set_mat4("model", transform.model.matrix().data());
            renderable.vao.bind();
            renderable.draw();
            renderable.vao.release();
        }
    }

    void GLMeshRenderPass::post_render(){

    }

    void GLMeshRenderPass::execute(){
        prepare();
        render();
        post_render();
    }
}
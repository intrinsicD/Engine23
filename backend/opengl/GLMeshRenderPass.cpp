//
// Created by alex on 28.02.24.
//

#include "GLMeshRenderPass.h"
#include "Engine.h"
#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"
#include "OpenGLUtils.h"
#include "Component.h"
#include "ResourceContainer.h"

namespace Bcg{
    //Currently the render pass is rendering the mesh
    void GLMeshRenderPass::prepare(){

    }

    void GLMeshRenderPass::render(){
        auto view = Engine::State().group<OpenGL::RenderableTriangles, Component<Transform<float>>>();
        if(view.empty()){
            return;
        }
        auto &camera = Engine::Context().get<Camera<float>>();
        //bind shader_program
        //update ubos (camera, etc...)

        auto &programs = Engine::Context().get<OpenGL::ShaderPrograms>();
        auto program = programs["simple_shading"];

        program.use();
        program.set_mat4("view", camera.get_view().data());
        program.set_mat4("projection", camera.get_projection().data());

        auto &transforms = Engine::Context().get<ResourceContainer<Transform<float>>>();

        for(auto entity_id : view){
            auto &transform_id = view.get<Component<Transform<float>>>(entity_id).index;
            auto &transform = transforms.pool[transform_id];
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
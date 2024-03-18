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

    void GLMeshRenderPass::prepare(){
        auto view = Engine::State().view<GLMeshRenderPass, Mesh>();
        std::vector<entt::entity> to_remove;
        for(auto entity_id : view){
           /* if(!Engine::State().all_of<OpenGL::RenderableTriangles>(entity_id)){
                auto &mesh = Engine::State().get<Mesh>(entity_id);
                auto positions = mesh.vertices.get<Eigen::Vector<double, 3>>("v_position");
                auto normals = mesh.vertices.get<Eigen::Vector<double, 3>>("v_normal");
                auto triangles = mesh.get_triangles();

                Engine::State().emplace<Transform>(entity_id);
                auto renderable_triangles = OpenGL::RenderableTriangles::Create();
                renderable_triangles.vbo = OpenGL::VertexBufferObject::Static();
                renderable_triangles.ebo = OpenGL::IndexBufferObject::Static();

                renderable_triangles.vao.create();
                renderable_triangles.vbo.create();
                renderable_triangles.ebo.create();

                renderable_triangles.vao.bind();
                renderable_triangles.vbo.bind();

                Eigen::Matrix<float, 3, -1> positions_float = Map(positions).transpose().cast<float>();
                Eigen::Matrix<float, 3, -1> normals_float = Map(normals).transpose().cast<float>();
                unsigned int size_in_bytes = positions_float.size() * sizeof(float);
                renderable_triangles.vbo.set_data(nullptr, 2 * size_in_bytes);

                renderable_triangles.vbo.set_sub_data(positions_float.data(), size_in_bytes, 0);
                renderable_triangles.vbo.set_sub_data(normals_float.data(), size_in_bytes, size_in_bytes);

                renderable_triangles.ebo.bind();
                renderable_triangles.ebo.set_data(triangles.get_void_ptr(), triangles.get_size_bytes());

                renderable_triangles.vao.set_float_attribute(0, positions.get_dims(), false, (void *) 0);
                renderable_triangles.vao.set_float_attribute(1, normals.get_dims(), false, (void *) (size_in_bytes));
                renderable_triangles.vao.set_float_attribute(2, normals.get_dims(), false, (void *) (size_in_bytes));

                renderable_triangles.vbo.release();
                renderable_triangles.vao.release();
                renderable_triangles.ebo.release();
                auto &programs = Engine::Context().get<OpenGL::ShaderPrograms>();
                renderable_triangles.program = programs["learn_opengl"];
                renderable_triangles.count = triangles.get_dims() * triangles.get_size();
                renderable_triangles.offset = 0;
                renderable_triangles.our_color[0] = 1.0f;
                renderable_triangles.our_color[1] = 0.5f;
                renderable_triangles.our_color[2] = 0.2f;

                Engine::State().emplace<OpenGL::RenderableTriangles>(entity_id, renderable_triangles);
                to_remove.push_back(entity_id);
            }*/
        }
        for(auto &entity_id : to_remove){
            Engine::State().remove<GLMeshRenderPass>(entity_id);
        }
    }

    void GLMeshRenderPass::render(){
        auto view = Engine::State().view<OpenGL::RenderableTriangles, Transform>();
        auto &camera = Engine::Context().get<Camera>();
        //bind shader_program
        //update ubos (camera, etc...)

        auto &programs = Engine::Context().get<OpenGL::ShaderPrograms>();
        auto program = programs["learn_opengl"];

        program.use();
        program.set_mat4("view", camera.get_view().data());
        program.set_mat4("projection", camera.get_projection().data());

        for(auto entity_id : view){
            auto &transform = view.get<Transform>(entity_id);
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
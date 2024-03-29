//
// Created by alex on 06.12.23.
//

#include "SystemVertexArrayObject.h"
#include "Engine.h"
#include "Commands.h"
#include "glad/gl.h"

namespace Bcg {
    OpenGL::VertexArrayObject SystemVertexArrayObject::create_vertex_array_object(std::string name) {

        OpenGL::VertexArrayObject vao;
        vao.name = name;

        glGenVertexArrays(1, &vao.id);
        OpenGL::AssertNoOglError();
        return vao;
    }

    void SystemVertexArrayObject::delete_vertex_array_object(OpenGL::VertexArrayObject &vao) {
        glDeleteVertexArrays(1, &vao.id);
        OpenGL::AssertNoOglError();
    }

    void SystemVertexArrayObject::bind_vertex_array_object(OpenGL::VertexArrayObject &vao) {
        glBindVertexArray(vao.id);
        OpenGL::AssertNoOglError();
    }

    void SystemVertexArrayObject::unbind_vertex_array_object() {
        glBindVertexArray(0);
        OpenGL::AssertNoOglError();
    }

    std::string SystemVertexArrayObject::name() {
        return "SystemVertexArrayObject";
    }

    void SystemVertexArrayObject::pre_init() {

    }

    void SystemVertexArrayObject::init() {
        Log::Info("Initialized", name()).enqueue();
    }

    void SystemVertexArrayObject::remove() {
        Log::Info("Removed", name()).enqueue();
    }
}
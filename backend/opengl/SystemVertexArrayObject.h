//
// Created by alex on 06.12.23.
//

#ifndef ENGINE23_SYSTEMVERTEXARRAYOBJECT_H
#define ENGINE23_SYSTEMVERTEXARRAYOBJECT_H

#include "EngineFwd.h"
#include "OpenGLUtils.h"

namespace Bcg {
    class SystemRendererOpenGL;

    class SystemVertexArrayObject{
    public:
        SystemVertexArrayObject() = default;

        ~SystemVertexArrayObject() = default;

        static std::string name();

        static OpenGL::VertexArrayObject create_vertex_array_object(std::string name);

        static void delete_vertex_array_object(OpenGL::VertexArrayObject &vao);

        static void bind_vertex_array_object(OpenGL::VertexArrayObject &vao);

        static void unbind_vertex_array_object();

    protected:
        friend Engine;
        friend SystemRendererOpenGL;

        static void pre_init();

        static void init();

        static void remove();
    };
}

#endif //ENGINE23_SYSTEMVERTEXARRAYOBJECT_H

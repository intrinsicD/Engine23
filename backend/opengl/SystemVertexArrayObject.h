//
// Created by alex on 06.12.23.
//

#ifndef ENGINE23_SYSTEMVERTEXARRAYOBJECT_H
#define ENGINE23_SYSTEMVERTEXARRAYOBJECT_H

#include "System.h"
#include "OpenGLUtils.h"

namespace Bcg {
    class SystemRendererOpenGL;

    class SystemVertexArrayObject : public System {
    public:
        SystemVertexArrayObject();

        ~SystemVertexArrayObject() override = default;

        OpenGL::VertexArrayObject create_vertex_array_object(std::string name);

        void delete_vertex_array_object(OpenGL::VertexArrayObject &vao);

        void bind_vertex_array_object(OpenGL::VertexArrayObject &vao);

        void unbind_vertex_array_object();

    protected:
        friend Engine;
        friend SystemRendererOpenGL;

        void pre_init() override;

        void init() override;

        void remove() override;
    };
}

#endif //ENGINE23_SYSTEMVERTEXARRAYOBJECT_H

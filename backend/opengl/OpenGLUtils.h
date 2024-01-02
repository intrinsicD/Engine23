//
// Created by alex on 05.12.23.
//

#ifndef ENGINE23_OPENGLUTILS_H
#define ENGINE23_OPENGLUTILS_H

#include <string>
#include <array>
#include <exception>
#include "Components.h"

namespace Bcg::OpenGL {
    void AssertNoOglError();

    std::string TypeToString(unsigned int type);

    unsigned int TypeToSize(unsigned int type);

    unsigned int TypeToElementCount(unsigned int type);

    //------------------------------------------------------------------------------------------------------------------
    // OpenGL Components
    //------------------------------------------------------------------------------------------------------------------

    struct Shader {
        unsigned int id = 0;
        unsigned int type = 0;
        std::string name;
        std::string filepath;
        std::string source;
        std::string error_message;

        void load_sources();

        void compile();

        bool check_compile_status();
    };

    struct ShaderProgram {
        unsigned int id = 0;
        std::string name;
        std::string error_message;
        Shader v_shader;
        Shader f_shader;
        Shader g_shader;
        Shader tc_shader;
        Shader te_shader;
        Shader c_shader;

        void load_shaders();

        void compile_from_sources();

        void link();

        bool check_link_status();

        void use();

        void set_vec3(const std::string &name, float r, float g, float b) const;

        void set_vec3(const std::string &name, const float *value) const;

        void set_mat4(const std::string &name, const float *value) const;
    };

    struct ShaderPrograms : public Cache<std::string, ShaderProgram> {
        using Cache<std::string, ShaderProgram>::Cache;
    };

    struct BufferObject {
        unsigned int id;
        unsigned int type;
        unsigned int size;
        unsigned int usage;
        std::string name;

        static BufferObject Static();

        static BufferObject Dynamic();

        void create();

        void destroy();

        void bind();

        void set_data(const void *data, unsigned int size);

        void set_sub_data(const void *data, unsigned int size, unsigned int offset);

        void release();
    };

    struct VertexBufferObject : public BufferObject {
        static VertexBufferObject Static();

        static VertexBufferObject Dynamic();
    };

    struct IndexBufferObject : public BufferObject {
        static IndexBufferObject Static();

        static IndexBufferObject Dynamic();
    };


    struct DoubleBuffer {
        std::array<BufferObject, 2> buffers;
        bool next_is_dirty = false;

        unsigned int current_id() const {
            return buffers[0].id;
        }

        unsigned int next_id() const {
            return buffers[1].id;
        }

        BufferObject &current() {
            return buffers[0];
        }

        BufferObject &next() {
            return buffers[1];
        }

        void swap() {
            if (next_is_dirty) {
                std::swap(buffers[0].id, buffers[1].id);
                next_is_dirty = false;
            }
        }
    };


    struct VertexAttribute {
        VertexAttribute(unsigned int index, unsigned int size, unsigned int type, bool normalized, unsigned int stride,
                        const void *pointer) : index(index), size(size), type(type), normalized(normalized),
                                               stride(stride), pointer(pointer) {}

        unsigned int index;
        unsigned int size;
        unsigned int type;
        bool normalized;
        unsigned int stride;
        const void *pointer;

        void enable();

        void disable();
    };

    struct VertexAttributeLayout {
        std::vector<VertexAttribute> attributes;
        unsigned int stride = 0;
    };

    struct VertexArrayObject {
        unsigned int id;
        std::string name;
        VertexAttributeLayout layout;

        void create();

        void destroy();

        void bind();

        void set_float_attribute(unsigned int index, unsigned int size, bool normalized, const void *pointer);

        void set_double_attribute(unsigned int index, unsigned int size, bool normalized, const void *pointer);

        void set_int_attribute(unsigned int index, unsigned int size, bool normalized, const void *pointer);

        void set_unsigned_int_attribute(unsigned int index, unsigned int size, bool normalized, const void *pointer);

        void release();
    };

    struct Renderable {
        OpenGL::VertexArrayObject vao;
        OpenGL::VertexBufferObject vbo;
        OpenGL::IndexBufferObject ebo;
        OpenGL::ShaderProgram program;
        unsigned int mode;
        unsigned int count;
        unsigned int type;
        unsigned int offset = 0;
        float our_color[3] = {1.0f, 1.0f, 1.0f};

        virtual void draw();
    };

    struct RenderableTriangles : public Renderable {
        static RenderableTriangles Create();

        void draw() override;
    };

    struct RenderableLines : public Renderable {
        static RenderableLines Create();

        void draw() override;
    };

    struct RenderablePoints : public Renderable {
        static RenderablePoints Create();

        void draw() override;
    };
}

#endif //ENGINE23_OPENGLUTILS_H

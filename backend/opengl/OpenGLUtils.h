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
    };

    struct ShaderProgram {
        unsigned int id = 0;
        std::string name;
        Shader v_shader;
        Shader f_shader;
        Shader g_shader;
        Shader tc_shader;
        Shader te_shader;
        Shader c_shader;
    };

    struct ShaderPrograms : public Cache<std::string, ShaderProgram> {
        using Cache<std::string, ShaderProgram>::Cache;
    };

    struct Buffer {
        enum class Format {
            Plain,
            Batched,
            Interleaved
        } format;
        unsigned int id;
        unsigned int type;
        unsigned int size;
        unsigned int usage;
        std::string name;
    };

    struct DoubleBuffer {
        std::array<Buffer, 2> buffers;
        bool next_is_dirty = false;

        unsigned int current_id() const {
            return buffers[0].id;
        }

        unsigned int next_id() const {
            return buffers[1].id;
        }

        Buffer &current() {
            return buffers[0];
        }

        Buffer &next() {
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
        std::string name;
        unsigned int type;
        unsigned int element_count = 1;
        unsigned int offset = 0;
    };

    struct VertexAttributeLayout {
        std::vector<VertexAttribute> attributes;
        unsigned int stride = 0;
    };

    struct VertexArrayObject{
        unsigned int id;
        std::string name;
        std::vector<Buffer> buffers;
        VertexAttributeLayout layout;
    };
}

#endif //ENGINE23_OPENGLUTILS_H

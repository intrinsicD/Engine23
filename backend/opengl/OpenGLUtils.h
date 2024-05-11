//
// Created by alex on 05.12.23.
//

#ifndef ENGINE23_OPENGLUTILS_H
#define ENGINE23_OPENGLUTILS_H

#include <string>
#include <array>
#include <exception>
#include <unordered_map>
#include <vector>
#include "ComponentGui.h"

namespace Bcg::OpenGL {
    void AssertNoOglError();

    std::string TypeToString(unsigned int type);

    unsigned int TypeToSize(unsigned int type);

    unsigned int TypeToElementCount(unsigned int type);

    struct DrawCall {
        unsigned int mode;
        unsigned int count;
        unsigned int type;
        unsigned int offset;
        unsigned int program;
        unsigned int vao;
        std::vector<unsigned int> textures;
    };

    struct MeshDrawCall : public DrawCall {
        MeshDrawCall();
    };

    struct GraphDrawCall : public DrawCall {
        GraphDrawCall();
    };

    struct PointCloudDrawCall : public DrawCall {
        PointCloudDrawCall();
    };

    struct VectorFieldDrawCall : public DrawCall {
        VectorFieldDrawCall();
    };

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

        operator unsigned int() const;

        void load_shaders();

        void compile_from_sources();

        void link();

        bool check_link_status();

        void use();

        void set_vec3(const std::string &name, float r, float g, float b) const;

        void set_vec3(const std::string &name, const float *value) const;

        void set_mat4(const std::string &name, const float *value) const;
    };

    struct ShaderPrograms : public std::unordered_map<std::string, ShaderProgram> {
        using std::unordered_map<std::string, ShaderProgram>::unordered_map;
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

        void set_float_attribute(unsigned int index, unsigned int size, bool normalized, unsigned int stride,
                                 const void *pointer);

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


    struct State {
        struct DepthTest {
            bool enabled = false;
            bool mask = false;
            unsigned int func = 0;
        } depth_test;
        struct Blend {
            bool enabled = false;
            unsigned int src = 0;
            unsigned int dst = 0;
            unsigned int equation = 0;
        } blending;
        struct CullFace {
            bool enabled = false;
            unsigned int mode = 0;
        } culling;
        struct PolygonMode {
            bool enabled = false;
            unsigned int face = 0;
            unsigned int mode = 0;
        } polygon_mode;
        struct ScissorTest {
            bool enabled = false;

            struct Box {
                int x = 0;
                int y = 0;
                int width = 0;
                int height = 0;

                bool operator==(const Box &other) const {
                    return x == other.x && y == other.y && width == other.width && height == other.height;
                }
            } box;
        } scissor_test;

        struct Viewport {
            int x = 0;
            int y = 0;
            int width = 0;
            int height = 0;

            bool operator==(const Viewport &other) const {
                return x == other.x && y == other.y && width == other.width && height == other.height;
            }
        } viewport;

        struct ClearColor {
            float r = 0;
            float g = 0;
            float b = 0;
            float a = 0;

            bool operator==(const ClearColor &other) const {
                return r == other.r && g == other.g && b == other.b && a == other.a;
            }
        } clear_color;

        struct ClearDepth {
            float depth = 1.0f;
        } clear_depth;

        struct ClearStencil {
            int stencil = 0;
        } clear_stencil;

        struct PointSize {
            bool enabled = false;
            float size = 1.0f;
        } point_size;

        struct LineWidth {
            bool enabled = false;
            float width = 1.0f;
        } line_width;

        struct PolygonOffset {
            bool enabled = false;
            float factor = 0.0f;
            float units = 0.0f;
        } polygon_offset;

        struct StencilTest {
            bool enabled = false;
            unsigned int func = 0;
            int ref = 0;
            unsigned int mask = 0;
            unsigned int sfail = 0;
            unsigned int dpfail = 0;
            unsigned int dppass = 0;
        } stencil_test;

        struct ColorMask {
            bool enabled = true;
            bool r = true;
            bool g = true;
            bool b = true;
            bool a = true;

            bool operator==(const ColorMask &other) const {
                return r == other.r && g == other.g && b == other.b && a == other.a;
            }
        } color_mask;

        void change_to_new_state(const State &new_state);
    };
}

namespace Bcg {
    template<>
    struct ComponentGui<OpenGL::Shader> {
        static void Show(OpenGL::Shader &shader);
    };

    template<>
    struct ComponentGui<OpenGL::ShaderProgram> {
        static void Show(OpenGL::ShaderProgram &program);
    };

    template<>
    struct ComponentGui<OpenGL::ShaderPrograms> {
        static void Show(OpenGL::ShaderPrograms &programs);
    };

    template<>
    struct ComponentGui<OpenGL::BufferObject> {
        static void Show(OpenGL::BufferObject &buffer);
    };

    template<>
    struct ComponentGui<OpenGL::VertexBufferObject> {
        static void Show(OpenGL::VertexBufferObject &buffer);
    };

    template<>
    struct ComponentGui<OpenGL::IndexBufferObject> {
        static void Show(OpenGL::IndexBufferObject &buffer);
    };

    template<>
    struct ComponentGui<OpenGL::DoubleBuffer> {
        static void Show(OpenGL::DoubleBuffer &buffer);
    };

    template<>
    struct ComponentGui<OpenGL::VertexAttribute> {
        static void Show(OpenGL::VertexAttribute &attribute);
    };

    template<>
    struct ComponentGui<OpenGL::VertexAttributeLayout> {
        static void Show(OpenGL::VertexAttributeLayout &layout);
    };

    template<>
    struct ComponentGui<OpenGL::VertexArrayObject> {
        static void Show(OpenGL::VertexArrayObject &vao);
    };

    template<>
    struct ComponentGui<OpenGL::Renderable> {
        static void Show(OpenGL::Renderable &renderable);
    };

    template<>
    struct ComponentGui<OpenGL::RenderableTriangles> {
        static void Show(OpenGL::RenderableTriangles &renderable);
    };

    template<>
    struct ComponentGui<OpenGL::RenderableLines> {
        static void Show(OpenGL::RenderableLines &renderable);
    };

    template<>
    struct ComponentGui<OpenGL::RenderablePoints> {
        static void Show(OpenGL::RenderablePoints &renderable);
    };

    struct ShaderStage {
        OpenGL::Shader shader;
    };

    struct Material {

    };

}

#endif //ENGINE23_OPENGLUTILS_H

//
// Created by alex on 05.12.23.
//

#include "OpenGLUtils.h"
#include "glad/gl.h"
#include <iostream>
#include <exception>
#include <fstream>
#include <filesystem>
#include <regex>

namespace Bcg::OpenGL {
    void AssertNoOglError() {
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error: " << TypeToString(err) << std::endl;
        }
    }

    std::string TypeToString(unsigned int type) {
        switch (type) {
            case (GL_FLOAT):
                return "float";
            case (GL_FLOAT_VEC2):
                return "vec2";
            case (GL_FLOAT_VEC3):
                return "vec3";
            case (GL_FLOAT_VEC4):
                return "vec4";
            case (GL_DOUBLE):
                return "double";
            case (GL_DOUBLE_VEC2):
                return "dvec2";
            case (GL_DOUBLE_VEC3):
                return "dvec3";
            case (GL_DOUBLE_VEC4):
                return "dvec4";
            case (GL_INT):
                return "int";
            case (GL_INT_VEC2):
                return "ivec2";
            case (GL_INT_VEC3):
                return "ivec3";
            case (GL_INT_VEC4):
                return "ivec4";
            case (GL_UNSIGNED_INT):
                return "unsigned int";
            case (GL_UNSIGNED_INT_VEC2):
                return "uvec2";
            case (GL_UNSIGNED_INT_VEC3):
                return "uvec3";
            case (GL_UNSIGNED_INT_VEC4):
                return "uvec4";
            case (GL_BOOL):
                return "bool";
            case (GL_BOOL_VEC2):
                return "bvec2";
            case (GL_BOOL_VEC3):
                return "bvec3";
            case (GL_BOOL_VEC4):
                return "bvec4";
            case (GL_FLOAT_MAT2):
                return "mat2";
            case (GL_FLOAT_MAT3):
                return "mat3";
            case (GL_FLOAT_MAT4):
                return "mat4";
            case (GL_FLOAT_MAT2x3):
                return "mat2x3";
            case (GL_FLOAT_MAT2x4):
                return "mat2x4";
            case (GL_FLOAT_MAT3x2):
                return "mat3x2";
            case (GL_FLOAT_MAT3x4):
                return "mat3x4";
            case (GL_FLOAT_MAT4x2):
                return "mat4x2";
            case (GL_FLOAT_MAT4x3):
                return "mat4x3";
            case (GL_DOUBLE_MAT2):
                return "dmat2";
            case (GL_DOUBLE_MAT3):
                return "dmat3";
            case (GL_DOUBLE_MAT4):
                return "dmat4";
            case (GL_DOUBLE_MAT2x3):
                return "dmat2x3";
            case (GL_DOUBLE_MAT2x4):
                return "dmat2x4";
            case (GL_DOUBLE_MAT3x2):
                return "dmat3x2";
            case (GL_DOUBLE_MAT3x4):
                return "dmat3x4";
            case (GL_DOUBLE_MAT4x2):
                return "dmat4x2";
            case (GL_DOUBLE_MAT4x3):
                return "dmat4x3";
            case (GL_SAMPLER_1D):
                return "sampler1D";
            case (GL_SAMPLER_2D):
                return "sampler2D";
            case (GL_SAMPLER_3D):
                return "sampler3D";
            case (GL_SAMPLER_CUBE):
                return "samplerCube";
            case (GL_SAMPLER_1D_SHADOW):
                return "sampler1DShadow";
            case (GL_SAMPLER_2D_SHADOW):
                return "sampler2DShadow";
            case (GL_SAMPLER_1D_ARRAY):
                return "sampler1DArray";
            case (GL_SAMPLER_2D_ARRAY):
                return "sampler2DArray";
            case (GL_SAMPLER_1D_ARRAY_SHADOW):
                return "sampler1DArrayShadow";
            case (GL_SAMPLER_2D_ARRAY_SHADOW):
                return "sampler2DArrayShadow";
            case (GL_SAMPLER_2D_MULTISAMPLE):
                return "sampler2DMS";
            case (GL_SAMPLER_2D_MULTISAMPLE_ARRAY):
                return "sampler2DMSArray";
            case (GL_SAMPLER_CUBE_SHADOW):
                return "samplerCubeShadow";
            case (GL_SAMPLER_BUFFER):
                return "samplerBuffer";
            case (GL_SAMPLER_2D_RECT):
                return "sampler2DRect";
            case (GL_SAMPLER_2D_RECT_SHADOW):
                return "sampler2DRectShadow";
            case (GL_INT_SAMPLER_1D):
                return "isampler1D";
            case (GL_INT_SAMPLER_2D):
                return "isampler2D";
            case (GL_INT_SAMPLER_3D):
                return "isampler3D";
            case (GL_INT_SAMPLER_CUBE):
                return "isamplerCube";
            case (GL_INT_SAMPLER_1D_ARRAY):
                return "isampler1DArray";
            case (GL_INT_SAMPLER_2D_ARRAY):
                return "isampler2DArray";
            case (GL_INT_SAMPLER_2D_MULTISAMPLE):
                return "isampler2DMS";
            case (GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY):
                return "isampler2DMSArray";
            case (GL_INT_SAMPLER_BUFFER):
                return "isamplerBuffer";
            case (GL_INT_SAMPLER_2D_RECT):
                return "isampler2DRect";
            case (GL_UNSIGNED_INT_SAMPLER_1D):
                return "usampler1D";
            case (GL_UNSIGNED_INT_SAMPLER_2D):
                return "usampler2D";
            case (GL_UNSIGNED_INT_SAMPLER_3D):
                return "usampler3D";
            case (GL_UNSIGNED_INT_SAMPLER_CUBE):
                return "usamplerCube";
            case (GL_UNSIGNED_INT_SAMPLER_1D_ARRAY):
                return "usampler2DArray";
            case (GL_UNSIGNED_INT_SAMPLER_2D_ARRAY):
                return "usampler2DArray";
            case (GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE):
                return "usampler2DMS";
            case (GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY):
                return "usampler2DMSArray";
            case (GL_UNSIGNED_INT_SAMPLER_BUFFER):
                return "usamplerBuffer";
            case (GL_UNSIGNED_INT_SAMPLER_2D_RECT):
                return "usampler2DRect";
            case (GL_IMAGE_1D):
                return "image1D";
            case (GL_IMAGE_2D):
                return "image2D";
            case (GL_IMAGE_3D):
                return "image3D";
            case (GL_IMAGE_2D_RECT):
                return "image2DRect";
            case (GL_IMAGE_CUBE):
                return "imageCube";
            case (GL_IMAGE_BUFFER):
                return "imageBuffer";
            case (GL_IMAGE_1D_ARRAY):
                return "image1DArray";
            case (GL_IMAGE_2D_ARRAY):
                return "image2DArray";
            case (GL_IMAGE_2D_MULTISAMPLE):
                return "image2DMS";
            case (GL_IMAGE_2D_MULTISAMPLE_ARRAY):
                return "image2DMSArray";
            case (GL_INT_IMAGE_1D):
                return "iimage1D";
            case (GL_INT_IMAGE_2D):
                return "iimage2D";
            case (GL_INT_IMAGE_3D):
                return "iimage3D";
            case (GL_INT_IMAGE_2D_RECT):
                return "iimage2DRect";
            case (GL_INT_IMAGE_CUBE):
                return "iimageCube";
            case (GL_INT_IMAGE_BUFFER):
                return "iimageBuffer";
            case (GL_INT_IMAGE_1D_ARRAY):
                return "iimage1DArray";
            case (GL_INT_IMAGE_2D_ARRAY):
                return "iimage2DArray";
            case (GL_INT_IMAGE_2D_MULTISAMPLE):
                return "iimage2DMS";
            case (GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY):
                return "iimage2DMSArray";
            case (GL_UNSIGNED_INT_IMAGE_1D):
                return "uimage1D";
            case (GL_UNSIGNED_INT_IMAGE_2D):
                return "uimage2D";
            case (GL_UNSIGNED_INT_IMAGE_3D):
                return "uimage3D";
            case (GL_UNSIGNED_INT_IMAGE_2D_RECT):
                return "uimage2DRect";
            case (GL_UNSIGNED_INT_IMAGE_CUBE):
                return "uimageCube";
            case (GL_UNSIGNED_INT_IMAGE_BUFFER):
                return "uimageBuffer";
            case (GL_UNSIGNED_INT_IMAGE_1D_ARRAY):
                return "uimage1DArray";
            case (GL_UNSIGNED_INT_IMAGE_2D_ARRAY):
                return "uimage2DArray";
            case (GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE):
                return "uimage2DMS";
            case (GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY):
                return "uimage2DMSArray";
            case (GL_UNSIGNED_INT_ATOMIC_COUNTER):
                return "atomic_uint";
            case (GL_ARRAY_BUFFER):
                return "array_buffer";
            case (GL_ELEMENT_ARRAY_BUFFER):
                return "element_array_buffer";
            case (GL_VERTEX_ARRAY):
                return "vertex_array";
            case (GL_INVALID_OPERATION):
                return "invalid_operation";
            case (GL_INVALID_ENUM):
                return "invalid_enum";
            case (GL_INVALID_VALUE):
                return "invalid_value";
            case (GL_OUT_OF_MEMORY):
                return "out_of_memory";
            case (GL_INVALID_FRAMEBUFFER_OPERATION):
                return "invalid_framebuffer_operation";
            case (GL_VERTEX_SHADER):
                return "vertex_shader";
            case (GL_FRAGMENT_SHADER):
                return "fragment_shader";
            case (GL_GEOMETRY_SHADER):
                return "geometry_shader";
            case (GL_TESS_CONTROL_SHADER):
                return "tess_control_shader";
            case (GL_TESS_EVALUATION_SHADER):
                return "tess_evaluation_shader";
            case (GL_COMPUTE_SHADER):
                return "compute_shader";
            default:
                return "unknown";
        }
    }

    unsigned int TypeToSize(unsigned int type) {
        switch (type) {
            case (GL_FLOAT):
                return sizeof(float);
            case (GL_FLOAT_VEC2):
                return sizeof(float) * 2;
            case (GL_FLOAT_VEC3):
                return sizeof(float) * 3;
            case (GL_FLOAT_VEC4):
                return sizeof(float) * 4;
            case (GL_DOUBLE):
                return sizeof(double);
            case (GL_DOUBLE_VEC2):
                return sizeof(double) * 2;
            case (GL_DOUBLE_VEC3):
                return sizeof(double) * 3;
            case (GL_DOUBLE_VEC4):
                return sizeof(double) * 4;
            case (GL_INT):
                return sizeof(int);
            case (GL_INT_VEC2):
                return sizeof(int) * 2;
            case (GL_INT_VEC3):
                return sizeof(int) * 3;
            case (GL_INT_VEC4):
                return sizeof(int) * 4;
            case (GL_UNSIGNED_INT):
                return sizeof(unsigned int);
            case (GL_UNSIGNED_INT_VEC2):
                return sizeof(unsigned int) * 2;
            case (GL_UNSIGNED_INT_VEC3):
                return sizeof(unsigned int) * 3;
            case (GL_UNSIGNED_INT_VEC4):
                return sizeof(unsigned int) * 4;
            case (GL_BOOL):
                return sizeof(bool);
            case (GL_BOOL_VEC2):
                return sizeof(bool) * 2;
            case (GL_BOOL_VEC3):
                return sizeof(bool) * 3;
            case (GL_BOOL_VEC4):
                return sizeof(bool) * 4;
            case (GL_FLOAT_MAT2):
                return sizeof(float) * 4;
            case (GL_FLOAT_MAT3):
                return sizeof(float) * 9;
            case (GL_FLOAT_MAT4):
                return sizeof(float) * 16;
            case (GL_FLOAT_MAT2x3):
                return sizeof(float) * 6;
            case (GL_FLOAT_MAT2x4):
                return sizeof(float) * 8;
            case (GL_FLOAT_MAT3x2):
                return sizeof(float) * 6;
            case (GL_FLOAT_MAT3x4):
                return sizeof(float) * 12;
            case (GL_FLOAT_MAT4x2):
                return sizeof(float) * 8;
            case (GL_FLOAT_MAT4x3):
                return sizeof(float) * 12;
            case (GL_DOUBLE_MAT2):
                return sizeof(double) * 4;
            case (GL_DOUBLE_MAT3):
                return sizeof(double) * 9;
            case (GL_DOUBLE_MAT4):
                return sizeof(double) * 16;
            case (GL_DOUBLE_MAT2x3):
                return sizeof(double) * 6;
            case (GL_DOUBLE_MAT2x4):
                return sizeof(double) * 8;
            case (GL_DOUBLE_MAT3x2):
                return sizeof(double) * 6;
            case (GL_DOUBLE_MAT3x4):
                return sizeof(double) * 12;
            case (GL_DOUBLE_MAT4x2):
                return sizeof(double) * 8;
            case (GL_DOUBLE_MAT4x3):
                return sizeof(double) * 12;
            case (GL_SAMPLER_1D):
                return sizeof(int);
            case (GL_SAMPLER_2D):
                return sizeof(int);
            case (GL_SAMPLER_3D):
                return sizeof(int);
            case (GL_SAMPLER_CUBE):
                return sizeof(int);
            case (GL_SAMPLER_1D_SHADOW):
                return sizeof(int);
            case (GL_SAMPLER_2D_SHADOW):
                return sizeof(int);
            case (GL_SAMPLER_1D_ARRAY):
                return sizeof(int);
            case (GL_SAMPLER_2D_ARRAY):
                return sizeof(int);
            case (GL_SAMPLER_1D_ARRAY_SHADOW):
                return sizeof(int);
            case (GL_SAMPLER_2D_ARRAY_SHADOW):
                return sizeof(int);
            case (GL_SAMPLER_2D_MULTISAMPLE):
                return sizeof(int);
            case (GL_SAMPLER_2D_MULTISAMPLE_ARRAY):
                return sizeof(int);
            case (GL_SAMPLER_CUBE_SHADOW):
                return sizeof(int);
            case (GL_SAMPLER_BUFFER):
                return sizeof(int);
            case (GL_SAMPLER_2D_RECT):
                return sizeof(int);
            case (GL_SAMPLER_2D_RECT_SHADOW):
                return sizeof(int);
            case (GL_INT_SAMPLER_1D):
                return sizeof(int);
            case (GL_INT_SAMPLER_2D):
                return sizeof(int);
            case (GL_INT_SAMPLER_3D):
                return sizeof(int);
            case (GL_INT_SAMPLER_CUBE):
                return sizeof(int);
            case (GL_INT_SAMPLER_1D_ARRAY):
                return sizeof(int);
            case (GL_INT_SAMPLER_2D_ARRAY):
                return sizeof(int);
            case (GL_INT_SAMPLER_2D_MULTISAMPLE):
                return sizeof(int);
            case (GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY):
                return sizeof(int);
            case (GL_INT_SAMPLER_BUFFER):
                return sizeof(int);
            case (GL_INT_SAMPLER_2D_RECT):
                return sizeof(int);
            case (GL_UNSIGNED_INT_SAMPLER_1D):
                return sizeof(unsigned int);
            case (GL_UNSIGNED_INT_SAMPLER_2D):
                return sizeof(unsigned int);
            case (GL_UNSIGNED_INT_SAMPLER_3D):
                return sizeof(unsigned int);
            case (GL_UNSIGNED_INT_SAMPLER_CUBE):
                return sizeof(unsigned int);
            case (GL_UNSIGNED_INT_SAMPLER_1D_ARRAY):
                return sizeof(unsigned int);
            case (GL_UNSIGNED_INT_SAMPLER_2D_ARRAY):
                return sizeof(unsigned int);
            case (GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE):
                return sizeof(unsigned int);
            case (GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY):
                return sizeof(unsigned int);
            case (GL_UNSIGNED_INT_SAMPLER_BUFFER):
                return sizeof(unsigned int);
            case (GL_UNSIGNED_INT_SAMPLER_2D_RECT):
                return sizeof(unsigned int);
            case (GL_IMAGE_1D):
                return sizeof(int);
            case (GL_IMAGE_2D):
                return sizeof(int);
            case (GL_IMAGE_3D):
                return sizeof(int);
            case (GL_IMAGE_2D_RECT):
                return sizeof(int);
            case (GL_IMAGE_CUBE):
                return sizeof(int);
            case (GL_IMAGE_BUFFER):
                return sizeof(int);
            case (GL_IMAGE_1D_ARRAY):
                return sizeof(int);
            case (GL_IMAGE_2D_ARRAY):
                return sizeof(int);
            case (GL_IMAGE_2D_MULTISAMPLE):
                return sizeof(int);
            case (GL_IMAGE_2D_MULTISAMPLE_ARRAY):
                return sizeof(int);
            case (GL_INT_IMAGE_1D):
                return sizeof(int);
            case (GL_INT_IMAGE_2D):
                return sizeof(int);
            case (GL_INT_IMAGE_3D):
                return sizeof(int);
            case (GL_INT_IMAGE_2D_RECT):
                return sizeof(int);
            case (GL_INT_IMAGE_CUBE):
                return sizeof(int);
            case (GL_INT_IMAGE_BUFFER):
                return sizeof(int);
            case (GL_INT_IMAGE_1D_ARRAY):
                return sizeof(int);
            case (GL_INT_IMAGE_2D_ARRAY):
                return sizeof(int);
            case (GL_INT_IMAGE_2D_MULTISAMPLE):
                return sizeof(int);
            case (GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY):
                return sizeof(int);
            case (GL_UNSIGNED_INT_IMAGE_1D):
                return sizeof(unsigned int);
            case (GL_UNSIGNED_INT_IMAGE_2D):
                return sizeof(unsigned int);
            case (GL_UNSIGNED_INT_IMAGE_3D):
                return sizeof(unsigned int);
            case (GL_UNSIGNED_INT_IMAGE_2D_RECT):
                return sizeof(unsigned int);
            case (GL_UNSIGNED_INT_IMAGE_CUBE):
                return sizeof(unsigned int);
            case (GL_UNSIGNED_INT_IMAGE_BUFFER):
                return sizeof(unsigned int);
            case (GL_UNSIGNED_INT_IMAGE_1D_ARRAY):
                return sizeof(unsigned int);
            case (GL_UNSIGNED_INT_IMAGE_2D_ARRAY):
                return sizeof(unsigned int);
            case (GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE):
                return sizeof(unsigned int);
            case (GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY):
                return sizeof(unsigned int);
            case (GL_UNSIGNED_INT_ATOMIC_COUNTER):
                return sizeof(unsigned int);
            case (GL_ARRAY_BUFFER):
                return sizeof(unsigned int);
            case (GL_ELEMENT_ARRAY_BUFFER):
                return sizeof(unsigned int);
            case (GL_VERTEX_ARRAY):
                return sizeof(unsigned int);
            case (GL_INVALID_OPERATION):
                return sizeof(unsigned int);
            case (GL_INVALID_ENUM):
                return sizeof(unsigned int);
            case (GL_INVALID_VALUE):
                return sizeof(unsigned int);
            case (GL_OUT_OF_MEMORY):
                return sizeof(unsigned int);
            case (GL_INVALID_FRAMEBUFFER_OPERATION):
                return sizeof(unsigned int);
            case (GL_VERTEX_SHADER):
                return sizeof(unsigned int);
            case (GL_FRAGMENT_SHADER):
                return sizeof(unsigned int);
            case (GL_GEOMETRY_SHADER):
                return sizeof(unsigned int);
            case (GL_TESS_CONTROL_SHADER):
                return sizeof(unsigned int);
            case (GL_TESS_EVALUATION_SHADER):
                return sizeof(unsigned int);
            case (GL_COMPUTE_SHADER):
                return sizeof(unsigned int);
            default:
                std::terminate();
        }
    }

    unsigned int TypeToElementCount(unsigned int type) {
        switch (type) {
            case (GL_FLOAT):
                return 1;
            case (GL_FLOAT_VEC2):
                return 2;
            case (GL_FLOAT_VEC3):
                return 3;
            case (GL_FLOAT_VEC4):
                return 4;
            case (GL_DOUBLE):
                return 1;
            case (GL_DOUBLE_VEC2):
                return 2;
            case (GL_DOUBLE_VEC3):
                return 3;
            case (GL_DOUBLE_VEC4):
                return 4;
            case (GL_INT):
                return 1;
            case (GL_INT_VEC2):
                return 2;
            case (GL_INT_VEC3):
                return 3;
            case (GL_INT_VEC4):
                return 4;
            case (GL_UNSIGNED_INT):
                return 1;
            case (GL_UNSIGNED_INT_VEC2):
                return 2;
            case (GL_UNSIGNED_INT_VEC3):
                return 3;
            case (GL_UNSIGNED_INT_VEC4):
                return 4;
            case (GL_BOOL):
                return 1;
            case (GL_BOOL_VEC2):
                return 2;
            case (GL_BOOL_VEC3):
                return 3;
            case (GL_BOOL_VEC4):
                return 4;
            case (GL_FLOAT_MAT2):
                return 4;
            case (GL_FLOAT_MAT3):
                return 9;
            case (GL_FLOAT_MAT4):
                return 16;
            case (GL_FLOAT_MAT2x3):
                return 6;
            case (GL_FLOAT_MAT2x4):
                return 8;
            case (GL_FLOAT_MAT3x2):
                return 6;
            case (GL_FLOAT_MAT3x4):
                return 12;
            case (GL_FLOAT_MAT4x2):
                return 8;
            case (GL_FLOAT_MAT4x3):
                return 12;
            case (GL_DOUBLE_MAT2):
                return 4;
            case (GL_DOUBLE_MAT3):
                return 9;
            case (GL_DOUBLE_MAT4):
                return 16;
            case (GL_DOUBLE_MAT2x3):
                return 6;
            case (GL_DOUBLE_MAT2x4):
                return 8;
            case (GL_DOUBLE_MAT3x2):
                return 6;
            case (GL_DOUBLE_MAT3x4):
                return 12;
            case (GL_DOUBLE_MAT4x2):
                return 8;
            case (GL_DOUBLE_MAT4x3):
                return 12;
            default:
                std::terminate();
        }
    }

    inline std::string load_text_file(std::string filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return "";
        }
        return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
    }

    inline void process_includes(std::string &source, const std::string &directory) {
        std::regex include_regex(R"(#include\s*"\s*([^"]+)\s*")");
        std::smatch match;
        while (std::regex_search(source, match, include_regex)) {
            std::string include_file = match.str(1);
            std::string include_source = load_text_file(directory + "/" + include_file);
            source.replace(match.position(), match.length(), include_source);
        }
    }

    void Shader::load_sources() {
        if (filepath.empty()) return;

        source = load_text_file(filepath);
        process_includes(source, std::filesystem::path(filepath).parent_path().string());
    }

    void Shader::compile() {
        id = glCreateShader(type);
        OpenGL::AssertNoOglError();
        const char *src = source.c_str();
        glShaderSource(id, 1, &src, nullptr);
        OpenGL::AssertNoOglError();
        glCompileShader(id);
        OpenGL::AssertNoOglError();
    }

    bool Shader::check_compile_status() {
        int result;
        glGetShaderiv(id, GL_COMPILE_STATUS, &result);
        OpenGL::AssertNoOglError();
        if (result == GL_FALSE) {
            int length;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
            OpenGL::AssertNoOglError();
            char *message = (char *) alloca(length * sizeof(char));
            glGetShaderInfoLog(id, length, &length, message);
            OpenGL::AssertNoOglError();
            glDeleteShader(id);
            OpenGL::AssertNoOglError();
            id = 0;
            error_message = message;
        }
        return result;
    }

    void ShaderProgram::load_shaders() {
        if (!v_shader.filepath.empty()) {
            v_shader.load_sources();
        }
        if (!f_shader.filepath.empty()) {
            f_shader.load_sources();
        }
        if (!g_shader.filepath.empty()) {
            g_shader.load_sources();
        }
        if (!tc_shader.filepath.empty()) {
            tc_shader.load_sources();
        }
        if (!te_shader.filepath.empty()) {
            te_shader.load_sources();
        }
        if (!c_shader.filepath.empty()) {
            c_shader.load_sources();
        }
        compile_from_sources();
    }

    void ShaderProgram::compile_from_sources() {
        id = glCreateProgram();
        OpenGL::AssertNoOglError();
        if (!v_shader.source.empty()) {
            v_shader.type = GL_VERTEX_SHADER;
            v_shader.compile();
            v_shader.check_compile_status();
            glAttachShader(id, v_shader.id);
            OpenGL::AssertNoOglError();
        }
        if (!f_shader.source.empty()) {
            f_shader.type = GL_FRAGMENT_SHADER;
            f_shader.compile();
            f_shader.check_compile_status();
            glAttachShader(id, f_shader.id);
            OpenGL::AssertNoOglError();
        }
        if (!g_shader.source.empty()) {
            g_shader.type = GL_GEOMETRY_SHADER;
            g_shader.compile();
            g_shader.check_compile_status();
            glAttachShader(id, g_shader.id);
            OpenGL::AssertNoOglError();
        }
        if (!tc_shader.source.empty()) {
            tc_shader.type = GL_TESS_CONTROL_SHADER;
            tc_shader.compile();
            tc_shader.check_compile_status();
            glAttachShader(id, tc_shader.id);
            OpenGL::AssertNoOglError();
        }
        if (!te_shader.source.empty()) {
            te_shader.type = GL_TESS_EVALUATION_SHADER;
            te_shader.compile();
            te_shader.check_compile_status();
            glAttachShader(id, te_shader.id);
            OpenGL::AssertNoOglError();
        }
        if (!c_shader.source.empty()) {
            c_shader.type = GL_COMPUTE_SHADER;
            c_shader.load_sources();
            c_shader.compile();
            c_shader.check_compile_status();
            glAttachShader(id, c_shader.id);
            OpenGL::AssertNoOglError();
        }
    }

    void ShaderProgram::link() {
        glLinkProgram(id);
        OpenGL::AssertNoOglError();

        if (v_shader.id) {
            glDetachShader(id, v_shader.id);
            OpenGL::AssertNoOglError();
        }
        if (f_shader.id) {
            glDetachShader(id, f_shader.id);
            OpenGL::AssertNoOglError();
        }
        if (g_shader.id) {
            glDetachShader(id, g_shader.id);
            OpenGL::AssertNoOglError();
        }
        if (tc_shader.id) {
            glDetachShader(id, tc_shader.id);
            OpenGL::AssertNoOglError();
        }
        if (te_shader.id) {
            glDetachShader(id, te_shader.id);
            OpenGL::AssertNoOglError();
        }
        if (c_shader.id) {
            glDetachShader(id, c_shader.id);
            OpenGL::AssertNoOglError();
        }
    }

    bool ShaderProgram::check_link_status() {
        int result;
        glGetProgramiv(id, GL_LINK_STATUS, &result);
        OpenGL::AssertNoOglError();
        if (result == GL_FALSE) {
            int length;
            glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
            OpenGL::AssertNoOglError();
            char *message = (char *) alloca(length * sizeof(char));
            glGetProgramInfoLog(id, length, &length, message);
            OpenGL::AssertNoOglError();
            glDeleteProgram(id);
            OpenGL::AssertNoOglError();
            id = 0;
            error_message = message;
        }
        return result;
    }


    void ShaderProgram::use() {
        glUseProgram(id);
        OpenGL::AssertNoOglError();
    }

    BufferObject BufferObject::Static() {
        return {0, 0, 0, GL_STATIC_DRAW, ""};
    }

    BufferObject BufferObject::Dynamic() {
        return {0, 0, 0, GL_DYNAMIC_DRAW, ""};
    }

    VertexBufferObject VertexBufferObject::Static() {
        return {0, GL_ARRAY_BUFFER, 0, GL_STATIC_DRAW, ""};
    }

    VertexBufferObject VertexBufferObject::Dynamic() {
        return {0, GL_ARRAY_BUFFER, 0, GL_DYNAMIC_DRAW, ""};
    }

    IndexBufferObject IndexBufferObject::Static() {
        return {0, GL_ELEMENT_ARRAY_BUFFER, 0, GL_STATIC_DRAW, ""};
    }

    IndexBufferObject IndexBufferObject::Dynamic() {
        return {0, GL_ELEMENT_ARRAY_BUFFER, 0, GL_DYNAMIC_DRAW, ""};
    }

    void BufferObject::create() {
        glGenBuffers(1, &id);
        OpenGL::AssertNoOglError();
    }

    void BufferObject::bind() {
        glBindBuffer(type, id);
        OpenGL::AssertNoOglError();
    }

    void BufferObject::set_data(const void *data, unsigned int size) {
        glBufferData(type, size, data, usage);
        OpenGL::AssertNoOglError();
    }

    void BufferObject::release() {
        glBindBuffer(type, 0);
        OpenGL::AssertNoOglError();
    }

    void VertexAttribute::enable() {
        glEnableVertexAttribArray(index);
        OpenGL::AssertNoOglError();
    }

    void VertexAttribute::disable() {
        glDisableVertexAttribArray(index);
        OpenGL::AssertNoOglError();
    }

    void VertexArrayObject::create() {
        glGenVertexArrays(1, &id);
        OpenGL::AssertNoOglError();
    }

    void VertexArrayObject::bind() {
        glBindVertexArray(id);
        OpenGL::AssertNoOglError();
    }

    void VertexArrayObject::release() {
        glBindVertexArray(0);
        OpenGL::AssertNoOglError();
    }

    void VertexArrayObject::set_float_attribute(unsigned int index, unsigned int size, bool normalized,
                                                const void *pointer) {
        auto &attribute = layout.attributes.emplace_back(index, size, GL_FLOAT, normalized, size * sizeof(float),
                                                         pointer);
        attribute.enable();
        glVertexAttribPointer(attribute.index, attribute.size, attribute.type, attribute.normalized, attribute.stride,
                              attribute.pointer);
        OpenGL::AssertNoOglError();
    }

    void VertexArrayObject::set_double_attribute(unsigned int index, unsigned int size, bool normalized,
                                                 const void *pointer) {
        auto &attribute = layout.attributes.emplace_back(index, size, GL_DOUBLE, normalized, size * sizeof(double),
                                                         pointer);
        attribute.enable();
        glVertexAttribPointer(attribute.index, attribute.size, attribute.type, attribute.normalized, attribute.stride,
                              attribute.pointer);
        OpenGL::AssertNoOglError();
    }

    void VertexArrayObject::set_int_attribute(unsigned int index, unsigned int size, bool normalized,
                                              const void *pointer) {
        auto &attribute = layout.attributes.emplace_back(index, size, GL_INT, normalized, size * sizeof(int),
                                                         pointer);
        attribute.enable();
        glVertexAttribPointer(attribute.index, attribute.size, attribute.type, attribute.normalized, attribute.stride,
                              attribute.pointer);
        OpenGL::AssertNoOglError();
    }

    void VertexArrayObject::set_unsigned_int_attribute(unsigned int index, unsigned int size, bool normalized,
                                                       const void *pointer) {
        auto &attribute = layout.attributes.emplace_back(index, size, GL_UNSIGNED_INT, normalized,
                                                         size * sizeof(unsigned int),
                                                         pointer);
        attribute.enable();
        glVertexAttribPointer(attribute.index, attribute.size, attribute.type, attribute.normalized, attribute.stride,
                              attribute.pointer);
        OpenGL::AssertNoOglError();
    }

    Renderable Renderable::Triangles() {
        Renderable renderable;
        renderable.mode = GL_TRIANGLES;
        renderable.type = GL_UNSIGNED_INT;
        return renderable;
    }

    Renderable Renderable::Lines() {
        Renderable renderable;
        renderable.mode = GL_LINES;
        renderable.type = GL_UNSIGNED_INT;
        return renderable;
    }

    Renderable Renderable::Points() {
        Renderable renderable;
        renderable.mode = GL_POINTS;
        renderable.type = GL_UNSIGNED_INT;
        return renderable;
    }

    void Renderable::draw(){
        program.use();
        vao.bind();
        glDrawElements(mode, count, type, (void *) offset);
        OpenGL::AssertNoOglError();
        vao.release();
    }
}
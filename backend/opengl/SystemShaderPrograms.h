//
// Created by alex on 05.12.23.
//

#ifndef ENGINE23_SYSTEMSHADERPROGRAMS_H
#define ENGINE23_SYSTEMSHADERPROGRAMS_H

#include "EngineFwd.h"
#include "OpenGLUtils.h"

namespace Bcg {
    class SystemRendererOpenGL;

    class SystemShaderPrograms {
    public:
        SystemShaderPrograms() = default;

        ~SystemShaderPrograms() = default;

        static std::string name();

        static OpenGL::Shader load_shader(std::string filepath, unsigned int type);

        static OpenGL::ShaderProgram load_program(std::string vs_filepath, std::string fs_filepath, std::string gs_filepath = "",
                                   std::string tcs_filepath = "", std::string tes_filepath = "",
                                   std::string cs_filepath = "");

        static bool check_link_status(unsigned int program_id);

        static bool add_to_watcher(OpenGL::ShaderProgram &program);

        static std::string glsl_base_path();

    protected:
        friend Engine;
        friend SystemRendererOpenGL;

        static void pre_init();

        static void init();

        static void remove();
    };
}

#endif //ENGINE23_SYSTEMSHADERPROGRAMS_H

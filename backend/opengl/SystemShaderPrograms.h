//
// Created by alex on 05.12.23.
//

#ifndef ENGINE23_SYSTEMSHADERPROGRAMS_H
#define ENGINE23_SYSTEMSHADERPROGRAMS_H

#include "System.h"
#include "OpenGLUtils.h"

namespace Bcg {
    class SystemRendererOpenGL;

    class SystemShaderPrograms : public System {
    public:
        SystemShaderPrograms();

        ~SystemShaderPrograms() override = default;

        OpenGL::Shader load_shader(std::string filepath, unsigned int type);

        OpenGL::Shader compile_shader(std::string source, unsigned int type);

        bool check_compile_status(unsigned int shader_id);

        OpenGL::ShaderProgram load_program(std::string vs_filepath, std::string fs_filepath, std::string gs_filepath = "",
                                   std::string tcs_filepath = "", std::string tes_filepath = "",
                                   std::string cs_filepath = "");

        bool check_link_status(unsigned int program_id);

        bool add_to_watcher(OpenGL::ShaderProgram &program);

        std::string glsl_base_path() const;

    protected:
        friend Engine;
        friend SystemRendererOpenGL;

        void pre_init() override;

        void init() override;

        void remove() override;
    };
}

#endif //ENGINE23_SYSTEMSHADERPROGRAMS_H

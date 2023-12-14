//
// Created by alex on 05.12.23.
//

#include "SystemShaderPrograms.h"
#include "Engine.h"
#include "glad/gl.h"
#include "Commands.h"
#include "Events.h"
#include "OpenGLUtils.h"
#include <regex>
#include <fstream>
#include <filesystem>

namespace Bcg {

    namespace SystemShaderProgramsInternal {
        void on_begin_frame(const Events::Begin<Frame> &event) {
            auto &watcher = Engine::Context().get<FileWatcher>();
            watcher.check();
        }

        void on_startup(const Events::Startup<Engine> &event) {
            Log::Info(SystemShaderPrograms::name() + ": Startup").enqueue();;
            auto point_cloud_program = SystemShaderPrograms::load_program(
                    std::filesystem::path(SystemShaderPrograms::glsl_base_path()) / "programs/point_cloud/point_cloud_vs.glsl",
                    std::filesystem::path(SystemShaderPrograms::glsl_base_path()) / "programs/point_cloud/point_cloud_fs.glsl");
            auto name = "point_cloud";
            point_cloud_program.name = name;

            auto &programs = Engine::Context().get<OpenGL::ShaderPrograms>();
            programs[name] = std::move(point_cloud_program);
            SystemShaderPrograms::add_to_watcher(programs[name]);

            Engine::Instance()->dispatcher.sink<Events::Begin<Frame>>().connect<&SystemShaderProgramsInternal::on_begin_frame>();
        }


        void on_shutdown(const Events::Shutdown<Engine> &event) {
            Engine::Instance()->dispatcher.sink<Events::Begin<Frame>>().disconnect<&SystemShaderProgramsInternal::on_begin_frame>();
            Log::Info(SystemShaderPrograms::name() + ": Shutdown").enqueue();
        }
    }

    OpenGL::Shader SystemShaderPrograms::load_shader(std::string filepath, unsigned int type) {
        OpenGL::Shader shader;
        shader.type = type;
        shader.filepath = filepath;
        shader.load_sources();
        if (shader.source.empty()) {
            Log::Error(name() + ": Failed to open file: " + filepath).enqueue();
            return {};
        }
        shader.compile();
        shader.check_compile_status();
        if (shader.id == 0) {
            Log::Error(name() + ": Failed to compile shader: " + shader.error_message).enqueue();
            return {};
        }
        return shader;
    }


    std::function<void()> WatcherCallback(OpenGL::ShaderProgram &program, OpenGL::Shader &shader) {
        return [&]() {
            auto system = SystemShaderPrograms();
            OpenGL::Shader new_shader;
            new_shader.filepath = shader.filepath;
            new_shader.type = shader.type;
            new_shader.load_sources();
            new_shader.compile();
            if (new_shader.check_compile_status()) {
                //check if new shader can be linked to program
                //if not then revert to old shader

                glAttachShader(program.id, new_shader.id);
                OpenGL::AssertNoOglError();
                glLinkProgram(program.id);
                OpenGL::AssertNoOglError();
                if (system.check_link_status(program.id) == GL_FALSE) {
                    Log::Warn("Watcher: Failed to link shader: " + shader.filepath).enqueue();
                    glDetachShader(program.id, new_shader.id);
                    OpenGL::AssertNoOglError();
                    glAttachShader(program.id, shader.id);
                    OpenGL::AssertNoOglError();
                    glLinkProgram(program.id);
                    OpenGL::AssertNoOglError();
                    if (system.check_link_status(program.id)) {
                        Log::Warn("Watcher: Reverted to old shader: " + shader.filepath).enqueue();
                    } else {
                        Log::Error("Watcher: Failed to link old shader: " + shader.filepath).enqueue();
                    }
                    glDetachShader(program.id, shader.id);
                } else {
                    glDeleteShader(shader.id);
                    OpenGL::AssertNoOglError();
                    glDetachShader(program.id, shader.id);
                    glDetachShader(program.id, new_shader.id);
                    OpenGL::AssertNoOglError();
                    shader = std::move(new_shader);
                    Log::Info("Watcher: Reloaded shader: " + shader.filepath).enqueue();
                }
            }
        };
    }

    OpenGL::ShaderProgram
    SystemShaderPrograms::load_program(std::string vs_filepath, std::string fs_filepath, std::string gs_filepath,
                                       std::string tcs_filepath, std::string tes_filepath,
                                       std::string cs_filepath) {
        if (vs_filepath.empty()) {
            Log::Error(name() + ": Vertex shader filepath is empty!").enqueue();
            return {};
        }
        if (fs_filepath.empty()) {
            Log::Error(name() + ": Fragment shader filepath is empty!").enqueue();
            return {};
        }
        OpenGL::ShaderProgram program;
        program.v_shader.filepath = vs_filepath;
        program.f_shader.filepath = fs_filepath;
        program.g_shader.filepath = gs_filepath;
        program.tc_shader.filepath = tcs_filepath;
        program.te_shader.filepath = tes_filepath;
        program.c_shader.filepath = cs_filepath;
        program.load_shaders();
        if ((!program.v_shader.id || !program.f_shader.id) && !program.c_shader.id) {
            Log::Error(name() + ": Needs at least vertex and fragment or compute shader!").enqueue();
            return program;
        }
        program.link();
        if (!program.check_link_status()) {
            Log::Error(name() + ": Failed to link program: " + program.error_message).enqueue();
            return program;
        }
        /*
        program.id = glCreateProgram();
        OpenGL::AssertNoOglError();
        program.v_shader = load_shader(vs_filepath, GL_VERTEX_SHADER);
        program.f_shader = load_shader(fs_filepath, GL_FRAGMENT_SHADER);
        program.g_shader = gs_filepath.empty() ? OpenGL::Shader{} : load_shader(gs_filepath, GL_GEOMETRY_SHADER);
        program.tc_shader = tcs_filepath.empty() ? OpenGL::Shader{} : load_shader(tcs_filepath, GL_TESS_CONTROL_SHADER);
        program.te_shader = tes_filepath.empty() ? OpenGL::Shader{} : load_shader(tes_filepath, GL_TESS_EVALUATION_SHADER);
        program.c_shader = cs_filepath.empty() ? OpenGL::Shader{} : load_shader(cs_filepath, GL_COMPUTE_SHADER);

        if (program.v_shader.id) {
            glAttachShader(program.id, program.v_shader.id);
            OpenGL::AssertNoOglError();
        }

        if (program.f_shader.id) {
            glAttachShader(program.id, program.f_shader.id);
            OpenGL::AssertNoOglError();
        }

        if (program.g_shader.id) {
            glAttachShader(program.id, program.g_shader.id);
            OpenGL::AssertNoOglError();
        }

        if (program.tc_shader.id) {
            glAttachShader(program.id, program.tc_shader.id);
            OpenGL::AssertNoOglError();
        }

        if (program.te_shader.id) {
            glAttachShader(program.id, program.te_shader.id);
            OpenGL::AssertNoOglError();
        }

        if (program.c_shader.id) {
            glAttachShader(program.id, program.c_shader.id);
            OpenGL::AssertNoOglError();
        }

        if ((!program.v_shader.id || !program.f_shader.id) && !program.c_shader.id) {
            Log::Error(m_name + ": Needs at least vertex and fragment or compute shader!").enqueue();
            return program;
        }

        glLinkProgram(program.id);
        OpenGL::AssertNoOglError();

        if (check_link_status(program.id) == GL_FALSE) {
            return program;
        }

        if (program.v_shader.id) {
            glDetachShader(program.id, program.v_shader.id);
            OpenGL::AssertNoOglError();
        }

        if (program.f_shader.id) {
            glDetachShader(program.id, program.f_shader.id);
            OpenGL::AssertNoOglError();
        }

        if (program.g_shader.id) {
            glDetachShader(program.id, program.g_shader.id);
            OpenGL::AssertNoOglError();
        }

        if (program.tc_shader.id) {
            glDetachShader(program.id, program.tc_shader.id);
            OpenGL::AssertNoOglError();
        }

        if (program.te_shader.id) {
            glDetachShader(program.id, program.te_shader.id);
            OpenGL::AssertNoOglError();
        }

        if (program.c_shader.id) {
            glDetachShader(program.id, program.c_shader.id);
            OpenGL::AssertNoOglError();
        }
*/
        return program;
    }

    bool SystemShaderPrograms::check_link_status(unsigned int program_id) {
        int result;
        glGetProgramiv(program_id, GL_LINK_STATUS, &result);
        OpenGL::AssertNoOglError();
        if (result == GL_FALSE) {
            int length;
            glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &length);
            OpenGL::AssertNoOglError();
            char *message = (char *) alloca(length * sizeof(char));
            glGetProgramInfoLog(program_id, length, &length, message);
            OpenGL::AssertNoOglError();
            Log::Error(name() + ": Failed to link program: \n" + std::string(message)).enqueue();
            glDeleteProgram(program_id);
            OpenGL::AssertNoOglError();
        }
        return result;
    }

    bool SystemShaderPrograms::add_to_watcher(OpenGL::ShaderProgram &program) {
        auto &watcher = Engine::Context().get<FileWatcher>();

        Log::Info(name() + ": Adding program to watcher: " + program.name).enqueue();

        if (program.v_shader.id) {
            watcher.add(program.v_shader.filepath, WatcherCallback(program, program.v_shader));
            Log::Info(name() + ": Added shader to watcher: " + program.v_shader.filepath).enqueue();
        }

        if (program.f_shader.id) {
            watcher.add(program.f_shader.filepath, WatcherCallback(program, program.f_shader));
            Log::Info(name() + ": Added shader to watcher: " + program.f_shader.filepath).enqueue();
        }

        if (program.g_shader.id) {
            watcher.add(program.g_shader.filepath, WatcherCallback(program, program.g_shader));
            Log::Info(name() + ": Added shader to watcher: " + program.g_shader.filepath).enqueue();
        }

        if (program.tc_shader.id) {
            watcher.add(program.tc_shader.filepath, WatcherCallback(program, program.tc_shader));
            Log::Info(name() + ": Added shader to watcher: " + program.tc_shader.filepath).enqueue();
        }

        if (program.te_shader.id) {
            watcher.add(program.te_shader.filepath, WatcherCallback(program, program.te_shader));
            Log::Info(name() + ": Added shader to watcher: " + program.te_shader.filepath).enqueue();
        }

        if (program.c_shader.id) {
            watcher.add(program.c_shader.filepath, WatcherCallback(program, program.c_shader));
            Log::Info(name() + ": Added shader to watcher: " + program.c_shader.filepath).enqueue();
        }

        return true;
    }

    std::string SystemShaderPrograms::glsl_base_path() {
        return "../backend/glsl/";
    }

    std::string SystemShaderPrograms::name()  {
        return "SystemShaderPrograms";
    }

    void SystemShaderPrograms::pre_init() {
        Engine::Context().emplace<OpenGL::ShaderPrograms>();
        Engine::Context().emplace<FileWatcher>();
    }

    void SystemShaderPrograms::init() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().connect<&SystemShaderProgramsInternal::on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().connect<&SystemShaderProgramsInternal::on_shutdown>();
        Log::Info(name() + ": Initialized").enqueue();
    }

    void SystemShaderPrograms::remove() {
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().disconnect<&SystemShaderProgramsInternal::on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().disconnect<&SystemShaderProgramsInternal::on_shutdown>();
        Engine::Context().erase<OpenGL::ShaderPrograms>();
        Log::Info(name() + ": Removed").enqueue();
    }
}
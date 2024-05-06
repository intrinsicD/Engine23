//
// Created by alex on 05.12.23.
//

#include "SystemShaderPrograms.h"
#include "Engine.h"
#include "glad/gl.h"
#include "Commands.h"
#include "Events/Events.h"
#include "OpenGLUtils.h"
#include <filesystem>
#include "File/FileWatcher.h"
#include "imgui.h"

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemShaderProgramsInternal {
        static bool show_gui = false;

        void on_update_gui_menu(const Events::Update<GuiMenu> &event);

        void on_update_gui(const Events::Update<Gui> &event);

        void on_begin_frame(const Events::Begin<Frame> &event);

        void on_startup(const Events::Startup<Engine> &event);

        void on_shutdown(const Events::Shutdown<Engine> &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemShaderProgramsInternal {
        void on_update_gui_menu(const Events::Update<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {
                if (ImGui::MenuItem("ShaderPrograms", nullptr, &show_gui)) {
                    Engine::Dispatcher().sink<Events::Update<Gui>>().connect<&on_update_gui>();
                }
                ImGui::EndMenu();
            }
        }

        void on_update_gui(const Events::Update<Gui> &event) {
            if (!show_gui) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&on_update_gui>();
                return;
            }

            if (ImGui::Begin("ShaderPrograms", &show_gui)) {
                auto &programs = Engine::Context().get<OpenGL::ShaderPrograms>();
                for (auto &program : programs) {
                    if (ImGui::TreeNode(program.first.c_str())) {
                        ComponentGui<OpenGL::ShaderProgram>::Show(program.second);
                        ImGui::TreePop();
                    }
                }
            }
            ImGui::End();
        }

        void on_begin_frame(const Events::Begin<Frame> &event) {
            auto &watcher = Engine::Context().get<FileWatcher>();
            watcher.check();
        }

        void on_startup(const Events::Startup<Engine> &event) {
            Log::Info(SystemShaderPrograms::name() , "Startup").enqueue();;
            auto point_cloud_program = SystemShaderPrograms::load_program(
                    std::filesystem::path(SystemShaderPrograms::glsl_base_path()) /
                    "programs/point_cloud/point_cloud_vs.glsl",
                    std::filesystem::path(SystemShaderPrograms::glsl_base_path()) /
                    "programs/point_cloud/point_cloud_fs.glsl");
            auto name = "point_cloud";
            point_cloud_program.name = name;

            auto &programs = Engine::Context().get<OpenGL::ShaderPrograms>();
            programs[name] = std::move(point_cloud_program);
            SystemShaderPrograms::add_to_watcher(programs[name]);

            Engine::Dispatcher().sink<Events::Begin<Frame>>().connect<&SystemShaderProgramsInternal::on_begin_frame>();
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().connect<&SystemShaderProgramsInternal::on_update_gui_menu>();
        }

        void on_shutdown(const Events::Shutdown<Engine> &event) {
            Engine::Dispatcher().sink<Events::Begin<Frame>>().disconnect<&SystemShaderProgramsInternal::on_begin_frame>();
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().connect<&SystemShaderProgramsInternal::on_update_gui_menu>();
            Log::Info(SystemShaderPrograms::name() , "Shutdown").enqueue();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------


namespace Bcg {
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

    std::string SystemShaderPrograms::name() {
        return "SystemShaderPrograms";
    }

    void SystemShaderPrograms::pre_init() {
        Engine::Context().emplace<OpenGL::ShaderPrograms>();
        Engine::Context().emplace<FileWatcher>();
    }

    void SystemShaderPrograms::init() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().connect<&SystemShaderProgramsInternal::on_startup>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().connect<&SystemShaderProgramsInternal::on_shutdown>();
        Log::Info("Initialized", name()).enqueue();
    }

    void SystemShaderPrograms::remove() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().disconnect<&SystemShaderProgramsInternal::on_startup>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().disconnect<&SystemShaderProgramsInternal::on_shutdown>();
        Engine::Context().erase<OpenGL::ShaderPrograms>();
        Log::Info("Removed", name()).enqueue();
    }
}
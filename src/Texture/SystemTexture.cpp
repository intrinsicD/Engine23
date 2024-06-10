//
// Created by alex on 08.05.24.
//

#include "SystemTexture.h"
#include "Engine.h"
#include "Components.h"
#include "Texture.h"
#include "TextureGui.h"
#include "Commands.h"
#include "Events.h"
#include "imgui.h"
#include "Input.h"
#include "ImGuiUtils.h"
#include "SOIL/SOIL.h"
#include "glad/gl.h"
#include "FilePath.h"


//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemTextureInternal {
        static bool show_gui_instance = false;
        static bool show_gui_components = false;
        static std::map<std::string, unsigned int> textures;
        static std::string selected_texture_path;
        static GLuint selected_texture_id = 0;

        void on_update_input_drop(const Events::Update<Input::Drop> &event);

        void on_update_gui_menu(const Events::Update<GuiMenu> &event);

        void on_update_gui_instance(const Events::Update<Gui> &event);

        void on_update_gui_components(const Events::Update<Gui> &event);

        void on_startup(const Events::Startup<Engine> &event);

        void on_shutdown(const Events::Shutdown<Engine> &event);

        void load_texture(const std::string &file_path);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemTextureInternal {

        void on_update_input_drop(const Events::Update<Input::Drop> &event) {
            auto &input = Engine::Context().get<Input>();

            for (const auto &file_path: input.drop.paths) {
                auto ext = FilePath::Extension(file_path);
                //Check if file can be loaded by SOIL
                if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".tga") {
                    load_texture(file_path);
                }
            }
        }

        void on_update_gui_menu(const Events::Update<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {
                if (ImGui::BeginMenu(SystemTexture::component_name().c_str())) {
                    if (ImGui::MenuItem("Instance", nullptr, &show_gui_instance)) {
                        Engine::Dispatcher().sink<Events::Update<Gui>>().connect<&on_update_gui_instance>();
                    }
                    if (ImGui::MenuItem("Components", nullptr, &show_gui_components)) {
                        Engine::Dispatcher().sink<Events::Update<Gui>>().connect<&on_update_gui_components>();
                    }
                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }
        }

        void on_update_gui_instance(const Events::Update<Gui> &event) {
            if (!show_gui_instance) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&SystemTextureInternal::on_update_gui_instance>();
                return;
            }

            if (ImGui::Begin(SystemTexture::component_name().c_str(), &show_gui_instance, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("Loaded Textures:");
                for (const auto &texture_pair : textures) {
                    if (ImGui::ImageButton(reinterpret_cast<void*>(static_cast<intptr_t>(texture_pair.second)), ImVec2(64, 64))) {
                        selected_texture_path = texture_pair.first;
                        selected_texture_id = texture_pair.second;
                    }
                    ImGui::SameLine();
                    ImGui::Text("%s", texture_pair.first.c_str());
                }
            }
            ImGui::End();

            if (selected_texture_id != 0) {
                if (ImGui::Begin((selected_texture_path + "###TextureWindow").c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                    ImVec2 window_size = ImGui::GetContentRegionAvail();
                    ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(selected_texture_id)), window_size);
                }
                ImGui::End();
            }
        }

        void on_update_gui_components(const Events::Update<Gui> &event) {
            if (!show_gui_components) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&on_update_gui_components>();
                return;
            }

            if (ImGui::Begin("TransformComponents", &show_gui_components)) {
                Components<Texture> components(SystemTexture::component_name());
                ImGuiUtils::Show(components);
            }
            ImGui::End();
        }

        void on_startup(const Events::Startup<Engine> &event) {
            Engine::Dispatcher().sink<Events::Update<Input::Drop>>().connect<&SystemTextureInternal::on_update_input_drop>();
            Log::Info("Startup", SystemTexture::name()).enqueue();
        }

        void on_shutdown(const Events::Shutdown<Engine> &event) {
            for (const auto &texture_pair : textures) {
                glDeleteTextures(1, &texture_pair.second);
            }
            textures.clear();

            Engine::Dispatcher().sink<Events::Update<Input::Drop>>().disconnect<&SystemTextureInternal::on_update_input_drop>();
            Log::Info("Shutdown", SystemTexture::name()).enqueue();
        }

        void load_texture(const std::string &file_path) {
/*
            unsigned int texture_id = SOIL_load_OGL_texture(
                    file_path.c_str(),
                    SOIL_LOAD_AUTO,
                    SOIL_CREATE_NEW_ID,
                    SOIL_FLAG_INVERT_Y | SOIL_FLAG_MIPMAPS
            );
*/


            int width, height, channels;
            unsigned char* image_data = SOIL_load_image(file_path.c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);

            if (!image_data) {
                Log::Error("SOIL loading error: " +  file_path).enqueue();
                return;
            }

            GLuint texture_id;
            glGenTextures(1, &texture_id);
            glBindTexture(GL_TEXTURE_2D, texture_id);
            glTexImage2D(GL_TEXTURE_2D, 0, channels == 4 ? GL_RGBA : GL_RGB, width, height, 0, channels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, image_data);
            glGenerateMipmap(GL_TEXTURE_2D);

            SOIL_free_image_data(image_data);


            if (texture_id == 0) {
                Log::Error("SOIL loading error: " +  file_path).enqueue();
            } else {
                Components<Texture> components(SystemTexture::component_name());
                auto tex_instance = components.create_instance();
                auto &texture = components.get_instance(tex_instance);
                texture.width = width;
                texture.height = height;
                texture.depth = channels;
                texture.id = texture_id;

                textures[file_path] = texture_id;
                Log::Info("Loaded", "Image using SOIL" + file_path).enqueue();
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------


namespace Bcg {
    std::string SystemTexture::name() {
        return "System" + component_name();
    }

    std::string SystemTexture::component_name() {
        return "Texture";
    }

    void SystemTexture::pre_init() {

    }

    void SystemTexture::init() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().connect<&SystemTextureInternal::on_startup>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().connect<&SystemTextureInternal::on_shutdown>();
        Engine::Dispatcher().sink<Events::Update<GuiMenu>>().connect<&SystemTextureInternal::on_update_gui_menu>();
        Log::Info("Initialized", name());
    }

    void SystemTexture::remove() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().disconnect<&SystemTextureInternal::on_startup>();
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().disconnect<&SystemTextureInternal::on_shutdown>();
        Engine::Dispatcher().sink<Events::Update<GuiMenu>>().disconnect<&SystemTextureInternal::on_update_gui_menu>();
        Log::Info("Removed", name());
    }
}
//
// Created by alex on 05.12.23.
//

#include "SystemBuffers.h"
#include "Engine.h"
#include "Events/Events.h"
#include "Commands.h"
#include "ComponentGui.h"
#include "Components.h"
#include "imgui.h"
#include "OpenGLUtils.h"
#include "Picker.h"
#include "ImGuiUtils.h"
#include <ostream>

//----------------------------------------------------------------------------------------------------------------------
// Predefines for better overview
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    struct Buffer {
        const char *name;
        unsigned int id;
        unsigned int type;
        unsigned int size;
        unsigned int usage;

        friend std::ostream &operator<<(std::ostream &stream, const Buffer &buffer) {
            stream << "Buffer Name: " << buffer.name << ", ID: " << buffer.id << ", Type: "
                   << OpenGL::TypeToString(buffer.type) << ", Size: "
                   << buffer.size << ", Usage: " << OpenGL::TypeToString(buffer.usage);
            return stream;
        }
    };

    struct Buffers : public std::vector<unsigned int> {
        using std::vector<unsigned int>::vector;

        friend std::ostream &operator<<(std::ostream &stream, const Buffers &buffer) {
            return stream;
        }
    };

    template<>
    struct ComponentGui<Buffer> {
        static void Show(const Buffer &buffer) {
            ImGui::Text("Name: %s ID: %u Type: %s Size: %u Usage: %s",
                        buffer.name,
                        buffer.id,
                        OpenGL::TypeToString(buffer.type).c_str(),
                        buffer.size,
                        OpenGL::TypeToString(buffer.usage).c_str());
        }

        static void Show(entt::entity entity_id) {
            auto &buffers = Engine::State().get<Buffers>(entity_id);
            Show(buffers);
        }

        static void Show(Buffers &buffers) {
            Components<Buffer> components;
            for (const auto &buffer_id: buffers) {
                Show(components.get_instance(buffer_id));
            }
        }
    };

    namespace SystemBuffersInternal {
        static bool show_gui_instance = false;
        static bool show_gui_components = false;


        void on_update_gui_menu(const Events::Update<GuiMenu> &event);

        void on_update_gui_instance(const Events::Update<Gui> &event);

        void on_update_gui_components(const Events::Update<Gui> &event);

        void on_startup_engine(const Events::Startup<Engine> &event);

        void on_shutdown_engine(const Events::Shutdown<Engine> &event);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation hidden internal functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    namespace SystemBuffersInternal {
        void on_update_gui_menu(const Events::Update<GuiMenu> &event) {
            if (ImGui::BeginMenu("Menu")) {
                if (ImGui::BeginMenu(SystemBuffers::component_name().c_str())) {
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
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&on_update_gui_instance>();
                return;
            }

            if (ImGui::Begin(SystemBuffers::component_name().c_str(), &show_gui_instance,
                             ImGuiChildFlags_AlwaysAutoResize)) {
                ComponentGui<Buffer>::Show(Engine::Context().get<Picker>().id.entity);
            }
            ImGui::End();
        }

        void on_update_gui_components(const Events::Update<Gui> &event) {
            if (!show_gui_components) {
                Engine::Dispatcher().sink<Events::Update<Gui>>().disconnect<&on_update_gui_components>();
                return;
            }

            if (ImGui::Begin("BufferComponents", &show_gui_components)) {
                Components<Buffer> buffers;
                ImGuiUtils::Show(buffers);
            }
            ImGui::End();
        }

        void on_startup_engine(const Events::Startup<Engine> &event) {
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().connect<&SystemBuffersInternal::on_update_gui_menu>();
            Log::Info(SystemBuffers::name(), "Startup").enqueue();
        }

        void on_shutdown_engine(const Events::Shutdown<Engine> &event) {
            Engine::Dispatcher().sink<Events::Update<GuiMenu>>().disconnect<&SystemBuffersInternal::on_update_gui_menu>();
            Log::Info(SystemBuffers::name(), "Shutdown").enqueue();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Implementation of public functions
//----------------------------------------------------------------------------------------------------------------------

namespace Bcg {
    std::string SystemBuffers::name() {
        return "System" + component_name();
    }

    std::string SystemBuffers::component_name() {
        return "Buffer";
    }

    void SystemBuffers::pre_init() {

    }

    void SystemBuffers::init() {
        Engine::Dispatcher().sink<Events::Startup<Engine>>().connect<&SystemBuffersInternal::on_startup_engine>();
        Log::Info("Initialized", name()).enqueue();
    }

    void SystemBuffers::remove() {
        Engine::Dispatcher().sink<Events::Shutdown<Engine>>().connect<&SystemBuffersInternal::on_shutdown_engine>();
        Log::Info("Removed", name()).enqueue();
    }
}
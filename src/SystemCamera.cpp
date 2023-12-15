//
// Created by alex on 15.12.23.
//

#include "SystemCamera.h"
#include "Engine.h"
#include "Components.h"
#include "Commands.h"
#include "Events.h"
#include "GLFW/glfw3.h"

namespace Bcg{
    namespace SystemCameraInternal{
        void on_update_input(const Events::Update<Input> &event){
            auto &keyboard = Engine::Context().get<Input>().keyboard;
            auto &camera = Engine::Context().get<Camera>();
            auto &time = Engine::Context().get<Time>();


            if(keyboard.keys[GLFW_KEY_W]){
                //move forward
                camera.position += camera.forward * camera.speed * float(time.mainloop.duration);
            }
            if(keyboard.keys[GLFW_KEY_S]){
                //move backward
                camera.position -= camera.forward * camera.speed * float(time.mainloop.duration);
            }

            auto right = glm::normalize(glm::cross(camera.up, camera.forward));
            if(keyboard.keys[GLFW_KEY_D]){
                //move right
                camera.position += right * camera.speed * float(time.mainloop.duration);
            }
            if(keyboard.keys[GLFW_KEY_A]){
                //move left
                camera.position -= right * camera.speed * float(time.mainloop.duration);
            }
            camera.forward = glm::normalize(glm::cross(right, camera.up));
        }

        void on_startup(const Events::Startup<Engine> &event){
            Engine::Instance()->dispatcher.sink<Events::Update<Input>>().connect<&on_update_input>();
            Log::Info(SystemCamera::name() + ": Startup").enqueue();
        }

        void on_shutdown(const Events::Shutdown<Engine> &event){
            Engine::Instance()->dispatcher.sink<Events::Update<Input>>().disconnect<&on_update_input>();
            Log::Info(SystemCamera::name() + ": Shutdown").enqueue();
        }
    }
    std::string SystemCamera::name() {
        return "camera";
    }

    void SystemCamera::pre_init() {
        //register necessary components
        auto &camera = Engine::Context().emplace<Camera>();
        camera.position = glm::vec3(0.0f, 0.0f, 3.0f);
        camera.target = glm::vec3(0.0f, 0.0f, 0.0f);
        camera.forward = glm::vec3(0.0f, 0.0f, -1.0f);
        camera.up = glm::vec3(0.0f, 1.0f, 0.0f);
        camera.speed = 0.5f;
        camera.fov = 45.0f;
        camera.aspect_ratio = 1.0f;
        camera.near = 0.1f;
        camera.far = 100.0f;
    }

    void SystemCamera::init() {
        //register event handlers
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().connect<&SystemCameraInternal::on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().connect<&SystemCameraInternal::on_shutdown>();
    }

    void SystemCamera::remove() {
        //unregister event handlers
        Engine::Instance()->dispatcher.sink<Events::Startup<Engine>>().disconnect<&SystemCameraInternal::on_startup>();
        Engine::Instance()->dispatcher.sink<Events::Shutdown<Engine>>().disconnect<&SystemCameraInternal::on_shutdown>();
    }
}
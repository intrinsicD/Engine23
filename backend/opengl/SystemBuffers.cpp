//
// Created by alex on 05.12.23.
//

#include "SystemBuffers.h"
#include "Engine.h"
#include "Commands.h"
#include "glad/gl.h"

namespace Bcg {
    SystemBuffers::SystemBuffers() : System("SystemBuffers") {

    }

    OpenGL::Buffer SystemBuffers::create_buffer(std::string name, unsigned int type) {
        OpenGL::Buffer buffer;
        glGenBuffers(1, &buffer.id);
        OpenGL::AssertNoOglError();
        buffer.name = name;
        buffer.type = type;
        return buffer;
    }

    void SystemBuffers::delete_buffer(OpenGL::Buffer &buffer) {
        glDeleteBuffers(1, &buffer.id);
        OpenGL::AssertNoOglError();
    }

    void SystemBuffers::set_buffer_data(OpenGL::Buffer &buffer, void *data, unsigned int size, unsigned int usage) {
        glBindBuffer(buffer.type, buffer.id);
        OpenGL::AssertNoOglError();
        glBufferData(buffer.type, size, data, usage);
        OpenGL::AssertNoOglError();
        buffer.size = size;
        buffer.usage = usage;
    }

    void
    SystemBuffers::set_buffer_sub_data(OpenGL::Buffer &buffer, void *data, unsigned int size, unsigned int offset) {
        glBindBuffer(buffer.type, buffer.id);
        OpenGL::AssertNoOglError();
        //Check if the buffer is big enough otherwise orphan it and reallocate
        if (offset + size > buffer.size) {
            glBufferData(buffer.type, offset + size, nullptr, buffer.usage);
            buffer.size = offset + size;
            OpenGL::AssertNoOglError();
        }
        glBufferSubData(buffer.type, offset, size, data);
        OpenGL::AssertNoOglError();
        buffer.format = OpenGL::Buffer::Format::Plain;
    }

    void SystemBuffers::pre_init() {

    }

    void SystemBuffers::init() {
        Log::Info(m_name + ": Initialized").enqueue();
    }

    void SystemBuffers::remove() {
        Log::Info(m_name + ": Removed").enqueue();
    }
}
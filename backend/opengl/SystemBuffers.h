//
// Created by alex on 05.12.23.
//

#ifndef ENGINE23_SYSTEMBUFFERS_H
#define ENGINE23_SYSTEMBUFFERS_H

#include "System.h"
#include "OpenGLUtils.h"

namespace Bcg {
    class SystemRendererOpenGL;

    class SystemBuffers : public System {
    public:
        SystemBuffers();

        ~SystemBuffers() override = default;

        OpenGL::Buffer create_buffer(std::string name, unsigned int type);

        void delete_buffer(OpenGL::Buffer &buffer);

        void set_buffer_data(OpenGL::Buffer &buffer, void *data, unsigned int size, unsigned int usage);

        void set_buffer_sub_data(OpenGL::Buffer &buffer, void *data, unsigned int size, unsigned int offset);
    protected:
        friend Engine;
        friend SystemRendererOpenGL;

        void pre_init() override;

        void init() override;

        void remove() override;
    };
}

#endif //ENGINE23_SYSTEMBUFFERS_H

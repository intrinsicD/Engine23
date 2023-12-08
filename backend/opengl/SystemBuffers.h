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

    protected:
        friend Engine;
        friend SystemRendererOpenGL;

        void pre_init() override;

        void init() override;

        void remove() override;
    };
}

#endif //ENGINE23_SYSTEMBUFFERS_H

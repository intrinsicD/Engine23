//
// Created by alex on 05.12.23.
//

#ifndef ENGINE23_SYSTEMBUFFERS_H
#define ENGINE23_SYSTEMBUFFERS_H

#include "EngineFwd.h"
#include "OpenGLUtils.h"

namespace Bcg {
    class SystemRendererOpenGL;

    class SystemBuffers {
    public:
        SystemBuffers() = default;

        ~SystemBuffers() = default;

        static std::string name();

    protected:
        friend Engine;
        friend SystemRendererOpenGL;

        static void pre_init() ;

        static void init();

        static void remove();
    };
}

#endif //ENGINE23_SYSTEMBUFFERS_H

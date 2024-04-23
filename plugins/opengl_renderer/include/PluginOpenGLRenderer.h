//
// Created by alex on 23.04.24.
//

#ifndef ENGINE23_PLUGINOPENGLRENDERER_H
#define ENGINE23_PLUGINOPENGLRENDERER_H


#include "Plugins.h"

namespace Bcg{
    class PluginOpenGLRenderer : public Plugin{
    public:
        PluginOpenGLRenderer();

        ~PluginOpenGLRenderer() override = default;

    protected:
        void pre_init() override;

        void init() override;

        void remove() override;
    };

    extern "C" Plugin* create_instance_opengl_renderer() {
        return new PluginOpenGLRenderer;
    }
}


#endif //ENGINE23_PLUGINOPENGLRENDERER_H

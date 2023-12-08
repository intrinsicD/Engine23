//
// Created by alex on 07.12.23.
//

#ifndef ENGINE23_PLUGINLEARNOPENGL_H
#define ENGINE23_PLUGINLEARNOPENGL_H

#include "Plugins.h"

namespace Bcg{
    class PluginLearnOpenGL : public Plugin{
    public:
        PluginLearnOpenGL();

        ~PluginLearnOpenGL() override = default;

    protected:
        void pre_init() override;

        void init() override;

        void remove() override;
    };

    extern "C" Plugin* create_instance_learn_opengl() {
        return new PluginLearnOpenGL;
    }
}

#endif //ENGINE23_PLUGINLEARNOPENGL_H

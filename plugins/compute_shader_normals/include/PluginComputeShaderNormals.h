//
// Created by alex on 21.12.23.
//

#ifndef ENGINE23_PLUGINCOMPUTESHADERNORMALS_H
#define ENGINE23_PLUGINCOMPUTESHADERNORMALS_H

#include "Plugins.h"

namespace Bcg{
    class PluginComputeShaderNormals : public Plugin{
    public:
        PluginComputeShaderNormals();

        ~PluginComputeShaderNormals() override = default;

    protected:
        void pre_init() override;

        void init() override;

        void remove() override;
    };

    extern "C" Plugin* create_instance_compute_shader_normals() {
        return new PluginComputeShaderNormals;
    }
}
#endif //ENGINE23_PLUGINCOMPUTESHADERNORMALS_H

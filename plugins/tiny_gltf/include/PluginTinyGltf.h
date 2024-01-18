//
// Created by alex on 18.01.24.
//

#ifndef ENGINE23_PLUGINTINYGLTF_H
#define ENGINE23_PLUGINTINYGLTF_H


#include "Plugins.h"

namespace Bcg{
    class PluginTinyGltf : public Plugin{
    public:
        PluginTinyGltf();

        ~PluginTinyGltf() override = default;

    protected:
        void pre_init() override;

        void init() override;

        void remove() override;
    };

    extern "C" Plugin* create_instance_tiny_gltf() {
        return new PluginTinyGltf;
    }
}
#endif //ENGINE23_PLUGINTINYGLTF_H

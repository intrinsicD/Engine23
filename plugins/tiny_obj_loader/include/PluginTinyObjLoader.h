//
// Created by alex on 11.12.23.
//

#ifndef ENGINE23_PLUGINTINYOBJLOADER_H
#define ENGINE23_PLUGINTINYOBJLOADER_H


#include "Plugins.h"

namespace Bcg{
    class PluginTinyObjLoader : public Plugin{
    public:
        PluginTinyObjLoader();

        ~PluginTinyObjLoader() override = default;

    protected:
        void pre_init() override;

        void init() override;

        void remove() override;
    };

    extern "C" Plugin* create_instance_tiny_obj_loader() {
        return new PluginTinyObjLoader;
    }
}
#endif //ENGINE23_PLUGINTINYOBJLOADER_H

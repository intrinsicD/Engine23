//
// Created by alex on 15.12.23.
//

#ifndef ENGINE23_PLUGINGEOMETRYPROCESSING_H
#define ENGINE23_PLUGINGEOMETRYPROCESSING_H

#include "Plugins.h"

namespace Bcg{
    class PluginGeometryProcessing : public Plugin{
    public:
        PluginGeometryProcessing();

        ~PluginGeometryProcessing() override = default;

    protected:
        void pre_init() override;

        void init() override;

        void remove() override;
    };

    extern "C" Plugin* create_instance_geometry_processing() {
        return new PluginGeometryProcessing;
    }
}


#endif //ENGINE23_PLUGINGEOMETRYPROCESSING_H

//
// Created by alex on 28.11.23.
//

#ifndef ENGINE23_SYSTEMRENDERERVULKAN_H
#define ENGINE23_SYSTEMRENDERERVULKAN_H

#include "System.h"

namespace Bcg {
    class SystemRendererVulkan : public System {
    public:
        SystemRendererVulkan();

        ~SystemRendererVulkan() override = default;

    protected:
        friend Engine;

        void pre_init() override;

        void init() override;

        void remove() override;
    };
}
#endif //ENGINE23_SYSTEMRENDERERVULKAN_H

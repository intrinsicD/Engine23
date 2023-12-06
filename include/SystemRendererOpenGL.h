//
// Created by alex on 27.11.23.
//

#ifndef ENGINE23_SYSTEMRENDEREROPENGL_H
#define ENGINE23_SYSTEMRENDEREROPENGL_H

#include "System.h"

namespace Bcg {
    class SystemRendererOpenGL : public System {
    public:
        SystemRendererOpenGL();

        ~SystemRendererOpenGL() override = default;

        void show(entt::entity entity_id);

        void hide(entt::entity entity_id);

    protected:
        friend Engine;

        void pre_init() override;

        void init() override;

        void remove() override;
    };
}

#endif //ENGINE23_SYSTEMRENDEREROPENGL_H

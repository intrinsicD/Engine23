//
// Created by alex on 22.05.24.
//

#ifndef ENGINE23_GRAPHICSMODULE_H
#define ENGINE23_GRAPHICSMODULE_H

#include <vector>

namespace Bcg{
    struct Viewport{
        int x, y, width, height;
    };

    struct Window{
        void *id;
        std::vector<Viewport> viewports;
    };

    class GraphicsModule{
    public:


    private:
    };
}

#endif //ENGINE23_GRAPHICSMODULE_H

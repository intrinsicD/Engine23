//
// Created by alex on 28.05.24.
//

#ifndef ENGINE23_RENDERCONTEXT_H
#define ENGINE23_RENDERCONTEXT_H

#include <vector>
#include "entt/fwd.hpp"

namespace Bcg {
    struct Attribute {
        std::string name;
        bool enabled;
        unsigned int index;
        unsigned int size;
        unsigned int type;
        bool normalized;
        unsigned int stride;
        void *pointer;
    };

    struct Uniform {
        std::string name;
        unsigned int location;
        unsigned int count;
        unsigned int type;
        void *data;
    };

    struct Texture {
        std::string name;
        unsigned int id;
        unsigned int width;
        unsigned int height;
        unsigned int channels;
        unsigned int target;
        unsigned int level;
        unsigned int internal_format;
        unsigned int border;
        unsigned int format;
        unsigned int type;
        void *data;
    };

    struct Material {
        unsigned int program_id;
        std::vector<unsigned int> attribute_ids;
        std::vector<unsigned int> uniform_ids;
        std::vector<unsigned int> texture_ids;
    };

    struct SceneViewContext {
        std::vector<entt::entity> entities;         //Entities to draw
        std::vector<unsigned int> materials_ids;    //how to draw the entities
    };

    struct Viewport {
        int x, y, width, height;
    };

    using WindowMap = std::unordered_map<void *, unsigned int>;
    using WindowViewportMap = std::unordered_map<unsigned int, std::vector<unsigned int>>;
    using ViewportWindowMap = std::unordered_map<unsigned int, unsigned int>;

    struct Window {
        void *window_ptr;
        int width;
        int height;
        float hdpi;
        std::string title;
    };

    struct WindowContext {
        unsigned int window_id;         //which window to draw to
        unsigned int viewport_id;       //which viewport to draw to
        unsigned int camera_id;         //which camera to use
        unsigned int mouse_id;         //which mouse in the context
    };

    struct RenderContext {
        unsigned int window_context_id;
        unsigned int scene_view_context_id;
    };
}

#endif //ENGINE23_RENDERCONTEXT_H

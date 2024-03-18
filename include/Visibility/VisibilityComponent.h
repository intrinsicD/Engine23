//
// Created by alex on 16.03.24.
//

#ifndef ENGINE23_VISIBILITYCOMPONENT_H
#define ENGINE23_VISIBILITYCOMPONENT_H

#include <functional>
#include <cstdint>

namespace Bcg {
    struct VisibilityComponent {
        // Primary visibility flag
        bool is_visible = true;

        // Bitmasks for layer-based visibility control
        uint32_t visibility_layers = 0xFFFFFFFF; // Entity's layers
        uint32_t visibility_mask = 0xFFFFFFFF; // Camera or renderer's layer mask

        // Flags for various culling mechanisms
        bool hidden_by_user = false;
        bool hidden_by_frustum_culling = false;
        bool hidden_by_occlusion_culling = false;
        bool hidden_by_lod_culling = false;
        bool hidden_by_distance_culling = false;

        // Custom visibility check, allows for complex or dynamic visibility logic
        std::function<bool()> custom_visibility_check = nullptr;

        // Method to update the entity's visibility based on culling flags and custom check
        void update_visibility() {
            if (custom_visibility_check) {
                is_visible = custom_visibility_check();
            } else {
                is_visible = !(hidden_by_user || hidden_by_frustum_culling ||
                               hidden_by_occlusion_culling || hidden_by_lod_culling ||
                               hidden_by_distance_culling) && is_visible_on_layer(visibility_mask);
            }
        }

        // Set the entity's visibility layers and mask for layer-based rendering control
        void set_visibility_layers(uint32_t layers, uint32_t mask) {
            visibility_layers = layers;
            visibility_mask = mask;
        }

        // Determines if the entity should be visible on the current rendering layer
        bool is_visible_on_layer(uint32_t current_layer_mask) const {
            return (visibility_layers & current_layer_mask) && (visibility_mask & current_layer_mask);
        }
    };

}

#endif //ENGINE23_VISIBILITYCOMPONENT_H

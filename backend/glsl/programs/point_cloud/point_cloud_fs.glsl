#version 460 core

struct ColorMap{
    bool use;
    int num_bins;
    int num_colors;
    int num_isolines;
    int isoline_thickness;
    dvec3 isolines_color;
    sampler1D color_map;
};

struct Light{
    dvec3 position;
    dvec3 color;
    double intensity;
};

struct Colors{
    dvec3 color;
    double min;
    double max;
    double alpha;
};

struct ScalarField {
    double scalarfield;
    double min;
    double max;
    double alpha;
};

uniform ColorMap color_map;

layout(std430, binding = 0) buffer LightBuffer {
    Light lights[];// dynamic array of lights
};

in Colors colors_vs;
in ScalarField scalar_field_vs;

out vec4 frag_color;

double normalize_value(double value, double min, double max) {
    // Assumes value is in range [min, max]
    // Returns a value in range [0, 1]
    return (value - min) / (max - min);
}

dvec3 apply_color_map(double value, double v_min, double v_max, int num_bins, int num_colors, int num_isolines, int isoline_thickness, dvec3 isolines_color, sampler1D color_map) {
    // Normalize and bin color value
    double normalized_value = normalize_value(value, v_min, v_max);
    double binned_value = floor(normalized_value * double(num_bins)) * double(num_bins > 0) / double(max(num_bins, int(1))) + normalized_value * double(num_bins <= 0);
    // Calculate color index and interpolation factor
    double color_index = floor(binned_value * (num_colors - 1));
    color_index = color_index - double(color_index == num_colors - 1 && num_colors > -1);
    double interpolation_factor = binned_value * (num_colors - 1) - color_index;

    // Fetch the upper and lower color bounds
    dvec3 color_lower = texelFetch(color_map, int(color_index), 0).xyz;
    dvec3 color_upper = texelFetch(color_map, int(color_index + 1), 0).xyz;

    // Compute the final color
    dvec3 final_color = mix(color_lower, color_upper, interpolation_factor);

    // Compute isoline color
    double isoline_param = normalized_value * (num_isolines + 1);
    double isoline_index = floor(isoline_param);
    double lower_bound = isoline_index + 1 - isoline_thickness * num_isolines;
    double upper_bound = isoline_index + 1 + isoline_thickness * num_isolines;
    bool near_isoline = isoline_param > lower_bound && isoline_param < upper_bound;

    // Choose final color
    return mix(final_color, isolines_color, double(near_isoline && num_isolines > 0));
}

dvec3 normalize_color(dvec3 color, double min, double max){
    // Normalize color to range [0, 1]
    // Assumes max-min != 0
    // Returns a color in RGB form
    return (color - min) / (max - min);
}

void main() {
    dvec3 color = dvec3(0.0);
    if (color_map.use){
        color = apply_color_map(scalar_field_vs.scalarfield, scalar_field_vs.min, scalar_field_vs.max, color_map.num_bins, color_map.num_colors, color_map.num_isolines, color_map.isoline_thickness, color_map.isolines_color, color_map.color_map);
    } else {
        color = normalize_color(colors_vs.color, colors_vs.min, colors_vs.max);
    }

    dvec3 final_color = color;
    for (int i = 0; i < lights.length(); ++i){
        //TODO
    }

    frag_color = vec4(final_color.xyz, colors_vs.alpha);
}

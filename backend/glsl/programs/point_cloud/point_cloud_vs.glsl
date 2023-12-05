#version 460 core

layout (location = 0) in dvec3 position;
layout (location = 1) in dvec3 normal;
layout (location = 2) in dvec3 color;
layout (location = 3) in double scalarfield;

struct Camera {
    dmat4 view;
    dmat4 projection;
};

layout (std140, binding = 0) uniform CameraSSBO {
    Camera camera;
};

uniform dmat4 model;
uniform double alpha;
uniform double color_min;
uniform double color_max;
uniform double scalarfield_min;
uniform double scalarfield_max;

struct PositionNormal {
    dvec3 position;
    dvec3 normal;
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

out PositionNormal world_vs;
out PositionNormal view_vs;
out Colors colors_vs;
out ScalarField scalar_field_vs;

void main() {
    colors_vs.color = color;
    colors_vs.min = color_min;
    colors_vs.max = color_max;
    colors_vs.alpha = alpha;

    scalar_field_vs.scalarfield = scalarfield;
    scalar_field_vs.min = scalarfield_min;
    scalar_field_vs.max = scalarfield_max;
    scalar_field_vs.alpha = alpha;

    dvec4 pos = model * dvec4(position, 1.0);
    world_vs.position = pos.xyz;
    world_vs.normal = normalize(transpose(inverse(dmat3(model))) * normal);

    pos = camera.view * pos;
    gl_Position = vec4(camera.projection * pos);
}

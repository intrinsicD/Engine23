#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

out vec3 Normal;
out vec3 Color;

void main() {
    mat3 normalMatrix = transpose(inverse(mat3(view * model)));
    Normal = normalize(normalMatrix * vec3(aNormal.x, aNormal.y, aNormal.z));
    Color = normalize(vec3(aColor.x, aColor.y, aColor.z));
    Color = vec3(1.0);
    gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
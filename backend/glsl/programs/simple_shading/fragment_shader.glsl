#version 330 core

in vec3 Normal;
in vec3 Color;

uniform vec3 our_color;

out vec4 FragColor;

void main() {
    //   FragColor = vec4(Normal * log(1 + gl_FragCoord.z), 1.0f);
    FragColor = vec4(Color * abs(Normal.z), 1.0f);
}
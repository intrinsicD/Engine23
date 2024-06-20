//
// Created by alex on 20.06.24.
//

#include "ComputeShader.h"
#include "glad/gl.h"
#include <iostream>

namespace Bcg {
    unsigned int createComputeShaderProgram(const char *source) {
        unsigned int program = glCreateProgram();
        unsigned int shader = glCreateShader(GL_COMPUTE_SHADER);

        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cerr << "Error: Compute shader compilation failed\n" << infoLog << std::endl;
        }

        glAttachShader(program, shader);
        glLinkProgram(program);

        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(program, 512, nullptr, infoLog);
            std::cerr << "Error: Shader program linking failed\n" << infoLog << std::endl;
        }

        glDeleteShader(shader);
        return program;
    }
}
//
// Created by alex on 20.06.24.
//

#include "FaceNormals.h"
#include "ComputeShader.h"
#include "glad/gl.h"
#include <iostream>

namespace Bcg {
    const char *computeShaderSource = R"(
        #version 430 core
        layout (local_size_x = 1) in;
        layout (std430, binding = 0) readonly buffer VertexPositions { dvec3 positions[]; };
        layout (std430, binding = 1) readonly buffer Indices { uvec3 indices[]; };
        layout (std430, binding = 2) writeonly buffer FaceNormals { dvec3 normals[]; };
        void main() {
            uint index = gl_GlobalInvocationID.x;
            uvec3 vertexIndices = indices[index];
            dvec3 v0 = positions[vertexIndices.x];
            dvec3 v1 = positions[vertexIndices.y];
            dvec3 v2 = positions[vertexIndices.z];
            normals[index] = normalize(cross(v1 - v0, v2 - v0));
        }
    )";

    void ComputeFaceNormals::compute() {
        if (!positions) {
            std::cerr << "positions property does not exist!\n";
            return;
        }
        if (!triangles) {
            std::cerr << "triangles property does not exist!\n";
            return;
        }
        if (!face_normals) {
            std::cerr << "face_normals property does not exist!\n";
            return;
        }
        GLuint vertexBuffer, indexBuffer, normalBuffer;

// Vertex buffer (double precision positions)
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, positions.get_size() * positions.get_dims_bytes(), positions.get_ptr(),
                     GL_STATIC_DRAW);

// Index buffer (unsigned int indices)
        glGenBuffers(1, &indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.get_size() * triangles.get_dims_bytes(), triangles.get_ptr(),
                     GL_STATIC_DRAW);

// Normal buffer (single precision normals)
        glGenBuffers(1, &normalBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, normalBuffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, face_normals.get_size() * face_normals.get_dims_bytes(), nullptr,
                     GL_DYNAMIC_DRAW);

        auto program = createComputeShaderProgram(computeShaderSource);
        if (program == 0) {
            std::cerr << "Failed to create compute shader program!\n";
            return;
        }
        glUseProgram(program);

// Bind buffers
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertexBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, indexBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, normalBuffer);

// Dispatch compute shader
        glDispatchCompute(triangles.get_size(), 1, 1);

// Ensure all computations are done
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

// Retrieve the computed normals
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, normalBuffer);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, face_normals.get_size() * face_normals.get_dims_bytes(), &face_normals[0][0]);
// Process the normals...
        glDeleteBuffers(1, &vertexBuffer);
        glDeleteBuffers(1, &indexBuffer);
        glDeleteBuffers(1, &normalBuffer);
        glDeleteProgram(program);
    };
}
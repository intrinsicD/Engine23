//
// Created by alex on 20.06.24.
//

#include "FaceNormals.h"
#include "ComputeShader.h"
#include "glad/gl.h"
#include "GLFW/glfw3.h"
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
            dvec3 edge1 = v1 - v0;
            dvec3 edge2 = v2 - v0;
            dvec3 normal = cross(edge1, edge2);

            // Check for zero-length normals and handle them
            if (length(normal) == 0.0) {
                normals[index] = dvec3(0.0, 0.0, 0.0);
            } else {
                normals[index] = normalize(normal);
            }
        }
    )";

    void FaceNormals::set_position_data(const void *data, unsigned int size, unsigned int element_bytes) {
        if (b_positions.id == -1) {
            glGenBuffers(1, &b_positions.id);
        }
        b_positions.size = size;
        b_positions.element_bytes = element_bytes;
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, b_positions.id);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size * element_bytes, data, GL_STATIC_DRAW);
    }

    void FaceNormals::set_position_buffer(unsigned int id, unsigned int size, unsigned int element_bytes) {
        if (id == -1) {
            std::cerr
                    << "FaceNormals::set_position_buffer() id==-1 so buffer does not exist! Please create it first.\n";
            return;
        }
        b_positions.id = id;
        b_positions.size = size;
        b_positions.element_bytes = element_bytes;
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, b_positions.id);
    }

    void FaceNormals::set_triangle_data(const void *data, unsigned int size, unsigned int element_bytes) {
        if (b_triangles.id == -1) {
            glGenBuffers(1, &b_triangles.id);
        }
        b_triangles.size = size;
        b_triangles.element_bytes = element_bytes;
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, b_triangles.id);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size * element_bytes, data, GL_STATIC_DRAW);
    }

    void FaceNormals::set_triangle_buffer(unsigned int id, unsigned int size, unsigned int element_bytes) {
        if (id == -1) {
            std::cerr
                    << "FaceNormals::set_triangle_buffer() id==-1 so buffer does not exist! Please create it first.\n";
            return;
        }
        b_triangles.id = id;
        b_triangles.size = size;
        b_triangles.element_bytes = element_bytes;
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, b_triangles.id);
    }

    void FaceNormals::set_normal_buffer(unsigned int id, unsigned int size, unsigned int element_bytes) {
        if (id == -1) {
            std::cerr << "FaceNormals::set_normal_buffer() id==-1 so buffer does not exist! Please create it first.\n";
            return;
        }
        b_normals.id = id;
        b_normals.size = size;
        b_normals.element_bytes = element_bytes;
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, b_normals.id);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size * element_bytes, nullptr, GL_STATIC_DRAW);
    }

    void FaceNormals::compile_program() {
        if (program == -1) {
            program = createComputeShaderProgram(computeShaderSource);
        }
    }

    void FaceNormals::compute() {
        if (b_normals.id == -1) {
            glGenBuffers(1, &b_normals.id);
        }

        if (b_normals.size != b_triangles.size || b_normals.element_bytes != b_positions.element_bytes) {
            set_normal_buffer(b_normals.id, b_triangles.size, b_positions.element_bytes);
        }

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, b_positions.id);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, b_triangles.id);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, b_normals.id);

        if (program == -1) {
            compile_program();
        }
        glUseProgram(program);

        // Dispatch compute shader
        glDispatchCompute(b_triangles.size, 1, 1);

        // Make sure writing to the buffer has finished
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    void FaceNormals::destroy_all() {
        glDeleteBuffers(1, &b_positions.id);
        glDeleteBuffers(1, &b_triangles.id);
        glDeleteBuffers(1, &b_normals.id);
        glDeleteProgram(program);
    }

    void FaceNormals::get_result(unsigned int buffer, unsigned int size_bytes, void *data) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size_bytes, data);
    }

    std::vector<glm::vec3> FaceNormals::get_result() {
        std::vector<glm::vec3> result(b_normals.size);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, b_normals.id);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, b_normals.size * b_normals.element_bytes, result.data());
        return result;
    }

    void test() {
        // Initialize GLFW and GLEW
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return;
        }

        GLFWwindow *window = glfwCreateWindow(800, 600, "Compute Shader Example", nullptr, nullptr);
        if (!window) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(window);

        // Example vertex and index data
        std::vector<glm::dvec3> positions = {
                {0.0, 0.0, 0.0},
                {1.0, 0.0, 0.0},
                {0.0, 1.0, 0.0},
                {1.0, 1.0, 0.0}
        };

        std::vector<glm::uvec3> triangles = {
                {0, 1, 2},
                {2, 1, 3}
        };

        FaceNormals face_normals;
        face_normals.set_position_data(positions.data(), positions.size(), sizeof(positions[0]));
        face_normals.set_triangle_data(triangles.data(), triangles.size(), sizeof(triangles[0]));
        face_normals.compile_program();
        face_normals.compute();

        // Read back the normals
        std::vector<glm::vec3> result = face_normals.get_result();

        for (const auto &normal: result) {
            std::cout << "Normal: (" << normal.x << ", " << normal.y << ", " << normal.z << ")\n";
        }

        //cleanup
        face_normals.destroy_all();

        glfwTerminate();
    }
}
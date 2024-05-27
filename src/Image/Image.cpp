//
// Created by alex on 27.05.24.
//

#include "Image.h"
#include "SOIL/SOIL.h"
#include <iostream>

namespace Bcg {

    Image::Image() : width(0), height(0), channels(0), data(nullptr) {}

    Image::Image(const std::string &file_path) : width(0), height(0), channels(0), data(nullptr) {
        load(file_path);
    }

    Image::~Image() {
        free_image();
    }

    void Image::load(const std::string &file_path) {
        free_image(); // Free any existing image data

        data = SOIL_load_image(file_path.c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);
        if (!data) {
            throw std::runtime_error("Failed to load image: " + file_path + " - " + SOIL_last_result());
        }
    }

    void Image::free_image() {
        if (data) {
            SOIL_free_image_data(data);
            data = nullptr;
            width = 0;
            height = 0;
            channels = 0;
        }
    }
}
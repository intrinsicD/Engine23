//
// Created by alex on 27.05.24.
//

#ifndef ENGINE23_IMAGE_H
#define ENGINE23_IMAGE_H

#include <string>

namespace Bcg {

    class Image {
    public:
        // Constructors and Destructor
        Image();

        explicit Image(const std::string &file_path);

        ~Image();

        operator bool() const { return data != nullptr; }

        // Load image from file
        void load(const std::string &file_path);

        int get_width() const { return width; }

        int get_height() const { return height; }

        int get_channels() const { return channels; }

        const unsigned char *get_data() const { return data; }

    private:
        int width;
        int height;
        int channels;
        unsigned char *data;

        void free_image();
    };
}

#endif //ENGINE23_IMAGE_H

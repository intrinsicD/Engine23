//
// Created by alex on 26.02.24.
//

#ifndef ENGINE23_ASSET_H
#define ENGINE23_ASSET_H

#include <string>
#include <iostream>

namespace Bcg {
    struct Asset {
        std::string name;
        std::string filepath;
        std::string type;

        friend std::ostream &operator<<(std::ostream &stream, const Asset &asset) {
            stream << "Asset: \n name: " + asset.name + "\n filepath: " << asset.filepath << "\n type: "
                   << asset.type + "\n";
            return stream;
        }
    };
}

#endif //ENGINE23_ASSET_H

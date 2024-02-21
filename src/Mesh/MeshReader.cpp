//
// Created by alex on 21.02.24.
//

#include "MeshReader.h"
#include "FilePath.h"

namespace Bcg{
    MeshReader::MeshReader(std::string filepath, Mesh &mesh) : filepath(std::move(filepath)), mesh(mesh) {}

    bool MeshReader::read() { return FilePath::Exists(filepath); }
}
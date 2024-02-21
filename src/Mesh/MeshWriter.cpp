//
// Created by alex on 21.02.24.
//

#include "MeshWriter.h"

namespace Bcg {
    MeshWriter::MeshWriter(std::string filepath, Bcg::Mesh &mesh) : filepath(std::move(filepath)), mesh(mesh) {}
}
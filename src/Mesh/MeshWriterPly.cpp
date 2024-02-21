//
// Created by alex on 21.02.24.
//

#include "MeshWriterPly.h"
#include "happly.h"

namespace Bcg {
    bool MeshWriterPly::write() {
        std::vector<std::array<double, 3>> meshVertexPositions;
        std::vector<std::vector<size_t>> meshFaceIndices;

        meshVertexPositions.reserve(mesh.num_vertices());
        meshFaceIndices.reserve(mesh.num_faces());
        auto positions = mesh.vertices.get<Eigen::Vector<double, 3 >>("v_position");
        for (const auto v: mesh.vertices) {
            meshVertexPositions.push_back({positions[v][0], positions[v][1], positions[v][2]});
        }
        auto triangles = mesh.get_triangles();

// Create an empty object
        happly::PLYData plyOut;

// add mesh data (elements are created automatically)
        plyOut.addVertexPositions(meshVertexPositions);
        plyOut.addFaceIndices(meshFaceIndices);


// write the object to file
        plyOut.write(filepath, happly::DataFormat::ASCII);
        return true;
    }
}
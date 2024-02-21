//
// Created by alex on 21.02.24.
//

#include "MeshReaderPly.h"
#include "happly.h"

namespace Bcg {
    bool MeshReaderPly::read() {
        if (!MeshReader::read()) return false;

        happly::PLYData plyIn(filepath);

        std::vector<std::array<double, 3>> vPos = plyIn.getVertexPositions();
        std::vector<std::array<unsigned char, 3>> vCol = plyIn.getVertexColors();
        std::vector<std::vector<size_t>> fInd = plyIn.getFaceIndices<size_t>();

        mesh.vertices.reserve(vPos.size());

        auto positions = mesh.vertices.get_or_add<Eigen::Vector<double, 3>>("v_position");
        auto colors = mesh.vertices.get_or_add<Eigen::Vector<double, 3>>("v_color");

        for (const auto &point: vPos) {
            mesh.add_vertex(Eigen::Vector<double, 3>(point.data()), positions);
        }

        if(!vCol.empty()){
            for (const auto &v : mesh.vertices) {
                const Eigen::Vector<double, 3> color(vCol[v.idx][0] / 255.0, vCol[v.idx][1] / 255.0, vCol[v.idx][2] / 255.0);
                colors[v] = color;
            }
        }else{
            mesh.vertices.erase(colors);
        }

        mesh.faces.reserve(fInd.size());
        for (const auto &face: fInd) {
            mesh.add_face({face[0], face[1], face[2]});
        }

        return mesh.faces.get_size() > 0;
    }
}
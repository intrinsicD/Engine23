//
// Created by alex on 20.06.24.
//

#ifndef ENGINE23_GRAPHEDGELENGTHS_H
#define ENGINE23_GRAPHEDGELENGTHS_H

#include "Graph.h"

namespace Bcg {
    double GraphEdgeLength(const Graph &graph, const EdgeHandle &e, const Property<Eigen::Vector<double, 3>> &positions);

    Property<double> GraphEdgeLengths(Graph &graph);
}

#endif //ENGINE23_GRAPHEDGELENGTHS_H

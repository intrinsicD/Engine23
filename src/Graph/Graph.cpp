//
// Created by alex on 20.02.24.
//

#include "Graph.h"

namespace Bcg {


    Graph::Graph() : Graph(VertexContainer(),
                           HalfedgeContainer(),
                           EdgeContainer(), "GraphHalfedge") {

    }

    Graph::Graph(const VertexContainer &vertices_, const HalfedgeContainer &halfedges_, const EdgeContainer &edges_,
                 std::string name)
            :
            PointCloud(vertices_, std::move(name)),
            halfedges(halfedges_),
            edges(edges_),
            v_connectivity(vertices.get_or_add<VertexConnectivity>("v_connectivity")),
            h_connectivity(halfedges.get_or_add<HalfedgeConnectivity>("h_connectivity")),
            m_num_deleted_halfedges(0) {
        v_connectivity.set_column_names(" v ");
        h_connectivity.set_column_names(" v | next_h | prev_h ");
        deleted_halfedges = halfedges.get<bool>("h_deleted");
        deleted_edges = edges.get<bool>("e_deleted");

        assert(v_connectivity);
        assert(h_connectivity);
        assert(deleted_halfedges);
        assert(deleted_edges);
    }

    Graph &Graph::copy(const Graph &other) {
        if (this != &other) {
            edges = EdgeContainer();
            halfedges = HalfedgeContainer();

            PointCloud::copy(other);

            edges.resize(other.edges.get_size());
            halfedges.resize(other.halfedges.get_size());

            v_connectivity.storage() = other.v_connectivity.storage();
            h_connectivity.storage() = other.h_connectivity.storage();
            deleted_halfedges.storage() = other.deleted_halfedges.storage();
            deleted_edges.storage() = other.deleted_edges.storage();

            // how many elements are deleted?
            m_num_deleted_halfedges = other.m_num_deleted_halfedges;
            m_num_deleted_edges = other.m_num_deleted_edges;

            assert(v_connectivity);
            assert(h_connectivity);
            assert(deleted_halfedges);
            assert(deleted_edges);
        }
        return *this;
    }

    Graph &Graph::operator=(const Graph &other) {
        if (this != &other) {
            PointCloud::operator=(other);

            edges = other.edges;
            halfedges = other.halfedges;

            v_connectivity = vertices.get_or_add<VertexConnectivity>("v_connectivity");
            h_connectivity = halfedges.get_or_add<HalfedgeConnectivity>("h_connectivity");
            deleted_halfedges = halfedges.get_or_add<bool>("h_deleted");
            deleted_edges = edges.get_or_add<bool>("e_deleted");

            // how many elements are deleted?
            m_num_deleted_halfedges = other.m_num_deleted_halfedges;
            m_num_deleted_edges = other.m_num_deleted_edges;

            assert(v_connectivity);
            assert(h_connectivity);
            assert(deleted_halfedges);
            assert(deleted_edges);
        }
        return *this;
    }

    Graph::operator bool() const {
        return PointCloud::operator bool() && v_connectivity && h_connectivity && deleted_halfedges && deleted_edges;
    }

    void Graph::clear() {
        PointCloud::clear();
        halfedges.clear();
        halfedges.free_unused_memory();
        v_connectivity = vertices.get_or_add<VertexConnectivity>("v_connectivity");
        h_connectivity = halfedges.get_or_add<HalfedgeConnectivity>("h_connectivity");
        deleted_halfedges = halfedges.get_or_add("h_deleted", false);
        deleted_edges = edges.get_or_add("e_deleted", false);
        m_num_deleted_halfedges = 0;
        m_num_deleted_edges = 0;
        assert(v_connectivity);
        assert(h_connectivity);
        assert(deleted_halfedges);
        assert(deleted_edges);
    }

    HalfedgeHandle Graph::get_halfedge(const VertexHandle &v) const {
        return v_connectivity[v].h;
    }

    void Graph::set_halfedge(const VertexHandle &v, const HalfedgeHandle &h) {
        v_connectivity[v].h = h;
    }

    VertexHandle Graph::get_to_vertex(const HalfedgeHandle &h) const {
        return h_connectivity[h].v;
    }

    void Graph::set_vertex(const HalfedgeHandle &h, const VertexHandle &v) {
        h_connectivity[h].v = v;
    }

    VertexHandle Graph::get_from_vertex(const HalfedgeHandle &h) const {
        return get_to_vertex(get_opposite(h));
    }

    HalfedgeHandle Graph::get_next(const HalfedgeHandle &h) const {
        return h_connectivity[h].nh;
    }

    void Graph::set_next(const HalfedgeHandle &h, const HalfedgeHandle &nh) {
        h_connectivity[h].nh = nh;
        h_connectivity[nh].ph = h;
    }

    HalfedgeHandle Graph::get_prev(const HalfedgeHandle &h) const {
        return h_connectivity[h].ph;
    }

    HalfedgeHandle Graph::get_opposite(const HalfedgeHandle &h) const {
        return (h & 1) ? h - 1 : h + 1;
    }

    HalfedgeHandle Graph::rotate_cw(const HalfedgeHandle &h) const {
        return get_next(get_opposite(h));
    }

    HalfedgeHandle Graph::rotate_ccw(const HalfedgeHandle &h) const {
        return get_opposite(get_prev(h));
    }

    EdgeHandle Graph::get_edge(const HalfedgeHandle &h) const {
        return (h >> 1);
    }

    HalfedgeHandle Graph::get_halfedge(const EdgeHandle &e, bool i) const {
        return (e << 1) + i;
    }

    VertexHandle Graph::get_vertex(const EdgeHandle &e, bool i) const {
        return get_to_vertex(get_halfedge(e, i));
    }

    bool Graph::is_isolated(const VertexHandle &v) const {
        return !halfedges.is_valid(get_halfedge(v));
    }

    bool Graph::is_boundary(const VertexHandle &v) const {
        auto h = get_halfedge(v);
        return get_prev(h) == get_opposite(h);
    }

    bool Graph::is_boundary(const HalfedgeHandle &h) const {
        return get_next(h) == get_opposite(h);
    }

    bool Graph::is_boundary(const EdgeHandle &e) const {
        return is_boundary(get_halfedge(e, 0)) || is_boundary(get_halfedge(e, 1));
    }

    bool Graph::has_garbage() const {
        return m_num_deleted_halfedges > 0 || m_num_deleted_edges > 0 || PointCloud::has_garbage();
    }

    void Graph::garbage_collection() {
        if (!has_garbage()) return;

        size_t nV = vertices.get_size();
        size_t nE = edges.get_size();
        size_t nH = halfedges.get_size();

        auto vmap = vertices.get_or_add<VertexHandle>("v_garbage_collection");
        auto hmap = halfedges.get_or_add<HalfedgeHandle>("h_garbage_collection");

        for (size_t i = 0; i < nV; ++i) {
            vmap[i] = VertexHandle(i);
        }

        for (size_t i = 0; i < nH; ++i) {
            hmap[i] = HalfedgeHandle(i);
        }

        if (nV > 0) {
            size_t i0 = 0;
            size_t i1 = nV - 1;

            while (true) {
                // find first deleted and last un-deleted
                while (!deleted_vertices[i0] && i0 < i1) {
                    ++i0;
                }
                while (deleted_vertices[i1] && i0 < i1) {
                    --i1;
                }
                if (i0 >= i1) break;

                // swap
                vertices.swap(i0, i1);
            }

            // remember new size
            nV = deleted_vertices[i0] ? i0 : i0 + 1;
        }

        if (nE > 0) {
            size_t i0 = 0;
            size_t i1 = nE - 1;

            while (true) {
                // find first deleted and last un-deleted
                while (!deleted_edges[i0] && i0 < i1) {
                    ++i0;
                }
                while (deleted_edges[i1] && i0 < i1) {
                    --i1;
                }
                if (i0 >= i1) break;

                // swap
                edges.swap(i0, i1);
                halfedges.swap(2 * i0, 2 * i1);
                halfedges.swap(2 * i0 + 1, 2 * i1 + 1);
            }

            // remember new size
            nE = deleted_edges[i0] ? i0 : i0 + 1;
            nH = 2 * nE;
        }

        // update vertex connectivity
        for (size_t v = 0; v < nV; ++v) {
            if (!is_isolated(v)) {
                set_halfedge(v, hmap[get_halfedge(v)]);
            }
        }

        for (size_t h = 0; h < nH; ++h) {
            set_vertex(h, vmap[get_to_vertex(h)]);
            auto next = get_next(h);
            if (halfedges.is_valid(next)) {
                set_next(h, hmap[next]);
            }
        }

        vertices.erase(vmap);
        halfedges.erase(hmap);

        vertices.resize(num_vertices());
        vertices.free_unused_memory();
        halfedges.resize(num_halfedges());
        halfedges.free_unused_memory();
        edges.resize(num_edges());
        edges.free_unused_memory();

        m_num_deleted_vertices = 0;
        m_num_deleted_halfedges = 0;
        m_num_deleted_edges = 0;
        assert(!has_garbage());
    }

    void Graph::delete_vertex(const VertexHandle &v) {
        if (deleted_vertices[v]) return;
        else {
            for (const auto h: get_halfedges(v)) {
                delete_edge(get_edge(h));
            }

            PointCloud::delete_vertex(v);
        }
    }

    HalfedgeHandle Graph::new_edge(const VertexHandle &v0, const VertexHandle &v1) {
        assert(v0 != v1);
        edges.push_back();
        halfedges.push_back();
        halfedges.push_back();

        HalfedgeHandle h(halfedges.get_size() - 2);
        HalfedgeHandle o(halfedges.get_size() - 1);

        set_vertex(h, v1);
        set_vertex(o, v0);

        assert(h_connectivity.is_dirty());
        assert(v_connectivity.is_dirty());
        return h;
    }

    HalfedgeHandle Graph::add_edge(const VertexHandle &v0, const VertexHandle &v1) {
        assert(h_connectivity);
        assert(v_connectivity);
        //check if edge exists between v0 and v1.
        auto h01 = find_halfedge(v0, v1);
        if (h01.is_valid()) {
            //edge exists, so just return it.
            return h01;
        }

        //there is no edge between v0 and v1 jet
        //check if v0 has an outgoing halfedge
        auto h0 = get_halfedge(v0);
        //check if v0 has an outgoing halfedge
        auto h1 = get_halfedge(v1);

        //make new edge between v0 and v1
        auto new_h = new_edge(v0, v1);
        auto new_o = get_opposite(new_h);

        if (h1.is_valid()) {
            auto p = get_prev(h1);
            set_next(p, new_o);
            set_next(new_h, h1);
        } else {
            set_next(new_h, new_o);
        }

        set_halfedge(v1, new_o);

        if (h0.is_valid()) {
            auto p = get_prev(h0);
            set_next(p, new_h);
            set_next(new_o, h0);
        } else {
            set_next(new_o, new_h);
        }

        set_halfedge(v0, new_h);

        assert(h_connectivity.is_dirty());
        assert(v_connectivity.is_dirty());
        return new_h;
    }

    HalfedgeHandle Graph::find_halfedge(const VertexHandle &v0, const VertexHandle &v1) const {
        if (halfedges.is_valid(get_halfedge(v0))) {
            for (const auto h: get_halfedges(v0)) {
                if (get_to_vertex(h) == v1) {
                    return h;
                }
            }
        }

        return {};
    }

    EdgeHandle Graph::find_edge(const VertexHandle &v0, const VertexHandle &v1) const {
        auto h = find_halfedge(v0, v1);
        return halfedges.is_valid(h) ? get_edge(h) : EdgeHandle();
    }

    void Graph::delete_edge(const EdgeHandle &e) {
        if (deleted_edges[e]) return;

        auto h0 = get_halfedge(e, 0);
        auto h1 = get_halfedge(e, 1);

        auto from_v = get_to_vertex(h1);
        auto to_v = get_to_vertex(h0);

        if (halfedges.is_valid(h0)) {
            auto p = get_prev(h0);
            auto n = get_next(h1);
            if (find_halfedge(get_to_vertex(n), from_v).is_valid()) {
                set_next(p, n);
            }
        }
        if (halfedges.is_valid(h1)) {
            auto p = get_prev(h1);
            auto n = get_next(h0);
            if (find_halfedge(get_to_vertex(n), to_v).is_valid()) {
                set_next(p, n);
            }
        }

        mark_edge_deleted(e);
    }

    void Graph::mark_edge_deleted(const EdgeHandle &e) {
        if (deleted_edges[e]) return;

        deleted_edges[e] = true;
        deleted_halfedges[get_halfedge(e, 0)] = true;
        deleted_halfedges[get_halfedge(e, 1)] = true;
        ++m_num_deleted_edges;
        ++m_num_deleted_halfedges;
        ++m_num_deleted_halfedges;
        deleted_edges.set_dirty();
    }

    size_t Graph::num_halfedges() const {
        return halfedges.get_size() - m_num_deleted_halfedges;
    }

    size_t Graph::num_deleted_halfedges() const {
        return m_num_deleted_halfedges;
    }

    size_t Graph::num_edges() const {
        return edges.get_size() - m_num_deleted_edges;
    }

    size_t Graph::num_deleted_edges() const {
        return m_num_deleted_edges;
    }

    //------------------------------------------------------------------------------------------------------------------

    Graph::VertexAroundVertexCirculator::VertexAroundVertexCirculator(const Graph *ds,
                                                                      const VertexHandle &v) : ds(ds),
                                                                                        active(false) {
        if (ds) {
            halfedge = ds->get_halfedge(v);
        }
    }

    bool Graph::VertexAroundVertexCirculator::operator==(const VertexAroundVertexCirculator &rhs) const {
        assert(ds);
        assert(ds == rhs.ds);
        return (active && (halfedge == rhs.halfedge));
    }

    bool Graph::VertexAroundVertexCirculator::operator!=(const VertexAroundVertexCirculator &rhs) const {
        return !operator==(rhs);
    }

    typename Graph::VertexAroundVertexCirculator &
    Graph::VertexAroundVertexCirculator::operator++() {
        assert(ds);
        auto next = ds->rotate_ccw(halfedge);
        halfedge = ds->halfedges.is_valid(next) ? next : halfedge;
        /*    halfedge = ds->rotate_ccw(halfedge);*/
        active = true;
        return *this;
    }

    typename Graph::VertexAroundVertexCirculator &
    Graph::VertexAroundVertexCirculator::operator--() {
        assert(ds);
        halfedge = ds->rotate_cw(halfedge);
        return *this;
    }

    VertexHandle Graph::VertexAroundVertexCirculator::operator*() const {
        assert(ds);
        return ds->get_to_vertex(halfedge);
    }

    Graph::VertexAroundVertexCirculator::operator bool() const {
        return halfedge.is_valid();
    }

    typename Graph::VertexAroundVertexCirculator &
    Graph::VertexAroundVertexCirculator::begin() {
        active = !halfedge.is_valid();
        return *this;
    }

    typename Graph::VertexAroundVertexCirculator &
    Graph::VertexAroundVertexCirculator::end() {
        active = true;
        return *this;
    }

    typename Graph::VertexAroundVertexCirculator Graph::get_vertices(const VertexHandle &v) const {
        return VertexAroundVertexCirculator(this, v);
    }

    //------------------------------------------------------------------------------------------------------------------

    Graph::HalfedgeAroundVertexCirculator::HalfedgeAroundVertexCirculator(const Graph *ds,
                                                                          const VertexHandle &v) : ds(ds),
                                                                                            active(false) {
        if (ds) {
            halfedge = ds->get_halfedge(v);
        }
    }

    bool
    Graph::HalfedgeAroundVertexCirculator::operator==(const HalfedgeAroundVertexCirculator &rhs) const {
        assert(ds);
        assert(ds == rhs.ds);
        return (active && (halfedge == rhs.halfedge));
    }

    bool
    Graph::HalfedgeAroundVertexCirculator::operator!=(const HalfedgeAroundVertexCirculator &rhs) const {
        return !operator==(rhs);
    }

    typename Graph::HalfedgeAroundVertexCirculator &
    Graph::HalfedgeAroundVertexCirculator::operator++() {
        assert(ds);
        halfedge = ds->rotate_ccw(halfedge);
        active = true;
        return *this;
    }

    typename Graph::HalfedgeAroundVertexCirculator &
    Graph::HalfedgeAroundVertexCirculator::operator--() {
        assert(ds);
        halfedge = ds->rotate_cw(halfedge);
        return *this;
    }

    HalfedgeHandle Graph::HalfedgeAroundVertexCirculator::operator*() const {
        return halfedge;
    }

    Graph::HalfedgeAroundVertexCirculator::operator bool() const {
        return halfedge.is_valid();
    }

    typename Graph::HalfedgeAroundVertexCirculator &
    Graph::HalfedgeAroundVertexCirculator::begin() {
        active = !halfedge.is_valid();
        return *this;
    }

    typename Graph::HalfedgeAroundVertexCirculator &
    Graph::HalfedgeAroundVertexCirculator::end() {
        active = true;
        return *this;
    }

    typename Graph::HalfedgeAroundVertexCirculator
    Graph::get_halfedges(const VertexHandle &v) const {
        return HalfedgeAroundVertexCirculator(this, v);
    }

    //------------------------------------------------------------------------------------------------------------------

    size_t Graph::get_valence(const VertexHandle &v) const {
        if (is_isolated(v)) return 0;
        size_t count = 0;
        for (const auto vj: get_vertices(v)) {
            if (vj.is_valid()) {
                ++count;
            }
        }
        return count;
    }



    std::ostream &operator<<(std::ostream &stream, const Graph &graph) {
        stream << "              GraphHalfedgeName: " << graph.get_name() << "\n";

        stream << "        HasProperty(v_position): " << graph.vertices.has("v_position") << "\n";
        stream << "    HasProperty(v_connectivity): " << graph.vertices.has("v_connectivity") << "\n";
        stream << "    HasProperty(h_connectivity): " << graph.halfedges.has("h_connectivity") << "\n";

        stream << "         HasProperty(v_deleted): " << graph.vertices.has("v_deleted") << "\n";
        stream << "         HasProperty(h_deleted): " << graph.halfedges.has("h_deleted") << "\n";
        stream << "         HasProperty(e_deleted): " << graph.edges.has("e_deleted") << "\n";

        stream << "                    has_garbage: " << graph.has_garbage() << "\n";
        stream << " num_vertices (Total - Deleted): " << graph.num_vertices() << "\n";
        stream << "num_halfedges (Total - Deleted): " << graph.num_halfedges() << "\n";
        stream << "    num_edges (Total - Deleted): " << graph.num_edges() << "\n";

        stream << "           num_deleted_vertices: " << graph.num_deleted_vertices() << "\n\n";
        stream << "          num_deleted_halfedges: " << graph.num_deleted_halfedges() << "\n\n";
        stream << "              num_deleted_edges: " << graph.num_deleted_edges() << "\n\n";

        stream << graph.vertices << "\n\n";

        stream << graph.halfedges << "\n\n";

        stream << graph.edges << "\n\n";
        return stream;
    }
}

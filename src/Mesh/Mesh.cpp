//
// Created by alex on 20.02.24.
//


#include <iostream>
#include "Mesh.h"
#include "VectorEigenMap.h"

namespace Bcg {

    Mesh::Mesh() : Mesh(VertexContainer(),
                        HalfedgeContainer(),
                        EdgeContainer(),
                        FaceContainer(), "MeshHalfedge") {

    }

    Mesh::Mesh(const VertexContainer &vertices_,
               const HalfedgeContainer &halfedges_,
               const EdgeContainer &edges_,
               const FaceContainer &faces_,
               std::string name) : Graph(vertices_, halfedges_, edges_, name), faces(faces_),
                                   hf_connectivity(halfedges.get_or_add<FaceHandle>("hf_connectivity")),
                                   f_connectivity(faces.get_or_add<FaceConnectivity>("f_connectivity")),
                                   m_num_deleted_faces(0) {
        hf_connectivity.set_column_names("f");
        f_connectivity.set_column_names("h");
        deleted_faces = faces.get<bool>("f_deleted");
        assert(hf_connectivity);
        assert(f_connectivity);
        assert(deleted_faces);
    }

    Mesh &Mesh::assign(const Mesh &other) {
        //all properties are linked (pointers are the same)
        if (this != &other) {
            faces = FaceContainer();

            Graph::copy(other);

            faces.resize(other.faces.get_size());

            hf_connectivity.storage() = other.hf_connectivity.storage();
            f_connectivity.storage() = other.f_connectivity.storage();
            deleted_faces.storage() = other.deleted_faces.storage();

            // how many elements are deleted?
            m_num_deleted_faces = other.m_num_deleted_faces;

            assert(hf_connectivity);
            assert(f_connectivity);
            assert(deleted_faces);
        }
        return *this;
    }

    Mesh &Mesh::operator=(const Mesh &other) {
        if (this != &other) {
            Graph::operator=(other);

            faces = other.faces;

            hf_connectivity = halfedges.get_or_add<FaceHandle>("hf_connectivity");
            f_connectivity = faces.get_or_add<FaceConnectivity>("f_connectivity");
            deleted_faces = faces.get_or_add<bool>("f_deleted");

            // how many elements are deleted?
            m_num_deleted_vertices = other.m_num_deleted_vertices;
            m_num_deleted_halfedges = other.m_num_deleted_halfedges;
            m_num_deleted_edges = other.m_num_deleted_edges;
            m_num_deleted_faces = other.m_num_deleted_faces;

            assert(hf_connectivity);
            assert(f_connectivity);
            assert(deleted_faces);
        }
        return *this;
    }

    Mesh::operator bool() const {
        return Graph::operator bool() && hf_connectivity && f_connectivity && deleted_faces;
    }

    void Mesh::clear() {
        Graph::clear();

        faces.clear();
        faces.free_unused_memory();
        hf_connectivity = halfedges.get_or_add<FaceHandle>("hf_connectivity");
        f_connectivity = faces.get_or_add<FaceConnectivity>("f_connectivity");
        deleted_faces = faces.get_or_add<bool>("f_deleted", false);
        m_num_deleted_faces = 0;
    }

    size_t Mesh::num_faces() const {
        return faces.get_size() - m_num_deleted_faces;
    }

    size_t Mesh::num_deleted_faces() const {
        return m_num_deleted_faces;
    }

    bool Mesh::is_valid(const FaceHandle &f) const {
        return f.idx < faces.get_size();
    }

    bool Mesh::has_garbage() const {
        return m_num_deleted_faces > 0 || Graph::has_garbage();
    }

    void Mesh::garbage_collection() {
        size_t nV = vertices.get_size();
        size_t nE = edges.get_size();
        size_t nH = halfedges.get_size();
        size_t nF = faces.get_size();

        auto vmap = vertices.get_or_add<VertexHandle>("v_garbage_collection");
        auto hmap = halfedges.get_or_add<HalfedgeHandle>("h_garbage_collection");
        auto fmap = faces.get_or_add<FaceHandle>("f_garbage_collection");

        for (size_t i = 0; i < nV; ++i) {
            vmap[i] = VertexHandle(i);
        }
        for (size_t i = 0; i < nH; ++i) {
            hmap[i] = HalfedgeHandle(i);
        }
        for (size_t i = 0; i < nF; ++i) {
            fmap[i] = FaceHandle(i);
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

        if (nF > 0) {
            size_t i0 = 0;
            size_t i1 = nF - 1;

            while (true) {
                // find 1st deleted and last un-deleted
                while (!deleted_faces[i0] && i0 < i1) {
                    ++i0;
                }
                while (deleted_faces[i1] && i0 < i1) {
                    --i1;
                }
                if (i0 >= i1) break;

                // swap
                faces.swap(i0, i1);
            }

            // remember new size
            nF = deleted_faces[i0] ? i0 : i0 + 1;
        }

        // update vertex connectivity
        VertexHandle v;
        for (size_t i = 0; i < nV; ++i) {
            v = VertexHandle(i);
            if (!is_isolated(v)) {
                Graph::set_halfedge(v, hmap[Graph::get_halfedge(v)]);
            }
        }

        // update halfedge connectivity
        HalfedgeHandle h;
        for (size_t i = 0; i < nH; ++i) {
            h = HalfedgeHandle(i);
            Graph::set_vertex(h, vmap[Graph::get_to_vertex(h)]);
            Graph::set_next(h, hmap[Graph::get_next(h)]);
            if (!is_boundary(h)) {
                set_face(h, fmap[get_face(h)]);
            }
        }

        // update handles of faces
        FaceHandle f;
        for (size_t i = 0; i < nF; ++i) {
            f = FaceHandle(i);
            set_halfedge(f, hmap[get_halfedge(f)]);
        }

        // remove handle maps
        vertices.erase(vmap);
        halfedges.erase(hmap);
        faces.erase(fmap);

        // finally, resize arrays
        vertices.resize(num_vertices());
        vertices.free_unused_memory();
        halfedges.resize(num_halfedges());
        halfedges.free_unused_memory();
        edges.resize(num_edges());
        edges.free_unused_memory();
        faces.resize(num_faces());
        faces.free_unused_memory();

        m_num_deleted_faces = 0;
        m_num_deleted_edges = 0;
        m_num_deleted_halfedges = 0;
        m_num_deleted_vertices = 0;

        assert(!has_garbage());
        assert(vertices.has_dirty_properties());
        assert(edges.has_dirty_properties());
        assert(halfedges.has_dirty_properties());
        assert(faces.has_dirty_properties());
    }

    bool Mesh::is_boundary(const VertexHandle &v) const {
        return is_boundary(Graph::get_halfedge(v));
    }

    bool Mesh::is_boundary(const HalfedgeHandle &h) const {
        return !(h.is_valid() && get_face(h).is_valid());
    }

    bool Mesh::is_boundary(const EdgeHandle &e) const {
        return is_boundary(Graph::get_halfedge(e, 0)) || is_boundary(Graph::get_halfedge(e, 1));
    }

    bool Mesh::is_boundary(const FaceHandle &f) const {
        if (!is_valid(f)) return true;
        HalfedgeHandle h = get_halfedge(f);
        HalfedgeHandle hh = h;
        do {
            if (is_boundary(get_opposite(h))) {
                return true;
            }
            h = get_next(h);
        } while (h != hh);
        return false;
    }

    bool Mesh::is_manifold(const VertexHandle &v) const {
        size_t count = 0;
        for (const auto h: Graph::get_halfedges(v)) {
            count += is_boundary(h);
            if (count > 2) return false;
        }
        return true;
    }

    bool Mesh::is_manifold() const {
        for (const auto v: vertices) {
            if (!is_manifold(v)) return false;
        }
        return true;
    }

    bool Mesh::is_triangle_mesh() const {
        for (const auto f: faces) {
            if (get_valence(f) != 3) return false;
        }
        return true;
    }

    bool Mesh::is_quad_mesh() const {
        for (const auto f: faces) {
            if (get_valence(f) != 4) return false;
        }
        return true;
    }

    bool Mesh::is_removal_ok(const EdgeHandle &e) const {
        HalfedgeHandle h0 = Graph::get_halfedge(e, 0);
        HalfedgeHandle h1 = Graph::get_halfedge(e, 1);

        VertexHandle v0 = Graph::get_to_vertex(h0);
        VertexHandle v1 = Graph::get_to_vertex(h1);

        FaceHandle f0 = get_face(h0);
        FaceHandle f1 = get_face(h1);

        // boundary?
        if (!f0.is_valid() || !f1.is_valid()) {
            return false;
        }

        // same face?
        if (f0 == f1) {
            return false;
        }

        // are the two faces connect through another vertex?
        for (const auto v: get_vertices(f0)) {
            if (v != v0 && v != v1) {
                for (const auto f: get_faces(v)) {
                    if (f == f1) {
                        return false;
                    }
                }
            }
        }

        return true;
    }

    bool Mesh::is_collapse_ok(const HalfedgeHandle &h) const {
        HalfedgeHandle v1v0 = Graph::get_opposite(h);
        VertexHandle v0 = Graph::get_to_vertex(v1v0);
        VertexHandle v1 = Graph::get_to_vertex(h);
        VertexHandle vl, vr;
        HalfedgeHandle h1, h2;

        // the edges v1-vl and vl-v0 must not be both boundary edges
        if (!is_boundary(h)) {
            h1 = Graph::get_next(h);
            vl = Graph::get_to_vertex(h1);
            h2 = Graph::get_next(h1);
            if (is_boundary(Graph::get_opposite(h1)) && is_boundary(Graph::get_opposite(h2))) {
                return false;
            }
        }

        // the edges v0-vr and vr-v1 must not be both boundary edges
        if (!is_boundary(v1v0)) {
            h1 = Graph::get_next(v1v0);
            vr = Graph::get_to_vertex(h1);
            h2 = Graph::get_next(h1);
            if (is_boundary(Graph::get_opposite(h1)) && is_boundary(Graph::get_opposite(h2))) {
                return false;
            }
        }

        // if vl and vr are equal or both invalid -> fail
        if (vl == vr) {
            return false;
        }

        // edge between two boundary vertices should be a boundary edge
        if (is_boundary(v0) && is_boundary(v1) && !is_boundary(h) && !is_boundary(v1v0)) {
            return false;
        }

        // test intersection of the one-rings of v0 and v1
        VertexAroundVertexCirculator vvit, vvend;
        vvit = vvend = Graph::get_vertices(v0);
        VertexHandle vv;
        do {
            vv = *vvit;
            if (vv != v1 && vv != vl && vv != vr)
                if (find_halfedge(vv, v1).is_valid()) {
                    return false;
                }
        } while (++vvit != vvend);

        // passed all tests
        return true;
    }

    bool Mesh::is_flip_ok(const EdgeHandle &e) const {
        // boundary edges cannot be flipped
        if (is_boundary(e)) {
            return false;
        }

        HalfedgeHandle h0 = Graph::get_halfedge(e, 0);
        HalfedgeHandle h1 = Graph::get_halfedge(e, 1);

        //flip is only allowed in triangles
        /*    if (get_valence(get_face(h0)) != 3 || get_valence(get_face(h1)) != 3) {
                return false;
            }*/

        // check if the flipped edge is already present in the mesh
        VertexHandle v0 = Graph::get_to_vertex(Graph::get_next(h0));
        VertexHandle v1 = Graph::get_to_vertex(Graph::get_next(h1));

        // this is generally a bad sign !!!
        if (v0 == v1) {
            return false;
        }

        return !find_halfedge(v0, v1).is_valid();
    }

    FaceHandle Mesh::get_face(const EdgeHandle &e, bool i) const {
        return get_face(Graph::get_halfedge(e, i));
    }

    FaceHandle Mesh::get_face(const HalfedgeHandle &h) const {
        return hf_connectivity[h].idx;
    }

    void Mesh::set_face(const HalfedgeHandle &h, const FaceHandle &f) {
        hf_connectivity[h] = f;
    }

    HalfedgeHandle Mesh::get_halfedge(const FaceHandle &f) const {
        return f_connectivity[f].h;
    }

    void Mesh::set_halfedge(const FaceHandle &f, const HalfedgeHandle &h) {
        f_connectivity[f].h = h;
    }

    size_t Mesh::get_valence(const FaceHandle &f) const {
        size_t count = 0;
        for (const auto v: get_vertices(f)) {
            count += vertices.is_valid(v) && !deleted_vertices[v];
        }
        return count;
    }

    FaceHandle Mesh::new_face() {
        faces.push_back();
        assert(faces.has_dirty_properties());
        return {faces.get_size() - 1};
    }

    FaceHandle Mesh::add_face(const std::vector<VertexHandle> &f_vertices) {
        size_t n = f_vertices.size();

        auto &f_halfedges = m_add_face_halfedges;
        auto &h_is_new = m_add_face_is_new;
        auto &h_needs_adjust = m_add_face_needs_adjust;
        f_halfedges.clear();
        f_halfedges.resize(n);
        h_is_new.clear();
        h_is_new.resize(n);
        h_needs_adjust.clear();
        h_needs_adjust.resize(n, false);

        // test for topological errors
        for (size_t i = 0, ii = 1; i < n; ++i, ++ii, ii %= n) {
            if (!is_boundary(f_vertices[i])) {
                std::cerr << "add_face: complex vertex!\n";
                return {};
            }

            f_halfedges[i] = Graph::find_halfedge(f_vertices[i], f_vertices[ii]);
            h_is_new[i] = !f_halfedges[i].is_valid();

            if (!h_is_new[i] && !is_boundary(f_halfedges[i])) {
                std::cerr << "add_face: complex edge!\n";
                return {};
            }
        }

        NextCache &next_cache = m_add_face_next_cache;
        next_cache.clear();
        next_cache.reserve(3 * n);
        HalfedgeHandle inner_next, inner_prev, outer_prev, outer_next, boundary_prev, boundary_next, patch_start, patch_end;
        // re-link patches if necessary
        for (size_t i = 0, ii = 1; i < n; ++i, ++ii, ii %= n) {
            if (!h_is_new[i] && !h_is_new[ii]) {
                inner_prev = f_halfedges[i];
                inner_next = f_halfedges[ii];

                if (Graph::get_next(inner_prev) != inner_next) {
                    // here comes the ugly part... we have to relink a whole patch
                    // search a free gap which will be between boundary_prev and boundary_next
                    outer_prev = Graph::get_opposite(inner_next);
                    outer_next = Graph::get_opposite(inner_prev);
                    boundary_prev = outer_prev;
                    do {
                        boundary_prev = Graph::get_opposite(Graph::get_next(boundary_prev));
                    } while (!is_boundary(boundary_prev) || boundary_prev == inner_prev);
                    boundary_next = Graph::get_next(boundary_prev);
                    assert(is_boundary(boundary_prev));
                    assert(is_boundary(boundary_next));
                    if (!is_boundary(boundary_prev)) {
                        std::cerr << "boundary_prev should be boundary!\n";
                    }
                    if (!is_boundary(boundary_next)) {
                        std::cerr << "boundary_next should be boundary!\n";
                    }

                    if (boundary_next == inner_next) {
                        std::cerr << "add_face: patch re-linking failed!\n";
                        return {};
                    }

                    // other halfedges' handles
                    patch_start = Graph::get_next(inner_prev);
                    patch_end = Graph::get_prev(inner_next);
                    // relink
                    next_cache.emplace_back(boundary_prev, patch_start);
                    next_cache.emplace_back(patch_end, boundary_next);
                    next_cache.emplace_back(inner_prev, inner_next);
                }
            }
        }

        // create missing edges
        for (size_t i = 0, ii = 1; i < n; ++i, ++ii, ii %= n) {
            if (h_is_new[i]) {
                f_halfedges[i] = Graph::new_edge(f_vertices[i], f_vertices[ii]);
            }
        }

        // create the face
        FaceHandle f(new_face());
        set_halfedge(f, f_halfedges[n - 1]);

        VertexHandle v;
        // setup halfedges
        for (size_t i = 0, ii = 1; i < n; ++i, ++ii, ii %= n) {
            v = f_vertices[ii];
            inner_prev = f_halfedges[i];
            inner_next = f_halfedges[ii];

            size_t id = 0;
            if (h_is_new[i]) {
                id |= 1;
            }
            if (h_is_new[ii]) {
                id |= 2;
            }

            if (id) {
                outer_prev = Graph::get_opposite(inner_next);
                outer_next = Graph::get_opposite(inner_prev);

                // set outer links
                switch (id) {
                    case 1: // prev is new, next is old
                        boundary_prev = Graph::get_prev(inner_next);
                        next_cache.emplace_back(boundary_prev, outer_next);
                        Graph::set_halfedge(v, outer_next);
                        break;

                    case 2: // next is new, prev is old
                        boundary_next = Graph::get_next(inner_prev);
                        next_cache.emplace_back(outer_prev, boundary_next);
                        Graph::set_halfedge(v, boundary_next);
                        break;

                    case 3: // both are new
                        if (!Graph::get_halfedge(v).is_valid()) {
                            Graph::set_halfedge(v, outer_next);
                            next_cache.emplace_back(outer_prev, outer_next);
                        } else {
                            boundary_next = Graph::get_halfedge(v);
                            boundary_prev = Graph::get_prev(boundary_next);
                            next_cache.emplace_back(boundary_prev, outer_next);
                            next_cache.emplace_back(outer_prev, boundary_next);
                        }
                        break;
                    default:
                        break;
                }

                // set inner link
                next_cache.emplace_back(inner_prev, inner_next);
            } else {
                h_needs_adjust[ii] = (Graph::get_halfedge(v) == inner_next);
            }

            // set face handle
            set_face(f_halfedges[i], f);
        }

        // process next halfedge cache
        NextCache::const_iterator ncIt(next_cache.begin()), ncEnd(next_cache.end());
        for (; ncIt != ncEnd; ++ncIt) {
            Graph::set_next(ncIt->first, ncIt->second);
        }

        // adjust vertices' halfedge handle
        for (size_t i = 0; i < n; ++i) {
            if (h_needs_adjust[i]) {
                adjust_outgoing_halfedge(f_vertices[i]);
            }
        }

        assert(faces.has_dirty_properties());
        assert(halfedges.has_dirty_properties());
        return f;
    }

    FaceHandle Mesh::add_triangle(const VertexHandle &v0, const VertexHandle &v1, const VertexHandle &v2) {
        return add_face({v0, v1, v2});
    }

    FaceHandle
    Mesh::add_quad(const VertexHandle &v0, const VertexHandle &v1, const VertexHandle &v2, const VertexHandle &v3) {
        return add_face({v0, v1, v2, v3});
    }

    void Mesh::adjust_outgoing_halfedge(const VertexHandle &v) {
        HalfedgeHandle h = Graph::get_halfedge(v);
        const HalfedgeHandle hh = h;

        if (h.is_valid()) {
            do {
                if (is_boundary(h)) {
                    Graph::set_halfedge(v, h);
                    return;
                }
                h = Graph::rotate_cw(h);
            } while (h != hh);
        }
    }

    void Mesh::delete_face(const FaceHandle &f) {
        if (deleted_faces[f]) {
            return;
        }

        mark_face_deleted(f);

        // boundary edges of face f to be deleted
        std::vector<EdgeHandle> deletedEdges;
        deletedEdges.reserve(3);

        // vertices of face f for updating their outgoing halfedge
        std::vector<VertexHandle> verts;
        verts.reserve(3);

        // for all halfedges of face f do:
        //   1) invalidate face handle.
        //   2) collect all boundary halfedges, set them deleted
        //   3) store vertex handles
        HalfedgeAroundFaceCirculator hc, hcend;
        hc = hcend = get_halfedges(f);

        do {
            set_face(*hc, FaceHandle());

            if (is_boundary(Graph::get_opposite(*hc))) {
                deletedEdges.push_back(Graph::get_edge(*hc));
            }

            verts.push_back(Graph::get_to_vertex(*hc));

        } while (++hc != hcend);

        // delete all collected (half)edges
        // delete isolated vertices
        if (!deletedEdges.empty()) {
            auto delit(deletedEdges.begin()), delend(deletedEdges.end());

            HalfedgeHandle h0, h1, next0, next1, prev0, prev1;
            VertexHandle v0, v1;

            for (; delit != delend; ++delit) {
                h0 = Graph::get_halfedge(*delit, 0);
                v0 = Graph::get_to_vertex(h0);
                next0 = Graph::get_next(h0);
                prev0 = Graph::get_prev(h0);

                h1 = Graph::get_halfedge(*delit, 1);
                v1 = Graph::get_to_vertex(h1);
                next1 = Graph::get_next(h1);
                prev1 = Graph::get_prev(h1);

                // adjust next and prev handles
                Graph::set_next(prev0, next1);
                Graph::set_next(prev1, next0);

                // mark edge deleted
                mark_edge_deleted(*delit);

                // update v0
                if (Graph::get_halfedge(v0) == h1) {
                    if (next0 == h1) {
                        mark_vertex_deleted(v0);
                    } else {
                        Graph::set_halfedge(v0, next0);
                    }
                }

                // update v1
                if (Graph::get_halfedge(v1) == h0) {
                    if (next1 == h0) {
                        mark_vertex_deleted(v1);
                    } else {
                        Graph::set_halfedge(v1, next1);
                    }
                }
            }

        }

        // update outgoing halfedge handles of remaining vertices
        //auto vit(vertices.begin()), vend(vertices.end());
        auto vit(verts.begin()), vend(verts.end());
        for (; vit != vend; ++vit) {
            adjust_outgoing_halfedge(*vit);
        }

        assert(has_garbage());
    }

    void Mesh::delete_edge_merge_faces(const EdgeHandle &e) {
        if (!is_removal_ok(e)) return;

        HalfedgeHandle h0 = Graph::get_halfedge(e, 0);
        HalfedgeHandle h1 = Graph::get_halfedge(e, 1);

        VertexHandle v0 = Graph::get_to_vertex(h0);
        VertexHandle v1 = Graph::get_to_vertex(h1);

        FaceHandle f0 = get_face(h0);
        FaceHandle f1 = get_face(h1);

        HalfedgeHandle h0_prev = Graph::get_prev(h0);
        HalfedgeHandle h0_next = Graph::get_next(h0);
        HalfedgeHandle h1_prev = Graph::get_prev(h1);
        HalfedgeHandle h1_next = Graph::get_next(h1);

        // adjust vertex->halfedge
        if (Graph::get_halfedge(v0) == h1) {
            Graph::set_halfedge(v0, h0_next);
        }
        if (Graph::get_halfedge(v1) == h0) {
            Graph::set_halfedge(v1, h1_next);
        }

        // adjust halfedge->face
        for (const auto h: get_halfedges(f0)) {
            set_face(h, f1);
        }

        // adjust halfedge->halfedge
        Graph::set_next(h1_prev, h0_next);
        Graph::set_next(h0_prev, h1_next);

        // adjust face->halfedge
        if (get_halfedge(f1) == h1) {
            set_halfedge(f1, h1_next);
        }

        mark_face_deleted(f0);
        mark_edge_deleted(e);

        assert(has_garbage());
    }

    void Mesh::delete_edge(const EdgeHandle &e) {
        if (deleted_edges[e]) {
            return;
        }

        FaceHandle f0 = get_face(e, 0);
        FaceHandle f1 = get_face(e, 1);

        if (faces.is_valid(f0)) {
            delete_face(f0);
        }
        if (faces.is_valid(f1)) {
            delete_face(f1);
        }
        //mark_edge_deleted(e);
    }

    void Mesh::delete_vertex(const VertexHandle &v) {
        if (deleted_vertices[v]) {
            return;
        }

        std::vector<FaceHandle> incident_faces;
        incident_faces.reserve(6);

        for (const auto f: get_faces(v)) {
            incident_faces.push_back(f);
        }

        for (const auto &f: incident_faces) {
            delete_face(f);
        }

        mark_vertex_deleted(v);
        assert(has_garbage());
    }

    void Mesh::triangulate() {
        for (const auto f: faces) {
            if (get_valence(f) != 3) {
                triangulate(f);
            }
        }
    }

    void Mesh::triangulate(const FaceHandle &f) {
        HalfedgeHandle h = get_halfedge(f);
        VertexHandle v0 = Graph::get_from_vertex(h);
        HalfedgeHandle nh = Graph::get_next(h);

        while (Graph::get_to_vertex(Graph::get_next(nh)) != v0) {
            HalfedgeHandle nnh(Graph::get_next(nh));

            FaceHandle new_f = new_face();
            set_halfedge(new_f, h);

            HalfedgeHandle new_h = Graph::new_edge(Graph::get_to_vertex(nh), v0);

            Graph::set_next(h, nh);
            Graph::set_next(nh, new_h);
            Graph::set_next(new_h, h);

            set_face(h, new_f);
            set_face(nh, new_f);
            set_face(new_h, new_f);

            h = Graph::get_opposite(new_h);
            nh = nnh;
        }
        set_halfedge(f, h);  //the last face takes the handle _fh

        Graph::set_next(h, nh);
        Graph::set_next(Graph::get_next(nh), h);

        set_face(h, f);
    }

    void Mesh::collapse(const HalfedgeHandle &h0) {
        //move v0 to v1
        HalfedgeHandle h1 = Graph::get_prev(h0);
        HalfedgeHandle o0 = Graph::get_opposite(h0);
        HalfedgeHandle o1 = Graph::get_next(o0);

        // remove edge
        remove_edge_helper(h0);

        // remove loops
        if (Graph::get_next(Graph::get_next(h1)) == h1) {
            remove_loop_helper(h1);
        }
        if (Graph::get_next(Graph::get_next(o1)) == o1) {
            remove_loop_helper(o1);
        }

        assert(faces.has_dirty_properties());
        assert(edges.has_dirty_properties());
        assert(has_garbage());
    }

    void Mesh::remove_edge_helper(const HalfedgeHandle &h) {
        HalfedgeHandle hn = Graph::get_next(h);
        HalfedgeHandle hp = Graph::get_prev(h);

        HalfedgeHandle o = Graph::get_opposite(h);
        HalfedgeHandle on = Graph::get_next(o);
        HalfedgeHandle op = Graph::get_prev(o);

        FaceHandle fh = get_face(h);
        FaceHandle fo = get_face(o);

        VertexHandle vh = Graph::get_to_vertex(h);
        VertexHandle vo = Graph::get_to_vertex(o);

        // halfedge -> vertex
        HalfedgeAroundVertexCirculator vhit, vhend;
        vhit = vhend = Graph::get_halfedges(vo);
        do {
            set_vertex(get_opposite(*vhit), vh);
        } while (++vhit != vhend);

        // halfedge -> halfedge
        Graph::set_next(hp, hn);
        Graph::set_next(op, on);

        // face -> halfedge
        if (fh.is_valid()) {
            set_halfedge(fh, hn);
        }
        if (fo.is_valid()) {
            set_halfedge(fo, on);
        }

        // vertex -> halfedge
        if (Graph::get_halfedge(vh) == o) {
            Graph::set_halfedge(vh, hn);
        }
        adjust_outgoing_halfedge(vh);
        Graph::set_halfedge(vo, HalfedgeHandle());

        mark_vertex_deleted(vo);
        mark_edge_deleted(Graph::get_edge(h));

        assert(has_garbage());
    }

    void Mesh::remove_loop_helper(const HalfedgeHandle &h) {
        HalfedgeHandle h1 = Graph::get_next(h);

        HalfedgeHandle o0 = Graph::get_opposite(h);
        HalfedgeHandle o1 = Graph::get_opposite(h1);

        VertexHandle v0 = Graph::get_to_vertex(h);
        VertexHandle v1 = Graph::get_to_vertex(h1);

        FaceHandle fh = get_face(h);
        FaceHandle fo = get_face(o0);

        // is it a loop ?
        assert((Graph::get_next(h1) == h) && (h1 != o0));

        // halfedge -> halfedge
        Graph::set_next(h1, Graph::get_next(o0));
        Graph::set_next(Graph::get_prev(o0), h1);

        // halfedge -> face
        set_face(h1, fo);

        // vertex -> halfedge
        Graph::set_halfedge(v0, h1);
        adjust_outgoing_halfedge(v0);
        Graph::set_halfedge(v1, o1);
        adjust_outgoing_halfedge(v1);

        // face -> halfedge
        if (fo.is_valid() && get_halfedge(fo) == o0) {
            set_halfedge(fo, h1);
        }

        // delete stuff
        if (fh.is_valid()) {
            mark_face_deleted(fh);
        }

        mark_edge_deleted(Graph::get_edge(h));

        assert(has_garbage());
    }

    VertexHandle Mesh::split(const FaceHandle &f, const VertexHandle &v) {
        HalfedgeHandle hend = get_halfedge(f);
        HalfedgeHandle h = Graph::get_next(hend);

        HalfedgeHandle hold = Graph::new_edge(Graph::get_to_vertex(hend), v);

        Graph::set_next(hend, hold);
        set_face(hold, f);

        hold = Graph::get_opposite(hold);

        while (h != hend) {
            HalfedgeHandle hnext = Graph::get_next(h);

            FaceHandle fnew = new_face();
            set_halfedge(fnew, h);

            HalfedgeHandle hnew = Graph::new_edge(Graph::get_to_vertex(h), v);

            Graph::set_next(hnew, hold);
            Graph::set_next(hold, h);
            Graph::set_next(h, hnew);

            set_face(hnew, fnew);
            set_face(hold, fnew);
            set_face(h, fnew);

            hold = Graph::get_opposite(hnew);

            h = hnext;
        }

        Graph::set_next(hold, hend);
        Graph::set_next(Graph::get_next(hend), hold);

        set_face(hold, f);

        Graph::set_halfedge(v, hold);
        assert(faces.has_dirty_properties());
        assert(edges.has_dirty_properties());
        return v;
    }

    HalfedgeHandle Mesh::split(const EdgeHandle &e, const VertexHandle &v) {
        HalfedgeHandle h0 = Graph::get_halfedge(e, 0);
        HalfedgeHandle o0 = Graph::get_halfedge(e, 1);

        VertexHandle v2 = Graph::get_to_vertex(o0);

        HalfedgeHandle e1 = Graph::new_edge(v, v2);
        HalfedgeHandle t1 = Graph::get_opposite(e1);

        FaceHandle f0 = get_face(h0);
        FaceHandle f3 = get_face(o0);

        Graph::set_halfedge(v, h0);
        Graph::set_vertex(o0, v);

        if (!is_boundary(h0)) {
            HalfedgeHandle h1 = Graph::get_next(h0);
            HalfedgeHandle h2 = Graph::get_next(h1);

            VertexHandle v1 = Graph::get_to_vertex(h1);

            HalfedgeHandle e0 = Graph::new_edge(v, v1);
            HalfedgeHandle t0 = Graph::get_opposite(e0);

            FaceHandle f1 = new_face();
            set_halfedge(f0, h0);
            set_halfedge(f1, h2);

            set_face(h1, f0);
            set_face(t0, f0);
            set_face(h0, f0);

            set_face(h2, f1);
            set_face(t1, f1);
            set_face(e0, f1);

            Graph::set_next(h0, h1);
            Graph::set_next(h1, t0);
            Graph::set_next(t0, h0);

            Graph::set_next(e0, h2);
            Graph::set_next(h2, t1);
            Graph::set_next(t1, e0);
        } else {
            Graph::set_next(Graph::get_prev(h0), t1);
            Graph::set_next(t1, h0);
            // halfedge handle of vh already is h0
        }

        if (!is_boundary(o0)) {
            HalfedgeHandle o1 = Graph::get_next(o0);
            HalfedgeHandle o2 = Graph::get_next(o1);

            VertexHandle v3 = Graph::get_to_vertex(o1);

            HalfedgeHandle e2 = Graph::new_edge(v, v3);
            HalfedgeHandle t2 = Graph::get_opposite(e2);

            FaceHandle f2 = new_face();
            set_halfedge(f2, o1);
            set_halfedge(f3, o0);

            set_face(o1, f2);
            set_face(t2, f2);
            set_face(e1, f2);

            set_face(o2, f3);
            set_face(o0, f3);
            set_face(e2, f3);

            Graph::set_next(e1, o1);
            Graph::set_next(o1, t2);
            Graph::set_next(t2, e1);

            Graph::set_next(o0, e2);
            Graph::set_next(e2, o2);
            Graph::set_next(o2, o0);
        } else {
            Graph::set_next(e1, Graph::get_next(o0));
            Graph::set_next(o0, e1);
            Graph::set_halfedge(v, e1);
        }

        if (Graph::get_halfedge(v2) == h0) {
            Graph::set_halfedge(v2, t1);
        }

        return t1;
    }

    HalfedgeHandle Mesh::insert_vertex(const EdgeHandle &e, const VertexHandle &v) {
        return insert_vertex(Graph::get_halfedge(e, 0), v);
    }

    HalfedgeHandle Mesh::insert_vertex(const HalfedgeHandle &h, const VertexHandle &v) {
        // before:
        //
        // v0      h0       v2
        //  o--------------->o
        //   <---------------
        //         o0
        //
        // after:
        //
        // v0  h0   v   h1   v2
        //  o------>o------->o
        //   <------ <-------
        //     o0       o1

        HalfedgeHandle h2 = Graph::get_next(h);
        HalfedgeHandle o0 = Graph::get_opposite(h);
        HalfedgeHandle o2 = Graph::get_prev(o0);
        VertexHandle v2 = Graph::get_to_vertex(h);
        FaceHandle fh = get_face(h);
        FaceHandle fo = get_face(o0);

        HalfedgeHandle h1 = Graph::new_edge(v, v2);
        HalfedgeHandle o1 = Graph::get_opposite(h1);

        // adjust halfedge connectivity
        Graph::set_next(h1, h2);
        Graph::set_next(h, h1);
        Graph::set_vertex(h, v);
        Graph::set_vertex(h1, v2);
        set_face(h1, fh);

        Graph::set_next(o1, o0);
        Graph::set_next(o2, o1);
        Graph::set_vertex(o1, v);
        set_face(o1, fo);

        // adjust vertex connectivity
        Graph::set_halfedge(v2, o1);
        adjust_outgoing_halfedge(v2);
        Graph::set_halfedge(v, h1);
        adjust_outgoing_halfedge(v);

        // adjust face connectivity
        if (fh.is_valid()) {
            set_halfedge(fh, h);
        }
        if (fo.is_valid()) {
            set_halfedge(fo, o1);
        }

        f_connectivity.set_dirty();
        assert(vertices.has_dirty_properties());
        assert(edges.has_dirty_properties());
        assert(halfedges.has_dirty_properties());
        assert(faces.has_dirty_properties());
        return o1;
    }

    HalfedgeHandle Mesh::insert_edge(const HalfedgeHandle &h0, const HalfedgeHandle &h1) {
        assert(get_face(h0) == get_face(h1));
        assert(get_face(h0).is_valid());

        VertexHandle v0 = Graph::get_to_vertex(h0);
        VertexHandle v1 = Graph::get_to_vertex(h1);
        HalfedgeHandle h2 = Graph::get_next(h0);
        HalfedgeHandle h3 = Graph::get_next(h1);
        HalfedgeHandle h4 = Graph::new_edge(v0, v1);
        HalfedgeHandle h5 = Graph::get_opposite(h4);
        FaceHandle f0 = get_face(h0);
        FaceHandle f1 = new_face();

        set_halfedge(f0, h0);
        set_halfedge(f1, h1);

        Graph::set_next(h0, h4);
        Graph::set_next(h4, h3);
        set_face(h4, f0);

        Graph::set_next(h1, h5);
        Graph::set_next(h5, h2);
        HalfedgeHandle h = h2;
        do {
            set_face(h, f1);
            h = Graph::get_next(h);
        } while (h != h2);

        assert(edges.has_dirty_properties());
        assert(faces.has_dirty_properties());
        return h4;
    }

    void Mesh::flip(const EdgeHandle &e) {
        assert(is_flip_ok(e));

        HalfedgeHandle a0 = Graph::get_halfedge(e, 0);
        HalfedgeHandle b0 = Graph::get_halfedge(e, 1);

        HalfedgeHandle a1 = Graph::get_next(a0);
        HalfedgeHandle a2 = Graph::get_next(a1);

        HalfedgeHandle b1 = Graph::get_next(b0);
        HalfedgeHandle b2 = Graph::get_next(b1);

        VertexHandle va0 = Graph::get_to_vertex(a0);
        VertexHandle va1 = Graph::get_to_vertex(a1);

        VertexHandle vb0 = Graph::get_to_vertex(b0);
        VertexHandle vb1 = Graph::get_to_vertex(b1);

        FaceHandle fa = get_face(a0);
        FaceHandle fb = get_face(b0);

        Graph::set_vertex(a0, va1);
        Graph::set_vertex(b0, vb1);

        Graph::set_next(a0, a2);
        Graph::set_next(a2, b1);
        Graph::set_next(b1, a0);

        Graph::set_next(b0, b2);
        Graph::set_next(b2, a1);
        Graph::set_next(a1, b0);

        set_face(a1, fb);
        set_face(b1, fa);

        set_halfedge(fa, a0);
        set_halfedge(fb, b0);

        if (Graph::get_halfedge(va0) == b0) {
            Graph::set_halfedge(va0, a1);
        }
        if (Graph::get_halfedge(vb0) == a0) {
            Graph::set_halfedge(vb0, b1);
        }

        assert(faces.has_dirty_properties());
        assert(edges.has_dirty_properties());
    }

    void Mesh::mark_face_deleted(const FaceHandle &f) {
        if (deleted_faces[f]) return;

        deleted_faces[f] = true;
        ++m_num_deleted_faces;
        deleted_faces.set_dirty();
        assert(has_garbage());
    }

    Property<Eigen::Vector<unsigned int, 3>> Mesh::get_triangles(bool triangulate_) {
        if (triangulate_ && !is_triangle_mesh()) {
            triangulate();
        }
        return get_triangles();
    }

    Property<Eigen::Vector<unsigned int, 3>> Mesh::get_triangles() {
        auto triangles = faces.get_or_add<Eigen::Vector<unsigned int, 3 >>("triangles");

        for (const auto f: faces) {
            std::vector<unsigned int> indices;
            for (const auto &v: get_vertices(f)) {
                indices.push_back(v.idx);
            }
            assert(indices.size() == 3);
            triangles[f] = MapConst(indices);
        }
        triangles.set_dirty();
        return triangles;
    }

    Property<Eigen::Vector<unsigned int, 6>> Mesh::get_triangles_adjacency(bool triangulate_) {
        if (triangulate_ && !is_triangle_mesh()) {
            triangulate();
        }
        auto triangle_adjacency = faces.get_or_add<Eigen::Vector<unsigned int, 6 >>("triangle_adjacency");
        for (const auto &f: faces) {
            std::vector<unsigned int> adjacency;
            for (const auto &h: get_halfedges(f)) {
                adjacency.push_back((unsigned int) Graph::get_from_vertex(h).idx);
                if (!is_boundary(h)) {
                    HalfedgeHandle o = Graph::get_opposite(h);
                    HalfedgeHandle op = Graph::get_prev(o);
                    adjacency.push_back((unsigned int) Graph::get_from_vertex(op).idx);
                } else {
                    adjacency.push_back((unsigned int) Graph::get_from_vertex(h).idx);
                }
            }
            assert(adjacency.size() == 6);
            triangle_adjacency[f] = Map(adjacency);
        }
        triangle_adjacency.set_dirty();
        return triangle_adjacency;
    }

    //------------------------------------------------------------------------------------------------------------------

    Mesh::FaceAroundVertexCirculator::FaceAroundVertexCirculator(const Mesh *ds, const VertexHandle &v) :
            ds(ds), active(false) {
        if (ds) {
            halfedge = ds->Graph::get_halfedge(v);
            if (halfedge.is_valid() && ds->is_boundary(halfedge)) {
                operator++();
            }
        }
    }

    bool Mesh::FaceAroundVertexCirculator::operator==(const FaceAroundVertexCirculator &rhs) const {
        assert(ds);
        assert(ds == rhs.ds);
        return (active && (halfedge == rhs.halfedge));
    }

    bool Mesh::FaceAroundVertexCirculator::operator!=(const FaceAroundVertexCirculator &rhs) const {
        return !operator==(rhs);
    }

    Mesh::FaceAroundVertexCirculator &Mesh::FaceAroundVertexCirculator::operator++() {
        assert(ds && halfedge.is_valid());
        do {
            halfedge = ds->rotate_ccw(halfedge);
        } while (ds->is_boundary(halfedge));
        active = true;
        return *this;
    }

    Mesh::FaceAroundVertexCirculator &Mesh::FaceAroundVertexCirculator::operator--() {
        assert(ds && halfedge.is_valid());
        do {
            halfedge = ds->rotate_cw(halfedge);
        } while (ds->is_boundary(halfedge));
        return *this;
    }

    FaceHandle Mesh::FaceAroundVertexCirculator::operator*() const {
        assert(ds && halfedge.is_valid());
        return ds->get_face(halfedge);
    }

    Mesh::FaceAroundVertexCirculator::operator bool() const {
        return halfedge.is_valid();
    }

    Mesh::FaceAroundVertexCirculator &Mesh::FaceAroundVertexCirculator::begin() {
        active = !halfedge.is_valid();
        return *this;
    }

    Mesh::FaceAroundVertexCirculator &Mesh::FaceAroundVertexCirculator::end() {
        active = true;
        return *this;
    }

    Mesh::FaceAroundVertexCirculator Mesh::get_faces(const VertexHandle &v) const {
        return FaceAroundVertexCirculator(this, v);
    }

    Mesh::VertexAroundFaceCirculator::VertexAroundFaceCirculator(const Mesh *ds, const FaceHandle &f)
            : ds(ds), active(false) {

        if (ds) {
            halfedge = ds->get_halfedge(f);
        }
    }

    bool Mesh::VertexAroundFaceCirculator::operator==(const VertexAroundFaceCirculator &rhs) const {
        assert(ds);
        assert(ds == rhs.ds);
        return (active && (halfedge == rhs.halfedge));
    }

    bool Mesh::VertexAroundFaceCirculator::operator!=(const VertexAroundFaceCirculator &rhs) const {
        return !operator==(rhs);
    }

    Mesh::VertexAroundFaceCirculator &Mesh::VertexAroundFaceCirculator::operator++() {
        assert(ds && halfedge.is_valid());
        halfedge = ds->Graph::get_next(halfedge);
        active = true;
        return *this;
    }

    Mesh::VertexAroundFaceCirculator &Mesh::VertexAroundFaceCirculator::operator--() {
        assert(ds && halfedge.is_valid());
        halfedge = ds->Graph::get_prev(halfedge);
        return *this;
    }

    VertexHandle Mesh::VertexAroundFaceCirculator::operator*() const {
        assert(ds && halfedge.is_valid());
        return ds->get_to_vertex(halfedge);
    }

    Mesh::VertexAroundFaceCirculator &Mesh::VertexAroundFaceCirculator::begin() {
        active = !halfedge.is_valid();
        return *this;
    }

    Mesh::VertexAroundFaceCirculator &Mesh::VertexAroundFaceCirculator::end() {
        active = true;
        return *this;
    }

    Mesh::VertexAroundFaceCirculator Mesh::get_vertices(const FaceHandle &f) const {
        return VertexAroundFaceCirculator(this, f);
    }


    Mesh::HalfedgeAroundFaceCirculator::HalfedgeAroundFaceCirculator(const Mesh *ds, const FaceHandle &f) : ds(
            ds),
                                                                                                            active(false) {
        if (ds) {
            halfedge = ds->get_halfedge(f);
        }
    }

    bool Mesh::HalfedgeAroundFaceCirculator::operator==(const HalfedgeAroundFaceCirculator &rhs) const {
        assert(ds);
        assert(ds == rhs.ds);
        return (active && (halfedge == rhs.halfedge));
    }

    bool Mesh::HalfedgeAroundFaceCirculator::operator!=(const HalfedgeAroundFaceCirculator &rhs) const {
        return !operator==(rhs);
    }

    Mesh::HalfedgeAroundFaceCirculator &Mesh::HalfedgeAroundFaceCirculator::operator++() {
        assert(ds && halfedge.is_valid());
        halfedge = ds->get_next(halfedge);
        active = true;
        return *this;
    }

    Mesh::HalfedgeAroundFaceCirculator &Mesh::HalfedgeAroundFaceCirculator::operator--() {
        assert(ds && halfedge.is_valid());
        halfedge = ds->get_prev(halfedge);
        return *this;
    }

    HalfedgeHandle Mesh::HalfedgeAroundFaceCirculator::operator*() const {
        return halfedge;
    }

    Mesh::HalfedgeAroundFaceCirculator &Mesh::HalfedgeAroundFaceCirculator::begin() {
        active = !halfedge.is_valid();
        return *this;
    }

    Mesh::HalfedgeAroundFaceCirculator &Mesh::HalfedgeAroundFaceCirculator::end() {
        active = true;
        return *this;
    }

    Mesh::HalfedgeAroundFaceCirculator Mesh::get_halfedges(const FaceHandle &f) const {
        return HalfedgeAroundFaceCirculator(this, f);
    }

    //------------------------------------------------------------------------------------------------------------------

    std::ostream &operator<<(std::ostream &stream, const Mesh &mesh) {
        stream << "              MeshHalfedgeName: " << mesh.get_name() << "\n";

        stream << "       HasProperty(v_position): " << mesh.vertices.has("v_position") << "\n";
        stream << "   HasProperty(v_connectivity): " << mesh.vertices.has("v_connectivity") << "\n";
        stream << "   HasProperty(h_connectivity): " << mesh.halfedges.has("h_connectivity") << "\n";
        stream << "  HasProperty(hf_connectivity): " << mesh.halfedges.has("hf_connectivity") << "\n";
        stream << "   HasProperty(f_connectivity): " << mesh.faces.has("f_connectivity") << "\n";

        stream << "        HasProperty(v_deleted): " << mesh.vertices.has("v_deleted") << "\n";
        stream << "        HasProperty(h_deleted): " << mesh.halfedges.has("h_deleted") << "\n";
        stream << "        HasProperty(e_deleted): " << mesh.edges.has("e_deleted") << "\n";
        stream << "        HasProperty(f_deleted): " << mesh.faces.has("f_deleted") << "\n";

        stream << "                    has_garbage: " << mesh.has_garbage() << "\n";
        stream << " num_vertices (Total - Deleted): " << mesh.num_vertices() << "\n";
        stream << "num_halfedges (Total - Deleted): " << mesh.num_halfedges() << "\n";
        stream << "    num_edges (Total - Deleted): " << mesh.num_edges() << "\n";
        stream << "    num_faces (Total - Deleted): " << mesh.num_faces() << "\n";

        stream << "            num_deleted_vertices: " << mesh.num_deleted_vertices() << "\n\n";
        stream << "           num_deleted_halfedges: " << mesh.num_deleted_halfedges() << "\n\n";
        stream << "               num_deleted_edges: " << mesh.num_deleted_edges() << "\n\n";
        stream << "               num_deleted_faces: " << mesh.num_deleted_faces() << "\n\n";

        stream << mesh.vertices << "\n\n";

        stream << mesh.halfedges << "\n\n";

        stream << mesh.edges << "\n\n";

        stream << mesh.faces << "\n\n";
        return stream;
    }
}
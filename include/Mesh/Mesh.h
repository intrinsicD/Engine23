//
// Created by alex on 20.02.24.
//

#ifndef ENGINE23_MESH_H
#define ENGINE23_MESH_H

#include "Graph.h"
#include "FaceContainer.h"

namespace Bcg {
    class Mesh : public Graph {
    public:
        struct FaceConnectivity {
            HalfedgeHandle h;

            friend std::ostream &operator<<(std::ostream &stream, const FaceConnectivity &value) {
                stream << "h: " << value.h.idx;
                return stream;
            }
        };

        FaceContainer faces;
        Property<FaceHandle> hf_connectivity;
        Property<FaceConnectivity> f_connectivity;
        Property<bool> deleted_faces;

        Mesh();

        Mesh(const VertexContainer &vertices,
             const HalfedgeContainer &halfedges,
             const EdgeContainer &edges,
             const FaceContainer &faces,
             std::string name = "MeshHalfedge");

        Mesh &assign(const Mesh &other);

        Mesh &operator=(const Mesh &other);

        operator bool() const override;

        void clear() override;

        size_t num_faces() const;

        size_t num_deleted_faces() const;

        bool is_valid(const FaceHandle &f) const;

        bool has_garbage() const override;

        void garbage_collection() override;

        bool is_boundary(const VertexHandle &v) const override;

        bool is_boundary(const HalfedgeHandle &h) const override;

        bool is_boundary(const EdgeHandle &e) const override;

        bool is_boundary(const FaceHandle &f) const;

        bool is_manifold(const VertexHandle &v) const;

        bool is_manifold() const;

        bool is_triangle_mesh() const;

        bool is_quad_mesh() const;

        bool is_removal_ok(const EdgeHandle &e) const;

        bool is_collapse_ok(const HalfedgeHandle &h) const;

        bool is_flip_ok(const EdgeHandle &e) const;

        FaceHandle get_face(const EdgeHandle &e, bool i) const;

        FaceHandle get_face(const HalfedgeHandle &h) const;

        void set_face(const HalfedgeHandle &h, const FaceHandle &f);

        HalfedgeHandle get_halfedge(const FaceHandle &f) const;

        void set_halfedge(const FaceHandle &f, const HalfedgeHandle &h);

        size_t get_valence(const FaceHandle &f) const;

        FaceHandle add_face(const std::vector<VertexHandle> &f_vertices);

        FaceHandle add_triangle(const VertexHandle &v0, const VertexHandle &v1, const VertexHandle &v2);

        FaceHandle
        add_quad(const VertexHandle &v0, const VertexHandle &v1, const VertexHandle &v2, const VertexHandle &v3);

        FaceHandle new_face();

        void adjust_outgoing_halfedge(const VertexHandle &v);

        void delete_face(const FaceHandle &f);

        void delete_edge_merge_faces(const EdgeHandle &e);

        void delete_edge(const EdgeHandle &e) override;

        void delete_vertex(const VertexHandle &v) override;

        void triangulate();

        void triangulate(const FaceHandle &f);

        void collapse(const HalfedgeHandle &h);

        void remove_edge_helper(const HalfedgeHandle &h);

        void remove_loop_helper(const HalfedgeHandle &h);

        VertexHandle split(const FaceHandle &f, const VertexHandle &v);

        HalfedgeHandle split(const EdgeHandle &e, const VertexHandle &v);

        HalfedgeHandle insert_vertex(const EdgeHandle &e, const VertexHandle &v);

        HalfedgeHandle insert_vertex(const HalfedgeHandle &h, const VertexHandle &v);

        HalfedgeHandle insert_edge(const HalfedgeHandle &h0, const HalfedgeHandle &h1);

        void flip(const EdgeHandle &e);

        void mark_face_deleted(const FaceHandle &f);

        Property<Eigen::Vector<unsigned int, 3>> get_triangles(bool triangulate);

        Property<Eigen::Vector<unsigned int, 3>> get_triangles();

        Property<Eigen::Vector<unsigned int, 6>> get_triangles_adjacency(bool triangulate = false);

        //--------------------------------------------------------------------------------------------------------------

        template<typename T, int N>
        VertexHandle
        split(const FaceHandle &f, const Eigen::Vector<T, N> &point, Property<Eigen::Vector<T, N>> &positions) {
            return split(f, add_vertex(point, positions));
        }

        template<typename T, int N>
        HalfedgeHandle
        split(const EdgeHandle &e, const Eigen::Vector<T, N> &point, Property<Eigen::Vector<T, N>> &positions) {
            return split(e, add_vertex(point, positions));
        }

        template<typename T, int N>
        HalfedgeHandle insert_vertex(const EdgeHandle &e, const Eigen::Vector<T, N> &point, Property<Eigen::Vector<T, N>> &positions) {
            return insert_vertex(e, add_vertex(point, positions));
        }

        //--------------------------------------------------------------------------------------------------------------

        struct FaceAroundVertexCirculator {
            const Mesh *ds;
            HalfedgeHandle halfedge;
            bool active;

            explicit FaceAroundVertexCirculator(const Mesh *ds = nullptr, const VertexHandle &v = VertexHandle());

            bool operator==(const FaceAroundVertexCirculator &rhs) const;

            bool operator!=(const FaceAroundVertexCirculator &rhs) const;

            FaceAroundVertexCirculator &operator++();

            FaceAroundVertexCirculator &operator--();

            FaceHandle operator*() const;

            operator bool() const;

            FaceAroundVertexCirculator &begin();

            FaceAroundVertexCirculator &end();
        };

        FaceAroundVertexCirculator get_faces(const VertexHandle &v) const;

        struct VertexAroundFaceCirculator {
            const Mesh *ds;
            HalfedgeHandle halfedge;
            bool active;

            explicit VertexAroundFaceCirculator(const Mesh *ds = nullptr, const FaceHandle &f = FaceHandle());

            bool operator==(const VertexAroundFaceCirculator &rhs) const;

            bool operator!=(const VertexAroundFaceCirculator &rhs) const;

            VertexAroundFaceCirculator &operator++();

            VertexAroundFaceCirculator &operator--();

            VertexHandle operator*() const;

            VertexAroundFaceCirculator &begin();

            VertexAroundFaceCirculator &end();
        };

        VertexAroundFaceCirculator get_vertices(const FaceHandle &f) const;

        struct HalfedgeAroundFaceCirculator {
            const Mesh *ds;
            HalfedgeHandle halfedge;
            bool active;

            explicit HalfedgeAroundFaceCirculator(const Mesh *ds = nullptr, const FaceHandle &f = FaceHandle());

            bool operator==(const HalfedgeAroundFaceCirculator &rhs) const;

            bool operator!=(const HalfedgeAroundFaceCirculator &rhs) const;

            HalfedgeAroundFaceCirculator &operator++();

            HalfedgeAroundFaceCirculator &operator--();

            HalfedgeHandle operator*() const;

            HalfedgeAroundFaceCirculator &begin();

            HalfedgeAroundFaceCirculator &end();
        };

        HalfedgeAroundFaceCirculator get_halfedges(const FaceHandle &f) const;

        friend std::ostream &operator<<(std::ostream &stream, const Mesh &mesh);

    protected:

        size_t m_num_deleted_faces = 0;
        std::vector<HalfedgeHandle> m_add_face_halfedges;
        std::vector<bool> m_add_face_is_new;
        std::vector<bool> m_add_face_needs_adjust;
        using NextCacheEntry = std::pair<HalfedgeHandle, HalfedgeHandle>;
        using NextCache = std::vector<NextCacheEntry>;
        NextCache m_add_face_next_cache;
    };
}
#endif //ENGINE23_MESH_H

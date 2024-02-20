//
// Created by alex on 20.02.24.
//

#ifndef ENGINE23_GRAPH_H
#define ENGINE23_GRAPH_H

#include "PointCloud.h"
#include "HalfedgeContainer.h"
#include "EdgeContainer.h"

namespace Bcg {
    class Graph : public PointCloud {
    public:
        struct VertexConnectivity {
            HalfedgeHandle h;

            friend std::ostream &operator<<(std::ostream &stream, const VertexConnectivity &value) {
                stream << value.h.idx;
                return stream;
            }
        };

        struct HalfedgeConnectivity {
            VertexHandle v;
            HalfedgeHandle nh;
            HalfedgeHandle ph;

            friend std::ostream &operator<<(std::ostream &stream, const HalfedgeConnectivity &value) {
                stream << value.v.idx << " " << value.nh.idx << " " << value.ph.idx;
                return stream;
            }
        };

        HalfedgeContainer halfedges;
        EdgeContainer edges;
        Property<VertexConnectivity> v_connectivity;
        Property<HalfedgeConnectivity> h_connectivity;
        Property<bool> deleted_halfedges;
        Property<bool> deleted_edges;

        Graph();

        ~Graph() override = default;

        explicit Graph(const VertexContainer &vertices,
                       const HalfedgeContainer &halfedges,
                       const EdgeContainer &edges,
                       std::string name = "GraphHalfedge");

        Graph &copy(const Graph &other);

        Graph &operator=(const Graph &other);

        operator bool() const override;

        void clear() override;

        [[nodiscard]] HalfedgeHandle get_halfedge(const VertexHandle &v) const;

        void set_halfedge(const VertexHandle &v, const HalfedgeHandle &h);

        [[nodiscard]] VertexHandle get_to_vertex(const HalfedgeHandle &h) const;

        void set_vertex(const HalfedgeHandle &h, const VertexHandle &v);

        [[nodiscard]] VertexHandle get_from_vertex(const HalfedgeHandle &h) const;

        [[nodiscard]] HalfedgeHandle get_next(const HalfedgeHandle &h) const;

        void set_next(const HalfedgeHandle &h, const HalfedgeHandle &nh);

        [[nodiscard]] HalfedgeHandle get_prev(const HalfedgeHandle &h) const;

        [[nodiscard]] HalfedgeHandle get_opposite(const HalfedgeHandle &h) const;

        [[nodiscard]] HalfedgeHandle rotate_cw(const HalfedgeHandle &h) const;

        [[nodiscard]] HalfedgeHandle rotate_ccw(const HalfedgeHandle &h) const;

        [[nodiscard]] EdgeHandle get_edge(const HalfedgeHandle &h) const;

        [[nodiscard]] HalfedgeHandle get_halfedge(const EdgeHandle &e, bool i) const;

        [[nodiscard]] VertexHandle get_vertex(const EdgeHandle &e, bool i) const;

        [[nodiscard]] bool is_isolated(const VertexHandle &v) const;

        [[nodiscard]] virtual bool is_boundary(const VertexHandle &v) const;

        [[nodiscard]] virtual bool is_boundary(const HalfedgeHandle &h) const;

        [[nodiscard]] virtual bool is_boundary(const EdgeHandle &e) const;

        [[nodiscard]] bool has_garbage() const override;

        void garbage_collection() override;

        void delete_vertex(const VertexHandle &v) override;

        HalfedgeHandle add_edge(const VertexHandle &v0, const VertexHandle &v1);

        [[nodiscard]] HalfedgeHandle find_halfedge(const VertexHandle &v0, const VertexHandle &v1) const;

        [[nodiscard]] EdgeHandle find_edge(const VertexHandle &v0, const VertexHandle &v1) const;

        virtual void delete_edge(const EdgeHandle &e);

        void mark_edge_deleted(const EdgeHandle &e);

        [[nodiscard]] size_t num_halfedges() const;

        [[nodiscard]] size_t num_deleted_halfedges() const;

        [[nodiscard]] size_t num_edges() const;

        [[nodiscard]] size_t num_deleted_edges() const;

        size_t get_valence(const VertexHandle &v) const;

        //--------------------------------------------------------------------------------------------------------------

        template<typename T, int N>
        double get_length(const EdgeHandle &e, const Property<Eigen::Vector<T, N>> &positions) const {
            return (positions[get_vertex(e, 1)] - positions[get_vertex(e, 0)]).norm();
        }

        template<typename T, int N>
        double get_squared_length(const EdgeHandle &e, const Property<Eigen::Vector<T, N>> &positions) const {
            return (positions[get_vertex(e, 1)] - positions[get_vertex(e, 0)]).squaredNorm();
        }

        //--------------------------------------------------------------------------------------------------------------

        struct VertexAroundVertexCirculator {
            const Graph *ds;
            HalfedgeHandle halfedge;
            bool active;

            explicit VertexAroundVertexCirculator(const Graph *ds = nullptr, const VertexHandle &v = VertexHandle());

            bool operator==(const VertexAroundVertexCirculator &rhs) const;

            bool operator!=(const VertexAroundVertexCirculator &rhs) const;

            VertexAroundVertexCirculator &operator++();

            VertexAroundVertexCirculator &operator--();

            VertexHandle operator*() const;

            operator bool() const;

            VertexAroundVertexCirculator &begin();

            VertexAroundVertexCirculator &end();
        };

        VertexAroundVertexCirculator get_vertices(const VertexHandle &v) const;

        //--------------------------------------------------------------------------------------------------------------

        struct HalfedgeAroundVertexCirculator {
            const Graph *ds;
            HalfedgeHandle halfedge;
            bool active;

            explicit HalfedgeAroundVertexCirculator(const Graph *ds = nullptr, const VertexHandle &v = VertexHandle());

            bool operator==(const HalfedgeAroundVertexCirculator &rhs) const;

            bool operator!=(const HalfedgeAroundVertexCirculator &rhs) const;

            HalfedgeAroundVertexCirculator &operator++();

            HalfedgeAroundVertexCirculator &operator--();

            HalfedgeHandle operator*() const;

            operator bool() const;

            HalfedgeAroundVertexCirculator &begin();

            HalfedgeAroundVertexCirculator &end();
        };

        HalfedgeAroundVertexCirculator get_halfedges(const VertexHandle &v) const;

        friend std::ostream &operator<<(std::ostream &stream, const Graph &graph);

    protected:

        HalfedgeHandle new_edge(const VertexHandle &v0, const VertexHandle &v1);

        size_t m_num_deleted_halfedges = 0;
        size_t m_num_deleted_edges = 0;
    };
}

#endif //ENGINE23_GRAPH_H

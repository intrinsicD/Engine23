//
// Created by alex on 26.08.21.
//

#ifndef BCG_FRAMEWORK_BCG_POINT_CLOUD_H
#define BCG_FRAMEWORK_BCG_POINT_CLOUD_H

#include "Eigen/Core"
#include "VertexContainer.h"
#include "EngineFwd.h"

namespace Bcg {

    class PointCloud {
    public:
        PropertyContainer object_properties;
        VertexContainer vertices;
        Property<bool> deleted_vertices;

        PointCloud();

        virtual ~PointCloud() = default;

        explicit PointCloud(const VertexContainer &vertices, std::string name = "PointCloud");

        PointCloud &copy(const PointCloud &other);

        PointCloud &operator=(const PointCloud &other);

        [[nodiscard]] const std::string &get_name() const;

        void set_name(const std::string &name);

        virtual operator bool() const;

        virtual void clear();

        [[nodiscard]] virtual bool has_garbage() const;

        virtual void garbage_collection();

        virtual void delete_vertex(const VertexHandle &v);

        void mark_vertex_deleted(const VertexHandle &v);

        [[nodiscard]] size_t num_vertices() const;

        [[nodiscard]] size_t num_deleted_vertices() const;

        VertexHandle new_vertex();

        template<class VectorType>
        VertexHandle add_vertex(const VectorType &point, Property<VectorType> &positions){
            auto v = new_vertex();
            positions[v] = point;
            return v;
        }

        friend std::ostream &operator<<(std::ostream &stream, const PointCloud &pc);

    protected:
        std::string m_name;
        size_t m_num_deleted_vertices = 0;
    };

}

#endif //BCG_FRAMEWORK_BCG_POINT_CLOUD_H

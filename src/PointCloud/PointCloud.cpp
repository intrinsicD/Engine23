//
// Created by alex on 20.02.24.
//
#include "PointCloud.h"

namespace Bcg {

    PointCloud::PointCloud() : PointCloud(VertexContainer(), "PointCloud") {}

    PointCloud::PointCloud(const VertexContainer &vertices_, std::string name) :
            vertices(vertices_),
            m_name(std::move(name)),
            m_num_deleted_vertices(0) {
        deleted_vertices = vertices.get<bool>("v_deleted");
        assert(deleted_vertices);
    }

    PointCloud &PointCloud::copy(const Bcg::PointCloud &other) {
        //copy positions and deleted_vertices (pointers are not the same, but values are)
        //all other properties are not copied
        if (this != &other) {
            vertices = VertexContainer();

            vertices.resize(other.vertices.get_size());

            deleted_vertices.storage() = other.deleted_vertices.storage();

            // how many elements are deleted?
            m_num_deleted_vertices = other.m_num_deleted_vertices;

            assert(deleted_vertices);
        }
        return *this;
    }

    PointCloud &PointCloud::operator=(const PointCloud &other) {
        //all properties are linked (pointers are the same)
        if (this != &other) {
            vertices = other.vertices;
            m_name = other.m_name;

            deleted_vertices = vertices.get_or_add<bool>("v_deleted");

            // how many elements are deleted?
            m_num_deleted_vertices = other.m_num_deleted_vertices;

            assert(deleted_vertices);
        }
        return *this;
    }

    const std::string &PointCloud::get_name() const { return m_name; }

    void PointCloud::set_name(const std::string &name) { m_name = name; }

    PointCloud::operator bool() const { return deleted_vertices; }

    void PointCloud::clear() {
        vertices.clear();
        vertices.free_unused_memory();
        deleted_vertices = vertices.get_or_add<bool>("v_deleted", false);

        m_num_deleted_vertices = 0;
        assert(deleted_vertices);
    }

    bool PointCloud::has_garbage() const { return m_num_deleted_vertices > 0; }

    void PointCloud::garbage_collection() {
        if (!has_garbage()) return;

        size_t nV = vertices.get_size();
        if (nV > 0) {
            VertexHandle i0 = 0;
            VertexHandle i1 = nV - 1;

            while (true) {
                // find first deleted_vertices and last un-deleted
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
        }

        vertices.resize(num_vertices());
        vertices.free_unused_memory();
        m_num_deleted_vertices = 0;
        assert(!has_garbage());
    }

    VertexHandle PointCloud::new_vertex() {
        vertices.push_back();
        return vertices.get_size() - 1;
    }


    void PointCloud::delete_vertex(const VertexHandle &v) {
        mark_vertex_deleted(v);
    }

    void PointCloud::mark_vertex_deleted(const VertexHandle &v) {
        if (deleted_vertices[v]) return;
        deleted_vertices[v] = true;
        ++m_num_deleted_vertices;
    }

    size_t PointCloud::num_vertices() const {
        return vertices.get_size() - m_num_deleted_vertices;
    }

    size_t PointCloud::num_deleted_vertices() const {
        return m_num_deleted_vertices;
    }

    std::ostream &operator<<(std::ostream &stream, const PointCloud &pc) {
        stream << "                PointCloudName: " << pc.get_name() << "\n";
        stream << "       HasProperty(v_position): " << pc.vertices.has("v_position") << "\n";
        stream << "        HasProperty(v_deleted): " << pc.vertices.has("v_deleted") << "\n";
        stream << "                   has_garbage: " << pc.has_garbage() << "\n";
        stream << "num_vertices (Total - Deleted): " << pc.num_vertices() << "\n";
        stream << "          num_deleted_vertices: " << pc.num_deleted_vertices() << "\n\n";
        stream << pc.vertices << "\n";
        return stream;
    }
}
//
// Created by alex on 23.05.24.
//

#ifndef ENGINE23_KDTREEADAPTERS_H
#define ENGINE23_KDTREEADAPTERS_H

#include "nanoflann.hpp"
#include "Eigen/Core"
#include "Property.h"
#include <vector>

namespace Bcg {
    template<typename T>
    struct is_eigen_matrix : std::false_type {};

    template<typename T, int M, int D>
    struct is_eigen_matrix<Eigen::Matrix<T, M, D>> : std::true_type {};

    template<typename T>
    struct is_std_vector : std::false_type {};

    template<typename T>
    struct is_std_vector<std::vector<T>> : std::true_type {};

    template<typename T>
    struct is_property : std::false_type {};

    template<typename T>
    struct is_property<Property<T>> : std::true_type {};

    template<typename Container, typename T, int D, class Enable = void>
    struct DatasetAdaptor;

    // Specialization for Eigen::Matrix
    template<typename T, int M, int D>
    struct DatasetAdaptor<Eigen::Matrix<T, M, D>, T, D, typename std::enable_if<is_eigen_matrix<Eigen::Matrix<T, M, D>>::value>::type> {
        using ContainerType = Eigen::Matrix<T, M, D>;

        explicit DatasetAdaptor(const ContainerType &positions) : positions(positions) {}

        const ContainerType &positions;

        inline size_t kdtree_get_point_count() const {
            return positions.rows();
        }

        inline T kdtree_get_pt(const size_t idx, const size_t dim) const {
            return positions(idx, dim);
        }

        template<class BBOX>
        bool kdtree_get_bbox(BBOX &) const {
            return false;
        }

        int dims() const { return positions.cols(); }
    };

    // Specialization for Property
    template<typename T, int D>
    struct DatasetAdaptor<Property<Eigen::Vector<T, D>>, T, D, typename std::enable_if<is_property<Property<Eigen::Vector<T, D>>>::value>::type> {
        using ContainerType = Property<Eigen::Vector<T, D>>;

        explicit DatasetAdaptor(const ContainerType &positions) : positions(positions) {}

        const ContainerType &positions;

        inline size_t kdtree_get_point_count() const {
            return positions.size();
        }

        inline T kdtree_get_pt(const size_t idx, const size_t dim) const {
            return positions[idx][dim];
        }

        template<class BBOX>
        bool kdtree_get_bbox(BBOX &) const {
            return false;
        }

        int dims() const { return D; }
    };

    // Specialization for std::vector
    template<typename T, int D>
    struct DatasetAdaptor<std::vector<Eigen::Vector<T, D>>, T, D, typename std::enable_if<is_std_vector<std::vector<Eigen::Vector<T, D>>>::value>::type> {
        using ContainerType = std::vector<Eigen::Vector<T, D>>;

        explicit DatasetAdaptor(const ContainerType &positions) : positions(positions) {}

        const ContainerType &positions;

        inline size_t kdtree_get_point_count() const {
            return positions.size();
        }

        inline T kdtree_get_pt(const size_t idx, const size_t dim) const {
            return positions[idx][dim];
        }

        template<class BBOX>
        bool kdtree_get_bbox(BBOX &) const {
            return false;
        }

        int dims() const { return D; }
    };

    template<typename Container, typename T, int D = 3, class Distance = nanoflann::metric_L2>
    class KDTree {
    public:
        using ElementType = Eigen::Vector<T, D>;
        using DatasetAdaptorType = DatasetAdaptor<Container, T, D>;
        using DistanceType = typename Distance::template traits<T, DatasetAdaptorType>::distance_t;
        using KDTreeType = nanoflann::KDTreeSingleIndexAdaptor<DistanceType, DatasetAdaptorType, D, unsigned int>;
        using ParamsType = nanoflann::KDTreeSingleIndexAdaptorParams;

        std::shared_ptr<KDTreeType> index;
        std::shared_ptr<DatasetAdaptorType> dataset;
        int leaf_size = 10;

        KDTree() = default;

        explicit KDTree(const Container &positions, int leaf_max_size = 10)
                : dataset(std::make_shared<DatasetAdaptorType>(positions)), leaf_size(leaf_max_size) {
            index = std::make_shared<KDTreeType>(dataset->dims(), *dataset, ParamsType(leaf_max_size));
            index->buildIndex();
        }

        // k-NN search
        void knnSearch(const ElementType &query, const size_t num_results,
                       std::vector<size_t> &out_indices, std::vector<T> &out_distances) const {
            out_indices.resize(num_results);
            out_distances.resize(num_results);

            nanoflann::KNNResultSet<T> resultSet(num_results);
            resultSet.init(&out_indices[0], &out_distances[0]);
            index->findNeighbors(resultSet, query.data());
        }

        // Unsorted radius search
        void radiusSearchUnsorted(const ElementType &query, const T radius,
                                  std::vector<std::pair<size_t, T>> &out_results) const {
            nanoflann::SearchParameters params(10);
            std::vector<std::pair<size_t, T>> results;

            nanoflann::RadiusResultSet<T, size_t> resultSet(radius, results);
            index->findNeighbors(resultSet, query.data(), params);
            out_results = results;
        }

        // Sorted radius search
        void radiusSearchSorted(const ElementType &query, const T radius,
                                std::vector<std::pair<size_t, T>> &out_results) const {
            nanoflann::SearchParameters params(10);
            params.sorted = true;
            std::vector<std::pair<size_t, T>> results;

            nanoflann::RadiusResultSet<T, size_t> resultSet(radius, results);
            index->findNeighbors(resultSet, query.data(), params);
            out_results = results;
        }
    };
}

#endif //ENGINE23_KDTREEADAPTERS_H

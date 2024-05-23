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

    template<typename T, int M = -1, int D = 3, class Distance = nanoflann::metric_L2>
    class KDtreeEigen {
        class DatasetAdaptor {
        public:
            DatasetAdaptor() = default;

            explicit DatasetAdaptor(const Eigen::Matrix<T, M, D> &positions) : positions(positions) {}

            const Eigen::Matrix<T, M, D> &positions;

            inline size_t kdtree_get_point_count() const {
                return positions.rows();
            }

            inline T kdtree_get_pt(const size_t idx, const size_t dim) const {
                return positions.row(idx)[dim];
            }

            template<class BBOX>
            bool kdtree_get_bbox(BBOX &) const {
                return false;
            }

            int dims() const { return positions.cols(); }
        };

    public:

        //TODO THis needs some fixing, make sure that for Dynmaic matrices this works!
        using metric_t = Distance::template traits<T, DatasetAdaptor>::distance_t;
        using index_t = nanoflann::KDTreeSingleIndexAdaptor<metric_t, DatasetAdaptor, D, unsigned int>;
        using params_t = nanoflann::KDTreeSingleIndexAdaptorParams;

        std::shared_ptr<index_t> index;
        std::shared_ptr<DatasetAdaptor> dataset;
        int leaf_size = 10;

        KDtreeEigen() = default;

        explicit KDtreeEigen(const Eigen::Matrix<T, M, D> &positions, int leaf_max_size = 10) : dataset(
                std::make_shared<DatasetAdaptor>(positions)), leaf_size(leaf_max_size) {
            index = std::make_shared<index_t>(dataset->dims(), *dataset, params_t(leaf_max_size));
            index->buildIndex();
        }

        void build(const Eigen::Matrix<T, M, D> &positions, int leaf_max_size = 10) {
            dataset = std::make_shared<DatasetAdaptor>(positions);
            index = std::make_shared<index_t>(dataset->dims(), *dataset, params_t(leaf_max_size));
            index->buildIndex();
        }
    };

    template<typename T, int M = -1, int D = 3, class Distance = nanoflann::metric_L2>
    struct KDTreeProperty {
        struct DatasetAdaptor {
            DatasetAdaptor() = default;

            explicit DatasetAdaptor(Property<T> positions) : positions(positions) {}

            Property<T> positions;

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

            int dims() const { return positions.get_dims(); }
        };

        using metric_t = Distance::template traits<T, DatasetAdaptor>::distance_t;
        using index_t = nanoflann::KDTreeSingleIndexAdaptor<metric_t, DatasetAdaptor, D, unsigned int>;
        using params_t = nanoflann::KDTreeSingleIndexAdaptorParams;

        std::shared_ptr<index_t> index;
        std::shared_ptr<DatasetAdaptor> dataset;
        int leaf_size = 10;

        KDTreeProperty() = default;

        explicit KDTreeProperty(Property<T> positions, int leaf_max_size = 10) : dataset(
                std::make_shared<DatasetAdaptor>(positions)), leaf_size(leaf_max_size) {
            index = std::make_shared<index_t>(dataset->dims(), *dataset, params_t(leaf_max_size));
            index->buildIndex();
        }

        void build(const Property<T> &positions, int leaf_max_size = 10) {
            dataset = std::make_shared<DatasetAdaptor>(positions);
            index = std::make_shared<index_t>(dataset->dims(), *dataset, params_t(leaf_max_size));
            index->buildIndex();
        }
    };

    template<typename T, int M = -1, int D = 3, class Distance = nanoflann::metric_L2>
    struct KDTreeVector {
        struct DatasetAdaptor {
            DatasetAdaptor() = default;

            explicit DatasetAdaptor(const std::vector<Eigen::Vector<T, D>> &positions) : positions(positions) {

            }

            const std::vector<Eigen::Vector<T, D>> &positions;

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

            int dims() const { return positions[0].size(); }
        };

        using metric_t = Distance::template traits<T, DatasetAdaptor>::distance_t;
        using index_t = nanoflann::KDTreeSingleIndexAdaptor<metric_t, DatasetAdaptor, D, unsigned int>;
        using params_t = nanoflann::KDTreeSingleIndexAdaptorParams;

        std::shared_ptr<index_t> index;
        std::shared_ptr<DatasetAdaptor> dataset;
        int leaf_size = 10;

        KDTreeVector() = default;

        explicit KDTreeVector(const std::vector<Eigen::Vector<T, D>> &positions, int leaf_max_size = 10) : dataset(
                std::make_shared<DatasetAdaptor>(positions)), leaf_size(leaf_max_size) {
            index = std::make_shared<index_t>(static_cast<int>(D), *dataset, params_t(leaf_max_size));
            index->buildIndex();
        }

        void build(const std::vector<Eigen::Vector<T, D>> &positions, int leaf_max_size = 10) {
            dataset = std::make_shared<DatasetAdaptor>(positions);
            index = std::make_shared<index_t>(static_cast<int>(D), *dataset, params_t(leaf_max_size));
            index->buildIndex();
        }
    };

    template<typename DatasetAdaptor, typename T, int M = -1, int D = 3, class Distance = nanoflann::metric_L2>
    struct KDTree {
        using metric_t = Distance::template traits<T, DatasetAdaptor>::distance_t;
        using index_t = nanoflann::KDTreeSingleIndexAdaptor<metric_t, DatasetAdaptor, D, unsigned int>;
        using params_t = nanoflann::KDTreeSingleIndexAdaptorParams;

        std::unique_ptr<DatasetAdaptor> dataset;
        std::unique_ptr<index_t> index;
        int leaf_size = 10;

        KDTree() = default;

        explicit KDTree(const std::vector<Eigen::Vector<T, D>> &positions, int leaf_max_size = 10) :
                dataset(std::make_unique<DatasetAdaptor>(positions)),
                index(std::make_unique<index_t>(dataset.dims(), *dataset, params_t(leaf_max_size))),
                leaf_size(leaf_max_size) {
            index->buildIndex();
        }
    };

    //TODO figure this out!
}

#endif //ENGINE23_KDTREEADAPTERS_H

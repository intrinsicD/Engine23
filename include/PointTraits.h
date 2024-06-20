//
// Created by alex on 20.06.24.
//

#ifndef ENGINE23_POINTTRAITS_H
#define ENGINE23_POINTTRAITS_H

namespace Bcg {
    /**
     * Some traits to access coordinates regardless of the specific implementation of point
     * inspired by boost.geometry, which needs to be implemented by new points.
     *
     */

    namespace traits {

        template<typename PointT>
        struct scalar_type;

        template<typename PointT>
        struct scalar_type<const PointT> : scalar_type<PointT> {};  // handle const PointT

        template<typename PointT>
        struct scalar_type<PointT&> : scalar_type<PointT> {};  // handle PointT&

        template<typename PointT, int D>
        struct access {
        };

        template<class PointT>
        struct access<PointT, 0> {
            static auto get(const PointT &p) {
                return p.x;
            }
        };

        template<class PointT>
        struct access<PointT, 1> {
            static auto get(const PointT &p) {
                return p.y;
            }
        };

        template<class PointT>
        struct access<PointT, 2> {
            static auto get(const PointT &p) {
                return p.z;
            }
        };
    }

/** convenience function for access of point coordinates **/
    template<int D, typename PointT>
    inline auto get(const PointT &p) {
        return traits::access<PointT, D>::get(p);
    }
}

#endif //ENGINE23_POINTTRAITS_H

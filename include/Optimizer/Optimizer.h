//
// Created by alex on 05.03.24.
//

#ifndef ENGINE23_OPTIMIZER_H
#define ENGINE23_OPTIMIZER_H

#include <cmath>

namespace Bcg {
    struct MaximizePolicy {
        template<class ParamsType, class GradType>
        void step(ParamsType &params, const GradType &adjusted_grads) {
            params += adjusted_grads;
        }
    };

    struct MinimizePolicy {
        template<class ParamsType, class GradType>
        void step(ParamsType &params, const GradType &adjusted_grads) {
            params -= adjusted_grads;
        }
    };

    template<class OptimizationPolicy, class GradAdjustPolicy>
    class OptimizerBase {
    public:
        OptimizerBase() = default;

        virtual ~OptimizerBase() = default;

        template<class GradType>
        GradType adjust(const GradType &grads){
            return m_grad_adjust_policy.adjust(grads);
        }

        template<class ParamsType, class GradType>
        void step(ParamsType &params, const GradType &adjusted_grads) {
            m_opt_policy.step(params, adjusted_grads);
        }

        OptimizationPolicy m_opt_policy;
        GradAdjustPolicy m_grad_adjust_policy;
    };
}

#endif //ENGINE23_OPTIMIZER_H

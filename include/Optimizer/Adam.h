//
// Created by alex on 05.03.24.
//

#ifndef ENGINE23_ADAM_H
#define ENGINE23_ADAM_H

#include "Optimizer.h"

namespace Bcg {
    template<class GradType>
    struct AdamGradAdjustPolicy {
        double learning_rate;
        double beta1, beta2;
        double epsilon;
        double t;
        GradType m, v;
        GradType m_hat, v_hat;

        GradType adjust(const GradType &grads) {
            t++;
            m = beta1 * m + (1 - beta1) * grads;
            v = beta2 * v + (1 - beta2) * grads * grads;
            m_hat = m / (1 - std::pow(beta1, t));
            v_hat = v / (1 - std::pow(beta2, t));
            return learning_rate * m_hat / (std::sqrt(v_hat) + epsilon);
        }
    };


    template<class OptimizationPolicy, class GradType>
    class AdamOptimizer : public OptimizerBase<OptimizationPolicy, AdamGradAdjustPolicy<GradType>> {
    public:
        AdamOptimizer() = default;

        ~AdamOptimizer() override = default;
    };
}
#endif //ENGINE23_ADAM_H

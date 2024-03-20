//
// Created by alex on 20.03.24.
//

#include "RenderPass.h"

namespace Bcg{
    bool RenderPass::add(entt::entity entity_id) {
        auto iter = m_entities.find(entity_id);
        if (iter != m_entities.end()) return false;
        m_entities.emplace(entity_id);
        return true;
    }

    bool RenderPass::remove(entt::entity entity_id) {
        auto iter = m_entities.find(entity_id);
        if (iter == m_entities.end()) return false;
        m_entities.erase(iter);
        return true;
    }

    RenderPass::Iterator_t RenderPass::begin() { return m_entities.begin(); }

    RenderPass::Iterator_t RenderPass::end() { return m_entities.end(); }

    RenderPass::ConstIterator_t RenderPass::begin() const { return m_entities.begin(); }

    RenderPass::ConstIterator_t RenderPass::end() const { return m_entities.end(); }
}
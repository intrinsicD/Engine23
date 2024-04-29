//
// Created by alex on 29.04.24.
//

#ifndef ENGINE23_RESOURCECONTAINER_H
#define ENGINE23_RESOURCECONTAINER_H

#include "PropertyContainer.h"

namespace Bcg{
    template<typename T>
    class ResourceContainer : public PropertyContainer{
    public:
        ResourceContainer() = default;

        explicit ResourceContainer(std::string name) : PropertyContainer(name) {
            pool = add<T>(name);
        }

        Property<T> pool;
    };
}

#endif //ENGINE23_RESOURCECONTAINER_H

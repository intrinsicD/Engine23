//
// Created by alex on 29.04.24.
//

#ifndef ENGINE23_RESOURCECONTAINER_H
#define ENGINE23_RESOURCECONTAINER_H

#include "PropertyContainer.h"
#include <set>

namespace Bcg{
    template<typename T>
    class ResourceContainer : public PropertyContainer{
    public:
        ResourceContainer() : PropertyContainer("Data") {
            pool = add<T>("Data");
        }

        Property<T> pool;
        std::vector<unsigned int> free_list;
        std::set<unsigned int> used_list;
    };
}

#endif //ENGINE23_RESOURCECONTAINER_H

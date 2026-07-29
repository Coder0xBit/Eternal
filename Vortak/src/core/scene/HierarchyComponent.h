#pragma once
#include "utils/Base.h"

#include <entt/entt.hpp>

namespace Vortak {
    struct HierarchyComponent {
        entt::entity parent = entt::null;

        entt::entity firstChild = entt::null;
        entt::entity nextSibling = entt::null;
        entt::entity previousSibling = entt::null;

        uint32_t childCount = 0;
    };
}

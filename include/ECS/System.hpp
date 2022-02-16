//
// Created by Sam on 2/7/2022.
//

#ifndef ELYS_ECS_SYSTEM_HPP
#define ELYS_ECS_SYSTEM_HPP

#include <set>

#include <Utils.hpp>


namespace Core {
class System {
  public:
    std::set<EntityID> mEntities;
};
}

#endif // ELYS_ECS_SYSTEM_HPP

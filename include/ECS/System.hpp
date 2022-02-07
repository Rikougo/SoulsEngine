//
// Created by Sam on 2/7/2022.
//

#ifndef ELYS_SYSTEM_HPP
#define ELYS_SYSTEM_HPP

#include <set>

#include <Utils.hpp>


namespace Core {
class System {
  public:
    std::set<Entity> mEntities;
};
}

#endif // ELYS_SYSTEM_HPP

//
// Created by sakeiru on 5/8/2022.
//

#include <ECS/System.hpp>

namespace Elys {
    void System::AddEntity(EntityID newEntity) { mEntities.insert(newEntity); }
    void System::RemoveEntity(EntityID removedEntity) { mEntities.erase(removedEntity); }
}
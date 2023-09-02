#pragma once

#include "ManagedObject.h"

class BhvAssassin;
class Entity;

class AITask : public ManagedObject
{
public:
    BhvAssassin* bhvAssassin; //0x0010
    Entity* player; //0x0018
}; //Size: 0x0020
assert_sizeof(AITask, 0x20);

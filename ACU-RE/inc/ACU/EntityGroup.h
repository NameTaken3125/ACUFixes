#pragma once

#include "Entity.h"
#include "SharedPtr.h"

class EntityGroup : public Entity
{
public:
	char pad_0170[64]; //0x0170
	SharedPtrNew<Entity>* entities; //0x01B0
	char pad_01B8[40]; //0x01B8
}; //Size: 0x01E0
assert_sizeof(EntityGroup, 0x1E0);

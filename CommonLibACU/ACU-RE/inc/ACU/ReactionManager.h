#pragma once

#include "ManagedObject.h"
#include "ACUHashmap.h"
#include "Enum_ReactionHash_ReactToWhat.h"

class AISerializedManager : public ManagedObject
{
public:
}; //Size: 0x0010
assert_sizeof(AISerializedManager, 0x10);

class ReactionRadiusData;
class ReactionManager : public AISerializedManager
{
public:
    // @members
	char pad_0010[332]; //0x0010
	float defaultReactionRadius; //0x015C
	char pad_0160[792]; //0x0160
	ACUHashmap<ReactionHash_ReactToWhat, ReactionRadiusData*> hashmapReactionRadiusData; //0x0478
	char pad_0480[184]; //0x0480

    // @helper_functions
    static ReactionManager* GetSingleton() { return *(ReactionManager**)0x1451B5220; }
}; //Size: 0x0538
assert_sizeof(ReactionManager, 0x538);

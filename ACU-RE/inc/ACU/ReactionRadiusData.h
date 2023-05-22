#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"
#include "Enum_ReactionHash_ReactToWhat.h"


class AbstractReactionData : public Object
{
public:
    SmallArray<ReactionHash_ReactToWhat> arrCorrespondingHashes;
    char pad_0014[4]; //0x0014
}; //Size: 0x0018
assert_sizeof(AbstractReactionData, 0x18);

class ReactionRadiusData : public AbstractReactionData
{
public:
    float flt_18; //0x0018
    float flt_1C; //0x001C
    float flt_20; //0x0020
    float flt_24; //0x0024
    float flt_28; //0x0028
    float flt_2C; //0x002C
    char pad_0030[8]; //0x0030
}; //Size: 0x0038
assert_sizeof(ReactionRadiusData, 0x38);

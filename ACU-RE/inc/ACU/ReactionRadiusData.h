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

struct ReactionRadiusData_floats
{
    float flt_0; //0x0000
    float flt_4; //0x0004
    float flt_8; //0x0008
    float flt_C; //0x000C
    float flt_10; //0x0010
    float flt_14; //0x0014
};
class ReactionRadiusData : public AbstractReactionData
{
public:
    ReactionRadiusData_floats flts;
    char pad_0030[8]; //0x0030
}; //Size: 0x0038
assert_sizeof(ReactionRadiusData, 0x38);

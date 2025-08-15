#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"
#include "DangerEvent.h"

class AbstractReactionData : public Object
{
public:
    SmallArray<DangerEvent::SubType> SubTypes;
    char pad_0014[4]; //0x0014
}; //Size: 0x0018
assert_sizeof(AbstractReactionData, 0x18);

struct ReactionRadiusData_floats
{
    float RepulsionZoneRadius; //0x0018
    float AttractionZoneRadius; //0x001C
    float ReactionZoneRadius; //0x0020
    float WitnessRadius; //0x0024 radiusUsedForUnsheatheBeforeGuard
    float ZCutoffUp; //0x0028
    float ZCutoffDown; //0x002C
};
class ReactionRadiusData : public AbstractReactionData
{
public:
    ReactionRadiusData_floats flts;
    uint8 EnforceRepulsionZone : 1; //0x0030
    uint8 bits_30_1_8 : 8 - 1;
    char pad_0031[7]; //0x0031
}; //Size: 0x0038
assert_sizeof(ReactionRadiusData, 0x38);

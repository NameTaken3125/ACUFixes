#pragma once

#include "ManagedObject.h"

class ReactionParams : public Object
{
public:
    char pad_0008[320]; //0x0008
}; //Size: 0x0148
assert_sizeof(ReactionParams, 0x148);

class PushStateChartParams : public ReactionParams
{
public:
}; //Size: 0x0148

class PushExclusiveReactionParams : public PushStateChartParams
{
public:
    char pad_0148[24]; //0x0148
}; //Size: 0x0160

class AlertedParams : public PushExclusiveReactionParams
{
public:
    char pad_0160[20]; //0x0160
    uint32 dword_174; //0x0174
    char pad_0178[112]; //0x0178
    uint32 dword_1E8; //0x01E8
    char pad_01EC[100]; //0x01EC
}; //Size: 0x0250
assert_sizeof(AlertedParams, 0x250);

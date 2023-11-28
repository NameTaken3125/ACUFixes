#pragma once

#include "CameraSelectorNode.h"

class BlenderParam;
class CameraSelectorBlenderNode : public CameraSelectorNode
{
public:
    SmallArray<BlenderParam*> blenderParams; //0x0050
    char pad_005C[4]; //0x005C
}; //Size: 0x0060
assert_sizeof(CameraSelectorBlenderNode, 0x60);

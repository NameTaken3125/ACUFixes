#pragma once

#include "basic_types.h"

enum class MissionReplicationStyle : uint32
{
	MissionReplicationStyle_Inherit = 0,
	MissionReplicationStyle_NoReplication = 2,
};
class MissionReplicationPack : public Object
{
public:
	MissionReplicationStyle ReplicationStyle; //0x0008
	char pad_000C[4]; //0x000C
}; //Size: 0x0010
assert_sizeof(MissionReplicationPack, 0x10);

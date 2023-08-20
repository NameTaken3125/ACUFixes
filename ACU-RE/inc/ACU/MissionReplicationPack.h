#pragma once

class MissionReplicationPack : public Object
{
public:
	char pad_0008[8]; //0x0008
}; //Size: 0x0010
assert_sizeof(MissionReplicationPack, 0x10);

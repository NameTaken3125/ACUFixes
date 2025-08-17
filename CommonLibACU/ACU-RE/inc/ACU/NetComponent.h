#pragma once

#include "Component.h"
#include "SmallArray.h"

class NetService;

class NetComponent : public Component
{
public:
	char pad_0020[536]; //0x0020
	SmallArray<NetService*> NetServices; //0x0238
	char pad_0244[52]; //0x0244
}; //Size: 0x0278
assert_sizeof(NetComponent, 0x278);

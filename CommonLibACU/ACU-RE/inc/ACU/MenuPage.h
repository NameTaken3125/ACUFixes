#pragma once

#include <array>
#include "ManagedObject.h"
#include "FancyVTable.h"

class MenuPage : public ManagedObject
{
public:
	std::array<FancyVFunction, 31>* fancyVTable; //0x0010
	char pad_0018[52]; //0x0018
	uint32 dword_4C; //0x004C
	char pad_0050[24]; //0x0050
}; //Size: 0x0068
assert_sizeof(MenuPage, 0x68);

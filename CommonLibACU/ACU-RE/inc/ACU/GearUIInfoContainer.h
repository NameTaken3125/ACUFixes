#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"

class GearPage;
class GearUIInfo;

class GearUIInfoContainer : public ManagedObject
{
public:
	char pad_0010[16]; //0x0010
	GearPage* gearPage; //0x0020
	char pad_0028[13]; //0x0028
	SmallArray<GearUIInfo*> gearUIInfos; //0x0035
	char pad_0041[7]; //0x0041
	class GearDataBindableObjectArray* gearDataBindableObjectArray; //0x0048
	uint32 dword_50; //0x0050
	char pad_0054[4]; //0x0054
}; //Size: 0x0058
assert_sizeof(GearUIInfoContainer, 0x58);

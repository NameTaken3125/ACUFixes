#pragma once

#include "AtomDataContainerWrapper.h"

class AtomRTCPDescriptor
{
public:
	char* RTCPName; //0x0000
	char* EditorTypeName; //0x0008
	uint32 ID; //0x0010
	uint32 DataOffset; //0x0014
	char pad_0018[8]; //0x0018
	AtomDataContainerWrapper InitialData; //0x0020
}; //Size: 0x0040
assert_sizeof(AtomRTCPDescriptor, 0x40);

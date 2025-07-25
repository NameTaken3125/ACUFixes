#pragma once

#include "AtomPoseParamExtension.h"

class AtomIkPathExtension : public AtomPoseParamExtension
{
public:
	uint32 TargetTrackID; //0x0010
	uint32 TargetTrackIDMirrored; //0x0014
	uint32 InstanceStateOffset; //0x0018
	uint32 IKPathParamsPtrOffset; //0x001C
	uint32 BackUpTrackIDMirroredOffset; //0x0020
	uint32 BackUpBlendIKPositionOffset; //0x0024
	uint8 BlendIKPosition; //0x0028
	char pad_0029[7]; //0x0029
}; //Size: 0x0030
assert_sizeof(AtomIkPathExtension, 0x30);

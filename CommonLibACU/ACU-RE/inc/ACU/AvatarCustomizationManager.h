#pragma once

#include "ManagedObject.h"
#include "SharedPtr.h"
#include "Skeleton.h"

class Entity;
class Animation;
class AvatarCustomizationLightTransform;
class UIEventReaction;


class AvatarCustomizationLightTransform
{
public:
	Matrix4f Transform; //0x0000
	SharedPtrNew<Entity>* Light; //0x0040
	char pad_0048[8]; //0x0048
}; //Size: 0x0050
assert_sizeof(AvatarCustomizationLightTransform, 0x50);

class AvatarCustomizationManager : public ManagedObject
{
public:
	// @members

	char pad_0010[12]; //0x0010
	SmallArray<UIEventReaction*> WeaponSpawnedActions; //0x001C
	SmallArray<UIEventReaction*> WeaponUnspawnedActions; //0x0028
	SmallArray<UIEventReaction*> CharacterEquippedActions; //0x0034
	SmallArray<UIEventReaction*> CharacterIdleActions; //0x0040
	char pad_004C[4]; //0x004C
	SharedPtrNew<Entity>* LightingScript; //0x0050
	SmallArray<AvatarCustomizationLightTransform*> LightInfos; //0x0058
	Skeleton::BipedBoneID WeaponAttachmentBone; //0x0064
	Skeleton::BipedBoneID GunAttachmentBone; //0x0068
	char pad_006C[4]; //0x006C
	SharedPtrNew<Animation>* DefaultAnimation; //0x0070
	SharedPtrNew<Animation>* WaistAnimation; //0x0078
	SharedPtrNew<Animation>* ChestAnimation; //0x0080
	SharedPtrNew<Animation>* ForearmsAnimation; //0x0088
	SharedPtrNew<Animation>* HeadAnimation; //0x0090
	SharedPtrNew<Animation>* LegsAnimation; //0x0098
	SharedPtrNew<Animation>* ColorsAnimation; //0x00A0
	SharedPtrNew<Animation>* SuitsAnimation; //0x00A8
	SharedPtrNew<Animation>* WeaponOneHandedAnimation; //0x00B0
	SharedPtrNew<Animation>* WeaponHeavyHandedAnimation; //0x00B8
	SharedPtrNew<Animation>* WeaponLongAnimation; //0x00C0
	SharedPtrNew<Animation>* WeaponRangeAnimation; //0x00C8
	SharedPtrNew<Animation>* WeaponRangePistolAnimation; //0x00D0
	SharedPtrNew<Animation>* WeaponNonLethalAnimation; //0x00D8
	SharedPtrNew<Animation>* ActiveSkillAnimation; //0x00E0
	SharedPtrNew<Animation>* ActiveBoosterAnimation; //0x00E8
	float AvatarZPosition; //0x00F0
	char pad_00F4[12]; //0x00F4


	// @helper_functions
	static AvatarCustomizationManager* GetSingleton() { return *(AvatarCustomizationManager**)0x1451EACA8; }
}; //Size: 0x0100
assert_sizeof(AvatarCustomizationManager, 0x100);

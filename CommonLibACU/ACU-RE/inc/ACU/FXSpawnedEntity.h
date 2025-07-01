#pragma once

#include "FXElement.h"
#include "SharedPtr.h"

class Entity;

enum class SkeletonCopyCatType : uint32
{
	SCCT_LKP_Ghost = 2,
};

class FXSpawnedEntity : public FXElement
{
public:
	Matrix4f OffsetMatrix; //0x0020
	SharedPtrNew<Entity>* SourceEntity; //0x0060 // GFX_LKP_CopyCatEntity when spawing player ghost.
	uint32 TargetBoneID;
	SkeletonCopyCatType CopyCatType;
	byte AddToHierarchy : 1;
	byte UsePool : 1;
	byte TargetCopyCat : 1;
	byte InheritVisibility : 1;

	char pad_71[0x90 - 0x71];

}; //Size: 0x0090
assert_sizeof(FXSpawnedEntity, 0x90);

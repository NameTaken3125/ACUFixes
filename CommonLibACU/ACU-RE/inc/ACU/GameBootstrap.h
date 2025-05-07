#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"
#include "SharedPtr.h"

class LoadInfo;
class World;

class GameBootstrap;
class HasBootstrapSharedPtr
{
public:
	// @members
	SharedPtrNew<GameBootstrap>* shared_bootstrap;

	// @helper_functions
	static HasBootstrapSharedPtr* GetSingleton() { return *(HasBootstrapSharedPtr**)0x145221998; }
};
assert_sizeof(HasBootstrapSharedPtr, 8);

class GameBootstrap : public ManagedObject
{
public:
	// @members
	SharedPtrNew<World>* InitialWorld; //0x0010
	SmallArray<SharedPtrNew<ManagedObject>*> GlobalObjects; //0x0018
	SmallArray<LoadInfo> LoadInfo_; //0x0024
	SmallArray<LoadInfo> MissionInfo; //0x0030
	char pad_003C[28]; //0x003C
	uint64 handle_MissionRoot; //0x0058

	// @helper_functions
	static GameBootstrap* GetSingleton() {
		auto* hasBootstrap = HasBootstrapSharedPtr::GetSingleton();
		if (!hasBootstrap) return nullptr;
		return hasBootstrap->shared_bootstrap->GetPtr();
	}
}; //Size: 0x0060
assert_sizeof(GameBootstrap, 0x60);
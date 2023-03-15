#pragma once

#include "basic_types.h"

class Entity;

class SharedPtr_mb
{
public:
	Entity* entity_mb; //0x0000
	uint32 refCount_mb; //0x0008
	uint32 dword_C; //0x000C
	char pad_0010[8]; //0x0010

	Entity* GetPtr() { return (Entity*)((uint64)entity_mb & ((int64)dword_C << 32 >> 63)); } // see at 0x14082E776
}; //Size: 0x0018
assert_sizeof(SharedPtr_mb, 0x18);


template<class ManagedObjectSubcls>
class SharedPtrNew
{
public:
    ManagedObjectSubcls* manObj; //0x0000
    uint32 refCount; //0x0008
    uint32 dword_C; //0x000C
    uint64 handle; //0x0010

    ManagedObjectSubcls* GetPtr() { return (ManagedObjectSubcls*)((uint64)manObj & ((int64)dword_C << 32 >> 63)); } // see at 0x14082E776
};

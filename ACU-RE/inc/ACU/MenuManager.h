#pragma once

#include "MandatoryUniverseComponent.h"
#include "EventListener.h"
#include "SharedPtr.h"
#include "SmallArray.h"

class World;
class MenuManager;
class MenuPage;
class EntityGroup;

class MenuManager_30_10
{
public:
	char pad_0000[8]; //0x0000
	SharedPtrNew<MenuPage>* shared_MenuPage; //0x0008
	SharedPtrNew<EntityGroup>* shared_EntityGroup; //0x0010
	char pad_0018[8]; //0x0018
	World* world; //0x0020
	char pad_0028[32]; //0x0028
}; //Size: 0x0048

class MenuManager_30
{
public:
	char pad_0000[16]; //0x0000
	SmallArray<MenuManager_30_10*> arrHasMenuPages; //0x0010
	char pad_001C[4]; //0x001C
	World* world; //0x0020
	MenuManager* menuManager; //0x0028
	char pad_0030[200]; //0x0030
	EventListener evnListener; //0x00F8
	char pad_0120[64]; //0x0120
}; //Size: 0x0160
assert_sizeof(MenuManager_30, 0x160);


class MenuManager : public UniverseComponent
{
public:
	// @members
	char pad_0010[32]; //0x0010
	MenuManager_30* menuPagesStack_mb; //0x0030
	char pad_0038[72]; //0x0038

	// @helper_functions
	static MenuManager* GetSingleton() { return *(MenuManager**)0x1451EACE8; }
}; //Size: 0x0080
assert_sizeof(MenuManager, 0x80);

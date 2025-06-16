#pragma once

#include <array>
#include "ManagedObject.h"
#include "SharedPtr.h"
#include "UIString.h"
#include "FancyVTable.h"

class PageDescriptionComponent;
class Entity;

class MenuPage : public ManagedObject
{
public:
	std::array<FancyVFunction, 31>* fancyVTable; //0x0010
	UIString m_TitleTxt; //0x0018
	uint32 Level; //0x001C
	SharedPtrNew<Entity>* MenuBackButton; //0x0020
	SharedPtrNew<Entity>* BreadCrumb; //0x0028
	char pad_0030[16]; //0x0030
	PageDescriptionComponent* pageDescriptionCpnt; //0x0040
	char pad_0048[4]; //0x0048
	uint32 dword_4C; //0x004C
	char pad_0050[24]; //0x0050
}; //Size: 0x0068
assert_sizeof(MenuPage, 0x68);

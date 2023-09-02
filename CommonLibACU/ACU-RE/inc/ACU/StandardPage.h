#pragma once

#include "MenuPage.h"

class ScrollingListComponent;

class StandardPage : public MenuPage
{
public:
	char pad_0068[24]; //0x0068
	ScrollingListComponent* scrollListCpnt_mb; //0x0080
}; //Size: 0x0088
assert_sizeof(StandardPage, 0x88);

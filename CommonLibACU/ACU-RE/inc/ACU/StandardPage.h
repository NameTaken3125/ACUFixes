#pragma once

#include "MenuPage.h"

class ScrollingListComponent;

class StandardPage : public MenuPage
{
public:
	SharedPtrNew<Entity>* m_ScrollingList; //0x0068
	UIString m_MissingDesignTxt; //0x0070
	UIString m_MissingMockupTxt; //0x0074
	UIString m_MissingDesignDescTxt; //0x0078
	UIString m_MissingMockupDescTxt; //0x007C
	ScrollingListComponent* scrollListCpnt_mb; //0x0080
}; //Size: 0x0088
assert_sizeof(StandardPage, 0x88);

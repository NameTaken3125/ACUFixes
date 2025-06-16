#pragma once

#include "ManagedObject.h"
#include "SmallArray.h"
#include "ButtonComponent.h"
#include "SharedPtr.h"

class PanelPageHelper
{
public:
    char pad_0000[56]; //0x0000
    uint8 isPickedASkillWaitingToConfirm; //0x0038
    char pad_0039[59]; //0x0039
    SmallArray<ButtonComponent_230> arrCallbacks74; //0x0074
    char pad_0080[36]; //0x0080
    SmallArray<ButtonComponent_230> arrCallbacksA4; //0x00A4
    char pad_00B0[96]; //0x00B0
}; //Size: 0x0110
assert_sizeof(PanelPageHelper, 0x110);


class ScrollableWidgetComponent : public WidgetComponent
{
public:
}; //Size: 0x0210
assert_sizeof(ScrollableWidgetComponent, 0x210);

class UITweener : public ManagedObject
{
public:
    char pad_0010[24]; //0x0010
}; //Size: 0x0028
assert_sizeof(UITweener, 0x28);

class UIEventReaction;
class EntityGroup;
class ScrollingListDataItem;

class ScrollingListComponent : public ScrollableWidgetComponent
{
public:
    char pad_0210[104]; //0x0210
    SharedPtrNew<EntityGroup>* shared_entityGroupWithLayoutCpnt; //0x0278
    char pad_0280[88]; //0x0280
    UITweener uiTweener; //0x02D8
    SmallArray<UIEventReaction*> arrUIEventReactions; //0x0300
    char pad_030C[160]; //0x030C
    SmallArray<ScrollingListDataItem*> arrDataItems; //0x03AC
    char pad_03B8[88]; //0x03B8
}; //Size: 0x0410
assert_offsetof(ScrollingListComponent, uiTweener, 0x2D8);
assert_offsetof(ScrollingListComponent, arrUIEventReactions, 0x300);
assert_offsetof(ScrollingListComponent, arrDataItems, 0x3AC);
assert_sizeof(ScrollingListComponent, 0x410);

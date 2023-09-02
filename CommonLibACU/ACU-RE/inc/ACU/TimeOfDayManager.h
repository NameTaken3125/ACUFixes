#pragma once

#include "MandatoryWorldComponent.h"
#include "SmallArray.h"
#include "EventListener.h"
#include "SharedPtr.h"


class TimeOfDayPropertyController;
class WeatherPropertyController;
class WindPropertyController;

class Season;
class WeatherControls;
class WorldAmbianceManagerProxy;

class WorldAmbianceManager : public MandatoryWorldComponent
{
public:
	TimeOfDayPropertyController* timeOfDayPropertyController; //0x0020
	WeatherPropertyController* weatherPropertyController28; //0x0028
	WindPropertyController* windPropertyController30; //0x0030
	EventListener evnListener; //0x0038
	SharedPtrNew<Season>* shared_currentSeason; //0x0060
	char pad_0068[16]; //0x0068
	SmallArray<SharedPtrNew<Season>*> arrSharedSeasons; //0x0078
	char pad_0084[16]; //0x0084
	uint8 byte94; //0x0094
	char pad_0095[3]; //0x0095
	WeatherControls* weatherControls; //0x0098
	uint8 byte_A0; //0x00A0
	char pad_00A1[3]; //0x00A1
	uint32 dword_A4; //0x00A4
	char pad_00A8[64]; //0x00A8
	WorldAmbianceManagerProxy* worldAmbianceManagerProxy; //0x00E8
	uint32 seasonIDtoRestore; //0x00F0
	char pad_00F4[12]; //0x00F4
}; //Size: 0x0100
assert_sizeof(WorldAmbianceManager, 0x100);

class WeatherManager : public WorldAmbianceManager
{
public:
}; //Size: 0x0100
assert_sizeof(WeatherManager, 0x100);

class TimeOfDayManager : public WeatherManager
{
public:
	char pad_0100[112]; //0x0100

	// @helper_functions
	static TimeOfDayManager* GetSingleton();
}; //Size: 0x0170
assert_sizeof(TimeOfDayManager, 0x170);

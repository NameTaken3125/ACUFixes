#pragma once

// "Season" is actually used for the Time Of Day.

#include "ManagedObject.h"
#include "SmallArray.h"
#include "SharedPtr.h"
#include "SoundState.h"




class PropertyControllerData;
class WorldAmbiance;

class BasePropertyController : public Object
{
public:
	class BasePropertyController_8* arr8; //0x0008
	char pad_0010[2]; //0x0010
	uint16 arr8size; //0x0012
	char pad_0014[4]; //0x0014
	PropertyControllerData* parentPropControllerData_mb; //0x0018
	char pad_0020[48]; //0x0020
	WorldAmbiance* worldAmbiance; //0x0050
	char pad_0058[8]; //0x0058
	void* arr60; //0x0060
	char pad_0068[2]; //0x0068
	uint16 arr60size; //0x006A
	char pad_006C[12]; //0x006C
	void* arr78; //0x0078
	char pad_0080[2]; //0x0080
	uint16 arr78size; //0x0082
	char pad_0084[4]; //0x0084
}; //Size: 0x0088
assert_sizeof(BasePropertyController, 0x88);

class TimeOfDayPropertyController : public BasePropertyController
{
public:
	class TimeOfDayPropertyControllerData** shared_timeOfDayPropertyControllerData; //0x0088
}; //Size: 0x0090
assert_sizeof(TimeOfDayPropertyController, 0x90);

class WeatherPropertyController : public BasePropertyController
{
public:
	class WeatherPropertyControllerData** shared_weatherPropCtrlData; //0x0088
}; //Size: 0x0090
assert_sizeof(WeatherPropertyController, 0x90);

class WindPropertyController : public BasePropertyController
{
public:
	char pad_0088[8]; //0x0088
}; //Size: 0x0090
assert_sizeof(WindPropertyController, 0x90);



// This is actually an index in the `TimeOfDayManager.arrLookupSeasonHandleBySeasonID_84`.
enum class SeasonID : unsigned int
{
	Paris_Morning = 4,
	Paris_Daytime = 5,
	Paris_Evening = 6,
	Paris_Night = 7,

	Franciade_0xC_orangeishDay = 0xC,
	Franciade_0x13_greyishNight = 0x13,

	//BelleEpoque_Evening = 6,
	//OccupiedParis_Night = 7,
};

class DataLayer;
class World;
class TimeOfDayManager;

class Season : public ManagedObject
{
public:
	SmallArray<SharedPtrNew<DataLayer>*> arrSharedDataLayers; //0x0010
	SeasonID seasonID_7night4morn; //0x001C
	SoundState soundState; //0x0020
	SharedPtrNew<WorldAmbiance>* shared_worldAmbiance; //0x0038
	TimeOfDayPropertyController todPropController; //0x0040
	WeatherPropertyController weatherPropController; //0x00D0
	WindPropertyController windPropController; //0x0160
	char pad_01F0[24]; //0x01F0
	World* world; //0x0208
	char pad_0210[32]; //0x0210
	TimeOfDayManager* todManager; //0x0230
	char pad_0238[56]; //0x0238
}; //Size: 0x0270
assert_sizeof(Season, 0x270);

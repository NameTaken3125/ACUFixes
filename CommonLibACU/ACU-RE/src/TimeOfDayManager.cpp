#include "pch.h"

#include "ACU/TimeOfDayManager.h"
#include "ACU/World.h"

TimeOfDayManager* TimeOfDayManager::GetSingleton()
{
	auto* world = World::GetSingleton();
	if (!world) { return nullptr; }
	return world->timeOfDayManager;
}

#include "pch.h"

#include "ACU/UISounds.h"
#include "ACU/SoundManager.h"
#include "ACU/AssassinSoundSettings.h"

UISounds* UISounds::GetSingleton()
{
    return SoundManager::GetSingleton().assassinSoundSettings->sharedPtr_UISounds->GetPtr();
}

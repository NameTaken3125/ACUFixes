#include "pch.h"

#include "ACU/ACUGlobalSoundSet.h"
#include "ACU/SoundManager.h"
#include "ACU/AssassinSoundSettings.h"

ACUGlobalSoundSet* ACUGlobalSoundSet::GetSingleton()
{
    return SoundManager::GetSingleton().assassinSoundSettings->sharedPtr_ACUGlobalSoundSet->GetPtr();
}

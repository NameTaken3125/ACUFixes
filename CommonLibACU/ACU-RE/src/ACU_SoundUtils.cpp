#include "pch.h"

#include "ACU/Sound/ACU_SoundUtils.h"
#include "ACU_DefineNativeFunction.h"
DEFINE_GAME_FUNCTION(DispatchSoundEvent_mb, 0x1401859B0, void, __fastcall, (SoundInstance* p_soundInstance, Entity* p_sourceEntity_mb));

namespace ACU::Sound {

void PlaySoundFromEntity(SoundInstance& soundInstance, Entity& sourceEntity)
{
    DispatchSoundEvent_mb(&soundInstance, &sourceEntity);
}

} // namespace ACU::Sound

#pragma once

class SoundInstance;
class Entity;

namespace ACU::Sound {

void PlaySoundFromEntity(SoundInstance& soundInstance, Entity& sourceEntity);

} // namespace ACU::Sound

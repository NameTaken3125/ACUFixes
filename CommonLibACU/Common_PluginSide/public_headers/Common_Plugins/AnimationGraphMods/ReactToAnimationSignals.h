#pragma once

#include "ACU/basic_types.h"

using SignalID_t = uint32;
//constexpr SignalID_t signal_RangedWeaponShotFired = 0x800023;
//constexpr SignalID_t signal_RangedWeaponUnholster = 0x80005A;
//constexpr SignalID_t signal_EnteredHidespotCloset = 0x800024;
class HumanStatesHolder;

class CustomReactionToAnimationSignals
{
public:
    virtual ~CustomReactionToAnimationSignals() {}
    virtual void OnSignalChangeDispatched(HumanStatesHolder* receivingEntityHumanStates, SignalID_t signalInt, bool isSignalOn) = 0;
};

namespace AnimationTools::Signals
{
void RegisterCustomReaction(CustomReactionToAnimationSignals& newCustomReaction);
void UnregisterCustomReaction(CustomReactionToAnimationSignals& reactionToRemove);
void RegisterSignal(SignalID_t signalInt);
} // namespace AnimationTools::Signals

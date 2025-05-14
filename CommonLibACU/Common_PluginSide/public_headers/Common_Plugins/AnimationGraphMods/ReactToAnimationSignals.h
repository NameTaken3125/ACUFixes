#pragma once

#include "ACU/basic_types.h"

class HumanStatesHolder;


namespace AnimationTools::Signals
{
// e.g.:
//constexpr SignalID_t signal_RangedWeaponShotFired = 0x800023;
//constexpr SignalID_t signal_RangedWeaponUnholster = 0x80005A;
//constexpr SignalID_t signal_EnteredHidespotCloset = 0x800024;
using SignalID_t = uint32;
using CustomReactionToAnimationSignals = void(HumanStatesHolder* receivingEntityHumanStates, SignalID_t signalInt, bool isSignalOn);
void RegisterCustomReaction(CustomReactionToAnimationSignals& newCustomReaction);
void UnregisterCustomReaction(CustomReactionToAnimationSignals& reactionToRemove);
void RegisterSignal(SignalID_t signalInt);
} // namespace AnimationTools::Signals

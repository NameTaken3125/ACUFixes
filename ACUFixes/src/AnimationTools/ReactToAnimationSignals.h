#pragma once

using SignalID_t = uint32;
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

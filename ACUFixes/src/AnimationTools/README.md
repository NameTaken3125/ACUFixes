## How the HoodControls animations work in the animation graph.

I. Layering my animation on top of normal gameplay.\
I take the "normal gameplay layering state" (that's `NewDemo_DEV.AtomGraph->RootStateMachine->States[0]`).\
I add another layer to it. This new layer will contain a state machine for the new hood animations.\
By adding some BoneWeights to the new layer, I limited the animations to the upper torso excluding the left arm
(because I figured it's already occupied by the hidden blade, phantom blade, pistols and bombs).

II. The "hood animations" state machine.\
High-level view of what I'm trying to achieve here:\
At any point during normal gameplay:
1. Player presses a "Toggle Hood" button.
2. Arno plays either "take hood off" or "put hood on" animation depending on whether the hood is _currently_ on or off.
Mid-level view:
1. Player presses a button.
2. Code figures out whether Arno has his hood on or off.
3. Code changes a single RTCP variable.
4. In response, the animation system plays one of 2 animations ("take hood off"/"put hood on") (and smoothly stops playing them).

What I need to do is to figure out how to make a state machine that allows me to play an animation exactly once
upon setting a given RTCP variable to a specific value (and smoothly stop).\
This simple functionality is not very easy to achieve. Because receiving feedback from the animation system can be problematic,
there are nonobvious opportunities enabling you to shoot yourself in the foot and to get "stuck" in some way.

I'll describe how I achieved this in stages of development.
The diagrams to various stages should be included somewhere near the distribution of this text, if not included in the repo itself,
and should make things clearer.
The ellipse boxes are nullstates, the boxes are playback states, the arrows are Transitions between states (with conditions stated nearby),
and the leftmost state is the single unconditional Initial state of the whole state machine.
1. Play an animation once after character loads and remain at the last frame of animation.\
For this, I need to add a single "play animation" state, and set it up as the single initial state (empty condition == "always true").\
![Stage 1](https://github.com/NameTaken3125/ACUFixes/releases/download/v0.9.2.3-ExtraMaterialsOnAnimationModding/HoodAnimationsStateMachine1.png)
2. Play an animation once after character loads then continue playing in a loop.\
For this, I take the singular state and add a Transition that simply restarts the same state.\
`Transition(targetState="same state", condition="playbackPercentage == 100%")`.\
![Stage 2](https://github.com/NameTaken3125/ACUFixes/releases/download/v0.9.2.3-ExtraMaterialsOnAnimationModding/HoodAnimationsStateMachine2.png)
3. Play an animation once when a given RTCP variable is set to a specific value.\
Since I want the animation to _not_be_layered_at_all_until_ the specified conditions are satisfied,
I cannot use just one "play animation" state anymore.\
Instead, I add a "null state", which will be my single Initial state.\
To this initial null state, I add a `Transition(targetState="play 'TakeHoodOff'", condition="HoodControlValue == 1")`.\
Now, after the character loads, he will not play the animation until I set the "**HoodControlValue**" to 1 (e.g. by ticking an ImGui checkbox),
at which point the playback begins and finishes, but what happens next depends on the Transitions that the two states have set up.\
If the playback state has no Transitions, then the animation becomes stuck at the last frame, without reacting to changes to "**HoodControlValue**".\
If the playback state has `Transition(targetState="same state", condition="playbackPercentage == 100%)`, then the animation will be replayed in a loop,
without reacting to changes to "**HoodControlValue**".\
Now for the gotchas:\
If the playback state has `Transition(targetState="defaultNullState", condition="HoodControlValue == 0")`, then the animation gets stuck at the last frame
until you manually set "**HoodControlValue**" to `0`. That's not what we want, obviously.\
If the playback state has `Transition(targetState="defaultNullState", condition="playbackPercentage == 100%")`, then
the animation system transitions to the default nullState, but because the value of "**HoodControlValue**" remains `1`, it immediately
transitions again into the playback state. As a result, the animation will play in a loop until you manually set "**HoodControlValue**" to `0`,
at which point it will _finish_playing_ then stop.\
This, again, is not what we want (not for the hood animation, at least).\
![Stage 3](https://github.com/NameTaken3125/ACUFixes/releases/download/v0.9.2.3-ExtraMaterialsOnAnimationModding/HoodAnimationsStateMachine3.png)
4. Play an animation _exactly_once_ when a given RTCP variable is set to a specific value, without manually stopping it.\
For this, I add a third state, "**theOtherNullState**" and the playback state receives a `Transition(targetState="theOtherNullState", condition="playbackPercentage == 100%")`.\
Now, after the character loads, he will not play the animation until I set the "**HoodControlValue**" to 1,
at which point the playback begins and finishes without playing in a loop or being stuck in the last frame.\
However, since the state machine is in "**theOtherNullState**", and since that one doesn't have any Transitions,
the animation system no longer reacts to changes to "**HoodControlValue**".\
So we have achieved the functionality of "play an animation cleanly exactly once", but it is a single-use functionality
(until the game reloads and the Player Entity rebuilds).\
![Stage 4](https://github.com/NameTaken3125/ACUFixes/releases/download/v0.9.2.3-ExtraMaterialsOnAnimationModding/HoodAnimationsStateMachine4.png)
5. Play "take hood off" animation exactly once when "**HoodControlValue**" is set to `1`, then play "put hood on" exactly once when "**HoodControlValue**" is set to `0`,
then again "take hood off" when `HoodControlValue==1`, then "put hood on" when `HoodControlValue==0` and so on.
I add a **fourth state**, the "play 'put hood on'" state.
Each of the four states contains a single Transition to the "next" state. The 'playback' states transition to Nulls
when the playback finishes, and the null states transition to Playback when the "**HoodControlValue**" is set to appropriate value.
The good news is that within this state machine, whenever you change the "**HoodControlValue**",
there is no way to get stuck in either the last frame of an animation or in an infinite animation loop.
The bad news is another gotcha:
Notice how there is no way to start playing "take hood off" animation from "**theOtherNullState**" (without going through the "put hood on")?
There is no way to play "take hood off" again immediately after having played it.
What if you do want to play it for some reason?
- What if the game's cutscene has put the hood back on without going through your custom animation shenanigans, and you want to take it off again?
- What if you yourself have manually toggled the visibility of the hood's Visual components from, say, the ImGui developer menu?
- What if the "take hood off" animation did play, but the hood ended up _not_ being taken off? As in, the animation signal was, uhh, lost in lag?
(I did say that receiving messages from the animation system can be problematic)

We need to further improve this state machine so that either one of the hood animations can be played exactly once, at any point, from either one of the
null states by changing the "**HoodControlValue**".\
To do this, more complicated Transitions and conditions will be used.\
Furthermore, the "take hood off" and "put hood on" will no longer exactly correspond to
"`HoodControlValue==1`" and "`HoodControlValue==0`".\
![Stage 5](https://github.com/NameTaken3125/ACUFixes/releases/download/v0.9.2.3-ExtraMaterialsOnAnimationModding/HoodAnimationsStateMachine5.png)
6. Play either one of "take hood off" or "put hood on" animations exactly once, regardless of whether it was just played.\
Basically:\
**defaultNullState** has  `Transition(targetState="play 'take hood off'", condition="HoodControlValue==1")`, but\
**theOtherNullState** has `Transition(targetState="play 'take hood off'", condition="HoodControlValue==3")`.\
The "play 'takeHoodOff'" state has two Transitions:\
`Transitions[0] = Transition(targetState="theOtherNullState", condition="playbackPercentage==100% AND HoodControlValue==1)`\
`Transitions[1] = Transition(targetState="defaultNullState",  condition="playbackPercentage==100% AND HoodControlValue==3)`.\
Similarly, the values of `0` and `2` are used to play the "put hood on" animation.\
Now, if the animation system is in "**theOtherNullState**" and I need to play the "take hood off" animation, then I can still do that by setting
the "**HoodControlValue**" to `3`. This will not result in a playback loop but instead end up in the "**defaultNullState**".\
A complication here is that I don't know which state the state machine is currently in.\
But I do know the current value of "**HoodControlValue**", and from that I know which of the null states the state machine _is_in_or_will_soon_end_up_in_.\
Knowing that, and knowing which of the two animations I want to be played, I can set the "**HoodControlValue**" to a new value.\
![Stage 6](https://github.com/NameTaken3125/ACUFixes/releases/download/v0.9.2.3-ExtraMaterialsOnAnimationModding/HoodAnimationsStateMachine6.png)
7. Interrupt "take hood off" with "put hood on" and vice versa.\
If I tap "hood toggle button" before the "take hood off" animation finishes,
then I'd expect this animation to immediately be interrupted by a "put hood on" animation. This would be nice,
responsive, civilized. With the current state machine, however, I'll end up finishing the "off" animation and only then playing the "on".\
To implement this "interrupt", I add a new Transition in each of the two playback states that immediately (smoothly over 0.25 seconds, actually) transition
to the "opposite" playback state if the "HoodControlValue" is one of the "opposite" values.\
(Note that in the current implementation, since I am using specifically the visibility of the Visual components as the indicator of the Hood State,
you actually need to wait until the hood Visuals get switched (about halfway through the animation)
before the second tap on the "hood toggle button" will actually interrupt the animation with the opposite one).
![Stage 7](https://github.com/NameTaken3125/ACUFixes/releases/download/v0.9.2.3-ExtraMaterialsOnAnimationModding/HoodAnimationsStateMachine7.png)

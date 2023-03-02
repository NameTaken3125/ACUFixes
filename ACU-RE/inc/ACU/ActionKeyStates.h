#pragma once

#include "Enum_ActionKeyCode.h"

class KeyStates
{
public:
    unsigned char isPressed_parkourUp; //0x0000
    unsigned char isPressed_LMB; //0x0001
    unsigned char isPressed_eagleVision; //0x0002
    unsigned char isPressed_parkourDownInteract; //0x0003
    unsigned char isPressed_disguise; //0x0004
    unsigned char isPressed_numpad4cycleLeftHand; //0x0005
    unsigned char isPressed_medicine; //0x0006
    unsigned char isPressed_numpad6cycleBombs; //0x0007
    unsigned char isPressed_map; //0x0008
    unsigned char isPressed_escape; //0x0009
    unsigned char isPressed_crouch; //0x000A
    unsigned char isPressed_RMB; //0x000B
    unsigned char isPressed_sprint; //0x000C
    unsigned char isPressed_bombDrop; //0x000D
    unsigned char isPressed_reload; //0x000E
    unsigned char isPressed_centerCamera; //0x000F
    unsigned char isPressed_phantom; //0x0010
    unsigned char isPressed_berserk; //0x0011
    unsigned char isPressed_gun; //0x0012
    unsigned char isPressed_smokeBomb; //0x0013
    unsigned char isPressed_stunBomb; //0x0014
    unsigned char isPressed_cherryBomb; //0x0015
    unsigned char isPressed_poisonBomb; //0x0016
    unsigned char isPressed_moneyPouch; //0x0017
    unsigned char isPressed_9_booster; //0x0018
    char pad_0019[7]; //0x0019

}; //Size: 0x0020
assert_sizeof(KeyStates, 0x20);

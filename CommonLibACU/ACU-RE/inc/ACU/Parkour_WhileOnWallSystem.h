#pragma once

class WhileOnWallSystem_28;
class WindowEntry;

class WhileOnWallSystem
{
public:
    char pad_0000[40]; //0x0000
    WhileOnWallSystem_28* p28; //0x0028
    char pad_0030[416]; //0x0030
    unsigned char byte_1d0; //0x01D0
    char pad_01D1[7]; //0x01D1
    void* climb; //0x01D8
    void* climbCornerInside; //0x01E0
    void* climbCornerOutside; //0x01E8
    void* climbFacade; //0x01F0
    void* offTheWall; //0x01F8
    void* overhang; //0x0200
    WindowEntry* windowEntryTester; //0x0208
    void* climbLadder; //0x0210
    void* parkour; //0x0218
    char pad_0220[456]; //0x0220
    float flt_eq_1; //0x03E8
    char pad_03EC[4]; //0x03EC
}; //Size: 0x03F0
assert_sizeof(WhileOnWallSystem, 0x3F0);

class WhileOnWallSystem_28_0
{
public:
    char pad_0000[496]; //0x0000
    void* currentLedge_mb; //0x01F0
    void* confirmedParkourAction; //0x01F8
    char pad_0200[264]; //0x0200
}; //Size: 0x0308
class WhileOnWallSystem_28
{
public:
    WhileOnWallSystem_28_0* p0; //0x0000
    char pad_0008[256]; //0x0008
}; //Size: 0x0108

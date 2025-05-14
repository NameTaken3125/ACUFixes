#pragma once

#include "basic_types.h"
#include "vmath/vmath.h"

struct IDXGISwapChain;

class HasSwapchain_6f8_0
{
public:
    char pad_0000[932]; //0x0000
    uint32 frameCount; //0x03A4
    char pad_03A8[96]; //0x03A8
}; //Size: 0x0408

class HasSwapchain_6f8
{
public:
    HasSwapchain_6f8_0* p0; //0x0000
    uint64 hWindow_8; //0x0008
    IDXGISwapChain* swapchain; //0x0010
    char pad_0018[16]; //0x0018
    uint32 swapchainPresent_syncInterval; //0x0028
    char pad_002C[68]; //0x002C
    uint64 hWindow_70; //0x0070
    char pad_0078[136]; //0x0078
}; //Size: 0x0100
assert_offsetof(HasSwapchain_6f8, swapchain, 0x10);

class HasSwapchain
{
public:
    char pad_0000[16]; //0x0000
    Vector2f windowSizeIncludingLetterbox; //0x0010
    char pad_0018[1752]; //0x0018
    uint64 hWindow; //0x06F0
    HasSwapchain_6f8* toSwapchain; //0x06F8
    char pad_0700[1080]; //0x0700

    // @helper_functions
    static HasSwapchain* GetSingleton() { return *(HasSwapchain**)0x14523C6D8; }
}; //Size: 0x0B38
assert_offsetof(HasSwapchain, toSwapchain, 0x6f8);

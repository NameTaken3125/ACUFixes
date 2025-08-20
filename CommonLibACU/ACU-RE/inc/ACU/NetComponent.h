#pragma once

#include "Component.h"
#include "SmallArray.h"

class NetService;

class NetComponent_20_20
{
public:
    virtual void Unk000();
    virtual void Unk008();
    virtual void Unk010();
    virtual void Unk018();
    virtual void Unk020();
    virtual void Unk028();
    virtual void Unk030();
    virtual void Unk038();
    virtual void Unk040();
    virtual void Unk048();
    virtual void Unk050();
    virtual void Unk058();
    virtual void Unk060();
    virtual void Unk068();
    virtual void Unk070();
    virtual void Unk078();
    virtual void Unk080();
    virtual void Unk088();
    virtual void Unk090();
    virtual void Unk098();
    virtual void Unk0A0();
    virtual void Unk0A8();
    virtual void Unk0B0();
    virtual void Unk0B8();
    virtual void Unk0C0();
    virtual void Unk0C8();

    char pad_0008[296]; //0x0008
}; //Size: 0x0130

class NetComponent_20
{
public:
    char pad_0000[32]; //0x0000
    NetComponent_20_20 stru_20; //0x0020
    char pad_0150[200]; //0x0150
}; //Size: 0x0218

class NetComponent : public Component
{
public:
    NetComponent_20 stru_20; //0x0020
    SmallArray<NetService*> NetServices; //0x0238
    char pad_0244[52]; //0x0244
}; //Size: 0x0278
assert_offsetof(NetComponent, stru_20.stru_20, 0x40);
assert_sizeof(NetComponent, 0x278);

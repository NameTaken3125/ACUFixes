#pragma once

#include "ACU/basic_types.h"
#include "ACU/BigArray.h"


class LoadingThread_2C0;

class LoadRequestRingbuffer
{
public:
	LoadingThread_2C0** arr; //0x0000
	uint32 numFreeSlots_mb; //0x0008
	uint32 nextInQueue; //0x000C
	uint32 whereToPutNew_mb; //0x0010
	uint32 ringbufferCapacity; //0x0014
	char pad_0018[8]; //0x0018
}; //Size: 0x0020
assert_sizeof(LoadRequestRingbuffer, 0x20);

class LoadingThread
{
public:
	// @members

	char pad_0000[24]; //0x0000
	uint32 threadID; //0x0018
	char pad_001C[316]; //0x001C
	uint64 criticalSection_158; //0x0158
	char pad_0160[132]; //0x0160
	uint32 dword_1E4; //0x01E4
	char pad_01E8[8]; //0x01E8
	HANDLE threadHandle; //0x01F0
	char pad_01F8[8]; //0x01F8
	LoadRequestRingbuffer ringbuffer200; //0x0200
	LoadRequestRingbuffer ringbuffer220; //0x0220
	LoadRequestRingbuffer ringbuffer240; //0x0240
	LoadRequestRingbuffer ringbuffer_260; //0x0260
	LoadRequestRingbuffer ringbuffer280gatherRelatedDatapacks; //0x0280
	LoadRequestRingbuffer ringbufferToLoadTasks_mb; //0x02A0
	LoadingThread_2C0* currentToLoad; //0x02C0
	char pad_02C8[8]; //0x02C8
	uint64 criticalSection_2D0; //0x02D0
	char pad_02D8[60]; //0x02D8
	uint32 numFreeRequestSlots; //0x0314
	char pad_0318[64]; //0x0318
	LoadRequestRingbuffer ringbufferForDestruction_mb; //0x0358
	char pad_0378[24]; //0x0378
	uint8 isLoadingPaused_mb; //0x0390
	char pad_0391[151]; //0x0391

	// @helper_functions
	static LoadingThread* GetSingleton() { return *(LoadingThread**)0x14525B8F8; }
}; //Size: 0x0428
assert_sizeof(LoadingThread, 0x428);

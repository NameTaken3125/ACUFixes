/*
Code from:
Toggle hardware data/read/execute breakpoints programmatically
Michael Chourdakis
https://www.codeproject.com/Articles/28071/Toggle-hardware-data-read-execute-breakpoints-prog
Thank you
*/

// 
#ifndef _HWBRK_H
#define _HWBRK_H

enum HWBRK_TYPE
	{
	HWBRK_TYPE_CODE,
	HWBRK_TYPE_READWRITE,
	HWBRK_TYPE_WRITE,
	};

enum HWBRK_SIZE
	{
	HWBRK_SIZE_1,
	HWBRK_SIZE_2,
	HWBRK_SIZE_4,
	HWBRK_SIZE_8,
	};

HANDLE SetHardwareBreakpoint(HANDLE hThread,HWBRK_TYPE Type,HWBRK_SIZE Size,void* s);
bool RemoveHardwareBreakpoint(HANDLE hBrk);
	

#endif
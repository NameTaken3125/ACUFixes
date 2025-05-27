#include "pch.h"

#include "EarlyHooks/VEH.h"

#include "MyLog.h"
static DEFINE_LOGGER_CONSOLE_AND_FILE(SOOLog, "[SOOverride]");

#include "ACU/LoadingThread.h"
#include "external/hwbrk/hwbrk.h"

// The function of interest has at least 3 call sites.
// Hooking the function at the prologue technically allows to catch more deserializations,
// but I'm not sure that I need them. Everything I tried to override
// was successfully caught from the callsite at 0x1426F0DC2.
void* g_CodeAddr_DeserializeSingleManagedObjectFrom2ndByte_Prologue = (void*)0x14270B510;
void* g_CodeAddr_DeserializeSingleManagedObjectFrom2ndByte_Callsite0 = (void*)0x1426F0DC2;
std::optional<VEHandler> g_EarlyHooks_SingleObjectOverride_VEHandler;

HANDLE g_SOO_HWBPHandle = nullptr;
void SingleObjectOverride_HWBP_SetBP()
{
	HANDLE loadingThreadHandle = OpenThread(THREAD_ALL_ACCESS, 0, LoadingThread::GetSingleton()->threadID);
	g_SOO_HWBPHandle = SetHardwareBreakpoint(loadingThreadHandle, HWBRK_TYPE_CODE, HWBRK_SIZE_1, g_CodeAddr_DeserializeSingleManagedObjectFrom2ndByte_Callsite0);
	//g_SOO_HWBPHandle = SetHardwareBreakpoint(LoadingThread::GetSingleton()->threadHandle, HWBRK_TYPE_CODE, HWBRK_SIZE_1, g_CodeAddr_DeserializeSingleManagedObjectFrom2ndByte_Prologue);
}
void SingleObjectOverride_HWBP_ClearBP()
{
	if (g_SOO_HWBPHandle)
	{
		RemoveHardwareBreakpoint(g_SOO_HWBPHandle);
		g_SOO_HWBPHandle = nullptr;
	}
}
class ManagedObject;
class DeserializationStream;
void DeserializeManagedObject_FullReplacement(DeserializationStream* deserState, ManagedObject** pManObj);
LONG HWBP_SingleObjectOverride_HandleException(::_EXCEPTION_POINTERS* ExceptionInfo)
{
	void* ExceptionAddress = ExceptionInfo->ExceptionRecord->ExceptionAddress;
	if (ExceptionAddress == g_CodeAddr_DeserializeSingleManagedObjectFrom2ndByte_Callsite0)
	{
		CONTEXT* ContextRecord = ExceptionInfo->ContextRecord;
		ContextRecord->EFlags |= EFLAGS_MASK_RF;

		DeserializeManagedObject_FullReplacement(
			(DeserializationStream*)ContextRecord->Rcx,
			(ManagedObject**)ContextRecord->Rdx
			);
		ContextRecord->Rip += 5;
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	else if (ExceptionAddress == g_CodeAddr_DeserializeSingleManagedObjectFrom2ndByte_Prologue)
	{
		ExceptionInfo->ContextRecord->EFlags |= EFLAGS_MASK_RF;
		ExceptionInfo->ContextRecord->Rip = (DWORD64)&DeserializeManagedObject_FullReplacement;
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

void SingleObjectOverride_InitList();
void SingleObjectOverride_HWBP_Start()
{
	SingleObjectOverride_InitList();
	g_EarlyHooks_SingleObjectOverride_VEHandler.emplace(1, HWBP_SingleObjectOverride_HandleException);
	SingleObjectOverride_HWBP_SetBP();
}
void SingleObjectOverride_HWBP_End()
{
	SingleObjectOverride_HWBP_ClearBP();
	g_EarlyHooks_SingleObjectOverride_VEHandler.reset();
}

#include "pch.h"

#include "ACU/LoadingThread.h"
#include "external/hwbrk/hwbrk.h"

#include "VEH.h"

#include "MyLog.h"


void* g_CodeAddr_DeserializeSingleManagedObject_Full =  (void*)0x1426F0CF7;
void* g_CodeAddr_WhenGameForgesJustOpened =             (void*)0x14272F794;
std::optional<VEHandler> g_EarlyHooks_SingleObjectOverride_VEHandler;

HANDLE g_HWBPHandle_DeserializeSingleObjectFull = nullptr;
HANDLE g_HWBPHandle_NewForgeAdded = nullptr;
HANDLE SetHardwareBreakpoint_Code(void* addr, DWORD threadID)
{
    HANDLE handle = OpenThread(THREAD_ALL_ACCESS, 0, threadID);
    return SetHardwareBreakpoint(handle, HWBRK_TYPE_CODE, HWBRK_SIZE_1, addr);
}
void AssetOverrides_EarlyHooks_HWBP_SetBP()
{
    g_HWBPHandle_DeserializeSingleObjectFull = SetHardwareBreakpoint_Code(g_CodeAddr_DeserializeSingleManagedObject_Full, LoadingThread::GetSingleton()->threadID);
    g_HWBPHandle_NewForgeAdded = SetHardwareBreakpoint_Code(g_CodeAddr_WhenGameForgesJustOpened, LoadingThread::GetSingleton()->threadID);
}
void AssetOverrides_EarlyHooks_HWBP_ClearBP()
{
    if (g_HWBPHandle_DeserializeSingleObjectFull)
    {
        RemoveHardwareBreakpoint(g_HWBPHandle_DeserializeSingleObjectFull);
        g_HWBPHandle_DeserializeSingleObjectFull = nullptr;
    }
    if (g_HWBPHandle_NewForgeAdded)
    {
        RemoveHardwareBreakpoint(g_HWBPHandle_NewForgeAdded);
        g_HWBPHandle_NewForgeAdded = nullptr;
    }
}
void WhenStartDeserializeFileInDatapackTogetherWithPrologue_FullReplacement(uint64 rax, uint64 rbx, uint64 rsi, uint64 rbp, uint64 rsp, uint64 r13);
LONG HWBP_SingleObjectOverride_HandleException(::_EXCEPTION_POINTERS* ExceptionInfo)
{
    void* ExceptionAddress = ExceptionInfo->ExceptionRecord->ExceptionAddress;
    if (ExceptionAddress == g_CodeAddr_DeserializeSingleManagedObject_Full)
    {
        ExceptionInfo->ContextRecord->EFlags |= EFLAGS_MASK_RF;
        CONTEXT* ContextRecord = ExceptionInfo->ContextRecord;
        WhenStartDeserializeFileInDatapackTogetherWithPrologue_FullReplacement(
            ContextRecord->Rax,
            ContextRecord->Rbx,
            ContextRecord->Rsi,
            ContextRecord->Rbp,
            ContextRecord->Rsp,
            ContextRecord->R13
        );
        ExceptionInfo->ContextRecord->Rip = 0x1426F0DC7;
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    else if (ExceptionAddress == g_CodeAddr_WhenGameForgesJustOpened)
    {
        ExceptionInfo->ContextRecord->EFlags |= EFLAGS_MASK_RF;
        void AssetOverrides_PutForgesInCorrectOrder(); AssetOverrides_PutForgesInCorrectOrder();
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

#include "AssetOverridesConfig.h"
void AssetOverrides_InitFromLoadOrder_EarlyHook();
void AssetOverrides_EarlyHooks_Start()
{
    if (!g_AssetOverridesConfig.enableOnStartup) return;
    AssetOverrides_InitFromLoadOrder_EarlyHook();
    g_EarlyHooks_SingleObjectOverride_VEHandler.emplace(1, HWBP_SingleObjectOverride_HandleException);
    AssetOverrides_EarlyHooks_HWBP_SetBP();
}
void AssetOverrides_EarlyHooks_End()
{
    AssetOverrides_EarlyHooks_HWBP_ClearBP();
    g_EarlyHooks_SingleObjectOverride_VEHandler.reset();
}
